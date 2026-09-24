#!/bin/bash
# Build named sources/reproduction and neutral blind artifacts.
# A clean source archive runs this same full build, with only recursion disabled.
set -euo pipefail
cd "$(dirname "$0")"
CHECK_CLEAN=1
case "${1:-}" in
  '') ;;
  --skip-clean-extraction) CHECK_CLEAN=0 ;;
  *) echo "usage: bash build_submission.sh [--skip-clean-extraction]"; exit 2 ;;
esac
SUB=submission
mkdir -p "$SUB"
HERE=$(pwd)
TMP=$(mktemp -d "$HERE/.submission-build.XXXXXX")
CLEAN=''
trap 'rm -rf "$TMP"; if [ -n "$CLEAN" ]; then rm -rf "$CLEAN"; fi' EXIT
DEPS="artin_correlations.tex FRONT.tex SECTION_content.tex DISC.tex assemble.py anonymise.py scrub_blind.py build_submission.sh fig_gap_delta.pdf make_figure.py LEAN_NOTE.md README.md"
# Remove the superseded identifying blind filenames from the working submission set.
rm -f "$SUB/artin_correlations_anonymous.pdf" "$SUB/artin_correlations_blind_supplement.zip"

latex_build() {
  local dir=$1 stem=$2
  ( cd "$dir"; for i in 1 2 3; do pdflatex -interaction=nonstopmode "$stem.tex" > /dev/null 2>&1 || true; done )
  local err ovh ovv und pages
  err=$(grep -c '^! ' "$dir/$stem.log" || true)
  ovh=$(grep -cE '^Overfull \\hbox' "$dir/$stem.log" || true)
  ovv=$(grep -cE '^Overfull \\vbox' "$dir/$stem.log" || true)
  und=$(grep -ciE 'undefined|multiply.defined' "$dir/$stem.log" || true)
  pages=$(pdfinfo "$dir/$stem.pdf" | awk '/^Pages/{print $2}')
  echo "   $stem pages=$pages errors=$err overfull_hbox=$ovh overfull_vbox=$ovv undefined=$und"
  [ "$err/$ovh/$ovv/$und" = '0/0/0/0' ] || { echo 'FAIL: LaTeX diagnostics'; exit 1; }
}

echo '== optional assembly'
python3 assemble.py

echo '== named manuscript'
latex_build . artin_correlations
PAGES=$(pdfinfo artin_correlations.pdf | awk '/^Pages/{print $2}')
cp artin_correlations.pdf "$SUB/artin_correlations_manuscript.pdf"

echo '== blind manuscript'
python3 anonymise.py artin_correlations.tex "$TMP/blind_manuscript.tex"
cp fig_gap_delta.pdf "$TMP/"
latex_build "$TMP" blind_manuscript
APAGES=$(pdfinfo "$TMP/blind_manuscript.pdf" | awk '/^Pages/{print $2}')
NSEC=$(grep -c '^\\section{' artin_correlations.tex || true)
ANSEC=$(grep -c '^\\section{' "$TMP/blind_manuscript.tex" || true)
D=$((APAGES - PAGES)); [ "${D#-}" -le 1 ] || { echo 'FAIL: blind page count'; exit 1; }
[ "$NSEC" = "$ANSEC" ] || { echo 'FAIL: blind section count'; exit 1; }
pdftotext "$TMP/blind_manuscript.pdf" "$TMP/blind_manuscript.txt"
python3 anonymise.py --audit blind-pdf-text "$TMP/blind_manuscript.txt"
pdfinfo "$TMP/blind_manuscript.pdf" > "$TMP/pdfinfo.txt"
python3 anonymise.py --audit blind-pdf-metadata "$TMP/pdfinfo.txt"
AUTH=$(awk -F': *' '/^Author/{print $2}' "$TMP/pdfinfo.txt")
[ -z "$AUTH" ] || { echo 'FAIL: blind Author metadata'; exit 1; }
echo "   blind pages=$APAGES (named $PAGES) sections=$ANSEC audit=clean"
cp "$TMP/blind_manuscript.pdf" "$SUB/blind_manuscript.pdf"

echo '== named source and reproduction archives'
rm -f "$SUB/artin_correlations_source.zip" "$SUB/artin_correlations_reproduction.zip"
zip -q -r "$SUB/artin_correlations_source.zip" $DEPS code results lean \
  -x 'lean/.lake/*' '*/__pycache__/*' '*.pyc' 'code/artin_payoff'
zip -q -r "$SUB/artin_correlations_reproduction.zip" code results lean README.md LEAN_NOTE.md \
  -x 'lean/.lake/*' '*/__pycache__/*' '*.pyc' 'code/artin_payoff'

echo '== blind supplement: scrub and audit every file'
mkdir "$TMP/blind"
cp -r code results lean LEAN_NOTE.md "$TMP/blind/"
rm -rf "$TMP/blind/lean/.lake" "$TMP/blind/code/__pycache__"
rm -f "$TMP/blind/code/artin_payoff"
cp "$TMP/blind_manuscript.tex" "$TMP/blind/blind_manuscript.tex"
python3 scrub_blind.py "$TMP/blind"
python3 - "$TMP/blind" <<'PY'
import pathlib, sys
from anonymise import audit
root = pathlib.Path(sys.argv[1])
problems = []
n = 0
for p in sorted(root.rglob('*')):
    if p.is_file():
        n += 1
        rel = str(p.relative_to(root))
        problems += audit(rel, 'filename')
        problems += audit(p.read_bytes().decode('utf-8', errors='replace'), rel)
if problems:
    print('\n'.join(problems))
    sys.exit(1)
print(f'   blind supplement: {n} files, audit=clean')
PY
rm -f "$SUB/blind_supplement.zip"
( cd "$TMP/blind"; zip -q -r "$HERE/$SUB/blind_supplement.zip" . )

echo '== build actual scrubbed Lean archive (optional shared cache)'
CACHE=/home/work/Projects/artin-lean/artin/.lake/packages
if [ -d "$CACHE/mathlib/.lake/build" ] && command -v "$HOME/.elan/bin/lake" >/dev/null; then
  mkdir "$TMP/leancheck"
  ( cd "$TMP/leancheck"; unzip -q "$HERE/$SUB/blind_supplement.zip" )
  mkdir -p "$TMP/leancheck/lean/.lake"
  ln -s "$CACHE" "$TMP/leancheck/lean/.lake/packages"
  ( export PATH="$HOME/.elan/bin:$PATH"; cd "$TMP/leancheck/lean"; lake build; bash gate.sh )
else
  echo 'SKIP: scrubbed Lean build requires an installed lake and the shared Mathlib cache; no download attempted.'
fi

if [ "$CHECK_CLEAN" = 1 ]; then
  echo '== source archive: full build from a clean /tmp extraction'
  CLEAN=$(mktemp -d /tmp/submission-source.XXXXXX)
  ( cd "$CLEAN"; unzip -q "$HERE/$SUB/artin_correlations_source.zip"
    env -u MATH_AUDIT_BASE -u CONSOLIDATED_DIR -u PAPER1_TEX -u PAPER3_TEX bash build_submission.sh --skip-clean-extraction
  )
  echo '   clean-extraction full build: EXIT=0'
else
  echo '   clean-extraction recursion only: skipped (all other build stages ran)'
fi
echo '== done'
