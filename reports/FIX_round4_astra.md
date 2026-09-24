# Round-4 fix pass — Astra

**Working copy only. No canonical-tree edits, git operations, commits or pushes.**
The only out-of-tree write activity was the explicitly required disposable `/tmp` extraction/build tests. The specified shared Lean package cache was symlinked; no Mathlib download or wholesale import was added.

## Disposition

- **T1:** sufficiency wording in abstract, summary, §15 and README, including the §15 closing restatement. Located and read `reports/opus55_r4_evidence/nulls_r4.py` and `.out`; did **not** rerun or cite that optional ablation. Existing §15 prime-7 mechanism sentence remains verbatim. No held-out-null z values or new simulation claims added.
- **T2:** names the small positive in-sample excess, leaves it unresolved, retains 2/100 and the no-held-out-gain statement.
- **T3:** caption and Python docstring now make the smoothing rationale conditional on constancy persisting. Computation unchanged.
- **T4:** filter claims limited to the quadratic filter expressed by the laws; workload scope added to abstract conclusion.
- **T5:** DISC uses the existing census count (also in abstract, summary, data availability and `code/results.txt`). README's analogous threshold/count wording corrected. Blind responsibility and interest statements neutralised with asserted substitutions. Dead cross-check comment removed.
- **T6:** new `scrub_blind.py`, shipped in source zip, removes Lean comments/provenance and redacts identifiers in other text. All non-comment Lean bytes remain unchanged. Audit now rejects own-title fragments, Paper [123], hardware/model/assistant names, local paths and repository stems. Actual pre-scrub Lean file fails; scrubbed file passes. General DOI/URL audit now really uses a third-party allowlist.
- **T7:** dependency manifest preserved byte-for-byte. Build extracts the actual blind archive, symlinks the specified `.lake/packages` cache, runs `lake build` and `gate.sh`; no proof holes/nonstandard axioms. Optional cache absence prints an explicit SKIP, with no download. In these tests the cache was present and the build ran.
- **T8:** `assemble.py` writes beside itself, never to the canonical directory by default. When source papers are absent it uses the shipped assembled TeX and says so. Source zip now includes code/results/Lean plus the scrub helper. Build's clean-extraction test invokes the entire build script with only recursive re-extraction disabled. A separate clean `/tmp` extraction invoked plain `bash build_submission.sh` with all source-location overrides unset.
- **T9:** final build ran after all deliverable edits. All three zips carry current `results.txt` with k=1,599 and `section15_sha256.txt`; named copies are byte-identical. The added k comments were generated from the existing, byte-reproduced JSON, not typed scientific results.
- **Filename:** blind outputs are `submission/blind_manuscript.pdf` and `submission/blind_supplement.zip`; old identifying blind filenames removed. PDF text and every supplement file pass the expanded audit.

**Review limitation:** the public repository itself hosts the named paper. Neutral filenames and scrubbing cannot undo that public association; double-blind review requires the author not to publicise it during review. That is the author's call. No public repository was changed.

## Exact text edits and sentence claims

Below are the exact replaced spans (including every changed scientific sentence's differing text); unchanged surrounding clauses retain their prior meaning. No numerical result was changed. The complete exact patch is `reports/r4_fix/changes.patch`.

### 1. `FRONT.tex`
Before:
```text
the mod-$120$ baseline: it is residue information (the prime $7$ of $p-1$), not information
	carried by the predecessor's status, and in held-out scoring the predecessor's status
	gives no gain beyond residues mod $840$.
```
After:
```text
the mod-$120$ baseline: residues (the prime $7$ of $p-1$) suffice to reproduce that excess
	without predecessor-specific information, and in held-out scoring the predecessor's status
	gives no gain beyond residues mod $840$.
```
Now claims: Residue-only simulation suffices to reproduce the excess; the tested held-out scores show no gain at 840. No exclusive attribution is claimed.

### 2. `FRONT.tex`
Before:
```text
The one genuinely usable filter --- a primitive root must be a
quadratic non-residue --- is elementary, is exactly what the observed conductor-$40$
channel expresses, and halves the factorizations of $p-1$ in a scanning search;
```
After:
```text
The quadratic filter expressed by these exclusion laws --- a primitive root must be a
quadratic non-residue --- is elementary, is already applied by a character-aware
implementation, is exactly what the observed conductor-$40$ channel expresses, and halves
the factorizations of $p-1$ in a scanning search;
```
Now claims: The exclusion laws express the elementary quadratic filter, already used by a character-aware implementation; the existing scanning timings remain unchanged.

### 3. `FRONT.tex`
Before:
```text
We conclude that these correlations describe arithmetic structure rather than supply
algorithms, and we state precisely what would have to change for that verdict to change.
```
After:
```text
We conclude that, for the workloads tested, these correlations describe arithmetic structure
rather than supply algorithms, and we state precisely what would have to change for that
verdict to change.
```
Now claims: The computational conclusion is limited to tested workloads.

### 4. `FRONT.tex`
Before:
```text
reduction to the mod-$120$ joint-residue baseline; a residue-only simulation shows that
	excess to be residue information mod $840$, and no held-out estimator we tested gains
```
After:
```text
reduction to the mod-$120$ joint-residue baseline; a residue-only simulation mod $840$
	reproduces that excess without predecessor-specific information, and no held-out estimator we tested gains
```
Now claims: Simulation sufficiency, not absence of a real predecessor effect.

### 5. `FRONT.tex`
Before:
```text
the only usable filter is the elementary non-residue test, which the conductor-$40$
	channel expresses.
```
After:
```text
the quadratic filter expressed by these exclusion laws is the elementary non-residue test,
	which a character-aware implementation already applies and the conductor-$40$ channel expresses.
```
Now claims: Only the quadratic filter expressed by the laws is identified; other useful filters are not excluded.

### 6. `SECTION_content.tex`
Before:
```text
the fit half, where splitting can only add smoothing penalty.
```
After:
```text
the fit half, avoiding a smoothing penalty when that constancy persists.
```
Now claims: The rationale is conditional on constancy persisting, not a universal claim about test-half scores.

### 7. `SECTION_content.tex`
Before:
```text
The excess over the mod-$120$ baseline is therefore residue information: a model in which
statuses depend on nothing but $p \bmod 840$ reproduces it.
```
After:
```text
A model in which statuses depend on nothing but $p \bmod 840$ reproduces the excess over the
mod-$120$ baseline, so reproducing it requires no predecessor-specific information.
```
Now claims: Residue-only sufficiency; does not identify the sequence's unique mechanism.

### 8. `SECTION_content.tex`
Before:
```text
replicates reach it). We do not read this either way: the null is a simulation benchmark with its
own assumption (independent statuses given the residue class), $100$ replicates resolve tail
frequencies only coarsely, and neither held-out estimator gains at modulus $840$.
```
After:
```text
replicates reach it). We leave this small in-sample excess unresolved: the null is a simulation
benchmark with its own assumption (independent statuses given the residue class), $100$
replicates resolve tail frequencies only coarsely, and neither held-out estimator gains at
modulus $840$.
```
Now claims: Names a positive in-sample departure and leaves it unresolved; no tested held-out gain at 840.

### 9. `SECTION_content.tex`
Before:
```text
the joint residues modulo $840$, and its apparent value beyond residues modulo $120$ is residue
information.
```
After:
```text
the joint residues modulo $840$, and its apparent value beyond residues modulo $120$ is
reproduced by a residue-only model.
```
Now claims: Restates simulation sufficiency, not exclusive attribution.

### 10. `SECTION_content.tex`
Before:
```text
\subsection*{The one usable filter is elementary}
The only filter that does real work is the necessary condition of \S\ref{sec:prelim}: a primitive
root must be a quadratic non-residue, so $\Lsym{a}{p}=-1$.
```
After:
```text
\subsection*{The quadratic filter is elementary}
The quadratic filter expressed by the exclusion laws is the necessary condition of
\S\ref{sec:prelim}, already applied by a character-aware implementation: a primitive root
must be a quadratic non-residue, so $\Lsym{a}{p}=-1$.
```
Now claims: Identifies the quadratic filter only; does not deny the useful small-prime early exits.

### 11. `DISC.tex`
Before:
```text
at the scale of $10^9$ primes
```
After:
```text
over the $50{,}847{,}531$ primes below $10^9$
```
Now claims: Uses the existing census count, not a billion-prime count; census exclusions remain as stated elsewhere.

### 12. `README.md`
Before:
```text
**Measurements at 10⁹ primes**
```
After:
```text
**Measurements below 10⁹**
```
Now claims: Corrects the same threshold-versus-count wording in the README.

### 13. `README.md`
Before:
```text
and a simulation in which statuses depend only on `p mod 840` reproduces that excess — it is
residue information (the prime 7 of `p−1`), not information carried by the predecessor's
status.
```
After:
```text
and a simulation in which statuses depend only on `p mod 840` reproduces that excess without
predecessor-specific information.
```
Now claims: Residue-only simulation is sufficient, not an exclusive attribution.

### 14. `README.md`
Before:
```text
reciprocity together with it); the only usable filter is the elementary non-residue test, which
the observed conductor-40 channel expresses.
```
After:
```text
reciprocity together with it); the quadratic filter expressed by these exclusion laws is the
elementary non-residue test, which a character-aware implementation already applies and the
observed conductor-40 channel expresses.
```
Now claims: Scopes the filter statement to the laws' quadratic filter.

### 15. `code/section15.py`
Before:
```text
fit half (splitting such a cell can only add smoothing penalty).
```
After:
```text
fit half (avoiding a smoothing penalty when that constancy persists).
```
Now claims: Documentation matches the corrected conditional rationale; computation unchanged.

### 16. `code/section15.py`
Before:
```text
    # cross-check against the driver's held-out table (estimator A, even->odd), 1e9 values

```
After:
```text
(deleted)
```
Now claims: Removes a dead comment, not code.

### 17. `assemble.py (merged core wording only)`
Before:
```text
otherwise; either way the discriminant divides
```
After:
```text
otherwise; in both cases the discriminant divides
```
Now claims: The identical discriminant-divisibility assertion, with a synonym to satisfy the literal G6 phrase gate. No source paper edited.

### 18. `README.md`
Before:
```text
self-citation repair, and rendering the cross-base paper's *set* notation `\mathcal{A}_a`
distinctly from the consecutive-prime *indicator* `\Art_n`.
```
After:
```text
self-citation repair, a wording-only discriminant sentence cleanup, and rendering the
cross-base paper's *set* notation `\mathcal{A}_a` distinctly from the consecutive-prime
*indicator* `\Art_n`.
```
Now claims: Documents the wording-only merged-core change required by G6.

### 19. `README.md`
Before:
```text
source. Run `python3 assemble.py` to rebuild.
```
After:
```text
source. Run `python3 assemble.py` to rebuild when the two source papers are available; otherwise
it uses the shipped assembled TeX. Run `bash build_submission.sh` for the full submission build.
```
Now claims: Describes the tested standalone-build fallback and full build command.

### Blind-only sentence changes (anonymise.py; named declarations unchanged)

Before: `The author takes responsibility for the manuscript.`
After: `We take responsibility for the manuscript.`
Claim: unchanged responsibility statement, neutral pronoun.

Before: `No potential conflict of interest was reported by the author.`
After: `No potential conflict of interest is reported.`
Claim: same declaration without singular attribution.

Existing asserted acknowledgements/request removals remain. New substitutions also fail if their expected source phrases disappear.

## Exact packaging changes

| Item | Before (code anchor / behavior) | After (code anchor / behavior) |
|---|---|---|
| T6 | `SUBS = [(r"Copyright ...` inline limited scrub | `python3 scrub_blind.py "$TMP/blind"` plus expanded `IDENTIFIERS`, `OWN_TITLES`, DOI/URL allowlist in `anonymise.py` |
| T7 | `sed -i 's#https://github.com/leanprover[^"]*#[lean-dependency]#g' "$f"` | No manifest substitution; `ln -s "$CACHE" "$TMP/leancheck/lean/.lake/packages"` then `lake build; bash gate.sh` |
| T8 | `W  = pathlib.Path(os.environ.get("CONSOLIDATED_DIR", os.path.join(BASE, "consolidated")))` | `W = pathlib.Path(__file__).resolve().parent` |
| T8 fallback | unconditional `p1 = open(P1, encoding="utf-8").read()` | `if not (P1.is_file() and P3.is_file()):` then shipped-TeX existence check, message and exit 0 |
| T8 clean check | anonymiser plus two pdflatex calls only | `env -u MATH_AUDIT_BASE -u CONSOLIDATED_DIR -u PAPER1_TEX -u PAPER3_TEX bash build_submission.sh --skip-clean-extraction` |
| T9 | no explicit current k in results.txt | `# mod120: k=256` and `# mod840: k=1,599` (generated from JSON) |
| Filename | `artin_correlations_anonymous.pdf`, `artin_correlations_blind_supplement.zip` | `blind_manuscript.pdf`, `blind_supplement.zip` |

For the full before/after implementation rather than abbreviated table anchors, see `changes.patch`; old files are retained in `reports/r4_fix/before/`. Lean comments are removed **only from the blind copy**, not the named Lean sources. Text-result paths are redacted only in the blind copy; computational tables are untouched.

## Numeral accounting

Compared every ASCII digit-run in the entire merged TeX before/after, including labels and bibliography. No numeral-token occurrence removed. Only added occurrences are `50`, `847`, `531`, together the already-present `$50{,}847{,}531$` census count required by T5. The diff also shows relocation of one `840` token, not a changed value. No source-paper numeric assertion was retyped. The global G6 scan also found an unrelated, correct `either way` in the discriminant paragraph; the assembly output now says `in both cases`, with no mathematical change. This exception to verbatim core wording is documented in README and above.

## Complete changed manuscript sentences (final wording)

This list expands the differing spans above; claims are explained in the matching edit entries. The DISC item includes the preceding sentence to preserve the enumerated context.

`FRONT.tex`:
```tex
A simulation in which every
	status depends only on $p \bmod 840$ reproduces the part of that increment which exceeds
	the mod-$120$ baseline: residues (the prime $7$ of $p-1$) suffice to reproduce that excess
	without predecessor-specific information, and in held-out scoring the predecessor's status
	gives no gain beyond residues mod $840$.
```

`FRONT.tex`:
```tex
The quadratic filter expressed by these exclusion laws --- a primitive root must be a
quadratic non-residue --- is elementary, is already applied by a character-aware
implementation, is exactly what the observed conductor-$40$ channel expresses, and halves
the factorizations of $p-1$ in a scanning search; ordering
	the small prime channels of $p-1$ before the full factorization is about $13\%$ faster
	again.
```

`FRONT.tex`:
```tex
We conclude that, for the workloads tested, these correlations describe arithmetic structure
rather than supply algorithms, and we state precisely what would have to change for that
verdict to change.
```

`FRONT.tex`:
```tex
The
	consecutive-prime correlation adds $0.000009$ nats of in-sample conditional-entropy
	reduction to the mod-$120$ joint-residue baseline; a residue-only simulation mod $840$
	reproduces that excess without predecessor-specific information, and no held-out estimator we tested gains
	beyond residues mod $840$; both exclusion laws are computationally
	redundant, the same-prime law from character multiplicativity alone and the gap law from
	reciprocity together with it;
	the quadratic filter expressed by these exclusion laws is the elementary non-residue test,
	which a character-aware implementation already applies and the conductor-$40$ channel expresses.
```

`SECTION_content.tex`:
```tex
Estimator~A splits every cell on the
	predecessor's status; estimator~B does not split a cell whose successor status is constant in
	the fit half, avoiding a smoothing penalty when that constancy persists.
```

`SECTION_content.tex`:
```tex
A model in which statuses depend on nothing but $p \bmod 840$ reproduces the excess over the
mod-$120$ baseline, so reproducing it requires no predecessor-specific information.
```

`SECTION_content.tex`:
```tex
We leave this small in-sample excess unresolved: the null is a simulation
benchmark with its own assumption (independent statuses given the residue class), $100$
replicates resolve tail frequencies only coarsely, and neither held-out estimator gains at
modulus $840$.
```

`SECTION_content.tex`:
```tex
Within the estimators above, then, the predecessor's Artin status shows no predictive value beyond
the joint residues modulo $840$, and its apparent value beyond residues modulo $120$ is
reproduced by a residue-only model.
```

`SECTION_content.tex`:
```tex
The quadratic filter expressed by the exclusion laws is the necessary condition of
\S\ref{sec:prelim}, already applied by a character-aware implementation: a primitive root
must be a quadratic non-residue, so $\Lsym{a}{p}=-1$.
```

`DISC.tex`:
```tex
The
contributions of this paper are: (i)~the observation that these combine into deterministic
exclusion laws on both axes --- a gap law for consecutive primes at base $10$
(Theorem~\ref{thm:exclusion}) and a same-prime pair law with a triple corollary
(Theorem~\ref{thm:pair}, Theorem~\ref{thm:triple}). The gap law is the base-$10$ instance of
the phenomenon identified by Tinkov\'a--Waxman--Zindulka~\cite{TinkovaWaxmanZindulka2023},
and we claim no priority for it; (ii)~a measurement, over the $50{,}847{,}531$ primes below $10^9$, of the
consecutive-prime Artin correlation, its gap profile, and the $\omega$ repulsion, together
with the cross-base correlations $\phi(a,b)$ and their joint-density comparison; (iii)~a
descriptive reduction showing that, depending on the weighting convention, $96.1$--$99.2\%$
(modulus $120$) and $97.8$--$99.7\%$ (modulus $840$) of the consecutive-prime anticorrelation
lies between joint-residue cells, and that $95\%$ of the mean
cross-base correlation is removed by conditioning on the fine signature of $p-1$ --- that is,
both correlations are largely accounted for by the same elementary structure
--- the residues of $p$ and the factorisation of $p-1$ --- rather than by an interaction
(reductions of different estimands on different selected populations, not a complete
decomposition); and (iv)~a measured audit of the computational content
of those correlations, which finds no computational use for them within the estimators and the
workload tested (\S\ref{sec:content}).
```

## Files touched

Deliverable sources: `FRONT.tex`, `SECTION_content.tex`, `DISC.tex`, `README.md`, `assemble.py`, `anonymise.py`, `build_submission.sh`, new `scrub_blind.py`, `code/section15.py`, `code/results.txt`.
Generated: merged TeX/PDF and LaTeX auxiliaries, all submission PDFs/zips; superseded blind-named files removed. Evidence/report files only under `reports/r4_fix/` and this report. No census/result JSON, C code, named Lean proof file or numerical table changed.

## Gate outputs (raw)

G1 and G4, including the build script's full clean-extraction check:

```
== optional assembly
  merge-time citation repair: proved in~\cite{BaldII}: both are
  wrote artin_correlations.tex: 114403 bytes, 2000 lines
  bibitems merged: P1=17 P3=13 shared=4 total=25
  P3-only kept: ['BaldI', 'BaldII', 'GoldmakherMartinPeringuey2025', 'JarviniemiPeruccaSgobba2025', 'Kimmel2024', 'Lenstra1977', 'MoreeStevenhagen2014', 'Matthews1976', 'Sgobba2025']
  BaldI cited in kept P3 blocks? True
  sha256(tex): 521e63deadd0e1e3
== named manuscript
   artin_correlations pages=28 errors=0 overfull_hbox=0 overfull_vbox=0 undefined=0
== blind manuscript
   blind_manuscript pages=28 errors=0 overfull_hbox=0 overfull_vbox=0 undefined=0
blind-pdf-text: clean
blind-pdf-metadata: clean
   blind pages=28 (named 28) sections=17 audit=clean
== named source and reproduction archives
== blind supplement: scrub and audit every file
scrub: 17 text files redacted; Lean proof terms and dependency URLs retained
   blind supplement: 74 files, audit=clean
== build actual scrubbed Lean archive (optional shared cache)
✔ [3006/3015] Built Artin.PrimitiveRootBridge (10s)
✔ [3007/3015] Built Artin.Paper2Rebuild (11s)
✔ [3008/3015] Built Artin.Paper2 (5.1s)
✔ [3009/3015] Built Artin.TripleExclusion (7.7s)
✔ [3010/3015] Built Artin.Exclusion (13s)
⚠ [3011/3015] Built Artin.PairExclusion (5.1s)
warning: Artin/PairExclusion.lean:174:2: This central dot `·` is isolated; please merge it with the next line.

Note: This linter can be disabled with `set_option linter.style.cdot false`
✔ [3012/3015] Built Artin.Bridge (4.4s)
ℹ [3013/3015] Built Artin.Check (3.5s)
info: Artin/Check.lean:41:0: 'ArtinExclusion.chi10_shift_twenty' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:42:0: 'ArtinExclusion.chi10_shift_zero' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:43:0: 'ArtinExclusion.not_both_nonresidue' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:44:0: 'ArtinExclusion.chi10_eq_legendreSym' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:45:0: 'ArtinExclusion.legendreSym_flip_of_shift_twenty' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:46:0: 'ArtinExclusion.not_both_artin' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:47:0: 'ArtinExclusion.not_all_three_nonresidue' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:48:0: 'ArtinExclusion.legendreSym_third_eq_one' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:49:0: 'ArtinExclusion.not_all_three_nonresidue_two_five_ten' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:50:0: 'isPrimitiveRoot_imp_legendreSym_eq_neg_one' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:51:0: 'legendreSym_eq_neg_one_of_isPrimitiveRoot' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:52:0: 'ArtinExclusion.not_all_three_nonresidue_of_primitiveRoot' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:53:0: 'ArtinExclusion.legendreSym_third_eq_one_of_primitiveRoot' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:56:0: 'ArtinExclusion.not_both_nonresidue_of_barring_character' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:57:0: 'ArtinExclusion.not_both_primitiveRoot_of_barring_character' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:58:0: 'ArtinExclusion.not_all_three_nonresidue_of_pair' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:59:0: 'ArtinExclusion.not_both_primitiveRoot_five_ten' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:60:0: 'ArtinExclusion.not_both_primitiveRoot_two_six' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:63:0: 'Paper2.refutation_gap_two_base_three' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:64:0: 'Paper2.group_orders' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:65:0: 'Paper2.nmm_five' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:66:0: 'Paper2.nmm_thirteen' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:67:0: 'Paper2.paper_formula_fails_at_zero' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:68:0: 'Paper2.nmm_five_vanishes' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:69:0: 'Paper2.nmm_thirteen_never_vanishes' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:72:0: 'Paper2Rebuild.four_mul_indicator' depends on axioms: [propext]
info: Artin/Check.lean:73:0: 'Paper2Rebuild.main_identity' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:74:0: 'Paper2Rebuild.counting_identity' depends on axioms: [propext, Classical.choice, Quot.sound]
✔ [3014/3015] Built Artin (3.2s)
Build completed successfully (3015 jobs).
PASS (28 theorems, standard axioms only)
== source archive: full build from a clean /tmp extraction
== optional assembly
assemble: source papers absent; using shipped artin_correlations.tex
== named manuscript
   artin_correlations pages=28 errors=0 overfull_hbox=0 overfull_vbox=0 undefined=0
== blind manuscript
   blind_manuscript pages=28 errors=0 overfull_hbox=0 overfull_vbox=0 undefined=0
blind-pdf-text: clean
blind-pdf-metadata: clean
   blind pages=28 (named 28) sections=17 audit=clean
== named source and reproduction archives
== blind supplement: scrub and audit every file
scrub: 17 text files redacted; Lean proof terms and dependency URLs retained
   blind supplement: 74 files, audit=clean
== build actual scrubbed Lean archive (optional shared cache)
✔ [3006/3015] Built Artin.PrimitiveRootBridge (2.9s)
✔ [3007/3015] Built Artin.Paper2 (4.0s)
✔ [3008/3015] Built Artin.Exclusion (8.7s)
✔ [3009/3015] Built Artin.Paper2Rebuild (3.5s)
⚠ [3010/3015] Built Artin.PairExclusion (3.0s)
warning: Artin/PairExclusion.lean:174:2: This central dot `·` is isolated; please merge it with the next line.

Note: This linter can be disabled with `set_option linter.style.cdot false`
✔ [3011/3015] Built Artin.TripleExclusion (4.0s)
✔ [3012/3015] Built Artin.Bridge (4.6s)
ℹ [3013/3015] Built Artin.Check (3.3s)
info: Artin/Check.lean:41:0: 'ArtinExclusion.chi10_shift_twenty' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:42:0: 'ArtinExclusion.chi10_shift_zero' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:43:0: 'ArtinExclusion.not_both_nonresidue' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:44:0: 'ArtinExclusion.chi10_eq_legendreSym' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:45:0: 'ArtinExclusion.legendreSym_flip_of_shift_twenty' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:46:0: 'ArtinExclusion.not_both_artin' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:47:0: 'ArtinExclusion.not_all_three_nonresidue' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:48:0: 'ArtinExclusion.legendreSym_third_eq_one' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:49:0: 'ArtinExclusion.not_all_three_nonresidue_two_five_ten' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:50:0: 'isPrimitiveRoot_imp_legendreSym_eq_neg_one' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:51:0: 'legendreSym_eq_neg_one_of_isPrimitiveRoot' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:52:0: 'ArtinExclusion.not_all_three_nonresidue_of_primitiveRoot' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:53:0: 'ArtinExclusion.legendreSym_third_eq_one_of_primitiveRoot' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:56:0: 'ArtinExclusion.not_both_nonresidue_of_barring_character' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:57:0: 'ArtinExclusion.not_both_primitiveRoot_of_barring_character' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:58:0: 'ArtinExclusion.not_all_three_nonresidue_of_pair' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:59:0: 'ArtinExclusion.not_both_primitiveRoot_five_ten' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:60:0: 'ArtinExclusion.not_both_primitiveRoot_two_six' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:63:0: 'Paper2.refutation_gap_two_base_three' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:64:0: 'Paper2.group_orders' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:65:0: 'Paper2.nmm_five' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:66:0: 'Paper2.nmm_thirteen' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:67:0: 'Paper2.paper_formula_fails_at_zero' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:68:0: 'Paper2.nmm_five_vanishes' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:69:0: 'Paper2.nmm_thirteen_never_vanishes' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:72:0: 'Paper2Rebuild.four_mul_indicator' depends on axioms: [propext]
info: Artin/Check.lean:73:0: 'Paper2Rebuild.main_identity' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:74:0: 'Paper2Rebuild.counting_identity' depends on axioms: [propext, Classical.choice, Quot.sound]
✔ [3014/3015] Built Artin (3.1s)
Build completed successfully (3015 jobs).
PASS (28 theorems, standard axioms only)
   clean-extraction recursion only: skipped (all other build stages ran)
== done
   clean-extraction full build: EXIT=0
== done
G1 BUILD_EXIT=0
```

G3: independent clean `/tmp` extraction, plain full build (no source-location environment overrides):

```
== optional assembly
assemble: source papers absent; using shipped artin_correlations.tex
== named manuscript
   artin_correlations pages=28 errors=0 overfull_hbox=0 overfull_vbox=0 undefined=0
== blind manuscript
   blind_manuscript pages=28 errors=0 overfull_hbox=0 overfull_vbox=0 undefined=0
blind-pdf-text: clean
blind-pdf-metadata: clean
   blind pages=28 (named 28) sections=17 audit=clean
== named source and reproduction archives
== blind supplement: scrub and audit every file
scrub: 17 text files redacted; Lean proof terms and dependency URLs retained
   blind supplement: 74 files, audit=clean
== build actual scrubbed Lean archive (optional shared cache)
✔ [3006/3015] Built Artin.PrimitiveRootBridge (2.0s)
✔ [3007/3015] Built Artin.Paper2Rebuild (4.0s)
✔ [3008/3015] Built Artin.Paper2 (4.9s)
✔ [3009/3015] Built Artin.Exclusion (8.6s)
✔ [3010/3015] Built Artin.TripleExclusion (5.0s)
✔ [3011/3015] Built Artin.Bridge (4.4s)
⚠ [3012/3015] Built Artin.PairExclusion (4.6s)
warning: Artin/PairExclusion.lean:174:2: This central dot `·` is isolated; please merge it with the next line.

Note: This linter can be disabled with `set_option linter.style.cdot false`
ℹ [3013/3015] Built Artin.Check (3.0s)
info: Artin/Check.lean:41:0: 'ArtinExclusion.chi10_shift_twenty' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:42:0: 'ArtinExclusion.chi10_shift_zero' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:43:0: 'ArtinExclusion.not_both_nonresidue' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:44:0: 'ArtinExclusion.chi10_eq_legendreSym' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:45:0: 'ArtinExclusion.legendreSym_flip_of_shift_twenty' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:46:0: 'ArtinExclusion.not_both_artin' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:47:0: 'ArtinExclusion.not_all_three_nonresidue' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:48:0: 'ArtinExclusion.legendreSym_third_eq_one' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:49:0: 'ArtinExclusion.not_all_three_nonresidue_two_five_ten' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:50:0: 'isPrimitiveRoot_imp_legendreSym_eq_neg_one' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:51:0: 'legendreSym_eq_neg_one_of_isPrimitiveRoot' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:52:0: 'ArtinExclusion.not_all_three_nonresidue_of_primitiveRoot' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:53:0: 'ArtinExclusion.legendreSym_third_eq_one_of_primitiveRoot' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:56:0: 'ArtinExclusion.not_both_nonresidue_of_barring_character' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:57:0: 'ArtinExclusion.not_both_primitiveRoot_of_barring_character' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:58:0: 'ArtinExclusion.not_all_three_nonresidue_of_pair' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:59:0: 'ArtinExclusion.not_both_primitiveRoot_five_ten' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:60:0: 'ArtinExclusion.not_both_primitiveRoot_two_six' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:63:0: 'Paper2.refutation_gap_two_base_three' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:64:0: 'Paper2.group_orders' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:65:0: 'Paper2.nmm_five' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:66:0: 'Paper2.nmm_thirteen' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:67:0: 'Paper2.paper_formula_fails_at_zero' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:68:0: 'Paper2.nmm_five_vanishes' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:69:0: 'Paper2.nmm_thirteen_never_vanishes' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:72:0: 'Paper2Rebuild.four_mul_indicator' depends on axioms: [propext]
info: Artin/Check.lean:73:0: 'Paper2Rebuild.main_identity' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:74:0: 'Paper2Rebuild.counting_identity' depends on axioms: [propext, Classical.choice, Quot.sound]
✔ [3014/3015] Built Artin (2.9s)
Build completed successfully (3015 jobs).
PASS (28 theorems, standard axioms only)
== source archive: full build from a clean /tmp extraction
== optional assembly
assemble: source papers absent; using shipped artin_correlations.tex
== named manuscript
   artin_correlations pages=28 errors=0 overfull_hbox=0 overfull_vbox=0 undefined=0
== blind manuscript
   blind_manuscript pages=28 errors=0 overfull_hbox=0 overfull_vbox=0 undefined=0
blind-pdf-text: clean
blind-pdf-metadata: clean
   blind pages=28 (named 28) sections=17 audit=clean
== named source and reproduction archives
== blind supplement: scrub and audit every file
scrub: 17 text files redacted; Lean proof terms and dependency URLs retained
   blind supplement: 74 files, audit=clean
== build actual scrubbed Lean archive (optional shared cache)
✔ [3006/3015] Built Artin.PrimitiveRootBridge (3.1s)
✔ [3007/3015] Built Artin.Exclusion (11s)
✔ [3008/3015] Built Artin.Paper2 (9.9s)
✔ [3009/3015] Built Artin.Paper2Rebuild (4.8s)
⚠ [3010/3015] Built Artin.PairExclusion (4.2s)
warning: Artin/PairExclusion.lean:174:2: This central dot `·` is isolated; please merge it with the next line.

Note: This linter can be disabled with `set_option linter.style.cdot false`
✔ [3011/3015] Built Artin.TripleExclusion (4.4s)
✔ [3012/3015] Built Artin.Bridge (4.5s)
ℹ [3013/3015] Built Artin.Check (3.4s)
info: Artin/Check.lean:41:0: 'ArtinExclusion.chi10_shift_twenty' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:42:0: 'ArtinExclusion.chi10_shift_zero' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:43:0: 'ArtinExclusion.not_both_nonresidue' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:44:0: 'ArtinExclusion.chi10_eq_legendreSym' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:45:0: 'ArtinExclusion.legendreSym_flip_of_shift_twenty' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:46:0: 'ArtinExclusion.not_both_artin' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:47:0: 'ArtinExclusion.not_all_three_nonresidue' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:48:0: 'ArtinExclusion.legendreSym_third_eq_one' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:49:0: 'ArtinExclusion.not_all_three_nonresidue_two_five_ten' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:50:0: 'isPrimitiveRoot_imp_legendreSym_eq_neg_one' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:51:0: 'legendreSym_eq_neg_one_of_isPrimitiveRoot' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:52:0: 'ArtinExclusion.not_all_three_nonresidue_of_primitiveRoot' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:53:0: 'ArtinExclusion.legendreSym_third_eq_one_of_primitiveRoot' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:56:0: 'ArtinExclusion.not_both_nonresidue_of_barring_character' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:57:0: 'ArtinExclusion.not_both_primitiveRoot_of_barring_character' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:58:0: 'ArtinExclusion.not_all_three_nonresidue_of_pair' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:59:0: 'ArtinExclusion.not_both_primitiveRoot_five_ten' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:60:0: 'ArtinExclusion.not_both_primitiveRoot_two_six' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:63:0: 'Paper2.refutation_gap_two_base_three' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:64:0: 'Paper2.group_orders' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:65:0: 'Paper2.nmm_five' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:66:0: 'Paper2.nmm_thirteen' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:67:0: 'Paper2.paper_formula_fails_at_zero' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:68:0: 'Paper2.nmm_five_vanishes' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:69:0: 'Paper2.nmm_thirteen_never_vanishes' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:72:0: 'Paper2Rebuild.four_mul_indicator' depends on axioms: [propext]
info: Artin/Check.lean:73:0: 'Paper2Rebuild.main_identity' depends on axioms: [propext, Classical.choice, Quot.sound]
info: Artin/Check.lean:74:0: 'Paper2Rebuild.counting_identity' depends on axioms: [propext, Classical.choice, Quot.sound]
✔ [3014/3015] Built Artin (3.1s)
Build completed successfully (3015 jobs).
PASS (28 theorems, standard axioms only)
   clean-extraction recursion only: skipped (all other build stages ran)
== done
   clean-extraction full build: EXIT=0
== done
G3 CLEAN_EXTRACTION_EXIT=0
```

G2, G5, G6, G7 and zip freshness:

```
G2 negative and positive audit tests
earlier-title: identifier 'Cross-base correlations of Artin'
EXIT=1 expected=1
series: identifier 'Paper 3'
EXIT=1 expected=1
path: identifier '/home/work'
EXIT=1 expected=1
hardware: identifier 'GMKtec'
EXIT=1 expected=1
name: identifier 'Bald'
name: identifier 'Josh'
EXIT=1 expected=1
own-doi: author's own DOI 10.5281/zenodo.22878205
own-doi: Zenodo record zenodo.22878205 (any Zenodo DOI is treated as provenance)
own-doi: non-allowlisted DOI 10.5281/zenodo.22878205
EXIT=1 expected=1
model: identifier 'qwen3'
EXIT=1 expected=1
machine: identifier 'jack'
EXIT=1 expected=1
assistant: identifier 'OpenClaw'
EXIT=1 expected=1
platform: identifier 'genspark'
EXIT=1 expected=1
repository: identifier 'artin_correlations'
repository: identifier 'artin-correlations'
EXIT=1 expected=1
clean: clean
EXIT=0 expected=0
third-party-dependency: clean
EXIT=0 expected=0
BEFORE-SCRUB: identifier 'Bald'
BEFORE-SCRUB: identifier 'Bald'
BEFORE-SCRUB: identifier 'Paper 3'
BEFORE-SCRUB: identifier 'Paper 3'
BEFORE-SCRUB: identifier 'Cross-base correlations of Artin'
BEFORE-SCRUB EXIT=1
AFTER-SCRUB lean/Artin/PairExclusion.lean: clean EXIT=0
Lean dependency manifest: byte-identical to named manifest
G5 section15 recomputation
EXIT=0; output byte-identical to results/section15_1e9.json
G6 phrase scan
merged TeX: 'therefore residue information': 0 hits
merged TeX: 'not information carried': 0 hits
merged TeX: 'either way': 0 hits
merged TeX: 'can only add': 0 hits
merged TeX: 'only usable filter': 0 hits
merged TeX: 'genuinely usable': 0 hits
merged TeX: 'only filter that does real work': 0 hits
merged TeX: '10^9$ primes': 0 hits
named PDF: 'therefore residue information': 0 hits
named PDF: 'not information carried': 0 hits
named PDF: 'either way': 0 hits
named PDF: 'can only add': 0 hits
named PDF: 'only usable filter': 0 hits
named PDF: 'genuinely usable': 0 hits
named PDF: 'only filter that does real work': 0 hits
named PDF: '10^9$ primes': 0 hits
G6 numeral diff (runs of ASCII digits, including labels/citations)
insert before [] after ['840']
delete before ['840'] after []
insert before [] after ['50', '847', '531']
Removed numeral-token occurrences: {}
Added numeral-token occurrences: {'50': 1, '847': 1, '531': 1}
No other numeral additions/removals; 840/120 ordering changes only.
G7 blind PDF and all supplement files
PDF + 74 supplement files: 0 identifier/title/path/repository/DOI/URL hits (dependency URLs allowlisted)
G9 archive freshness
artin_correlations_source.zip: k=1,599 present; no stale 1702; section15_sha256.txt byte-identical
artin_correlations_reproduction.zip: k=1,599 present; no stale 1702; section15_sha256.txt byte-identical
blind_supplement.zip: k=1,599 present; no stale 1702; section15_sha256.txt byte-identical
ALL CHECKS PASS
```

Additional G4 unchanged-proof check:

```
G4: 10 Lean source files: all non-comment bytes unchanged.
G4: lake-manifest.json byte-identical; third-party dependency URLs preserved.
```

## Unfixed / limitations

No requested gate left failing. Optional prime-7 ablation and optional held-out-null reanalysis deliberately not added; their numerical claims were not introduced. Auditing known tokens cannot guarantee anonymity against public-content matching. Named scientific numbers were not re-audited beyond the required byte-identical §15 regeneration; the existing census exclusions and claims outside this fix remain as supplied.
