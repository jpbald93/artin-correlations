# Round-5 adversarial review of the round-4 fix pass (ee91f8f..8473712), opus55

**Verdict: MINOR.** The prose edits are correct and scoped, and every number is preserved. Three problems remain, none in a theorem or a number: one new sentence in DISC overstates its denominator, the assembly fallback can silently ship stale TeX, and the blind scrub leaves series provenance in the supplement and damages some prose.

**Hash note (history rewrite):** 8473712 → 02d2318 and ee91f8f → 101e7bb. The tree objects match: `git rev-parse <c>^{tree}` gives 560ab79f… for 8473712 and 02d2318, and 1e4a3260… for ee91f8f and 101e7bb. `git diff --stat 101e7bb 02d2318` is the same (21 files, +1538/−170). All findings apply unchanged to the new hashes.

Workspace `consolidated/` is byte-identical to the repo at 8473712 for all audited files (`cmp`: DISC, FRONT, SECTION_content, artin_correlations.tex, assemble.py, anonymise.py, scrub_blind.py, build_submission.sh, README.md, section15.py → "same"). All checks ran in `/tmp/r5`.

## Findings

### F1 MINOR: DISC puts the wrong denominator on the cross-base and ω measurements
New text (DISC.tex, merged line 1653):
> "(ii) a measurement, over the $50{,}847{,}531$ primes below $10^9$, of the consecutive-prime Artin correlation, its gap profile, and the $\omega$ repulsion, together with the cross-base correlations $\phi(a,b)$ and their joint-density comparison"

The paper's own counts under that one phrase:
```
$ grep -n "50{,}847{,}53\|50{,}847{,}52" paper/artin_correlations.tex
59:  ... over $50{,}847{,}530$ pairs)
187: $50{,}847{,}530$ consecutive pairs up to $10^9$
424: $50{,}847{,}524$ primes below $10^9$ for all $66$ pairs of our base set
624: $50{,}847{,}532$ primes, and $p < 31$ ... leaving $50{,}847{,}524$
931: (50{,}847{,}530 \text{ pairs}, \; p \le 10^9)      <- the omega repulsion
1166: $x = 10^9$ over $50{,}847{,}524$ primes.
```
- The number 50,847,531 is the size of the consecutive census (p ≥ 7). The correlation, the gap profile and ω are measured over **50,847,530 pairs**.
- φ(a,b) is measured over 50,847,532 primes (p ≥ 5), and the joint-density and signature runs over 50,847,524 primes (p ≥ 31).

So the sentence is exact only if read as "the census of 50,847,531 primes", and it is wrong for everything after "together with". The abstract (line 51) states the same number correctly: "with $50{,}847{,}531$ of them, $p\ge7$, in the consecutive census". Suggested fix: "over all primes below $10^9$ (the consecutive census has $50{,}847{,}531$ primes with $p\ge7$)", or return to "at the scale of $10^9$".

The README has the same problem under its new header: "**Measurements below 10⁹** (50,847,531 primes, exact counts) … cross-base correlation φ(a,b) …". Only the header wording is new there.

### F2 MINOR: the assembly fallback silently ignores edited sources; the new README instruction fails on the public repo
`assemble.py` exits 0 and uses the shipped TeX whenever **either** source paper is missing (`if not (P1.is_file() and P3.is_file())`). It never checks FRONT/SECTION_content/DISC against the shipped TeX. So a user of the source zip who edits FRONT.tex gets a clean build that does not contain the edit:
```
$ cd /tmp/r5/nosrc && echo "% EDITED" >> FRONT.tex && python3 assemble.py; echo rc=$?; grep -c EDITED artin_correlations.tex
assemble: source papers absent; using shipped artin_correlations.tex
rc=0
0
```
The source zip ships FRONT.tex, SECTION_content.tex and DISC.tex, but the build never reads them.

The reverse direction is fine. When both source papers are present, regeneration is byte-identical to the shipped TeX:
```
$ python3 assemble.py | tail -1; cmp shipped.tex artin_correlations.tex && echo IDENTICAL; cmp shipped.tex <repo>/paper/artin_correlations.tex && echo IDENT_REPO
  sha256(tex): 521e63deadd0e1e3
IDENTICAL
IDENT_REPO
```

The new README line "Run `bash build_submission.sh` for the full submission build" fails in a clone of the public repo, because the TeX sits in `paper/` and the script expects it at the root:
```
$ git clone … && git checkout 8473712 && bash build_submission.sh --skip-clean-extraction | head -2
== optional assembly
assemble: source papers and shipped manuscript are missing        (exit 1)
```
Suggested fixes:
- In fallback mode, rebuild from FRONT/SECTION/DISC plus the cached core blocks, or at least fail when those files differ from the shipped TeX (for example, check a hash of each against its span).
- State in the README that the build runs from the source zip, not from a repo checkout.

### F3 MINOR: the blind supplement still carries series provenance and a workspace path, and the scrub damages prose
The build reports `blind supplement: 74 files, audit=clean`, but the shipped zip contains:
```
results/pilot_log.txt:99: Saved JSON: Prime Math/the source study Full file/results/pilot_results.json   <- workspace dir layout survives
lean/Artin/Paper2.lean, Paper2Rebuild.lean; `namespace Paper2`; Artin.lean/Check.lean: `import Artin.Paper2`, `#print axioms Paper2.*`
results/delta_1e10…md:79, delta_1e11…md:80, independent_audit…md:85: `/tmp/p1_audit/`
results/independent_audit…md:29: "this VM (2-core Xeon E5-2673 v4) | … AMD Ryzen AI MAX+ 395, 32 cores"
```
- `Paper2` tells a reviewer that the arbitrary-base law (the manuscript's `\cite{Withheld}`) is paper 2 of the author's own series. That undoes the point of withholding that reference.
- The substitution `\bPaper\s+[123]\b` → "the source study" produces broken text: "**The the source study theorem holds at 10¹⁰**" (delta_1e10 line 36), "Independent recomputation of the the source study census" (independent_audit line 1), and "Prime Math/the source study Full file".

I tested the audit (`anonymise.audit`) against likely variants. It misses:
- `Paper~3`, `paper III`, `Paper2`, `Paper-1`
- line-broken or hyphenated titles: `Cross-\nbase correlations of Artin`, `Correla-\ntions between primitive root statuses`
- titles with markup inside: `Correlations between \emph{primitive} root statuses`
- `zenodo 22863946` with a space
- `J.~B.`, `NucBox`, `Tailscale 100.72.132.55`, `Toronto`, `by the present author`, `our earlier paper`

It does catch upper-case titles, `github.com/jpbald93`, `doi:10.5281/zenodo.*`, `/home/work` and `the  author`.

I checked the shipped blind artifacts for these variants: no own title and no name, email or ORCID survives in blind_manuscript.pdf/.tex or in the zip. The actual leaks are the ones listed at the top of this finding.

### F4 NIT: the new assemble.py comment cites a gate that does not exist
The new comment reads "Wording-only cleanup required by the submission phrase gate". There is no phrase gate in the build:
```
$ grep -rln -i phrase --include=*.py --include=*.sh .
./assemble.py          (only the comment itself)
```
The docstring was not updated either. It still says "copied VERBATIM (byte-for-byte) … Only the section titles of two sections are edited", which the README's updated list of edits now contradicts.

### F5 NIT: the README dropped the pointer to the shipped Section 15 tables
The edit removed "(a gzipped copy is shipped as `results/joint_residue_tables_1e9.txt.gz`, sha256 in `results/section15_sha256.txt`)". The file is still shipped, and it is the fastest way to reproduce Section 15: `zcat` → section15.py gives a byte-identical JSON (see item 5). The README now offers only the full `dump 1e9` recompute.

### F6 NIT: "for the workloads tested" says nothing about the cross-base correlations
The abstract now reads "We conclude that, for the workloads tested, these correlations describe arithmetic structure rather than supply algorithms". "These correlations" includes φ(a,b). The paper says (merged line ~1630) "We have not evaluated a cross-base predictive workload at all". The scoping is therefore correct but empty for the cross-base axis, and the abstract does not say so. Suggested wording: "for the consecutive-prime workloads tested".

## Verdict per item

**1. Changed prose: PASS except F1 and F6.**
- "reproduces the excess … without predecessor-specific information" / "so reproducing it requires no predecessor-specific information": correct. The claim is existential (a residue-only model suffices, with 24/100 replicates ≥ observed). It no longer asserts that the predecessor carries zero information, so it claims neither more nor less than the null shows.
- "We leave this small in-sample excess unresolved": correct and consistent with 2/100 replicates and with no held-out gain. The later sentence "shows no predictive value beyond … mod 840" is scoped to "the estimators above" (held-out), so the two do not contradict.
- Caption "avoiding a smoothing penalty when that constancy persists": true. Code line 122 skips the split when `(f01+f11)==0 or (f00+f10)==0`, i.e. the successor is constant in the fit half. The old "can only add" was false; the new wording is conditional and correct.
- "The quadratic filter expressed by the exclusion laws … already applied by a character-aware implementation" and the new subsection title: consistent. No "only filter" claim remains anywhere:
  ```
  $ grep -n -iE "only usable|genuinely usable|only filter|does real work|one usable" paper/artin_correlations.tex README.md
  (none)
  ```
  So the filter table, where the small-ℓ early exit gives about 13% more, no longer contradicts the text. The subsection title undersells the early-exit half of that subsection, but nothing in it is false.
- DISC 50,847,531: F1.

**2. "either way" → "in both cases": PASS.** Meaning is unchanged. In context (merged line 398): "of fundamental discriminant d if d≡1 (mod 4) and 4d otherwise; in both cases the discriminant divides 4d". The comment attached to the edit is F4.

**3. assemble.py fallback: MINOR (F2).**
- With both source papers present: regeneration is byte-identical to the shipped TeX (sha256 prefix 521e63deadd0e1e3).
- With either paper absent: a stale merged TeX ships silently, even when FRONT, SECTION_content or DISC have been edited.
- A clone of the public repo cannot build at all.

**4. Scrub and audit: MINOR (F3).** The scrub does not break anything functional:
- `py_compile` passes on all code/*.py.
- `gcc -fsyntax-only -fopenmp` passes on all code/*.c.
- Every *.json loads.
- The scrubbed Lean archive builds (`Build completed successfully (3015 jobs)` / `PASS (28 theorems, standard axioms only)`), though only because the shared Mathlib cache is present on this machine.
- `results/joint_residue_tables_1e9.txt` in the blind zip matches sha256 `c815e158…`.
- blind_manuscript.pdf: Author, Title and Creator metadata empty; 28 pp (same as named); no own title, name, DOI or repo link in the pdftotext output.
- Known damage: `dataset_sha256.txt` paths are replaced, so `sha256sum -c` on it no longer works, plus the prose damage in F3.

**5. Numerals: PASS.**
```
$ python3 (Counter of numerals, ee91f8f vs 8473712 merged TeX)
added Counter({'50{,}847{,}531': 1})
removed Counter()
$ zcat results/joint_residue_tables_1e9.txt.gz > jrt.txt; python3 code/section15.py jrt.txt > s15.json; cmp s15.json results/section15_1e9.json && echo S15_IDENTICAL
rc=0
S15_IDENTICAL
```
The added line in code/results.txt (k=256 / k=1,599) matches merged line 1485.

**6. Clean-extraction build: PASS, EXIT=0.**
```
$ cd /tmp/r5/clean && unzip -q …/submission/artin_correlations_source.zip && env -i PATH=/usr/local/bin:/usr/bin:/bin HOME=$HOME bash build_submission.sh; echo EXIT=$?
== optional assembly / assemble: source papers absent; using shipped artin_correlations.tex
   artin_correlations pages=28 errors=0 overfull_hbox=0 overfull_vbox=0 undefined=0
   blind_manuscript pages=28 errors=0 overfull_hbox=0 overfull_vbox=0 undefined=0
   blind pages=28 (named 28) sections=17 audit=clean
scrub: 17 text files redacted; … blind supplement: 74 files, audit=clean
Build completed successfully (3015 jobs). PASS (28 theorems, standard axioms only)
   clean-extraction full build: EXIT=0
EXIT=0
```
- The workspace zip and the repo zip differ in bytes (timestamps) but extract to identical content (`diff -r` gives no output).
- The Lean stage uses the hard-coded cache `/home/work/Projects/artin-lean/...`. On any other machine it prints SKIP rather than failing, so the scrubbed Lean build is checked only on this machine.

## Could not falsify
- Numeral preservation: the only change is the added 50,847,531.
- Regenerating from the sources gives TeX byte-identical to the shipped TeX.
- section15.py output is byte-identical to results/section15_1e9.json.
- The clean /tmp build exits 0 with no environment variables set.
- The new sufficiency wording ("reproduces … without predecessor-specific information", "reproduced by a residue-only model") is correct.
- The "unresolved" wording for the mod-840 excess is correct.
- The estimator-B caption matches the code.
- The "either way" → "in both cases" edit leaves the meaning unchanged.
- No "only usable filter" claim survives, so the filter table no longer contradicts the text.
- The blind PDF text and metadata contain no name, ORCID, email, own DOI, own title or repo link.
- The scrub leaves the Python, C, JSON and Lean builds working.
