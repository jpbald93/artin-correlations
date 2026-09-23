#!/bin/bash
# Build the consolidated paper's submission set. Portable: no absolute paths.
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
echo "== source zip"
zip -q -j "$SUB/artin_correlations_source.zip" artin_correlations.tex FRONT.tex SECTION_content.tex \
    DISC.tex assemble.py fig_gap_delta.pdf make_figure.py
echo "== reproduction zip"
rm -f "$SUB/artin_correlations_reproduction.zip"
zip -q -r "$SUB/artin_correlations_reproduction.zip" code results lean README.md LEAN_NOTE.md \
    -x "lean/.lake/*" "code/__pycache__/*" "*.aux" "*.log" "*.out"
echo "== done"; ls -la "$SUB"
