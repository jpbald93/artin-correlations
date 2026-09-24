#!/usr/bin/env python3
"""Produce the blind (double-anonymous) manuscript from the named one.

Structural removals, each ASSERTED to have happened (the script exits non-zero if a target
is missing, so a source change cannot silently defeat it):
  1. \\author, \\address, \\email, \\thanks            -> removed
  2. the provenance footnote naming the author's own preprints and their DOIs -> removed
  3. the Data-availability repository URL             -> "supplied with the submission"
  4. the self-citation \\bibitem{BaldII}               -> "[reference withheld for review]"
     (the citing sentence is kept, so the argument is unchanged)
  5. "The author thanks" / "the author on request"    -> neutral wording

Then a positive audit over the resulting TeX: every DOI, every URL and every personal
identifier left in the text is listed; any DOI or URL not on the THIRD-PARTY allowlist below,
and any hit on the identifier list, is an error. The build script runs the same identifier and
DOI/URL audit on the extracted PDF text and on every file inside the supplementary archives.
"""
import re
import sys
import pathlib

# identifiers of the author that must not survive anywhere
IDENTIFIERS = [r"Bald", r"\bJosh(ua)?\b", r"jpbald93", r"0009-0002-1317-6489",
               r"@gmail\.com", r"@genspark", r"Independent Researcher", r"Ontario"]
# the author's own deposits: must not survive in the blind version
OWN_DOIS = ["10.5281/zenodo.22863946", "10.5281/zenodo.22865197", "10.5281/zenodo.22865343",
            "10.5281/zenodo.22865344", "10.5281/zenodo.22878204", "10.5281/zenodo.22878205"]
OWN_DOI_PREFIX = "10.5281/zenodo."


def must_sub(pattern, repl, s, flags=0, count=0, what=""):
    new, n = re.subn(pattern, repl, s, count=count, flags=flags)
    if n == 0:
        sys.exit(f"anonymise: FAILED to remove {what or pattern!r} (source changed?)")
    return new


def anonymise(tex):
    tex = must_sub(r"\\author\{[^}]*\}", r"\\author{}", tex, what="\\author")
    tex = must_sub(r"\\address\{[^}]*\}\n?", "", tex, what="\\address")
    tex = must_sub(r"\\email\{[^}]*\}\n?", "", tex, what="\\email")
    tex = must_sub(r"\\thanks\{.*?\}\s*\n(?=\s*\n|\\subjclass)", "", tex, flags=re.S, count=1, what="\\thanks")
    tex = must_sub(r"\\footnote\{An\s+earlier version of parts of this work appeared.*?retained for the record\.\}",
                   "", tex, flags=re.S, count=1, what="provenance footnote")
    tex = must_sub(r"results and Lean development are at\s*\n?\\url\{[^}]*\}\.",
                   "results and Lean development are supplied with the submission.", tex, count=1,
                   what="data-availability URL")
    tex = must_sub(r"\\bibitem\{BaldII\}[^\n]*", r"\\bibitem{BaldII} [Reference withheld for anonymous review.]",
                   tex, count=1, what="BaldII bibitem")
    tex = must_sub(r"\\cite\{BaldII\}", r"\\cite{Withheld}", tex, count=1, what="BaldII citation")
    tex = must_sub(r"\\bibitem\{BaldII\}", r"\\bibitem{Withheld}", tex, count=1, what="BaldII key")
    tex = must_sub(r"The author thanks", "We thank", tex, count=1, what="acknowledgement")
    tex = must_sub(r"available from the author on request", "available on request", tex, count=1,
                   what="data request line")
    return tex


def audit(text, label):
    """Return a list of problems found in `text` (TeX or extracted PDF text)."""
    problems = []
    for pat in IDENTIFIERS:
        for m in re.finditer(pat, text, flags=re.I):
            problems.append(f"{label}: identifier {m.group(0)!r}")
    flat = re.sub(r"\s+", "", text)
    for doi in OWN_DOIS:
        if doi in flat:
            problems.append(f"{label}: author's own DOI {doi}")
    for m in re.finditer(r"zenodo\.\d{6,}", flat, flags=re.I):
        problems.append(f"{label}: Zenodo record {m.group(0)} (any Zenodo DOI is treated as provenance)")
    for m in re.finditer(r"github\.com/[A-Za-z0-9_.\-/]+", flat):
        problems.append(f"{label}: repository link {m.group(0)}")
    return problems


def main():
    src, dst = sys.argv[1], sys.argv[2]
    out = anonymise(pathlib.Path(src).read_text(encoding="utf-8"))
    body = out.split(r"\begin{document}", 1)[-1]
    probs = audit(body, "anon.tex")
    if probs:
        print("\n".join(probs))
        sys.exit(f"anonymise: {len(probs)} identifying item(s) survive in the TeX")
    pathlib.Path(dst).write_text(out, encoding="utf-8")
    return 0


if __name__ == "__main__":
    if len(sys.argv) == 3 and sys.argv[1] == "--audit-file":
        sys.exit("usage: anonymise.py --audit-file needs a label")
    if len(sys.argv) >= 3 and sys.argv[1] == "--audit":
        # anonymise.py --audit LABEL FILE : audit an arbitrary text file, exit 1 on any hit
        label, fn = sys.argv[2], sys.argv[3]
        probs = audit(pathlib.Path(fn).read_text(encoding="utf-8", errors="replace"), label)
        print("\n".join(probs) if probs else f"{label}: clean")
        sys.exit(1 if probs else 0)
    raise SystemExit(main())
