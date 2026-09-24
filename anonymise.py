#!/usr/bin/env python3
"""Produce the blind (double-anonymous) manuscript from the named one.

Structural removals, each ASSERTED to have happened (the script exits non-zero if a target
is missing, so a source change cannot silently defeat it):
  1. \\author, \\address, \\email, \\thanks            -> removed
  2. the provenance footnote naming the author's own preprints and their DOIs -> removed
  3. the Data-availability repository URL             -> "supplied with the submission"
  4. the self-citation \\bibitem{BaldII}               -> "[reference withheld for review]"
     (the citing sentence is kept, so the argument is unchanged)
  5. acknowledgements, requests, responsibility and interest -> neutral wording

Then audit known author identifiers, own-title fragments, provenance tokens, DOI strings
and URLs. Only the two cited third-party DOIs and Lean dependency repository URLs are
allowlisted. This is a mechanical leakage check, not a guarantee against identification
from public scientific content.
"""
import re
import sys
import pathlib

# identifiers of the author that must not survive anywhere
IDENTIFIERS = [r"Bald", r"\bJosh(ua)?\b", r"jpbald93", r"0009-0002-1317-6489",
               r"@gmail\.com", r"@genspark", r"Independent Researcher", r"Ontario",
               r"\bPaper[\s~-]*(?:[123]|I{1,3})\b", r"GMKtec", r"NucBox", r"Tailscale",
               r"100\.72\.132\.55", r"192\.168\.2\.61", r"Toronto", r"present author",
               r"our (?:earlier|previous|prior) (?:paper|preprint|work)", r"J\.\s*~?\s*B\.",
               r"qwen3", r"\bjack\b", r"OpenClaw",
               r"/?home/work", r"genspark", r"artin[_-]correlations", r"\bthe\s+author\b"]
OWN_TITLES = [r"Correlations\s+between\s+primitive\s+root\s+statuses",
              r"Cross[ -]base\s+correlations\s+of\s+Artin",
              r"(?:Quadratic\s+)?Exclusion\s+laws\s+for\s+consecutive\s+Artin\s+primes"]
THIRD_PARTY_DOIS = {"10.1016/j.jnt.2022.10.006", "10.1515/integers-2012-0043"}
DEPENDENCY_URL = re.compile(r"https://github\.com/(?:leanprover|leanprover-community)/[A-Za-z0-9_.-]+(?:\.git)?/?$")
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
    tex = must_sub(r"The author takes responsibility", "We take responsibility", tex, count=1,
                   what="responsibility statement")
    tex = must_sub(r"No potential conflict of interest was reported by the author\.",
                   "No potential conflict of interest is reported.", tex, count=1,
                   what="interest declaration")
    return tex


def audit(text, label):
    """Return a list of problems found in `text` (TeX or extracted PDF text)."""
    problems = []
    for pat in IDENTIFIERS + OWN_TITLES:
        for m in re.finditer(pat, text, flags=re.I):
            problems.append(f"{label}: identifier {m.group(0)!r}")
    # normalised copy: drop TeX commands and braces, join hyphenated line breaks, squeeze
    # whitespace, lower-case -- titles are searched in this form as well
    norm = re.sub(r"-\s*\n\s*", "", text)
    norm = re.sub(r"\\[A-Za-z]+\*?", " ", norm).replace("{", " ").replace("}", " ").replace("~", " ")
    norm = re.sub(r"\s+", " ", norm).lower()
    for pat in OWN_TITLES:
        for m in re.finditer(pat.replace("Cross[ -]base", "cross[ -]?base"), norm, flags=re.I):
            problems.append(f"{label}: own title (normalised) {m.group(0)!r}")
    squeezed = norm.replace(" ", "").replace("-", "")
    for t in ("correlationsbetweenprimitiverootstatuses", "crossbasecorrelationsofartin",
              "exclusionlawsforconsecutiveartinprimes"):
        if t in squeezed:
            problems.append(f"{label}: own title (squeezed) {t!r}")
    flat = re.sub(r"\s+", "", text)
    for m in re.finditer(r"zenodo\W{0,3}\d{6,}", text, flags=re.I):
        problems.append(f"{label}: Zenodo record {m.group(0)!r}")
    for doi in OWN_DOIS:
        if doi in flat:
            problems.append(f"{label}: author's own DOI {doi}")
    for m in re.finditer(r"zenodo\.\d{6,}", flat, flags=re.I):
        problems.append(f"{label}: Zenodo record {m.group(0)} (any Zenodo DOI is treated as provenance)")
    for m in re.finditer(r"10\.\d{4,9}/[A-Za-z0-9._;()/:+-]+", text):
        doi = m.group(0).rstrip(".;,)")
        if doi not in THIRD_PARTY_DOIS:
            problems.append(f"{label}: non-allowlisted DOI {doi}")
    for m in re.finditer(r"https?://[^\s{}<>\\\"`]+", text):
        url = m.group(0).rstrip(".;,)")
        if not DEPENDENCY_URL.fullmatch(url) and url not in {"https://doi.org/" + d for d in THIRD_PARTY_DOIS}:
            problems.append(f"{label}: non-allowlisted URL {url}")
    for m in re.finditer(r"github\.com/[A-Za-z0-9_.\-/]+", text):
        if not DEPENDENCY_URL.fullmatch("https://" + m.group(0)):
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
