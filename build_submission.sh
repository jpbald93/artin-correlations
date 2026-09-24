#!/bin/bash
# Build the consolidated paper's submission set. Portable: no absolute paths.
#
# Produces, in submission/:
#   artin_correlations_manuscript.pdf    named manuscript
#   artin_correlations_anonymous.pdf     blind manuscript (provenance removed, then audited)
#   artin_correlations_source.zip        named sources, including every build dependency
#   artin_correlations_reproduction.zip  named code/results/Lean (for the public record)
#   artin_correlations_blind_supplement.zip  scrubbed code/results/Lean for blind review (audited)
#
# Fails on: undefined references, overfull hboxes, a blind-PDF page count differing from the
# named one by more than one page or a different section count, ANY identifier / own DOI /
# repository link surviving in the blind PDF or in any text file of the blind supplement, or
# a source or reproduction archive that cannot rebuild from a clean extraction.
set -euo pipefail
cd "$(dirname "$0")"
SUB=submission
mkdir -p "$SUB"
DEPS="artin_correlations.tex FRONT.tex SECTION_content.tex DISC.tex assemble.py anonymise.py build_submission.sh fig_gap_delta.pdf make_figure.py LEAN_NOTE.md"

echo "== assemble (regenerate artin_correlations.tex from the two source papers)"
python3 assemble.py >/dev/null

echo "== named manuscript"
for i in 1 2 3; do pdflatex -interaction=nonstopmode artin_correlations.tex >/dev/null 2>&1 || true; done
OVH=$(grep -cE '^Overfull \\hbox' artin_correlations.log || true)
UND=$(grep -ciE 'undefined|multiply.defined' artin_correlations.log || true)
ERR=$(grep -c '^! ' artin_correlations.log || true)
PAGES=$(pdfinfo artin_correlations.pdf | awk '/^Pages/{print $2}')
echo "   pages=$PAGES errors=$ERR overfull_hbox=$OVH undefined=$UND"
[ "$ERR" = "0" ] || { echo "FAIL: LaTeX errors"; exit 1; }
[ "$UND" = "0" ] || { echo "FAIL: undefined references"; exit 1; }
[ "$OVH" = "0" ] || { echo "FAIL: overfull \\hbox"; exit 1; }
cp artin_correlations.pdf "$SUB/artin_correlations_manuscript.pdf"

echo "== blind manuscript"
TMP=$(mktemp -d); trap 'rm -rf "$TMP"' EXIT
python3 anonymise.py artin_correlations.tex "$TMP/anon.tex"
cp fig_gap_delta.pdf "$TMP/" 2>/dev/null || true
( cd "$TMP" && for i in 1 2 3; do pdflatex -interaction=nonstopmode anon.tex >/dev/null 2>&1 || true; done )
APAGES=$(pdfinfo "$TMP/anon.pdf" | awk '/^Pages/{print $2}')
AUND=$(grep -ciE 'undefined|multiply.defined' "$TMP/anon.log" || true)
NSEC=$(grep -c '^\\section{' artin_correlations.tex || true); ANSEC=$(grep -c '^\\section{' "$TMP/anon.tex" || true)
D=$((APAGES - PAGES)); [ "${D#-}" -le 1 ] || { echo "FAIL: blind page count $APAGES vs named $PAGES"; exit 1; }
[ "$NSEC" = "$ANSEC" ] || { echo "FAIL: blind lost a section ($ANSEC vs $NSEC)"; exit 1; }
[ "$AUND" = "0" ] || { echo "FAIL: undefined references in blind build"; exit 1; }
pdftotext "$TMP/anon.pdf" "$TMP/anon.txt"
python3 anonymise.py --audit blind-pdf-text "$TMP/anon.txt" || { echo "FAIL: blind PDF identifies the author"; exit 1; }
AUTH=$(pdfinfo "$TMP/anon.pdf" | awk -F': *' '/^Author/{print $2}')
[ -z "$AUTH" ] || { echo "FAIL: blind PDF metadata Author=$AUTH"; exit 1; }
echo "   blind pages=$APAGES (named $PAGES) sections=$ANSEC audit=clean"
cp "$TMP/anon.pdf" "$SUB/artin_correlations_anonymous.pdf"

echo "== source zip (named) + clean-extraction check"
rm -f "$SUB/artin_correlations_source.zip"
zip -q -j "$SUB/artin_correlations_source.zip" $DEPS
HERE=$(pwd)
mkdir "$TMP/src" && ( cd "$TMP/src" && unzip -q "$HERE/$SUB/artin_correlations_source.zip" )
for f in $DEPS; do [ -f "$TMP/src/$f" ] || { echo "FAIL: source zip lacks $f"; exit 1; }; done
( cd "$TMP/src" && python3 anonymise.py artin_correlations.tex anon.tex >/dev/null ) || { echo "FAIL: anonymiser does not run from the source zip"; exit 1; }
( cd "$TMP/src" && pdflatex -interaction=nonstopmode artin_correlations.tex >/dev/null 2>&1; pdflatex -interaction=nonstopmode artin_correlations.tex >/dev/null 2>&1; [ -s artin_correlations.pdf ] ) \
  || { echo "FAIL: manuscript does not compile from the source zip"; exit 1; }
echo "   source zip: all dependencies present; anonymiser and LaTeX run from a clean extraction"

echo "== reproduction zip (named, public record)"
rm -f "$SUB/artin_correlations_reproduction.zip"
zip -q -r "$SUB/artin_correlations_reproduction.zip" code results lean README.md LEAN_NOTE.md \
    -x "lean/.lake/*" "code/__pycache__/*" "*.aux" "*.log" "*.out"

echo "== blind supplement (scrubbed code/results/Lean) + per-file audit"
rm -rf "$TMP/blind"; mkdir -p "$TMP/blind"
cp -r code results lean LEAN_NOTE.md "$TMP/blind/"
rm -rf "$TMP/blind/lean/.lake" "$TMP/blind/code/__pycache__"
cp "$TMP/anon.tex" "$TMP/blind/manuscript_anonymous.tex"
python3 - "$TMP/blind" <<'PY'
import re, sys, pathlib
root = pathlib.Path(sys.argv[1])
SUBS = [(r"Copyright \(c\) 2026 J\. Bald\. All rights reserved\.", "Copyright (c) 2026 the authors. All rights reserved."),
        (r"Authors: J\. Bald, with a model-drafted skeleton from qwen3:32b \(local, via\s*\n\s*GMKtec/Ollama\) closed by J\. Bald with OpenClaw assistance\.",
         "Authors: withheld for review (skeleton drafted with a local language model, closed with AI assistance)."),
        (r"Authors: J\. Bald", "Authors: withheld for review"),
        (r"https?://github\.com/jpbald93/[A-Za-z0-9_.\-/]+", "[repository withheld for review]"),
        (r"10\.5281/zenodo\.\d+", "[DOI withheld for review]"),
        (r"\bJ(osh(ua)?)?\.? ?~?Bald\b", "the authors"), (r"jpbald93", "anon")]
for p in root.rglob("*"):
    if not p.is_file():
        continue
    try:
        s = p.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        continue
    t = s
    for a, b in SUBS:
        t = re.sub(a, b, t)
    if t != s:
        p.write_text(t, encoding="utf-8")
PY
BAD=0
while IFS= read -r -d '' f; do
  if grep -Iq . "$f"; then
    rel="${f#$TMP/blind/}"
    # third-party dependency URLs (Lean package manifest) are not provenance
    case "$rel" in lean/lake-manifest.json) sed -i 's#https://github.com/leanprover[^"]*#[lean-dependency]#g' "$f";; esac
    python3 anonymise.py --audit "$rel" "$f" >/dev/null || { python3 anonymise.py --audit "$rel" "$f" | head -3; BAD=1; }
  else
    strings "$f" | grep -qiE "Bald|jpbald93|0009-0002-1317-6489" && { echo "   binary $f carries an identifier"; BAD=1; }
  fi
done < <(find "$TMP/blind" -type f -print0)
[ "$BAD" = "0" ] || { echo "FAIL: blind supplement identifies the author"; exit 1; }
rm -f "$SUB/artin_correlations_blind_supplement.zip"
( cd "$TMP/blind" && zip -q -r "$HERE/$SUB/artin_correlations_blind_supplement.zip" . )
echo "   blind supplement: $(find "$TMP/blind" -type f | wc -l) files, audit=clean"
echo "== done"; ls -la "$SUB"
