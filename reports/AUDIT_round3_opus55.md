# Round-3 adversarial audit (narrow scope): opus-5.5

**Verdict: MAJOR REVISION.**

- All the arithmetic reproduces, and the driver's new assertion works.
- Four claims written today do not survive: F1, F2, F3 and F7.
  - The mod-120 "predecessor signal" is explained by residue information.
  - The abstract contradicts §15 on the between shares.
  - The negative held-out gains come from the smoothing, not from the data.
  - The anonymiser can be defeated with one click.
- All four can be fixed by editing text. None touches a theorem, a count or the practical verdict.

## Scope and method

**Files audited:**

| File | sha256 at 19:41 EDT |
|---|---|
| `artin_correlations.tex` | `4adeaa07…813c` |
| `SECTION_content.tex` | `a2c707ee…a5a8` |
| `code/artin_payoff.c` | `0e57d8d6…0090` |

The driver was edited at **19:31**, after the tex was built at 19:24 and while this audit was running. Snapshots are in `opus55_r3_evidence/`. All commands below run from that directory.

**Route (different from rounds 1 and 2):**

1. **New census, `fs_census.c`: a segmented factor sieve on p−1.**
   - Each sieving prime q walks its multiples m and tests 10^(m/q) mod (m+1). The leftover cofactor is the single large prime.
   - It uses no trial division and no per-prime factoring.
   - It also writes a per-prime stream (p mod 840, status, hash bit), which is needed for the serial null in (4).
   - For p < 10^5 it agrees with SymPy: 3617 = 3617.
   - At 10^9: `primes>=7 50847531 artin 19016617 last 999999937`.
   - The resulting cell tables are byte-identical to my round-2 trial-division tables (`identical to r2 tables: True`).
2. **Decomposition in exact rational arithmetic** (`fractions.Fraction`, `exact_decomp.py`). No floating point is involved.
3. **Both null schemes re-implemented in numpy** (`nulls_r3.py`), with the author's statistic and mine run side by side. A C harness runs the author's own `perm_table`/`gnorm` code across several seeds.
4. **A new serial null** (`residue_null.py`). The real prime sequence is kept, so the overlap Y_n = X_{n+1} and all gap dependence are preserved. Each prime's status is then redrawn independently, Bernoulli with the empirical rate of its class p mod L.
5. **Per-cell logistic heterogeneity tests with cross-half replication** (`hetero.py`, `replicate_dc.out`).

## Findings

### F1 — MAJOR — the mod-120 "predecessor signal" is residue information mod 7, not predecessor information

**Quoted text:**
> "a real but very small predecessor signal beyond the mod-120 residues"
>
> "positive evidence of predecessor information"

**Test.** Run the serial null in which the statuses carry **no** information beyond p mod 840. The paper's in-sample statistic at mod 120 is fully reproduced.

**Command:** `python3 residue_null.py 30`

**Output:**
```
observed: mod120 9.1809e-06  mod840 1.8326e-05
null: status ~ Bernoulli(rate[p mod 120])  mod120 gain: mean=2.5491e-06 sd=2.087e-07   -> z=31.8
null: status ~ Bernoulli(rate[p mod 840])  mod120 gain: mean=8.7680e-06 sd=5.518e-07   -> z=0.75
                                           mod840 gain: mean=1.6919e-05 sd=5.299e-07   -> z=2.66
```

**What this means.**
- Everything that is "significant at mod 120" is the predecessor's status acting as a proxy for p mod 7.
- The paper already gives this exact warning for g mod 40: "the apparent signal is residue information re-appearing".
- Against the mod-840 baseline, what is left is marginal (z ≈ 2.7).

**Fix.** Rephrase in the abstract and in §15: "the mod-120 excess is accounted for by residues mod 840; beyond them, at most a marginal residual (z ≈ 2.4–2.8 in-sample, not replicated held out)".

### F2 — MAJOR — the abstract and summary contradict §15, and the new ">98% at mod 840 under every convention" is false

**What §15 says:**
> "Under the Kitagawa ordering with P(C=c|X=1) weights the between share is 96.1% (mod 120) and 98.4% (mod 840) … Every convention agrees that … more than 98% at mod 840"

**What the abstract (tex l.68) and summary (l.188–190) say:**
> "96.1% and 97.8%" … "residual within-cell association (−0.000545, −0.000310)"

**Discussion (iii), l.1634:** still says "93–97%".

**Command:** `python3 exact_decomp.py`

**Output (exact rationals):**
```
mod840  COV identity exact: True  within=-0.000064320 between=-0.014075839 share=0.995451
  [q0:=0]  Kitagawa-w1 identity exact: True within=-0.000225792 share_w1=0.984032 share_w0=0.997103 share_sym=0.990568
  [q0:=q1] Kitagawa-w1 identity exact: True within=-0.000310245 share_w1=0.978059 share_w0=0.997103 share_sym=0.987581
  X=1-only pairs=4020
```

**Why the numbers disagree.**
- The w¹-weighted Kitagawa split at mod 840 is **not well defined**. It depends on what value is given to q⁰_c in the 675 cells that contain no X=0 pairs.
- The driver uses q⁰:=0 (a code comment) and gets 98.4%. My round-2 value, and the abstract's, used q⁰:=q¹ and get 97.8%.
- The paper says "we state ours explicitly", but never states this choice.
- Under the q⁰:=q¹ assignment, which the abstract itself uses, the "every convention >98%" claim fails at 97.8%.
- The defensible statement is **">96% (mod 120), >97.5% (mod 840)"**.

### F3 — MAJOR — the negative held-out gains in Table `tab:holdout` come from the smoothing, not from the data

**Quoted caption:**
> "Both gains are negative: adding the predecessor's status makes held-out prediction slightly worse."

**The cause.**
- 256 (mod 120) and 1,726 (mod 840) X-varying cells have a successor that is **never** Artin: 12.98M pairs in each case, with 0 Y=1 pairs.
- In those cells, splitting by X doubles the Jeffreys smoothing penalty and buys nothing.

**Command:** `python3 smoothing_tax.py`

**Output:**
```
mod120 even->odd: paper statistic -3.1933e-06   same, but constant-successor cells not split +1.8913e-06
       odd->even: paper statistic -4.8925e-06                                                   +9.3662e-08
mod840 even->odd: -5.6248e-05 -> -2.5096e-05 ;  odd->even: -5.6701e-05 -> -2.6888e-05
```

**Consequences.**
- With the obvious correction, the mod-120 held-out gain is **positive in both directions**.
- The "loss" in the caption is a property of the estimator.
- The same smoothing cost produces the permutation null's −1.0e−5 held-out mean.

### F4 — MINOR — item 4: the signal-size estimate is inconsistent in both the author's version and mine; the bias-corrected size is about 7e−6

**The author's formula** (g_in + g_out)/2 averages the **full-N** smoothed in-sample gain (6.59e−6) with a **half-N** held-out gain. The optimism does not cancel at different N.

**My round-2 figure of 3.9e−6 is also inconsistent.** It mixed an MLE in-sample gain with a Jeffreys held-out gain. **I retract it.**

**Command:** `python3 signal_size.py`

**Output:**
```
mod120: AUTHOR (full-N smoothed in + half-N out)/2: 1.699e-06 8.498e-07
        textbook (fit-half in, same stat, + out)/2:  1.302e-06 1.357e-06
        round-2 (fit-half MLE in + out)/2:           3.923e-06 3.948e-06
        LR: G=933.6 df=256  I=(G-df)/(2N)=6.664e-06   95% CI [5.62e-06, 7.81e-06]
```

**Reading the output.**
- Permutation bias correction gives the same answer: 9.181e−6 − 2.53e−6 = 6.65e−6.
- The split-sample estimators all carry the F3 smoothing cost, so they do **not** estimate the signal size.
- "Of order 10⁻⁶" understates the size by about 5–7×. After F1, the size is attributable to residues anyway.

### F5 — MINOR — item 2: k = 1,702 is wrong, 1,599 is right, and the text's own definition gives neither

**Quoted text:**
> "adds one probability per cell whose successor is not constant, and there are k=256 … and k=1,702"

**Command:** `exact_decomp.out`

**Output:**
```
mod120 {'m1&Yvar':256,'m0&m1&Yvar':256,'Yvar':512}
mod840 {'m1&Yvar':1702,'m0&m1&Yvar':1599,'Yvar':3456,'m1only&Yvar':103}
```

**Where the two counts come from.**
- The count 1,702 includes 103 cells that contain **only** X=1 pairs. Splitting such a cell by X adds no parameter.
- The driver edited at 19:31 now requires `m0>0` and prints 1599 (`author_all.json`: `"k": 1599, "k_over_2N": 1.572e-05`). The paper (l.1477) and `results.txt` (l.105–106) still say 1702 and 1.674e−5.
- Read literally, "successor not constant" means 512 and 3,456 cells.

**Fix.** The definition should read: "cells in which **both** X and Y vary", giving 1.57e−5.

### F6 — MINOR — item 3: the z scores differ because of Monte-Carlo noise and a different statistic, not because of the normal shape; "none at 840" is too strong

**Normal shape versus exact draw**, on identical data and statistic (`nulls120.out`):
```
normal ins_author_full z=32.60 | exact ins_author_full z=31.10
normal ho_eo_fitonly   z= 6.06 | exact ho_eo_fitonly   z= 6.07
normal ho_eo_both      z= 9.55 | exact ho_eo_both      z= 9.22
```

**The author's own code across seeds** (`./harness <seed> 200`): z = 26.89, 29.88, 30.97, 34.28, 30.45. The value 26.9 is simply the low end for seed 20260923.

**The held-out difference is a design choice, not an error:**
- the author permutes the fit half only, which gives 6.1 / 4.9;
- my round-2 run permuted both halves, which gives 9.2–9.6 / 6.6–7.0.

Both are valid; they are different conditional nulls.

**The statistic is mislabelled.** The z of 26.9 is computed for the **smoothed** in-sample gain (6.59e−6). The text calls it "the in-sample increment", which it gives as 0.000009 (the MLE, 9.18e−6).

**Mod 840:**
- in-sample z = 2.33 (normal) and 2.79 (exact), i.e. one-sided p ≈ 0.003–0.01;
- the serial null gives 2.66;
- per-cell z correlation between the two hash halves is 0.076 over 1,599 cells, about 3σ.

So the mod-840 result is "marginal", not "no signal detectable".

**Serial dependence.** The cell-exchangeable null is **empirically adequate** here. The serial null (mean 2.549e−6, sd 2.09e−7) matches the cell permutation (2.53e−6, 2.1e−7). But it is the wrong null for the stated claim: see F1.

### F7 — MAJOR — item 10: the anonymiser can be defeated

**What passes.**
- `bash build_submission.sh` in a clean copy exits 0: `pages=28 … anon pages=27 identifying-text hits=0` (`build_r3.log`).
- The anonymous PDF metadata is empty.

**What gets through anyway.**
- The anonymous PDF still contains the author's own Zenodo DOIs:
  - footnote 1: `doi:10.5281/zenodo.22863946`, `…22878204`;
  - ref [18]: "Anonymous … doi:10.5281/zenodo.22865343".
- Crossref/Zenodo API for all three returns `{'name': 'Bald, Joshua', 'orcid': '0009-0002-1317-6489'}`. One click identifies the author.
- The `https://github.com/ANON/...` links in the anonymous PDF are dead.
- `build_submission.sh` says "self-citations removed". They were only renamed.

**The reproduction zip also identifies the author:**
```
unzip -p …reproduction.zip README.md | grep -n Bald  ->  129:**Author:** Josh Bald (Independent Researcher, Ontario, Canada)
grep -rIli bald (unzipped) -> FRONT.tex, DISC.tex, README.md, assemble.py, build_submission.sh, 7 lean/*.lean files
```

**Why the check misses this.** The check greps only `bald|jpbald93|@gmail|@genspark` in the PDF. It never checks DOIs or the supplementary zips.

### F8 — MINOR — stale artefacts

- **Stale binary.** `code/artin_payoff` (built 14:09) ships inside the reproduction zip. It has no `all` or `nullsim` modes: `strings … | grep -c nullsim` → 0.
- **Stale results.** `code/results.txt` predates the 19:31 driver change: it has k = 1702, and no `zero_within_completion`.

### F9 — NIT — item 8 caption: the smoothing is misdescribed

The caption says "Jeffreys (0.5) smoothing is applied to empty and sparse cells". `hold_report` adds 0.5 to **every** cell.

The split description is now **correct**:
- code: `t=mix64(p_succ)&1; (t?K:J)`, then `hold_report(J,K)`, so the model is fitted on even hashes and tested on odd;
- the odd half has 25,421,106 pairs (`halves.out`: `even half 25426424 odd half 25421106`).

## Verdicts on the 10 items

1. **N1 identity — VERIFIED, but the surrounding claim is FALSIFIED.**
   - The printed identity is exactly my round-2 proposal: π_c r_c(1−r_c)δ_c = π_c Cov_c. It holds exactly in rationals.
   - 98.69% and 99.55% are the same quantity I computed; the driver's identity errors are 2e−17.
   - The convention labels are right for mod 120. At mod 840 the w¹ share depends on an unstated q⁰ convention (98.4 vs 97.8).
   - "More than 98% at mod 840 under every convention" is false, and the abstract and summary contradict §15 (F2).
2. **k — 1,599 is right; the author's 1,702 counts 103 X=1-only cells.** The driver has since been fixed, but the paper has not. The text's definition does not match either number (F5).
3. **Permutation null — both implementations are sound.**
   - The normal shape is not the cause: exact and normal agree to within about 1.5 in z.
   - The in-sample difference is seed noise (26.9 to 34.3), and the statistic is mislabelled.
   - The held-out difference is fit-only versus both-halves permutation.
   - Serial dependence does not distort this null. But the null is the wrong one for a "predecessor information" claim (F1, F6).
4. **Split-sample estimate — neither 1.7e−6 nor my 3.9e−6 is a valid estimate of the signal size.**
   - The consistent version gives 1.3e−6, and it is biased by the smoothing.
   - The bias-corrected size is 6.7e−6 [5.6, 7.8]e−6 (F4).
5. **Pooled model and "heterogeneous δ_c" — reproduced, and statistically SOUND, but the mechanism is misattributed.**
   - The pooled held-out gains reproduce: −5.232e−8 and −7.826e−7.
   - Mod 120: G_pooled = 14.7 (1 df), G_hetero = 919.0 (255 df). The δ_c signs split 96 positive / 160 negative.
   - All 13 cells with |z_even| > 3 have the same sign in the odd half, and 11 of them have |z_odd| > 2 (`hetero.out`, `replicate_dc.out`).
   - The heterogeneity is what averaging over p mod 7 inside mod-120 cells predicts (F1; `mod7_explains.out`: sign agreement 0.72).
6. **X-only mass — VERIFIED.** 25,421,524 / 50,847,530 = 0.499956 at mod 120. These are the X=0-only cells.
7. **Withdrawn bound — the replacement wording "model- and workload-limited" is appropriate, and does not overclaim.**
   - The signal claims around it are not consistent: the size is under-stated (F4), the attribution is wrong (F1), mod 840 is marginal rather than absent (F6), and the held-out "worse" is an artefact (F3).
8. **Held-out caption — split direction and N VERIFIED.** The smoothing wording is wrong (F9).
9. **Driver assertion — VERIFIED.**
   - The unperturbed driver exits 0.
   - Changing `C_B += (w1-w0)*p` to `*q0` gives `IDENTITY FAILURE`, exit 1 (`pert1.c`).
   - Dropping X=1-only cells from the within sum gives exit 1 (`pert2.c`).
   - The asserted sums are exactly the printed covariance formula.
10. **Packaging — the build runs as claimed (28 / 27 pp, 0 hits), but the anonymiser is DEFEATED** by the Zenodo DOIs and by the supplementary zip (F7).

## What I could not falsify

- Every count, δ, the cell tables, both decomposition identities, the 98.69% and 99.55% shares, the X-only mass, the pooled and held-out numbers, the split direction and the driver's fail-closed assertion.
- These were reproduced by a third census route and exact arithmetic.
