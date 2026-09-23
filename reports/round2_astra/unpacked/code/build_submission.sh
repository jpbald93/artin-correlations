#!/bin/bash
# build_submission.sh — Paper 3 submission set, from the CURRENT paper/crossbase_artin.tex.
# Produces, in submission/:
#   crossbase_artin_manuscript.pdf     named manuscript
#   crossbase_artin_anonymous.pdf      anonymized (author/thanks/ORCID/email/repo stripped, metadata cleared)
#   crossbase_artin_source.zip         flat named LaTeX source
#   crossbase_artin_reproduction.zip   portable package (excludes backups + audit correspondence)
# Adapted from Paper 2's build_submission.sh. Fails loudly rather than shipping
# a manuscript with bad boxes, undefined refs, or surviving identifying text.
# Usage:  bash code/build_submission.sh
set -euo pipefail
cd "$(dirname "$0")/.."
ROOT="$PWD"
SUB="$ROOT/submission"
TEX="$ROOT/paper/crossbase_artin.tex"
mkdir -p "$SUB"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

echo "== named manuscript"
cd "$ROOT/paper"
for i in 1 2 3; do pdflatex -interaction=nonstopmode crossbase_artin.tex >/dev/null 2>&1; done
BOX=$(grep -cE '^(Overfull|Underfull)' crossbase_artin.log || true)
UND=$(grep -ciE 'undefined|multiply.defined' crossbase_artin.log || true)
PAGES=$(pdfinfo crossbase_artin.pdf | awk '/^Pages/{print $2}')
echo "   pages=$PAGES badboxes=$BOX undefined=$UND"
[ "$UND" = "0" ] || { echo "FAIL: undefined references"; exit 1; }
cp crossbase_artin.pdf "$SUB/crossbase_artin_manuscript.pdf"

echo "== anonymous manuscript"
cp "$TEX" "$WORK/anon.tex"
python3 - "$WORK/anon.tex" <<'PY'
import re, sys
p = sys.argv[1]
s = open(p).read()
s = s.replace(r'\author{Josh Bald}', r'\author{}')
# \address is an identifying field. "Ontario, Canada" survived an earlier build
# because the gate only grepped for names and URLs, never the address.
k = s.find(r'\address{')
if k != -1:
    m = k + len(r'\address{'); dd = 1
    while dd and m < len(s):
        if s[m] == '{': dd += 1
        elif s[m] == '}': dd -= 1
        m += 1
    s = s[:k] + s[m:]
i = s.find(r'\thanks{')
if i != -1:
    j = i + len(r'\thanks{'); d = 1
    while d and j < len(s):
        if s[j] == '{': d += 1
        elif s[j] == '}': d -= 1
        j += 1
    s = s[:i] + s[j:]
s = s.replace('Josh Bald', 'the author')
s = s.replace('jpbald93@gmail.com', 'email withheld for review')
s = s.replace('0009-0002-1317-6489', 'ORCID withheld for review')
s = re.sub(r'https?://github\.com/jpbald93/[^\s}{,)]*', 'repository URL withheld for review', s)
s = s.replace('jpbald93', 'withheld')
# Self-citations: this paper cites two companion preprints by the same author.
# Blind review requires those to be anonymised in the bibliography too, in the
# standard way (cited as anonymous companion work, author name removed).
s = s.replace('J.~Bald,\n\\emph{Correlations between primitive root statuses of consecutive primes}',
              'Author omitted for review,\n\\emph{Correlations between primitive root statuses of consecutive primes}')
s = s.replace('J.~Bald,\n\\emph{Exclusion laws for consecutive Artin primes in arbitrary bases}',
              'Author omitted for review,\n\\emph{Exclusion laws for consecutive Artin primes in arbitrary bases}')
s = s.replace('J.~Bald,', 'Author omitted for review,')
s = s.replace(r'\begin{document}',
              '\\hypersetup{pdfauthor={},pdftitle={},pdfsubject={},pdfkeywords={},pdfcreator={}}\n\\begin{document}', 1)
open(p, 'w').write(s)
PY
cd "$WORK"
for i in 1 2 3; do pdflatex -interaction=nonstopmode anon.tex >/dev/null 2>&1; done
APAGES=$(pdfinfo anon.pdf | awk '/^Pages/{print $2}')
[ "$APAGES" = "$PAGES" ] || { echo "FAIL: anon page count $APAGES != $PAGES"; exit 1; }
pdftotext -layout anon.pdf anon.txt
if grep -qiE 'josh|bald|jpbald93|0009-0002-1317-6489|github\.com/jpbald93|ontario|canada' anon.txt; then
  echo "FAIL: identifying text in anonymous PDF"; grep -niE 'josh|bald|jpbald93|ontario|canada' anon.txt | head; exit 1
fi
pdfinfo anon.pdf > anon_meta.txt
if grep -iE '^(Author|Title|Subject|Keywords)' anon_meta.txt | grep -qiE 'josh|bald|artin|primitive'; then
  echo "FAIL: identifying metadata in anonymous PDF"; exit 1
fi
cp anon.pdf "$SUB/crossbase_artin_anonymous.pdf"
cp anon_meta.txt "$ROOT/results/anonymous_pdf_metadata.txt"
echo "   pages=$APAGES, no identifying text or metadata"

echo "== named source zip"
rm -f "$SUB/crossbase_artin_source.zip"
cd "$ROOT/paper"
zip -q -X "$SUB/crossbase_artin_source.zip" crossbase_artin.tex

echo "== reproduction zip"
REPRO="$WORK/repro"
mkdir -p "$REPRO"/{code,paper,results,submission,lean/Artin}
cp "$ROOT"/code/*.py "$ROOT"/code/*.c "$ROOT"/code/*.sh "$REPRO/code/" 2>/dev/null || true
cp "$ROOT"/paper/crossbase_artin.tex "$ROOT"/paper/crossbase_artin.pdf "$REPRO/paper/"
cp "$ROOT"/results/* "$REPRO/results/" 2>/dev/null || true
cp "$SUB/crossbase_artin_anonymous.pdf" "$SUB/crossbase_artin_source.zip" "$REPRO/submission/"
cp "$ROOT"/lean/Artin/*.lean "$REPRO/lean/Artin/"
cp "$ROOT"/lean/Artin.lean "$ROOT"/lean/gate.sh "$ROOT"/lean/BUILD.md "$ROOT"/lean/README_LEAN.md "$REPRO/lean/" 2>/dev/null || true
for f in lakefile.toml lean-toolchain lake-manifest.json; do
  [ -f "$ROOT/lean/$f" ] && cp "$ROOT/lean/$f" "$REPRO/lean/"
done
cp "$ROOT"/README.md "$ROOT"/LEAN_NOTE.md "$REPRO/" 2>/dev/null || true
# NOTE: backups/ and audit_*/ are deliberately excluded (working material, not deliverables).
cd "$REPRO"
find . -type f ! -name MANIFEST.sha256 -print0 | sort -z | xargs -0 sha256sum > MANIFEST.sha256
rm -f "$SUB/crossbase_artin_reproduction.zip"
zip -qr -X "$SUB/crossbase_artin_reproduction.zip" .
if unzip -l "$SUB/crossbase_artin_reproduction.zip" | grep -qiE 'endorsement|backups/|audit_2026'; then
  echo "FAIL: reproduction zip contains excluded material"; exit 1
fi

echo "== done"
cd "$SUB" && ls -la *.pdf *.zip
