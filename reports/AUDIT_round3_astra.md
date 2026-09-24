MAJOR REVISION

# Round-3 adversarial audit — Astra (§15 and new driver/build changes only)

**The covariance repair and negative held-out scores survive. The new significance, signal-size, heterogeneity and universal-convention claims do not.** The 1,702 count is not the number of additional identifiable parameters. The anonymous PDF still directly identifies the author through explicit self-identifying DOIs.

Commands below run from `Prime Math/consolidated`; evidence and newly written programs are in `reports/round3_astra/`. I did not change manuscript, driver, anonymiser or build sources. The requested build regenerated submission artifacts. No supplied results file or previous audit census was used as numerical input.

Audited SHA-256:
```
SECTION_content.tex a2c707ee90700b5fcdcaed99203f385e7cb79b34ec6c8470768d40f37bb3a5a8
code/artin_payoff.c f4ec084cce49245cfde913d32cc493ccc1f8de8ffbde25d711c0f5f473277141
build_submission.sh 5159a52b92095587f0d8af974876f552044e297ea24c5f7c627862921a4bfcd8
anonymise.py f77fda8f804e70e83e5e04976c16292ee786c0915931693c3aac4c61949a6564
```

## Independent enumeration

I wrote a new C++ odd-integer smallest-factor sieve, primitive-root test and table generator, not an extraction of the author's routines. It enumerates primes from 7 to 10^9, factors each relevant p−1 using its own sieve, tests powers of 10, and independently implements the specified successor hash. It saves both split tables mod 840; a new Python program aggregates mod 120 and computes all quantities directly. Independent exact hypergeometric sampling uses NumPy, not the driver's sampler. A separate profile-likelihood fit checks the pooled model. Author functions were exercised **only after** generating these independent counts, to identify differences between the advertised and actual statistics and verify the executable assertions.

Exact commands:
```sh
g++ -O3 -Wall reports/round3_astra/enumerate.cpp -o reports/round3_astra/enumerate
/usr/bin/time -p reports/round3_astra/enumerate 1000000000 > reports/round3_astra/enumerate.log 2>&1
python3 reports/round3_astra/analyze.py | tee reports/round3_astra/analyze.log
python3 reports/round3_astra/driver_tables.py
gcc -O3 reports/round3_astra/driver_tables.c -lm -o reports/round3_astra/driver_tables
reports/round3_astra/driver_tables > reports/round3_astra/driver.log
```
Raw output:
```
primes_ge7=50847531 artin=19016617 even=25426424 odd=25421106 pairs=50847530
real 81.75
user 64.50
sys 2.71
MOD 120 N 50847530 total [19758045, 12072868, 12072869, 6943748] delta np.float64(-0.014140158839795136)
MOD 840 N 50847530 total [19758045, 12072868, 12072869, 6943748] delta np.float64(-0.014140158839795136)
```

## Findings (11)

### 1. MAJOR — Covariance identity correct; the claimed convention robustness is not

**Quotes:** “the law of total covariance gives [displayed identity]”; “Every convention agrees that … more than 98% at mod 840”; “Cells in which the predecessor's status never varies carry no identifiable within-cell association”.

**Algebraic check:** Let r=P(X=1). Since Cov(X,Y)=r(1−r)δ, conditional covariance in an occupied, two-sided cell is r_c(1−r_c)δ_c. Also
```
Cov(r_C,p_C)/Var(X)
 = sum_c pi_c (r_c-r) p_c/[r(1-r)]
 = sum_c (w1_c-w0_c)p_c.
```
This proves exactly the printed covariance identity. Set the within contribution to zero when X is constant; δ_c itself is undefined there. An explicit zero-contribution convention should be given at the formula, not an unsupported conditional probability.

**Commands:** `python3 reports/round3_astra/analyze.py`; `python3 reports/round3_astra/theorycheck.py`.
Raw output:
```
MOD 120
cov W,B,error -0.0001856682932822347 -0.013954490546512897 3.469446951953614e-18
shares w1,w0,sym,cov [96.14277359685477, 99.21784795344483, 97.6803107751498, 98.68694336898322]
zero-within convention shares [96.14277359685477, 99.21784795344483, 97.6803107751498]
MOD 840
cov W,B,error -6.431987459877054e-05 -0.014075838965196374 -8.673617379884035e-18
shares w1,w0,sym,cov [98.40318432232147, 99.71034219242215, 99.0567632573718, 99.54512622292648]
zero-within convention shares [97.8059317991588, 99.71034219242215, 98.75813699579048]
RATIONAL_IDENTITY mixed-supported-and-one-sided cells [[3, 1, 1, 3], [6, 2, 1, 1], [0, 0, 2, 3], [4, 1, 0, 0]] delta 75/187 within 196/935 between 179/935 error 0
```
The four labels **are attached to the correct weight formulas in the driver**, but only under its asymmetric missing-support completion: when X=0 never occurs, set q0=0 and use δ_c=q1; when X=1 never occurs, omit that cell's within contribution (equivalently complete q1=q0). The manuscript does not state this convention. It assigns artificial within association to X=1-only cells, contrary to the quoted prose. Using the natural zero-within-association completion for **both** types of X-constant cells yields **97.8059%** for w1 at mod 840, not >98%. For w1, the between formula is Σ(w1−w0)q0; for w0 it is Σ(w1−w0)q1; symmetric uses their average, with consistent completed probabilities. These are exact decompositions too. Thus the universal wording is false. At mod 120, no X=1-only cell exists, so all four quoted values reproduce without this problem. Covariance percentages **98.69/99.55** are independently verified and unambiguous.

Small numerical qualification: my vectorized independent sums reproduce the covariance identity within 2e−17, but the **actual current driver's** independently summed covariance errors are −1.73e−17 and **−8.33e−17**. Its claim of 2e−17 “at both moduli” is not reproducible with this compilation. This is harmless summation-order rounding, not mathematical failure; say machine precision rather than promising 2e−17.

### 2. MAJOR — 1,702 includes 103 cells with no parameter to add

**Quote:** “splitting each cell on the predecessor's status adds one probability per cell whose successor is not constant … k=256 … k=1,702”.

**Commands:** `python3 reports/round3_astra/analyze.py`; `OPENBLAS_NUM_THREADS=1 python3 reports/round3_astra/extra.py`.
Raw output:
```
MOD 120
counts bothX 512 bothXY 256 authorX1Yvar 256 X0emptyYvar 0 Yvar 512 k/2N 2.5173297503339886e-06
MOD 840
counts bothX 3448 bothXY 1599 authorX1Yvar 1702 X0emptyYvar 103 Yvar 3456 k/2N 1.5723477620250186e-05
M 840 spurious-df cells 103 mass 1448 Y1 1086 example [[0, 0, 20, 79], [0, 0, 1, 73], [0, 0, 3, 2]]
MLEnull exact mean 2.5321833466380666e-06 sd 2.2012171059208122e-07 df-reference 2.5173297503339886e-06
MLEnull exact mean 1.680298991395009e-05 sd 5.642119776348016e-07 df-reference 1.5723477620250186e-05
```
The prior auditor's **256/1,599** counts are correct for observed cells with both X and both Y margins present. The author's code tests `m1>0 && t1>0 && n-t1>0` but **not m0>0**. The 103 extra cells have X identically 1: conditioning on X cannot add a fitted response probability. “Whose successor is not constant” alone is also insufficient, since there are 512/3,456 Y-variable cells including X=0-only cells.

`k/(2N)` is the regular iid **null likelihood-gain expectation** for k additional identifiable interior parameters, not training-versus-test optimism (whose leading scale is k/N). Sparse margins and dependence mean neither expression is certified here. Exact fixed-margin Monte Carlo on the independent full table gives **1.6803e−5**, remarkably close to the author's erroneous 1702/(2N)=1.6736e−5—but for a different reason: finite/sparse-cell corrections. That numerical coincidence does not validate the dimension count. At mod 120 the reference scale is sound as a heuristic; “indistinguishable” at mod 840 requires the actual null and a specified threshold, not a comparison of orders of magnitude.

### 3. MINOR — The normal approximation measurably biases the mod-840 null; its final moments are not exact

**Quotes:** “redrawing each 2×2 table from the hypergeometric distribution”; driver: “Mean and variance are exact; the normal shape is the right approximation for an aggregate over ~10^4 cells.”

**Command:** `OPENBLAS_NUM_THREADS=1 python3 reports/round3_astra/nullcheck.py > reports/round3_astra/nullcheck.log 2>&1`.
Each row reports [in-sample, even→odd, odd→even], with **2,000 replicates per statistic**, independent of the author's RNG. Raw output:
```
MOD 120 observed [ 6.59212927e-06 -3.19332286e-06 -4.89245792e-06]
normal R 2000 mean [-3.21046418e-08 -1.01255924e-05 -9.99615650e-06] sd [2.24302509e-07 1.12888331e-06 1.04525862e-06] z [29.53258941  6.14082033  4.88271368]
exact R 2000 mean [-3.40276258e-08 -1.00455621e-05 -9.99311280e-06] sd [2.24667121e-07 1.09987089e-06 1.01987786e-06] z [29.49322035  6.23003962  5.0012409 ]
MOD 840 observed [-5.38613155e-08 -5.62477175e-05 -5.67006549e-05]
normal R 2000 mean [-1.39290718e-06 -5.89026512e-05 -5.75670115e-05] sd [5.67118617e-07 1.86328868e-06 1.89194730e-06] z [2.36113896 1.42486439 0.45791796]
exact R 2000 mean [-1.57316249e-06 -5.82621868e-05 -5.68751412e-05] sd [5.62063084e-07 1.87382631e-06 1.83550009e-06] z [2.70307946 1.07505654 0.09506199]
SPARSE n=10,m=1,y=1 exact mean,var 0.1 0.09 rounded clipped normal mean,var 0.09121121972586782 0.08289173312198728
```
The author plugs exact μ and variance into a Gaussian, **then rounds and clips**. The resulting integer draw no longer has exact hypergeometric moments, as the explicit one-Bernoulli example proves. Aggregation can normalize fluctuations, but cannot remove bias from a nonlinear log-score. At mod 840 the in-sample null-mean shift is 1.80e−7, about ten combined Monte Carlo mean standard errors; the held-out shifts are about 6.4e−7/6.9e−7. z changes by roughly 0.3–0.4. At mod 120 I did not find an approximation effect large enough to reverse the substantive comparison; both samplers give a large standardized excess. Replace the sampler with exact hypergeometric draws and disclose Monte Carlo uncertainty. Neither a 200-replicate SD nor an approximately Gaussian statistic licenses interpreting z≈27 as a calibrated Gaussian tail probability.

### 4. MAJOR — The printed increment is not the tested increment, and this is not a calibrated prime-sequence significance test

**Quotes:** “the in-sample increment … z=+26.9 … z=+2.4”; “positive evidence of predecessor information”; “no signal beyond the null”.

**Commands:** `python3 reports/round3_astra/analyze.py`; `reports/round3_astra/driver_tables`.
Raw output:
```
MOD 120 mle full 9.180854327770054e-06 gin full smoothed 6.592129265675249e-06
MOD 840 mle full 1.8326383867817596e-05 gin full smoothed -5.386131553928991e-08
MOD 840 fit bit 0 hold_nullfunctional -5.624771754258148e-05 hold_reportfunctional -6.110788522907281e-05
MOD 840 fit bit 1 hold_nullfunctional -5.6700654870189234e-05 hold_reportfunctional -6.0763924363478685e-05
"null120": {"replicates": 200,
 "in_sample": {"observed": 6.592129e-06, "null_mean": -2.506441e-08, "null_sd": 2.461273e-07, "z": 26.89},
 "heldout_even_to_odd": {"observed": -3.193323e-06, "null_mean": -1.014912e-05, "null_sd": 1.143147e-06, "z": 6.08},
 "heldout_odd_to_even": {"observed": -4.892458e-06, "null_mean": -9.945915e-06, "null_sd": 1.022689e-06, "z": 4.94}}
"null840": {"replicates": 200,
 "in_sample": {"observed": -5.386132e-08, "null_mean": -1.392284e-06, "null_sd": 5.585983e-07, "z": 2.40},
 "heldout_even_to_odd": {"observed": -5.624772e-05, "null_mean": -5.902058e-05, "null_sd": 1.739040e-06, "z": 1.59},
 "heldout_odd_to_even": {"observed": -5.670065e-05, "null_mean": -5.775800e-05, "null_sd": 1.775026e-06, "z": 0.60}}
```
All six stated z numbers reproduce **as driver outputs**. But the entropy table uses unsmoothed MLE mutual information; the null tests `hold_gain(F,F)`, a Jeffreys-smoothed score difference. At mod 840 its observed “increment” is actually **negative**, −5.39e−8, not +1.83e−5. Internally, in-sample and held-out null statistics use the same `hold_gain` scoring functional with different fit/test arguments; they are not the same as the displayed in-sample diagnostic. Additionally `hold_gain` falls back to the residue probability if either fit X margin is empty, whereas the table's `hold_report` does not. This changes the mod-840 held-out statistic by 4–5e−6. At mod 120 the latter mismatch is absent.

A fixed-margin hypergeometric null is appropriate for **iid exchangeable pairs within each cell under X ⟂ Y | C**, or as a clearly labelled artificial contingency-table benchmark. Here Y_i=X_(i+1). Independent per-cell permutations do not preserve that constraint, serial structure, drift within cells, or cross-cell coupling. Fixed margins do not repair exchangeability. The hash split does not remove overlap between training and test sequences. For held-out tests, only the training table is randomized and the observed test table is frozen; this is a conditional label-randomization benchmark, not a resimulation of a joint independent train/test experiment.

Concrete executable counterexample (output saved in `dependence.log`):
```sh
python3 - <<'PY'
x=[0,1,1,0];p=list(zip(x[:-1],x[1:]));q=[(p[0][0],p[2][1]),p[1],(p[2][0],p[0][1])]
print('original_pairs',p,'response_permutation_preserves_margins',q,'overlap_valid',all(q[i][1]==q[i+1][0] for i in range(len(q)-1)))
PY
```
```
original_pairs [(0, 1), (1, 1), (1, 0)] response_permutation_preserves_margins [(0, 0), (1, 1), (1, 1)] overlap_valid False
```
This does not prove serial dependence causes the observed excess, and I do **not** assert it does. It proves the advertised randomization is not automatically a valid null on consecutive-prime sequences. z is meaningful as standardized departure from that **chosen simulation**, not a calibrated significance test without an exchangeability/dependence argument. “Real”, “significant” and “no signal” are stronger than established. Even under a Gaussian convention, +2.4 is not generically “no signal” (the significance threshold/multiplicity rule is unstated); the exact-null standardized in-sample excess here is +2.70. Report benchmark departures and uncalibrated uncertainty, or supply a justified sequence-aware inference procedure.

### 5. MAJOR — The new 1.7e−6/8.5e−7 signal estimator averages unmatched sample sizes and penalties

**Quote:** “a real but very small predecessor signal … of order 10−6 nats by the split-sample estimate (g_in+g_out)/2 (1.7×10−6 and 8.5×10−7 in the two directions)”.

**Commands:** `python3 reports/round3_astra/analyze.py`; `python3 reports/round3_astra/theorycheck.py`.
Raw output:
```
MOD 120 gin full smoothed 6.592129265675249e-06
train gin 5.797618300532689e-06 7.606162808649767e-06
fit bit 0 estimate full-half 1.6994032018593412e-06 estimate matched-half 1.302147719288061e-06
fit bit 1 estimate full-half 8.498356711355035e-07 estimate matched-half 1.3568524426227625e-06
NULL_EXPECTED_naive_full-half_average -1.2586648751669943e-06 correct_full_half_iid_combination (2*gin_full+gout_half)/3
```
The manuscript numbers exactly recover an average of the **full-data smoothed** resubstitution gain and a **half-trained** held-out gain—not training and test performance of the same fitted model. For an ideal regular iid MLE with true gain I and k additional parameters, E[g_in(N)]≈I+k/(2N), while E[g_out trained on N/2]≈I−k/N. Their average is I−k/(4N), **not I**; at mod 120 this residual leading bias alone is −1.26e−6, comparable to the claimed signal. Under those assumptions, the full/half combination cancelling these particular penalties would be (2g_in+g_out)/3, not 1/2. That is an illustration, not a validated estimator for these sparse dependent data.

Furthermore the actual score uses asymmetric Jeffreys penalties even in Y-constant cells, not the regular interior-MLE setup. The null simulations show near-zero full smoothed gain but ~−1e−5 held-out gain at mod 120, inconsistent with a simple symmetric optimism cancellation. Matched-half descriptive averages are 1.30e−6 and 1.36e−6, but remain neither an oracle-information estimate nor a bound without assumptions/bias/uncertainty analysis. A weighted sum of empirical log scores is interpretable **as that score functional**; calling it the size of a “real signal” is not justified. The present correction can understate information through unmatched penalties while simultaneously overclaiming its certainty/existence.

### 6. MAJOR — A failed pooled fit does not establish heterogeneous, much less sign-varying, true effects

**Quote:** “a pooled offset buys nothing while cell-level information is significant … so the signal is heterogeneous across cells (a sign-varying δ_c) rather than a common shift”.

**Commands:** `python3 reports/round3_astra/poolcheck.py`; `python3 reports/round3_astra/theorycheck.py`; `reports/round3_astra/driver_tables`.
Raw output:
```
M 120 beta -0.009136463960550623 pooled_gain_vs_Jeffreys -5.232212660355451e-08 alphaMLE_beta0_vs_Jeffreys -2.7179729772397465e-08 pooled_gain_vs_matched_MLE -2.5142396831157043e-08
M 840 beta -0.004693483482741733 pooled_gain_vs_Jeffreys -7.853445012084848e-07 alphaMLE_beta0_vs_Jeffreys -7.150106088593677e-07 pooled_gain_vs_matched_MLE -7.033389234911702e-08
"pooled840": {"beta": -0.004601, "heldout_gain": -7.826e-07}
ALL_POSITIVE_TRUE_DELTAS [0.00090361 0.00209579] OVERESTIMATED_COMMON_SHIFT_TEST_GAIN -5.994739080567024e-05
```
The rounded −5e−8/−8e−7 claims reproduce, including the independent profiled fit. They show **no observed advantage on this split**. They do not identify why. Alternatives include coefficient estimation noise, intercept refitting penalties, shrinkage/misspecification, small power, the uncalibrated table-null significance above, and common-sign effects with varying magnitudes. The explicit last line is a population-score counterexample: true probabilities have the **same positive logit shift .01 in both cells**, but estimating it as .04 makes held-out loss worse. Thus the qualitative implication is invalid even without serial dependence. Heterogeneity also does not logically imply sign variation.

A concrete confound in these data: at mod 840 **−7.15e−7 of the ~−7.85e−7 loss** is already incurred by replacing Jeffreys intercepts with MLE intercepts **at β=0**. Almost none is attributable to adding β under a matched-intercept comparison. The model is not generally regularized: active-cell α and β are fitted without a penalty; only unsupported/boundary cells retain Jeffreys probabilities. “Within noise” is not supported by a reported error estimate.

There is also a small fit inconsistency: the β-score includes X=0-empty cells although α fitting and evaluation exclude them. Fresh count output (`dependence.log`):
```
M 120 X0-empty fit cells included in beta-score but excluded from alpha-fitting and evaluation 0 fit pairs 0
M 840 X0-empty fit cells included in beta-score but excluded from alpha-fitting and evaluation 102 fit pairs 1705
```
This explains the slight β discrepancy against the independent fit (−.004601 versus −.004693). It does not reverse the rounded negative result. Test interactions directly with a justified uncertainty model; do not infer sign-varying latent effects from this contrast.

### 7. NIT — X-constant mass and corrected hash caption withstand falsification

**Quotes:** “about half of all pairs at mod 120”; “fitted … EVEN 64-bit hash (bit 0) … odd-hash half (25,421,106 pairs)”.

**Commands:** `reports/round3_astra/enumerate 1000000000`; `python3 reports/round3_astra/analyze.py`.
Raw output:
```
even=25426424 odd=25421106 pairs=50847530
MOD 120 massX0only 0.49995592706273045 massX1only 0.0 massXconstant 0.49995592706273045
MOD 840 massX0only 0.49995889672517035 massX1only 7.905988747142684e-05 massXconstant 0.5000379566126417
MOD 120 fit bit 0 hold_reportfunctional -3.193322861956567e-06
MOD 840 fit bit 0 hold_reportfunctional -6.110788522907281e-05
```
**Cannot falsify either correction.** Mod-120 X-constant mass is 49.9956%, all X=0-only. Code uses the least significant bit of SplitMix64(successor), fits bit value 0, tests value 1. Both caption direction and exact test count match. “Bit 0” would be clearer as “least significant bit equal to 0”; it currently mixes a bit position with a bit value, but the even/odd wording disambiguates it. Code's `x_only_mass` only counts X=0-only, not all X-constant cells; this makes no difference to the specific mod-120 claim.

### 8. MAJOR — The withdrawn-bound disclaimer is correct, but adjacent universal economic claims survive it

**Quotes:** “model- and workload-limited finding, not … an information-theoretic bound: nothing here bounds the oracle conditional mutual information, other estimators or other ranges”; immediately before it, “an edge several orders of magnitude below **any threshold** at which a predictor could pay for itself”; section opening: “could it predict generator availability, or yield a faster algorithm? … the answer is no”.

**Command:**
```sh
sed -n '8,12p;99,113p' SECTION_content.tex
```
Raw output (relevant lines):
```
whether the correlation is \emph{computationally} worth anything --- could it predict generator
availability, or yield a faster algorithm? We answer by measurement, and the answer is no.
...
mod $840$; and in either case an edge several orders of magnitude below any threshold at which a
predictor could pay for itself.
...
workload-limited finding, not as an information-theoretic bound: nothing here bounds the oracle
conditional mutual information, other estimators or other ranges.
```
The disclaimer **itself is correct and appropriately scoped**; I cannot falsify it. But it contradicts the nearby “any threshold” inference. No universal conversion exists from average nats per pair to computational payoff without a decision problem, predictor acquisition/evaluation cost, and cost/benefit of errors. A tiny average gain can matter in a rare costly subset or at negligible marginal prediction cost. Conversely it may be worthless in the tested scanning workload. Negative scores for these estimators establish no demonstrated gain, not impossibility of prediction or of a faster algorithm. Keep the disclaimer and restrict the opening/conclusion consistently; remove the unquantified universal threshold. Finding 5 also removes the evidentiary foundation for that asserted “edge”.

### 9. NIT — The new assertion is real and checks the displayed covariance formula

**Quote:** “the driver accumulates both sums independently and asserts it”.

**Exact commands:**
```sh
python3 reports/round3_astra/harness.py
for v in normal perturbed; do
  gcc -O2 reports/round3_astra/harness_$v.c -lm -o reports/round3_astra/harness_$v
  reports/round3_astra/harness_$v
  echo EXIT=$?
done
```
The harness preserves the author function, renames its original main, and calls it on `[3,1,1,3]` and `[6,2,1,1]`. The perturbed copy changes **only** `C_B += (w1-w0)*p;` to `C_B += 1.01*(w1-w0)*p;`. Original driver is untouched.
Raw output:
```
normal:
"covariance": {"within": 0.350000000, "between": 0.066666667, "between_share": 0.1600, "identity_error": 5.55e-17}
"ASSERT": "both identities hold"
EXIT=0
perturbed:
"covariance": {"within": 0.350000000, "between": 0.067333333, "between_share": 0.1616, "identity_error": 6.67e-04}
"ASSERT": "IDENTITY FAILURE"
"ASSERT_FAIL_test": true
EXIT=1
```
**Cannot falsify the repair.** Unlike round 2, `C_W` and `C_B` are independently accumulated from cell counts, and the printed unconditional `p_c` between formula is exactly the asserted one. Failure is genuinely nonzero. The tolerance is 1e−12, not 2e−17; the latter is merely a claimed observed residual (see finding 1).

### 10. MAJOR — Build succeeds and reports zero hits, but the actual anonymous PDF still identifies the author

**Quotes:** build: “author block, addresses, repository URLs and self-citations removed”; anonymiser docstring: “any identifying name, address, ORCID or repository URL”; claimed “27 pp, 0 hits”.

**Exact commands:**
```sh
./build_submission.sh > reports/round3_astra/build.log 2>&1
python3 reports/round3_astra/packagecheck.py | tee reports/round3_astra/packagecheck.log
pdftotext submission/artin_correlations_anonymous.pdf reports/round3_astra/anon.txt
sed -n '96,104p' reports/round3_astra/anon.txt
```
Raw output:
```
== named manuscript
   pages=28 overfull_hbox=0 undefined=0
== anonymised manuscript (author block, addresses, repository URLs and self-citations removed)
   anon pages=27 identifying-text hits=0
== done
ANONYMISER_PROBE_PDF_TEXT
Joshua B.
ORCID: 0000-0002-1825-0097
https://research.example.org/joshua-b/
joshua@proton.me
BUILD_IDENTIFYING_PATTERN_HITS 0
```
All four injected identifiers survive anonymization **and PDF extraction**, yet the build's exact identifying-text pattern counts zero. This is a finite literal substitution/grep list, not an identifier scrubber. More importantly, the **unmodified delivered anonymous PDF** says:
```
1An earlier version of parts of this work appeared as the preprints Correlations between primitive root
statuses of consecutive primes (Zenodo, doi:10.5281/zenodo.22863946) and Cross-base correlations of Artin
primes (Zenodo, doi:10.5281/zenodo.22878204). This paper consolidates those two preprints and adds the audit
of §15; it supersedes them, and the preprints are retained for the record.
```
This explicitly links the submission's author to named preprints; a self-citation has not been removed or made third-person. Fresh DOI lookup, not an inference from memory:
```sh
python3 - <<'PY'
import requests,json
r=requests.get('https://api.datacite.org/dois/10.5281/zenodo.22863946',timeout=30)
print(r.status_code);a=r.json()['data']['attributes']
print(json.dumps({'doi':a['doi'],'creators':a['creators'],'titles':a['titles']},indent=2))
PY
```
Raw response fields:
```
200
"doi": "10.5281/zenodo.22863946"
"name": "Bald, Joshua"
"nameIdentifier": "0009-0002-1317-6489"
"title": "CORRELATIONS BETWEEN PRIMITIVE ROOT STATUSES OF CONSECUTIVE PRIMES"
```
The source's repository URLs are not removed either, but transformed into fake `https://github.com/ANON/...` URLs. Actual PDF metadata has empty Author/Title, so I found no metadata leak there. **27 pages and 0 literal-pattern hits are true; successful anonymization is false.** Scrub explicit provenance/self-identification in the blind version, use a positive audit of names/identifiers/links, and distinguish retained third-person citations from explicit “our earlier version” provenance. No generic regex can certify double blindness by itself.

### 11. MINOR — The newly required anonymiser is missing from both delivered archives

**Quote:** build produces a “source zip and a reproduction zip”; its new build step invokes `python3 anonymise.py`.

**Exact commands:**
```sh
python3 reports/round3_astra/packagecheck.py
mkdir -p reports/round3_astra/zipcheck
unzip -qo submission/artin_correlations_source.zip -d reports/round3_astra/zipcheck
(cd reports/round3_astra/zipcheck; python3 anonymise.py artin_correlations.tex anon.tex; echo ANONYMISE_EXIT=$?)
```
Raw output (`True/False` are presence of build script/anonymiser):
```
submission/artin_correlations_source.zip True False
submission/artin_correlations_reproduction.zip True False
python3: can't open file '.../reports/round3_astra/zipcheck/anonymise.py': [Errno 2] No such file or directory
ANONYMISE_EXIT=2
```
Shipping the new build script fixed one round-2 omission, but its **new dependency** is omitted from both archives. Even if the earlier external assembly-input problem is separately solved, the new anonymous build path cannot run from the archive. Include and exercise all dependencies in a clean extraction. This is precisely new packaging breakage introduced by the fix; I have not broadened this round into a re-audit of all old archive issues.

## Explicit verdict on all ten requested items

| Item | Verdict |
|---|---|
| **1. Covariance identity, conventions, shares** | **Identity and covariance shares PASS.** Four printed labels match driver formulas under its undocumented asymmetric unsupported-cell completion. Universal >98% robustness **FAILS**: a legitimate zero-within completion gives 97.8059%. Driver's own mod-840 residual is 8.33e−17, not ≤2e−17. Finding 1. |
| **2. 256/1,702 and k/(2N)** | **FAIL.** Correct observed interior dimensions 256/1,599; 103 X=1-only cells spuriously counted. k/(2N) is a heuristic null MLE-gain scale, not general optimism; finite-cell exact-null mean happens to lie near erroneous 1702/(2N). Finding 2. |
| **3. Permutation null and z** | **Six reported driver z values reproduce; inference FAILS.** Rounded/clipped normal is not hypergeometric and biases mod-840 calibration; tested in-sample statistic differs from entropy table, and mod-840 held-out statistic differs from holdout table. Sequence exchangeability/dependence justification is absent. Findings 3–4. |
| **4. Split-sample estimate** | **Arithmetic PASS; interpretation FAIL.** 1.6994e−6/8.4984e−7 are full-smoothed/half-heldout averages, not validated signal-size estimates. Sample-size and regularization penalties do not cancel. Finding 5. |
| **5. Pooled model and heterogeneity** | **Rounded held-out gains PASS; inference FAIL.** No advantage does not establish latent heterogeneity/sign changes. Intercept refitting dominates mod-840 loss; β-score mask also inconsistent. Finding 6. |
| **6. X-only mass** | **PASS.** 49.9955927063% at mod 120; cannot falsify “about half”. Finding 7. |
| **7. Withdrawn upper bound** | **Disclaimer PASS; surrounding conclusion still overclaims.** No numerical information upper bound is now expressly asserted there, but “any threshold”/generic “answer is no” remain unjustified. Finding 8. |
| **8. Held-out caption** | **PASS.** Fit even bit value 0, test odd value 1, independently counted 25,421,106 test pairs. Finding 7. |
| **9. Independent sums and exit failure** | **PASS.** Exact printed covariance formula is checked; one-term perturbation triggers exit 1. Finding 9. |
| **10. Packaging/anonymization** | **Build regeneration/counts PASS; anonymity and archival reproducibility FAIL.** Fresh 27-page PDF has 0 pattern hits but explicit identifying self-provenance; ORCID/name/URL/email probes evade detector; new anonymise.py missing in both ZIPs. Findings 10–11. |

**Narrow disposition:** retain the repaired covariance arithmetic, independently reproducible negative scores, X-constant mass, corrected caption and real assertion. Remove or justify the new significance/estimator/heterogeneity/economic claims, correct parameter/support conventions, and rebuild an actually blind, self-contained submission package. No excluded-gap arithmetic or elementary exclusion theorem was challenged by this round.
