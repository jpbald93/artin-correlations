# Adversarial audit — Correlations of Artin status among primes

Auditor: opus-5.5 subagent, 2026-09-23. Manuscript audited: `artin_correlations.tex`, sha256
`699489d9b6a4956117f6fb4f291d201fd9acf09606fd28df3cfc21e6d6802beb` (13:21 EDT build, 27 pp).
**The file was rewritten while I was auditing it.** The 12:07 build I started from was replaced
at 13:21. Every finding below was re-checked against the 13:21 text. Where the 13:21 text had
already fixed something wrong in the 12:07 text, I say so explicitly.
My scripts and outputs are in `reports/opus55_evidence/`.

## Verdict (ACCEPT / MINOR REVISION / MAJOR REVISION / REJECT)

**MAJOR REVISION.**

- **Mathematics:** the theorems are correct, and I checked them line by line. The Lean gate passes.
- **Census numbers:** nearly every one reproduces exactly from my own independent census.
- **Why not minor:** two claims used as headlines misstate what their own measurements show.
  - The "removes 93% / 97%" statement in the abstract, the summary and the Discussion.
  - The new section's conclusion that the correlation's predictive edge "is absent".
- **Other problems:**
  - The priority citation (Tinková–Waxman–Zindulka) has the wrong volume and pages.
  - The data-availability URL returns 404.
  - A side condition in the new section is wrong.
  - There are several leftovers from the merge.
- **Nothing here invalidates a theorem or a census count.** Every fix is textual, or swaps one
  metric for another I have already computed (below).

## What I verified independently (method + result)

**Method.** I wrote my own segmented census (`census.c`). It is independent of the authors' code:
it runs a segmented factor sieve over m = p−1, which gives ω(p−1), the fine signature and the
Artin indicators for all 12 bases, plus Euler-criterion QR bits. I checked it against SymPy
(`n_order`, `factorint`) for every prime below 10^5: 9,590 primes × (12 Artin bits + 12 QR bits
+ ω + signature), **0 mismatches**. I then ran it over all p ≤ 10^9 in 9.5 min on one core.
All analysis is in my own numpy scripts. I did **not** re-run the authors' analysis, with one
exception: their `search3` binary, re-run only to measure timing variance.

| Claim | Paper | Mine | Status |
|---|---|---|---|
| primes 7…999,999,937 | 50,847,531 | 50,847,531 (also from a separate plain sieve `gapcount.c`) | ✓ |
| Artin (base 10) | 19,016,617 | 19,016,617 | ✓ |
| joint table | [[19758045,12072868],[12072869,6943748]] | identical | ✓ |
| P(A\|A), P(A\|¬A), δ | 0.365141, 0.379281, −0.014140158840 | 0.36514108, 0.37928124, −0.0141401588398 | ✓ |
| Pearson χ², signed √ | 10,167; −100.8 | 10,166.66; −100.83 | ✓ |
| pairs at g ∈ {20,60}; doubly-Artin | 2,195,882; 0 | 2,195,882; 0 | ✓ |
| g ≡ 20 (40): pairs / Artin predecessor / both | 2,214,511 / 984,678 / 0 | 2,214,511 / 984,678 / 0 | ✓ |
| QR flip at g≡20 (40); QR preserved at g≡0 (40) | "every case" | 0 violations in 2,214,511; 0 in 712,316 | ✓ |
| pairs with g≡20 (40) below 10^7 | 23,956 | 23,956 | ✓ |
| all 30 cells of Table `tab:gaps` (N, P(A\|A), P(A\|¬A), δ(g) to 3 dp) | — | 30/30 exact | ✓ |
| δ(60), δ(40); P(A\|A) at g=40 | −0.592, +0.643; 0.899 | −0.59203, +0.64281; 0.89936 | ✓ |
| number of qualifying gaps; extra gaps; pair-weighted mean δ(g) | 36; {62,64,66,70,72,78}; +0.00159 over 50,114,759 | same; same; +0.0015881 over 50,114,759 | ✓ |
| r(ω(p_n−1), ω(p_{n+1}−1)) | −0.0410 | −0.041024 | ✓ |
| mod-12 diagonal | 0.1813–0.1815 | 0.18131–0.18152 | ✓ |
| residue conditioning mod 120 | 145 cells, 12,255,203 pairs (24.10%), Σχ²=789.2, δ_res=−0.001055 | identical | ✓ |
| residue conditioning mod 840 | 633, 12,086,765 (23.77%), 719.8, −0.000473 | identical | ✓ |
| split-sample mod 120 | −0.00096, −0.00094 | −0.00096, −0.00094 | ✓ |
| **δ(10^8)** | **−0.013360** | **−0.0133630** (5,761,450 pairs) | **✗ (F5)** |
| prime counts at 10^10 / 10^11 | 455,052,508 / 4,118,054,810 | same (my plain sieve) | ✓ |
| pairs at g∈{20,60} below 10^10 / 10^11 | 20,700,958 / 194,296,748 | same (my plain sieve) | ✓ (counts only) |
| CE marginal; mod 120 residues; +prev; gain; incr. | 0.661047; 0.241529; 0.241520; 0.419517; 0.000009 | identical to 6 dp | ✓ |
| CE mod 840; +prev; gain; incr. | 0.237235; 0.237217; 0.423812; 0.000018 | identical | ✓ |
| g mod 40 "trap" | ≈0.02 nats | 0.021841 nats | ✓ |
| held-out gains | −3.19e−6 (mod 120), −6.11e−5 (mod 840) | −3.193e−6, −6.111e−5 on the same split; −4.89e−6, −6.08e−5 on the reverse split | ✓ (but see F2, F10) |
| search at 10^15: primes / factorizations / small-ℓ exps | 722; 722/376/300; 402 | 722; 376 QNR; 76 small-ℓ rejections; 300 survivors; 402 exps (my own Pollard-ρ code) | ✓ |
| cross-base N (p≥5) | 50,847,532 | 50,847,532 | ✓ |
| 64/66 positive; mean φ; negative pairs | 64; 0.0352; (2,10),(3,15) | 64; 0.035206; (2,10),(3,15) | ✓ |
| φ\|ω, φ\|sig mean; removed | 0.0158 / 55%; 0.0019 / 95% | 0.015808 / 55.1%; 0.001856 / 94.7% | ✓ |
| covariance-scale; mean-abs scale | 57%/95% (95.3%); 51%/87% | 56.5%/95.27%; 51.0%/87.0% | ✓ |
| eligible fraction sig; sig+QR | 73%; 23% | 73.1%; 23.4% | ✓ |
| TC/NC group means (Table `tab:residual`) | 0.0314/0.0049/0.0179/0.0049; 0.0363/0.0010/0.0010/0.0052 | 0.03144/0.00487/0.01787/0.00494; 0.03631/0.00097/0.00097/0.00521 | ✓ |
| TC abs residual excluding (2,6) | 0.0077 | 0.007736 | ✓ |
| six negative residuals, and their values after QR conditioning | listed | all 12 numbers exact | ✓ |
| (2,6): φ\|sig, φ\|sig,QR; diagonal cell | +0.1598, +0.0055; 1,775,686 | +0.15977, +0.00547; 888,676+887,010 = 1,775,686, off-diagonal 0 | ✓ |
| range over the 47 (2,6) cells | +0.39 to +0.98 | +0.386 to **+1.000** | ✗ (F12) |
| top-10 table, TC flags, P(A₁₀\|A₂)=0.3551 vs 0.3853 | — | all exact | ✓ |
| pair-law violations over all 66 pairs; joint density on χ_a=χ_b half | 0; 0.299 | 0; 0.29902 | ✓ |
| marginal densities (2, 5, 13) | 0.373957, 0.393642, 0.376362 | same | ✓ |
| at x=3·10^7, "a further twelve" pairs just below zero, all within 3σ₀ | — | 18 negative = 6 + 12; the 12 extra all have \|z\| ≤ 1.2 | ✓ |
| Kummer model: mean / max rel. error | 0.012% / 0.040% at (21,29) | 0.0121% / 0.0396% at (21,29) (my own implementation, ε = \|V∩cyclo(L)\|, Euler product to 3·10^6) | ✓ |
| single-base Hooley densities | 0.373956, 0.393638, 0.376368 | same | ✓ |
| Table `tab:model` rows | — | all match, except the (15,21) measured φ (F13) | ✓ |
| ord₁₀(999,999,937) | 333,333,312 | 333,333,312 | ✓ |
| (2,8) jointly primitive at 3,5,11,29,53 | — | 3,5,11,29,53,59 | ✓ |
| residue classes for d=3 (mod 12), d=2 (mod 8), d=5 (mod 5) | — | exact for all p < 10^5 | ✓ |
| Lean `./gate.sh` | PASS | `PASS (28 theorems, standard axioms only)` | ✓ |

**Proofs, checked line by line.**

- **Theorem `thm:exclusion`: correct.**
  - Hypotheses: p, p′ > 5 is used only to exclude 2 and 5. At p = 3 (3 → 23) the flip still holds: (10|3) = +1 and (10|23) = −1.
  - Mod-8 step: the map r ↦ r+4 sends {1,7} to {5,3}. Correct.
  - Mod-5 step: correct.
  - It is exactly Tinková–Waxman–Zindulka Thm 1.1 condition (2)(b) with g₀ = 10 and d ≡ 4 (mod 8), 10 | d. I checked this against the arXiv PDF, so the "cf." is justified.
- **Corollary `cor:coupling`:** correct.
- **Conductor remark:** correct, including the counterexample a = 98, p = 7 and 23.
- **Theorem `thm:pair`:** correct.
  - The case p | ab is handled.
  - d = 1 ⇔ sqf(a) = sqf(b), and the theorem is then vacuous. Correct.
  - "Half of all primes": correct for d > 1. The character is non-principal of conductor dividing 4d, and Dirichlet gives density 1/2.
- **Theorem `thm:triple` and the odd-p restriction:** correct. At p = 2, bases 3, 5 and 15 are all vacuously primitive roots, so odd p really is needed.
- **The (2,6) mod-12 argument in §`sec:entanglement`:** correct.

## Findings (numbered; each: severity BLOCKER/MAJOR/MINOR/NIT; location; what is wrong; evidence; suggested fix)

**F1 — MAJOR — "removes 93% and 97%": in the abstract (l. 67–69), Summary item 5 (l. 186–188) and Discussion (iii) (l. 1570–1575).**

- **What is wrong.** The figure is computed as 1 − |δ_res| / |δ_global|:
  - 1 − 0.001055/0.014140 = 92.5%
  - 1 − 0.000473/0.014140 = 96.7%
- **Why it misleads.** δ_res is a within-cell mean over the ≈24% doubly-non-residue subpopulation. The unconditioned δ on that same subpopulation is not −0.014. It has the **opposite sign**:
  - +0.00208 on the 145 selected mod-120 cells;
  - +0.00280 on the 633 selected mod-840 cells;
  - +0.00181 on all doubly-QNR pairs.
  - (All three from my own computation, `gaps_decomp.py` and the inline check.)
  - So on the population where δ_res is measured, conditioning did not "remove 93%" of anything. It turned a small positive association into a smaller negative one.
- **The body concedes this, but the headlines don't.** §`sec:decomposition` says outright that these are "three different quantities computed on three different populations, not one quantity being reduced", and that this is "not a decomposition". The abstract and summary still say "removes". A hedge in the body does not license the headline.
- **Evidence for a proper metric.** A decomposition that does exist gives a cleaner and stronger statement. Total covariance Cov(Art_n, Art_{n+1}) = −0.0033105 (`decomp_cov.py`, law of total covariance, all 50,847,530 pairs, no thresholding). Its within-cell share is:
  - mod 120: −4.35e−5, i.e. **1.31%** (so 98.7% lies between cells);
  - mod 840: −1.51e−5, i.e. **0.45%**;
  - mod 40 (for comparison): +1.63e−3, i.e. −49%. The within-cell part has the opposite sign. The quadratic channel alone overshoots, and the mod-3 channel is needed.
- **Suggested fix.** Replace "removes 93%/97% of a selected-cell signed residual association" everywhere with the total-covariance decomposition ("98.7% / 99.6% of Cov(Art_n, Art_{n+1}) is between joint-residue cells mod 120 / 840"). Or drop the percentages altogether.

**F2 — MAJOR — the claim that the predictive edge is absent: §`sec:content` after Table `tab:holdout` ("under the only test that can establish predictive skill, it is absent"), the abstract ("shows no gain at all") and Discussion (iv) ("which is nil").**

- **What is wrong.** The held-out comparison is a K-cell model against a model with twice as many cells, each with its own free probability. So a negative held-out gain is what one expects from the parameter count alone, whether or not a real effect of order 10⁻⁶ nats exists.
- **Evidence.**
  - Number of informative cells, where both predecessor states occur and Art_{n+1} varies: **256** (mod 120) and **1,599** (mod 840). Given how many cells are informative, the expected resubstitution optimism is about k/(2N):
    - **2.5e−6 nats** at mod 120, against the reported 9e−6;
    - **1.57e−5 nats** at mod 840, against the reported 1.8e−5.
  - So the mod-840 "0.000018" is almost entirely overfitting, and even the mod-120 figure is inflated about 1.4×.
  - A one-parameter model (cell-specific intercept plus a single shared logit coefficient β for the predecessor), fitted on one hash half and scored on the other: β ≈ −0.004 to −0.009; held-out gain **+9.4e−8 and −1.2e−7** nats at mod 120 (`onepar.py`). That is indistinguishable from zero, not "absent".
    - (My mod-840 one-parameter fit is unreliable because of sparse-cell intercepts. I do not rely on it.)
- **Suggested fix.**
  - State the result as an upper bound: "no detectable held-out gain; the in-sample increment is of the order of the k/2N optimism; any real edge is below ~10⁻⁵ nats".
  - Report a low-parameter held-out model, not only the 2K-cell one.
  - Delete "absent" and "nil".
  - The conclusion (no computational value) survives; the wording overstates the evidence.

**F3 — MAJOR — the Tinková–Waxman–Zindulka entry in the bibliography: wrong volume and pages.**

- **What is wrong.** The paper gives *J. Number Theory* **247** (2023), 274–304. Crossref (DOI 10.1016/j.jnt.2022.10.006) gives **245** (April 2023), **203–232**.
- **Why it matters.** This is the priority citation for Theorem 1 ("we claim no priority").
- **Suggested fix.** Correct it and add the DOI.

**F4 — MAJOR — the repository URL in Data availability (l. ~1569).**

- **What is wrong.** "The code, manuscript, results and Lean development are at https://github.com/jpbald93/artin-correlations." That URL returns **HTTP 404** (tested 2026-09-23). The GitHub API lists the user's public repos, and `artin-correlations` is not among them; `consecutive-artin` and `cross-base-artin-correlations` are.
- **Why it matters.** The reproducibility claim is currently false.
- **Suggested fix.** Publish the repository, or cite the existing repos or a Zenodo DOI.

**F5 — MINOR — δ(10⁸) is wrong in the 5th significant figure: Open questions, display after `conj:persist`; the interpolant f(t); the quoted increments.**

- **What is wrong.** The paper gives δ(10⁸) = −0.013360. I get **−0.0133630** (5,761,451 primes 7…99,999,989; 5,761,450 pairs).
  - The project's own files agree with me: `astra_evidence/census_1e8.txt` has −0.013363024, `Paper 6/results/decay_check.json` has −0.013363196, and the legacy `data_1e8.csv` gives −0.0133633.
- **Knock-on effects.**
  - The first increment is −0.000777, not −0.000780.
  - f(t) was fitted to the wrong value. With the paper's coefficients, f(8) = −0.013360 exactly, so f does *not* "reproduce all four measured values".
  - The other derived numbers survive: ratios 0.750 and 0.471; δ·log x = −0.2462…; geometric extrapolation −0.015161.
- **Suggested fix.** Use −0.013363, refit f, and give the first increment as −0.000777.

**F6 — MINOR — wrong side condition in the redundancy paragraph of §`sec:content` (l. 1486–1489).**

- **What is wrong.** "…whence $c$ is not Artin (for $c$ not dividing $ab$)". The needed hypothesis is **p ∤ abc**, which is what the identity χ_c = χ_aχ_b needs. "c ∤ ab" is irrelevant.
- **Why it matters.** As written, the condition *excludes the paper's own headline triple*: c = 10 divides ab = 10 for (2, 5, 10). The same goes for (3, 5, 15) and (3, 7, 21).
- **Suggested fix.** Replace it with "(for odd $p \nmid abc$)".

**F7 — MINOR — the abstract contradicts the body on how the gap law follows.**

- **What is wrong.** The abstract (l. 90) says "Both exclusion laws are computationally redundant: *each follows from the multiplicativity of quadratic characters*." The body of §`sec:content` now says, correctly: "multiplicativity alone does not fix which sign the shifted product takes"; the constant sign needs reciprocity and the supplementary law for 2.
- **History.** The 12:07 version of the body wrongly said the product χ₁₀(p)χ₁₀(p+g) "takes a fixed value +1". The 13:21 version correctly says −1, but the abstract was not brought into line.
- **Suggested fix.** "each follows from reciprocity and the multiplicativity of quadratic characters".

**F8 — MINOR — tension between "redundant" and "usable content": §`sec:pairlaw` against §`sec:content`.**

- **What is wrong.**
  - §`sec:pairlaw` argues the pair law is a gain "in usable content" over the triple law, because its barring condition is decidable from p alone.
  - §`sec:content` calls the same law computationally redundant "once the relevant individual quadratic characters are already available".
  - That qualifier makes "redundant" close to a tautology: *any* consequence of χ_a and χ_b is redundant once χ_a and χ_b are known.
- **Suggested fix.** Say plainly that the individual QNR test (O(1) by reciprocity) already rejects every prime the pair law rejects, so the pair law never rejects a candidate the per-base filter keeps. That is the substantive statement.

**F9 — MINOR — "exactly what the observed conductor-40 channel expresses": abstract l. 91–92 and §`sec:content`, "The one usable filter".**

- **What is wrong.** The measured channel structure is not conductor-40 alone. Conditioning on joint residues mod 40 leaves a within-cell covariance of **+49%** of the total, with the opposite sign (F1 evidence). Only mod 120 = 40·3 absorbs 98.7%. The mod-3 divisibility channel is essential to the correlation.
- **Why the filter claim is still fine.** The QNR *filter* does correspond to the mod-40 information.
- **Suggested fix.** "The QNR filter corresponds to the conductor-40 part of the channel; the correlation itself also needs the mod-3 channel, which is not a filter."

**F10 — NIT — Table `tab:holdout` caption: fit and test halves are swapped, and the smoothing is misdescribed.**

- **What is wrong.** The caption says cell probabilities are "fitted on pairs whose successor prime has an odd 64-bit hash and scored on the complementary half ($25{,}421{,}106$ pairs)".
  - In `artin_payoff.c`, `(t?K:J)` with `hold_report(J,K)`: pairs with **odd** hash go to K, which is the **test** set.
  - My count: the odd-hash half is exactly 25,421,106 pairs, and fitting on even, scoring on odd reproduces −3.193e−6 / −6.111e−5 exactly.
  - So the caption's fit/test assignment is inverted.
- **Also:** "Jeffreys smoothing is applied to empty and sparse cells". The code applies +0.5 to **every** cell.
- **Suggested fix.** Correct the caption. Report both split directions: I get −4.89e−6 / −6.08e−5 for the reverse, with the same sign.

**F11 — MINOR — timing ratios in §`sec:content` (Table `tab:filter`) and the abstract.**

- **What is wrong.**
  - (a) The abstract says the early exit is "about 13% faster again". 1.149× is 15% *faster*, i.e. 13% *less time*. The caption states this correctly; the abstract conflates the two.
  - (b) Single-run timings: I re-ran the authors' `search3` three times on a 2-core VM. The counters were identical. The ratios were not:
    - B/C = 1.228, 1.073, 1.157;
    - A/C = 2.38, 2.10, 2.19.
    - So "15%" carries roughly ±8 points of noise.
  - (c) The caption now says "single-threaded", but `code/results.txt` l. 54 still says "32 threads". The code (`search3`) has no OpenMP pragma, so single-threaded is right and the package note is stale.
  - (d) An extra observation: in my own implementation **all 300** primes that survive QNR + small-ℓ are Artin. So strategy C's 300 factorizations do no filtering; they are pure certification. That supports point (iv) of "What would change the verdict" and is worth stating.
- **Suggested fix.** Report the median of ≥5 runs, or report factorization counts only; fix the abstract wording; fix `results.txt`.

**F12 — NIT — the range of within-cell φ for (2,6) (l. 1200–1202).**

- **What is wrong.** "Across the 47 cells with v₂(p−1)=1, 3∤p−1 and at least 200 primes, the within-cell φ ranges from +0.39 to +0.98." The exactly diagonal cell described in the previous sentence (φ = +1, 1,775,686 primes) is one of those 47.
- **Evidence.** I get 47 cells, min +0.386, max **+1.000**; the second largest is +0.975.
- **Suggested fix.** "+0.39 to +1".

**F13 — NIT — Table `tab:model`, row (15,21): measured φ.**

- **What is wrong.** The row gives measured φ = +0.0237. The measured value is 0.023649, which rounds to **+0.0236**. The authors' own `crossbase_fine_summary.json` has 0.0236490.
- **Suggested fix.** Correct the entry.

**F14 — MINOR — leftovers from the merge.**

- (a) **l. 494:** "the gap exclusion law for consecutive primes proved in~\cite{BaldII}". In the merged paper the gap law is **Theorem `thm:exclusion` of this paper**; BaldII is the arbitrary-bases generalisation. Cite both.
- (b) **l. 502–503:** "the vanishing criterion recalled in Section~\ref{sec:intro}". The new Introduction recalls no vanishing criterion or degree drop (grep for vanish/degree/compositum in §1 finds nothing). It is a dangling back-reference inherited from Paper 3's introduction.
- (c) **l. 1113:** "$\omega$ is the statistic through which the consecutive-prime papers routed the mechanism". This is now *this* paper, §`sec:omega`.
- (d) **`results/kummer_model_comparison.json`:** this copy is the **superseded, buggy-ε** output (mean relative error 0.0367%, max 0.674%; (5,13) model j = 0.164671). The `code/` copy is the corrected one (0.0121%). The package ships both under the same name, and the one in `results/` contradicts the paper.
- (e) **Data section (l. ~531):** 10¹¹ density 0.3739543 "agrees … to five decimal places". Rounded to 5 dp this is 0.37395 against C = 0.37396, so "agrees to within 2×10⁻⁶" is the honest form. `results/delta_1e1{0,1}*.md` still say "agrees to 6 dp", which is false at 10¹¹ (|Δ| = 1.5e−6).

**F15 — NIT — notation (target 7).**

- **The two symbols are not confused.**
  - `\Art` appears only as the consecutive-prime indicator Art_n (defined l. 220 as $\mathbf 1[p_n\in\Acal_{10}]$).
  - `\Acal_a` is always the set (for P3 blocks, `assemble.py` replaced `\Art` with `\Acal` wholesale).
  - The 12:07 text's "$\Acal_a(p)=1$" (a set used as a function) is fixed at 13:21 via $X_a(p)$.
- **Residual slips.**
  - l. 574: "$(\mathbf{1}[\Art_n], \mathbf{1}[\Art_{n+1}])$" is the indicator of an indicator. Write $(\Art_n,\Art_{n+1})$.
  - §`sec:content`: "marginal density of $\Art$" uses a bare `\Art` with no subscript.
  - $X_a$ is introduced in §`sec:prelim` but never used after l. 222.

**F16 — MINOR — overstated or self-cancelling wording elsewhere.**

- (a) **l. 1134–1139:** "A model that reproduces the observed within-signature conditional marginals … — *equivalently*, one in which the two indicators are conditionally independent given the signature — accounts for 95% … Conditional independence alone is not enough". The sentence asserts an equivalence and then denies it. Drop "equivalently".
- (b) **"The one genuinely usable filter":** the same section then measures a second usable device (small-ℓ early exit, −20% factorizations). Calling it "test ordering" rather than a filter is fine, but "one" is then rhetorical.
- (c) **The naive baseline (A) is a strawman.** Any standard implementation tests ℓ = 2 first, which *is* strategy B. The paper half-concedes this ("a standard filter"). Say explicitly that B is the realistic baseline, so the only non-standard delta is B→C.

**F17 — MINOR — citations never used in the text.**

- **What is wrong.** Six bibliography entries are never `\cite`d: Artin1965, FanPollack2025, PeruccaShparlinski2025, Kimmel2024, Lenstra1977, Sgobba2025. They survived the merge; amsart prints them anyway.
- **Also:** Knapowski–Turán, Ko and Ash–Beltis–Gross–Sinnott are named in §1 (l. 122) with no citation.
- **Suggested fix.** Cite the uncited entries or delete them, and add references for the three named works.

**Checked and clean (no finding):**

- **Cross-references:** all resolve. The PDF contains no "??", and the log has no undefined references.
- **Summary items:** each of items 1–11 is proved or measured in the body.
- **Abstract numerals:** every number in the abstract matches the body, apart from F1, F2, F7 and F11(a).
- **§`sec:content` against the rest of the paper:** it does not numerically contradict §`sec:decomposition` or the exclusion-law sections. The contradictions are only the interpretive ones in F1, F7 and F9.
- **The g mod 40 "trap":** the claim and its explanation are correct (0.0218 nats).

## Citations check (table: key / verified? / problem)

| key | verified? | problem |
|---|---|---|
| Artin1965 | book, not in Crossref; standard | never cited (F17) |
| BakerPollack2016 | ✓ Crossref 10.1515/forum-2014-0137, Forum Math 28(4) 675–687 | online 2015, issue 2016: fine. The Discussion's summary matches the arXiv abstract (unconditional; ≥exp(Cm) primes) |
| ConwayGuy1996 | book, standard | page range 166–171 not checked |
| Erdos1935 | ✓ 10.1093/qmath/os-6.1.205, 6, 205–213 | — |
| FanPollack2025 | ✓ 10.1112/mtk.70055, Mathematika 71(4) e70055 | never cited |
| GarciaKahoroLuca2019 | ✓ 10.1080/10586458.2017.1360809, Exp. Math 28(2) 151–160 | online 2017, print 2019: fine |
| GarciaLucaShiUdell2020 | ✓ 10.1016/j.jnt.2019.08.003, JNT 208, 400–417 | — |
| GuptaMurty1984 | ✓ 10.1007/BF01388719, Invent. 78(1) 127–130 | — |
| HardyLittlewood1923 | ✓ 10.1007/BF02403921, Acta Math 44, 1–70 | — |
| HeathBrown1986 | ✓ 10.1093/qmath/37.1.27, 27–38 | — |
| Hooley1967 | ✓ 10.1515/crll.1967.225.209, Crelle 225, 209–220 | — |
| KlurmanShparlinskiTeravainen2025 | ✓ 10.1112/blms.70103, BLMS 57(8) 2429–2443; arXiv 2412.13355 ✓ | — |
| **TinkovaWaxmanZindulka2023** | **✗** | **wrong volume and pages: actually JNT 245 (2023) 203–232, doi 10.1016/j.jnt.2022.10.006 (F3).** arXiv 2010.15988 ✓. Thm 1.1(2)(b) does cover base 10, g≡20 (40) ✓ |
| LOS2016 | ✓ 10.1073/pnas.1605366113, PNAS 113(31) | E4446–E4454 is the standard pagination; Crossref returned no pages |
| Moree2012 | ✓ 10.1515/integers-2012-0043 | "12A, A13" is the conventional citation; Crossref files it as 12(6). Fine |
| PeruccaShparlinski2025 | ✓ 10.1112/blms.70011, BLMS 57(3) 978–991 | never cited |
| Pollack2014 | ✓ 10.2140/ant.2014.8.1769, ANT 8(7) 1769–1786 | Discussion's summary (GRH, runs may be consecutive) matches the abstract |
| BaldII | ✓ Zenodo concept DOI 10.5281/zenodo.22865343 resolves; record title matches; GitHub repo exists | self-citation used for the wrong theorem (F14a) |
| GoldmakherMartinPeringuey2025 | ✓ arXiv 2502.19601, title and authors match | — |
| JarviniemiPeruccaSgobba2025 | ✓ 10.1007/s40993-025-00620-2, RNT 11, art. 42 | "no. 42" should read "Paper No. 42" (it is issue 1); "19 pp" not checked |
| Kimmel2024 | ✓ 10.4064/aa230810-29-6, Acta Arith 216(4) 329–348 | never cited |
| Lenstra1977 | ✓ 10.1007/BF01389788, Invent. 42, 201–224 | never cited |
| MoreeStevenhagen2014 | ✓ 10.4064/aa163-1-2, Acta Arith 163(1) 15–32 | — |
| Matthews1976 | ✓ 10.4064/aa-29-2-113-146, Acta Arith 29(2) 113–146 | — |
| Sgobba2025 | ✓ arXiv 2508.08996, title and author match | never cited |

No duplicate entries survived the merge: all 25 keys refer to distinct works.

## What I could NOT verify, and why

- **Artin statuses at 10¹⁰ and 10¹¹:** δ(10¹⁰) = −0.014723, δ(10¹¹) = −0.014998, the zero doubly-Artin count at 10¹⁰/10¹¹, and the densities 0.373955 / 0.3739543. My census runs at about 1.1 min per 10⁸ on one core, so 10¹⁰ would take about 2 h and 10¹¹ about 20 h on this 2-core, 8 GB VM. I did verify the **prime counts and the gap-{20,60} pair counts** at both cutoffs with my own sieve. Together with the theorem, which I verified, "zero doubly-Artin" is forced anyway.
- **Absolute timings (0.1642 / 0.0841 / 0.0732 s):** these depend on hardware. On a different machine I could reproduce only the counters (exactly) and the ratio spread (F11).
- **The "independent recomputation":** I did not audit the authors' `independent_audit_1e9.c` / `stitch_audit.py` sharding. My census serves as a third implementation at 10⁹ instead.
- **The LOS 2016 transition-matrix comparison ("consistent in magnitude"):** I did not re-derive the LOS predictions.
- **The fidelity of the Lean statements to the paper:** the gate passes and I read the key PairExclusion statements, which match. I did not audit all 28 theorems. `LEAN_NOTE.md` exists in `consolidated/`.
- **Book page ranges (Conway–Guy) and the "19 pp" in JPS:** not checked.

## Overall assessment

The theorems are correct, and so is the arithmetic. From my own census code I reproduced every census-level number I tested at 10⁹ exactly: the joint table, δ, χ², all 30 gap cells, both residue-conditioning rows, ω, every cross-base figure, and the Kummer comparison to four digits. The one exception is δ(10⁸) (F5).

What needs revising is the interpretation:

- **F1:** the 93%/97% headline divides numbers taken from different populations whose baseline signs are opposite. The paper's own body concedes this; the proper covariance decomposition gives 98.7%/99.6% and should replace it.
- **F2:** the new section reads a held-out loss from an over-parameterised model as proof that the effect is "absent". The honest conclusion is an upper bound (edge below ~10⁻⁵ nats, not detectable).
- **F3 and F4:** the priority citation has the wrong volume and pages, and the advertised repository does not exist publicly.

The rest (F5–F17) is small numeric slips, merge leftovers and wording. Once F1–F4 are fixed, the paper would be a sound, carefully hedged computational note with correct elementary theorems.
