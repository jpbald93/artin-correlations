#!/bin/bash
# Build the consolidated paper's submission set. Portable: no absolute paths.
# Produces: the named manuscript, an ANONYMISED manuscript (checked for identifying text),
# a source zip and a reproduction zip. Fails on undefined refs or overfull hboxes.
set -euo pipefail
cd "$(dirname "$0")"
SUB=submission
mkdir -p "$SUB"

echo "== assemble (regenerate artin_correlations.tex from the two source papers)"
python3 assemble.py

echo "== named manuscript"
for i in 1 2 3; do pdflatex -interaction=nonstopmode artin_correlations.tex >/dev/null 2>&1; done
OVH=$(grep -cE '^Overfull \\hbox' artin_correlations.log || true)
UND=$(grep -ciE 'undefined|multiply.defined' artin_correlations.log || true)
PAGES=$(pdfinfo artin_correlations.pdf | awk '/^Pages/{print $2}')
echo "   pages=$PAGES overfull_hbox=$OVH undefined=$UND"
[ "$UND" = "0" ] || { echo "FAIL: undefined references"; exit 1; }
[ "$OVH" = "0" ] || { echo "FAIL: overfull \\hbox"; exit 1; }
cp artin_correlations.pdf "$SUB/artin_correlations_manuscript.pdf"

echo "== anonymised manuscript (author block, addresses, repository URLs and self-citations removed)"
TMP=$(mktemp -d); trap 'rm -rf "$TMP"' EXIT
cp artin_correlations.tex "$TMP/anon.tex"
python3 anonymise.py artin_correlations.tex "$TMP/anon.tex"
( cd "$TMP" && for i in 1 2 3; do pdflatex -interaction=nonstopmode anon.tex >/dev/null 2>&1 || true; done )
APAGES=$(pdfinfo "$TMP/anon.pdf" | awk '/^Pages/{print $2}')
NSEC=$(grep -c '^\\section{' artin_correlations.tex || true); ANSEC=$(grep -c '^\\section{' "$TMP/anon.tex" || true)
D=$((APAGES - PAGES)); [ "${D#-}" -le 1 ] || { echo "FAIL: anon page count $APAGES vs named $PAGES"; exit 1; }
[ "$NSEC" = "$ANSEC" ] || { echo "FAIL: anon lost a section ($ANSEC vs $NSEC)"; exit 1; }
HITS=$(pdftotext "$TMP/anon.pdf" - 2>/dev/null | grep -ciE "bald|jpbald93|@gmail|@genspark" || true)
echo "   anon pages=$APAGES identifying-text hits=$HITS"
[ "$HITS" = "0" ] || { echo "FAIL: identifying text in the anonymised PDF"; exit 1; }
cp "$TMP/anon.pdf" "$SUB/artin_correlations_anonymous.pdf"

echo "== source zip"
rm -f "$SUB/artin_correlations_source.zip"
zip -q -j "$SUB/artin_correlations_source.zip" artin_correlations.tex FRONT.tex SECTION_content.tex \
    DISC.tex assemble.py build_submission.sh fig_gap_delta.pdf make_figure.py LEAN_NOTE.md
echo "== reproduction zip"
rm -f "$SUB/artin_correlations_reproduction.zip"
zip -q -r "$SUB/artin_correlations_reproduction.zip" code results lean README.md LEAN_NOTE.md \
    build_submission.sh assemble.py FRONT.tex SECTION_content.tex DISC.tex \
    -x "lean/.lake/*" "code/__pycache__/*" "*.aux" "*.log" "*.out" "code/build_submission.sh"
echo "== done"; ls -la "$SUB"
