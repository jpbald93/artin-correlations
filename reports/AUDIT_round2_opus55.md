# Round-2 adversarial audit: *Correlations of Artin status among primes*

- **Auditor:** opus-5.5 subagent, 2026-09-23, from 14:25 EDT.
- **Manuscript audited:** `artin_correlations.tex`, sha256 `ad5b359db5fe98d0095f5a00d8dddc057b76f85b0cd5f35424fd3682029c577b` (28 pp). This hash is byte-identical to `paper/artin_correlations.tex` in the GitHub repo, and also to the output of a fresh `assemble.py` run.
- **Evidence:** everything is in `reports/opus55_r2_evidence/`. Every command below is run from that directory.

## (a) Verdict

**MAJOR REVISION.** Every problem below can be fixed in the text. The theorems, the census, the citations, the repository and the Lean gate are all sound.

However, the two pieces of new text that were written to fix round-1 F1 and F2 are themselves wrong:

1. **The F1 decomposition formula is false as printed.** The paper's own driver does not check it; its "assertion" is tautological.
2. **The F2 optimism argument has two errors.**
   - It uses the wrong formula (k/N labelled as k/(2N)) and overcounts k.
   - Its conclusion ("consistent with overfitting, not with information") is **falsified at mod 120** by a null simulation (z ≈ 31).

Seven round-1 items were not touched at all.

## Method: what is different from round 1

I used a different route from last time.

**New census program (`td_census.c`).**
- It does **not** use the round-1 per-prime record file or its factor sieve.
- It uses a plain segmented Eratosthenes sieve for the primes.
- Artin(10) status comes from **trial division of each p−1** by primes below 65536, with the test 10^((p−1)/q) ≠ 1 for each distinct q.
- It streams straight into joint-residue count tables (mod 120 and mod 840) for three populations: all pairs, the even-hash half and the odd-hash half. The hash is the same `mix64`, applied to the successor, as in the author's code.
- No record file is written.

**Check against SymPy.** For p < 10^5 I compared against SymPy `n_order`: 3,617 Artin primes by both methods.

**Result at 10^9** (45 s on 2 cores):
```
primes>=7 50847531 artin 19016617 last 999999937
joint00 19758045 01 12072868 10 12072869 11 6943748
joint_1e8 2239116 1367048 1367048 788239        -> delta(1e8) = -0.013363024
g20mod40 2214511 predA 984678 both 0
```

**Analysis route.**
- All analysis is in new numpy code that works on the cell tables only.
  - This includes the decomposition algebra: several weight conventions, the Kitagawa form and the total-covariance form.
  - Where it scores held-out models, it computes held-out scores in both split directions.
- The pooled model is a new implementation. It fits a **Mantel–Haenszel common log-odds ratio**, with per-cell intercepts found by bisection on a Jeffreys-adjusted marginal. In round 1 I used a Newton pooled-β logit instead.
- I added a **permutation-null simulation**: within each cell and each hash half, the 2×2 table is redrawn with both margins fixed.

I did not use any driver output or `results/` file, except to diagnose the author's code.

## (d) The F1 discrepancy: which number is right (96.1% vs 98.7%)?

**Summary.** Both 96.1% and 98.7% are correct under their own conventions. The paper states neither convention correctly. The displayed identity mixes the two, and as printed it is false.

**What the paper prints (§content):**
> δ = Σ_c w_c¹ δ_c (within) + Σ_c (w_c¹ − w_c⁰) p_c (between), with p_c = P(Y=1 | C=c). "This is an identity, not a model, and we assert it in the driver."

**What the driver actually computes** (`code/artin_payoff.c` l.164–185):
```c
within += (a/s1)*dc;               // sum w1_c * delta_c  over non-degenerate cells
double between=delta-within;       // "between" DEFINED as the residual
if(between<0){ between=delta-within; }   // no-op
if(fabs((within+between)-delta) > 1e-12) ...   // tautologically true
```
So the "assertion" cannot fail. The printed between term Σ(w¹−w⁰)p_c is never computed at all.

**My recomputation** (`python3 decomp_r2.py`, `python3 resolution_r2.py` → `decomp_r2.out`, `resolution_r2.out`):
```
mod120: delta=-0.014140159
   author-within(w1)=-0.000545  residual-between=-0.013595  share_between=0.9614
   printed-formula between sum(w1-w0)p_c=-0.013954  within+printed=-0.014499908  (identity gap 3.597e-04)
   correct Kitagawa between sum(w1-w0)q0_c=-0.013595   identity gap 3.5e-17
   alt ordering: within(w0)=-0.000111 share_between=0.9922
   cov: within share=1.3131%  between share=98.6869%
mod840: author-within=-0.000310 residual-between=-0.013830 share=0.9781
   printed-formula between=-0.014076  within+printed=-0.014386084 (identity gap 2.459e-04)
   alt ordering within(w0)=-0.000041 share=0.9971 ; cov between share 99.5451%
symmetric weights (w1+w0)/2: mod120 97.68%, mod840 98.76%
resolution: sum_c (n_c/N) Cov_c / Var(X) + sum_c (w1-w0) p_c = delta EXACTLY
   mod120: -1.8567e-04 + -0.013954 = -0.0141401588  (between share 0.9869)
   mod840: -6.4320e-05 + -0.014076 = -0.0141401588  (between share 0.9955)
```

**Resolution.**
- **The author's 96.1% / 97.8% is correct** for the Kitagawa ordering with **w¹ weights**, i.e. δ = Σ w¹(q¹−q⁰) + Σ(w¹−w⁰)**q⁰_c**. The between term uses q⁰_c = P(Y=1 | X=0, C=c), **not p_c**.
  - The author computes the between term as a residual, so the numbers are right even though the formula is wrong.
  - At mod 840 there is a further detail: 675 cells contain only X=1 pairs. Their mass, 8.4e−5, also lands in the residual.
- **My round-1 98.7% / 99.6% is also correct.** It is the law-of-total-covariance split, rescaled by Var(X).
  - Its between term is **exactly the p_c term the paper prints**: Σ(w¹−w⁰)p_c = Cov_between / Var(X), since −0.013954 = −0.013954.
  - Its within term is Σ(n_c/N)Cov_c/Var(X), not Σ w¹δ_c.
- **So the displayed equation takes the within term from one decomposition and the between term from the other.** The sum misses δ by 3.6e−4 (mod 120) and 2.5e−4 (mod 840).
- **The between share depends on convention.** At mod 120 it is 96.1% (w¹), 97.7% (symmetric), 98.7% (covariance) or 99.2% (w⁰).
- **The robust statement is "between 96% and 99%".** Every convention agrees that more than 96% (mod 120) or more than 97.8% (mod 840) of δ lies between cells.
- **Stability:** a range split at 5e8 gives 95.7% / 96.6% at mod 120 (`rangesplit_r2.out`). So the paper's "a range split does not change the picture" holds up.

**Verdict on round-1 F1: PARTIALLY FIXED.** It is replaced by new finding N1.

## (b) Round-1 findings: status

| # | Status | Evidence |
|---|---|---|
| F1: 93%/97% compared different populations | **PARTIALLY FIXED** | Abstract and summary now use a within/between split, and the numbers are right under the w¹ convention. But the displayed identity is false (N1). Discussion (iii), l.1600, still reads "93–97%", and the subsection title at l.1485 still says "What the 93% and 97% of §decomposition measure", although §decomposition no longer contains those figures (N4). |
| F2: "absent" was too strong | **PARTIALLY FIXED, with new errors** | "Absent" is gone from §content and the abstract. The new justification is wrong (N2). Discussion (iv), l.1605, still says the content "is nil". Summary item 11 (l.208) still says "shows no held-out gain". |
| F3: TWZ volume/pages | **FIXED** | Crossref 10.1016/j.jnt.2022.10.006 gives `'Artin twin primes' 245 203-232 [Tinková, Waxman, Zindulka]`, which matches the bib entry (l.1874–1878). |
| F4: repo 404 | **FIXED** (with a gap, N5) | `curl` returns 200. The repo is public; it was created and pushed 2026-09-23T18:15Z, and has 2 commits. Every tracked file is byte-identical to the workspace, except that the PDF differs only in its rebuild timestamp (pdftotext diff is empty). |
| F5: δ(10⁸) | **FIXED / STALE** | The paper now gives −0.013363. My census gives −0.013363024. f(8…11) reproduces −0.013363, −0.014140, −0.014723, −0.014998. Increments −0.000777, −0.000583, −0.000275; ratios 0.750, 0.472; geometric extrapolation −0.015160. |
| F6: triple-corollary hypothesis | **FIXED** | l.1519 now reads "whenever the three bases are units modulo p (i.e. p∤abc)". This is exactly what χ_c = χ_aχ_b needs. (2,5,10) is covered for every odd p ≠ 5; p = 5 is trivially non-Artin for 5 and 10. |
| F7: abstract said "multiplicativity" for both laws | **PARTIALLY FIXED** | The abstract (l.89–90) is now right. Where reciprocity enters: in the proof of Thm `thm:exclusion` (l.272–274), (5\|p) = (p\|5) is reciprocity, and the second supplementary law gives (2\|p) mod 8 (l.278–279). The proofs of Thm `thm:pair` and `thm:triple` (l.379–384, 438–441) use only multiplicativity and the obstruction. But **summary item 11 (l.209) still says "both exclusion laws are consequences of character multiplicativity"**, which contradicts the fix. |
| F8: redundant vs. usable content | **PARTIALLY FIXED** | "Redundant" is now scoped ("once the relevant individual quadratic characters are already available", l.1520). The substantive sentence (the per-base QNR test already rejects everything the pair law rejects) is still not stated. |
| F9: "exactly what the conductor-40 channel expresses" | **NOT FIXED** | The abstract (l.91) and §content (l.1530) are unchanged. New evidence makes it stronger: conditioning on mod 40 alone gives within +0.019991 and between −0.034131 (241% of δ, opposite sign; `mod40_r2.out`). The mod-3 channel is essential. |
| F10: hold-out caption has fit and test halves swapped | **NOT FIXED** | The caption (l.1459) still says "fitted on pairs whose successor prime has an odd 64-bit hash and scored on the complementary half (25,421,106 pairs)". The code puts odd hashes in K, and `hold_report(J,K)` scores K. 25,421,106 is the **odd** half (`content_r2.out`: `N_test=25421106` for fit even / test odd). The smoothing is still misdescribed: the caption says "empty and sparse cells", but the code adds +0.5 everywhere. |
| F11: timings | **PARTIALLY FIXED** | The caption now says "single-run, hardware-dependent" and gives 15% / 13% correctly. The abstract (l.93) still says "about 13% faster again". `code/results.txt` l.54 still says "32 threads". |
| F12: "+0.39 to +0.98" | **NOT FIXED** | l.1203 unchanged. The diagonal cell with φ = +1 is one of the 47. |
| F13: (15,21) φ +0.0237 | **NOT FIXED** | l.1344 still reads +0.0237; the measured value is 0.023649. |
| F14: merge leftovers | **PARTIALLY FIXED** | (a) **FIXED**: l.495 now reads "proved above (Theorem thm:exclusion); the arbitrary-base form is BaldII". BaldII is still cited. The assemble.py rewrite is applied, and a fresh assemble run reproduces the shipped tex byte-for-byte. (b) **NOT FIXED**: l.503 still says "the vanishing criterion recalled in Section intro"; §1 mentions only "entanglement of … Kummer–Galois extensions", with no vanishing criterion or degree drop. (c) **NOT FIXED**: l.1114 still reads "the consecutive-prime papers routed". (d) **FIXED**: `results/` and `code/kummer_model_comparison.json` are identical (cmp); (5,13) model_joint = 0.16394342319726188; mean / max relative error 0.0121% / 0.0396%. (e) **NOT FIXED**: l.546 still says "to five decimal places" for 0.3739543 vs 0.3739558, which round to 0.37395 vs 0.37396. |
| F15: notation | **NOT FIXED** | l.575 still has `\mathbf{1}[\Art_n]`; l.1407 still has a bare `\Art`. |
| F16: wording | **NOT FIXED** | (a) l.1135 still says "equivalently, one in which the two indicators are conditionally independent" and then denies it. (b) "The one genuinely usable filter" (abstract l.90) is unchanged. |
| F17: uncited bibliography entries | **FIXED** (mostly) | All 25 `\bibitem`s are now cited and none are undefined (script: `uncited: [] undefined: set()`). Knapowski–Turán, Ko and Ash–Beltis–Gross–Sinnott (l.122) are still named without a citation. |
| Stale item: kummer_model_comparison.json | **REFUTED my round-1 claim, as of now** | The shipped `results/kummer_model_comparison.json` does contain the corrected 0.16394342319726188 (sha256 e0d54059…, the same as the repo copy). Its mtime is 13:19, so it was replaced before my round-1 report was finalised. |

## (c) New findings introduced by the fixes

### N1 — MAJOR — the displayed within/between identity is false; the driver's "assertion" is vacuous

**Where:** §content, l.1487–1498.

**Quoted text:**
> "…+ Σ_c (w_c¹ − w_c⁰) p_c … where … p_c = P(Y=1 | C=c). This is an identity, not a model, and we assert it in the driver."

**What is wrong.**
- With the printed p_c, within + between = −0.014499908 at mod 120, against δ = −0.014140159.
- The driver defines `between = delta - within`, so it asserts nothing (see (d)).

**Reproduce:** `python3 decomp_r2.py` (the "printed-formula" lines).

**Fix.**
- Replace p_c by q⁰_c = P(Y=1 | X=0, C=c).
- State that X-only cells (675 at mod 840, mass 8.4e−5) go into the between term.
- Say that the share depends on convention: 96.1% with w¹ weights, 97.7% symmetric, 98.7% covariance, 99.2% w⁰ at mod 120. Report the range, or pick one convention and justify it.
- Compute the between term explicitly in the driver so the assertion is not vacuous. Also delete the no-op line 181.

### N2 — MAJOR — the optimism argument has an arithmetic error, overcounts parameters, and its conclusion is falsified at mod 120

**Where:** §content, l.1468–1479; the abstract (l.87–88) inherits the conclusion.

**Quoted text:**
> "…augmented model carries 512 (mod 120) and 3,448 (mod 840) additional probabilities, and the classical k/(2N) optimism is 1.0×10⁻⁵ and 6.8×10⁻⁵ nats respectively. The apparent in-sample gain is thus consistent with overfitting, not with information."
>
> "…at most of order 10⁻⁶ nats of usable predictive information"

**Error 1 (arithmetic).** 512/(2·50,847,530) = **5.03e−6**, not 1.0e−5, and 3448/(2N) = **3.39e−5**, not 6.8e−5. The quoted figures are k/N: they come from the driver's `optimism_2x` column, labelled as k/(2N).

**Error 2 (k).** Of the 512 non-degenerate mod-120 cells, **256 have a constant successor**: the successor is a QR, so Y ≡ 0 in both X-groups. Splitting such a cell adds no free parameter. The effective k is **256** (mod 120) and **1,599** (mod 840), which gives **2.52e−6** and **1.57e−5** (`degenerate_r2.out`, `content_r2.out`).

**Error 3 (the inference).**
- At mod 120 the observed in-sample gain of 9.18e−6 is 3.6× the correct optimism. The likelihood-ratio statistic is G = 2N·gain = **933.6 on 256 df**.
- Permutation null, with margins fixed per cell and per hash half, 40 replicates (`python3 null_sim.py` → `null_sim.out`):
```
in-sample full:     null mean=2.532e-06 sd=2.106e-07 observed=9.181e-06 z=31.6
held-out even->odd: null mean=-9.959e-06 sd=7.460e-07 observed=-3.193e-06 z=9.1
held-out odd->even: null mean=-9.921e-06 sd=9.130e-07 observed=-4.893e-06 z=5.5
```
- The negative held-out gain at mod 120 is **far less negative than pure overfitting would produce**. So there is real predecessor information beyond the mod-120 residues.
- Split-sample estimate I ≈ (g_in + g_out)/2: **3.9e−6 nats** in both directions (`optimism_r2.out`). That is several times the stated "upper bound of order 10⁻⁶".
- At mod 840 the data are consistent with the null (in-sample z = 2.9; held-out z = 1.0 and −0.4; `null_sim840.out`). That is the defensible finding: the edge is detectable beyond mod 120 and vanishes into noise beyond mod 840.

**What survives.**
- The paper's practical conclusion: a few 10⁻⁶ nats is worthless for search.
- The statement "the held-out 2K-cell model loses".

**What does not survive.**
- "consistent with overfitting, not with information" (at mod 120).
- The 10⁻⁶ figure presented as a *bound*: nothing in the paper derives it.

**Fix.**
- Correct the arithmetic and k.
- Report the mod-120 signal honestly, e.g. "real but ≈4×10⁻⁶ nats, significant at mod 120 (G = 934 on 256 df), not detectable at mod 840".
- Base any bound on the pooled model (N3) or on the split-sample estimate.

### N3 — MINOR — the omitted pooled-model result is a code bug, not a finding; the correct pooled result supports the paper but is not reported

**Where:** §content, l.1474–1476 ("it likewise produced no gain"), and the shipped `artin_payoff.c cepool`.

**The author's driver**, re-run by me (`/tmp/ap_r2 cepool 1000000000`, 2 min 23 s):
```
"pooled_mod120": {... "beta": -0.009136, "heldout_gain": -1.211e-07}
"pooled_mod840": {... "beta": -0.004693, "heldout_gain": -2.353e-04}
```

**Why the mod-840 figure is wrong.**
- The mod-840 −2.35e−4 is an artefact.
- `pool_report` fits per-cell α by unregularised MLE. It falls back only when `f01+f11==0`.
- In cells where the fit half is all-Y=1, or is otherwise separated, α runs to the ±1e−12 clip. The result is catastrophic test losses: **136 nats in a single cell**, e.g. cell (521,629) with fit [0 65 0 0] and test [6 43 0 0] (`python3 pool_diag.py` → `pool_diag.out`).

**My independent pooled model** (Mantel–Haenszel β with Jeffreys-adjusted per-cell α) gives:
```
mod120: beta=-0.00914 heldout=-2.54e-08 | beta=-0.00414 heldout=+1.92e-07  (two split directions)
mod840: beta=-0.00469 heldout=-7.46e-08 | beta=-0.00005 heldout=+1.47e-09
```

**Assessment.**
- Held-out gains of about ±2e−7 are genuinely zero within noise. So the omission is **not material to the conclusion**, and "likewise produced no gain" is substantively right.
- But the shipped `cepool` mode reproduces a wrong and alarming number that the paper does not mention.

**Fix.** Regularise α in `cepool` (or delete the mode), and report the pooled numbers in the paper.

**Note the tension with N2.** A pooled single-β model gains nothing held out, yet cell-level predecessor information is significant at mod 120. So the mod-120 signal is heterogeneous across cells: a sign-varying δ_c, not a common offset. That is worth one sentence.

### N4 — MINOR — the fixes left stale and contradictory text

**The stale passages:**
- (a) Subsection title, l.1485: "What the 93% and 97% of §decomposition measure". §decomposition no longer states 93/97, and the subsection now explains 96.1 / 97.8.
- (b) Discussion (iii), l.1600: "93–97% … on the selected-cell metrics". This conflicts with the abstract's 96.1 / 97.8, and nothing reconciles the two.
- (c) Discussion (iv), l.1605: "which is nil". This conflicts with the new upper-bound framing.
- (d) Summary item 11, l.207–209: "shows no held-out gain; both exclusion laws are consequences of character multiplicativity". This conflicts with the abstract's reciprocity distinction (the F7 fix).
- (e) Repo `README.md` l.67: "93 %/97 % by joint residues".

**Reproduce:** `grep -n "93\|nil\|character multiplicativity" artin_correlations.tex`

**Fix:** make all of these consistent with the fixed abstract.

### N5 — MINOR — the public repo omits the evidence for the 10¹⁰ / 10¹¹ claims

**What is missing.** Tracked files match the workspace byte-for-byte. But `results/delta_1e10_2026-09-11.md`, `delta_1e11_2026-09-11.md`, `independent_audit_2026-09-11.md`, `PREREGISTERED_1e11.md`, `analysis_1e9_log.txt`, `theorem_log.txt`, `dataset_sha256.txt` and `final_consistency_checks.txt` are in the workspace but **not in the repo**.

**Why it matters.** The paper's δ(10¹⁰), δ(10¹¹), the "independent recomputation" and "every number … reproducible" all rest on these files.

**Reproduce:** the loop over `find code results …` in my session printed `not-in-repo:` for each file.

**Fix:** push them, or state that the outputs at those cutoffs are not deposited.

### N6 — NIT — numeral preservation

**The check.** Among numerals of three or more digits in the merged tex, 41 do not occur in Paper 1 or Paper 3. **All 41** occur in the author-written `FRONT.tex`, `SECTION_content.tex` or `DISC.tex`, so none is untraceable. But "every numeral traces to a source paper" is only true if those three files count as sources.

**Of these 41, the following are wrong or misdescribed:**
- 3448 and 512 as "additional probabilities" (N2);
- 1.0e−5 and 6.8e−5 (N2).

**The rest I reproduced exactly:**
- 0.000545 / 0.013595 / 0.000310 / 0.013830 / 96.1 / 97.8;
- 2,214,511 / 984,678;
- 25,421,106;
- 0.605 bits (0.419517/ln 2 = 0.6052);
- 1.3e−5 bits.

## Build, Lean, citations

- **Build.** In a scratch copy, `CONSOLIDATED_DIR=/tmp/r2build python3 assemble.py` reproduces the shipped tex byte-for-byte. Three pdflatex passes give **28 pages, 0 overfull hboxes, 0 undefined or multiply-defined references**, and 1 underfull box. The PDF contains no "??".
- **Lean.** `cd lean && ./gate.sh` gives `PASS (28 theorems, standard axioms only)` (`gate_r2.log`). `LEAN_NOTE.md` is accurate.
  - **Formalised:**
    - the base-10 gap law as a mod-40 character flip, including `not_both_artin` for actual primes with `(p':ZMod 40) = p + 20`;
    - the primitive-root ⇒ Legendre −1 bridge;
    - the pair and triple character-parity cores with explicit nonvanishing hypotheses;
    - the concrete instances (5,10), (2,6) and (2,5,10).
  - **Not formalised:**
    - the generic sqf(ab) ⇒ residue-classes-mod-4d translation, which is done by hand per instance, as the paper says;
    - any census or asymptotic statement;
    - the within/between identity.
- **Citations.** Crossref confirms the corrected TWZ entry. The Integers DOI resolves on Crossref. DataCite resolves all three Zenodo DOIs (22863946, 22865343, 22878204). All 25 bibliography items are cited, with no dangling keys.

## Bottom line

**What I could not falsify:**
- the census: every count at 10⁸ and 10⁹ matches, by a third independent implementation;
- the theorems, including the corrected triple scope;
- the TWZ citation, the repo, the Kummer JSON, δ(10⁸) and the Lean gate.

**What I did falsify:** the two paragraphs written to answer F1 and F2.
- **F1:** the numbers are fine, but the printed formula is not the identity it claims to be, and the conventions are unstated.
- **F2:** the "overfitting, not information" conclusion is wrong at mod 120 (z ≈ 31), and the optimism arithmetic is off by a factor of 2 on top of an overcounted k.

About half of the small round-1 items (F9, F10, F12, F13, F14b/c/e, F15, F16) were not addressed. None of these touches a theorem or a count, and none changes the paper's practical verdict.
