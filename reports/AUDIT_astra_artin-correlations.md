# Adversarial audit — Correlations of Artin status among primes

## Verdict (ACCEPT / MINOR REVISION / MAJOR REVISION / REJECT)

**MAJOR REVISION.** The principal exclusion theorems survive examination, and a genuinely independent census reproduces the main consecutive-prime results through **10^9**. However, the new computational-content section contains a false character identity, calls resubstitution conditional entropies predictive performance, and makes an unsupported universal impossibility claim. The consolidated front matter reintroduces false assertions that the old body explicitly disclaims. There are also incorrect bibliography records and conflicting artifacts in the reproduction package. These are not grounds to reject the underlying census, but they prevent acceptance of this manuscript as written.

## What I verified independently (method + result)

All audit arithmetic below was executed in code. Audit sources and outputs are in `reports/astra_evidence/`; none of the independent census/model code imports the authors' implementations.

* **Fresh census:** I wrote `independent_census.cpp`, using an odd-integer smallest-prime-factor sieve, exact integer factor extraction, and my own modular-power/primitive-root routines. This is a separate implementation, not a rerun of either supplied census. I ran it to 10^7, 10^8, and **10^9**. At 10^9 it returns 50,847,531 primes starting at 7, 19,016,617 Artin primes, and the exact pair table `[19758045,12072868;12072869,6943748]`. Consequently delta = **−0.014140158840**, Pearson = **10166.663432261**, and the omega correlation = **−0.041023655738**. The conditional probabilities round as claimed. See `census_1e9.txt`.
* **Gap claims:** independently recovered the gap-20 table `[439086,543417;841540,0]`, gap-60 table `[95684,138855;137300,0]`, and gap-40 table `[339775,117249;19298,172458]`. The gap-20/60 total is 2,195,882, with zero joint Artin events. Deltas at 40 and 60 are +0.642812798601 and −0.592033734262. Across **all** gaps 20 mod 40 the counts are exactly **2,214,511 / 984,678 / 0** (total / Artin predecessor / joint Artin), with zero character-flip failures. The 10^7 count of 23,956 also reproduces.
* **Residue tables:** independently recovered all published thresholded counts and statistics: mod 120 has 145 cells, 12,255,203 pairs, delta −0.001054700824 and sum chi-square 789.223015; mod 840 has 633 cells, 12,086,765 pairs, delta −0.000473323459 and sum chi-square 719.813118. The arithmetic magnitude reductions round to 93% and 97%; their interpretation is a separate problem below.
* **New entropy table:** independently reproduced marginal entropy **0.661046629240**, mod-120 conditional entropies **0.241529498993 / 0.241520318139**, and mod-840 conditional entropies **0.237235118009 / 0.237216791625**. Incremental empirical information is **0.000009180854 / 0.000018326384** nats. All displayed six-place values and the conversions to bits are consistent with these unrounded numbers. Gap-class-only incremental information is **0.021841224359** nats, supporting the approximately 0.02 comparison, but not the word “spurious” without qualification.
* **Actual held-out test:** in the same independent census, fitted Jeffreys-smoothed cell probabilities using pairs with successor below 5×10^8, then scored the remaining pairs. Adding predecessor status changes held-out log loss by **−0.000005900559** nats improvement at mod 120 and **−0.000078961067** at mod 840: both are deteriorations. This is evidence against useful prediction for these specific fitted models, not an impossibility theorem. The split and smoothing are explicit in my code.
* **Independent cross-base censuses:** ran all twelve bases and all 66 pairs through **10^8**, separately from the supplied data. At 10^7 and 10^8 there are 64 positive pairs; at 10^8 mean phi is 0.035153597, omega-conditioned mean 0.014627610, and signature-conditioned mean 0.001752202. See `crossbase_1e8.txt` and `census_1e7.txt`.
* **Cross-base 10^9 aggregates:** wrote a separate analyzer, `recompute_tables.py`, operating on the supplied raw contingency tables rather than their summaries. It obtains means **0.03520593 / 0.01580834 / 0.00185646**, with only (2,10) and (3,15) negative; the advertised 55%/95% signed-mean arithmetic is correct. Mean-absolute residuals are **0.0178713016** for the 15 triple-completed pairs, **0.0077358272** after removing (2,6), and **0.0009711223** for the other 51. Thus the rounded 0.018 / 0.008 / 0.001 figures reproduce. Zero unequal-character joint-Artin events across all 66 pairs also follows from independently summing the raw tables.
* **Independent Kummer numerical evaluation:** independently implemented the ramified inclusion–exclusion sum, enumerating four states per ramified prime and counting the quadratic subgroup, with a separate sieve/Euler product through 3,000,000. Against the supplied 10^9 counts, mean absolute relative error is **0.0121232378%**, maximum **0.0395651563%**, at (21,29). The corrected mathematical comparison reproduces; one shipped result file does not.
* **Independent large-prime search:** used SymPy's prime enumeration/factorization/order tests, not the supplied driver, to enumerate until 300 base-10 successes above 10^15. Exact counters: **722 primes, 376 nonresidues, 300 early-exit survivors, 402 small-channel exponentiations**, final prime **1000000000027097**. The supplied driver was additionally compiled and run three times; the exact counters agree, while timings differ as expected on different hardware.
* **Proof checks:** checked the gap proof's mod-5 preservation and mod-8 flip; enumerated all reduced classes mod 40; only shifts 0 and 20 have universally fixed product signs. Checked the pair proof's separate `p | ab` case, nonprincipal conductor/density argument for d>1, and d=1 vacuity. Checked the triple implication and the necessity of odd p. Direct small-prime tests over bases 1 through 25, including squares and square factors, found no pair/triple counterexample (`independent_checks.py`). The stated simultaneous primitive-root examples for 2 and 8 at 3,5,11,29,53 are correct. At p=2, all of 3,5,15 are primitive roots, as required for the odd-prime caveat.
* **Lean:** ran `lean/./gate.sh`; result **PASS (28 theorems, standard axioms only)**. Inspected the checked theorem signatures rather than treating the gate as proof of every prose assertion. No `sorry` or added axiom problem was found in the checked files.
* **References:** parsed every label/reference/citation: no missing cross-reference, no duplicate label, no missing bibliography key. Checked all 25 bibliography entries against Crossref, DataCite/arXiv, or publisher records; details below. Inspected the assembly script and the existing TeX log. The log has no unresolved-reference warning.

## Findings (numbered; each: severity BLOCKER/MAJOR/MINOR/NIT; location; what is wrong; evidence; suggested fix)

### 1. MAJOR — The new section gives the wrong sign for its central character product

**Location:** `artin_correlations.tex:1413–1416`.

**Wrong:** The product for a gap 20 mod 40 is asserted to be **+1**, “so the two characters are opposite.” Opposite nonzero signs have product **−1**, not +1. This directly contradicts the correctly proved exclusion theorem.

**Evidence:** Independent direct computation at p=23, p+g=43 gives `(10/23)=−1`, `(10/43)=+1`, product **−1**. The exhaustive independent census found zero failures of the negative-product identity. These primes need not be consecutive; the theorem and this identity do not require consecutiveness.

**Fix:** Replace +1 by −1. Also say the fixed sign follows from reciprocity and the shift, together with multiplicativity; multiplicativity alone does not establish which sign the shifted product takes.

### 2. MAJOR — The advertised “predictive information” is measured on the same observations used to fit every probability

**Location:** new section, especially lines 1379–1404; `code/artin_payoff.c:99–115`.

**Wrong:** `ce_report` estimates each conditional probability from a cell's counts and scores those same counts. This computes plug-in empirical conditional entropy, and its difference computes empirical conditional mutual information. It is not held-out predictive cross-entropy, and a finer nested model is guaranteed not to have worse resubstitution loss. At the claimed 10^−5-nat scale this distinction is material.

**Evidence:** The code explicitly uses `pc=m/s`, `q=n11/p1`, and `r=n01/p0` inside the log likelihood for the very same m,n11,n01 counts. My independent half-range holdout yields gains **−5.900559×10^−6** and **−7.8961067×10^−5** nats, not the positive values in the table. There are 256 and 1,599 nondegenerate full-census cells respectively. For orientation only, the usual independent-multinomial leading resubstitution scales df/(2N) would be 2.52×10^−6 and 1.57×10^−5 nats; the latter is comparable to the entire displayed 1.83×10^−5 gain. I do not assume that iid calibration is valid for these serially structured data.

**Fix:** Call the existing table an **empirical conditional-entropy diagnostic**. Add an explicitly specified held-out or cross-fitted predictive experiment, smoothing/sparse-cell policy, and sensitivity to the split. Do not call the 10^9 values “converged” (`code/results.txt`) without a bias/sparsity analysis.

### 3. MAJOR — A small empirical gain does not imply that no predictor can improve on residues

**Location:** lines 1403–1405 (“no predictor built on it can beat one built on the residues”), 1365–1370, 1465–1478, and abstract/summary claims of computational redundancy and “only” usable filters.

**Wrong:** An absolute no-predictor assertion is not established by one pair of tabular models and one scanning workload. It conflicts with the section's later statement that this is not an impossibility theorem. On the empirical distribution the displayed positive conditional information itself means the fitted augmented probabilities have *strictly better* log loss. “Not useful at a demonstrated workload/cost scale” is not “cannot beat.” Likewise, absence of an evaluated cross-base predictive workload is not a measurement proving that axis has no computational content.

**Evidence:** Independently reproduced improvements of 9.180854×10^−6 and 1.8326384×10^−5 nats are positive in-sample; the held-out experiment supports a narrower negative result only. The same section measures a second useful filter—small-ell early rejection—after saying the nonresidue test is the “only filter that does real work” (1428). Lines 1467–1475 list untested conditions but end “None of these holds,” exceeding what was tested. The “statistically overwhelming” opening also abandons the body's explicit descriptive/noncalibrated interpretation of chi-square and phi/sigma0.

**Fix:** Restrict the conclusion to **no demonstrated net advantage beyond these baselines in the tested workloads**. State the scope of “redundant”: once the relevant individual quadratic characters are already available, these support exclusions add no further rejection. Explicitly cover the same-prime **pair** law; the new paragraph actually discusses the triple law in its place. Remove the universal no-predictor claim and retain the earlier statistical caveats.

### 4. MAJOR — The signature is falsely presented as all information required by the primitive-root criterion

**Location:** preliminaries 210–221; repeated in the census description around 625 and in 1093–1096.

**Wrong:** Small bases do not make large prime-divisor channels unnecessary. The finite signature neither records all prime divisors of p−1 nor all relevant power-residue outcomes. “Records exactly the information … the Artin criterion consults for small bases” is false.

**Evidence:** Independent exact counterexample: **p=137**, p−1=2^3·17, ord_137(10)=8. The quadratic nonresidue test passes, and there are no odd q≤13 channels to reject, but the **q=17** primitive-root test fails. More strongly, **139 and 223** have identical signature `(1,{3},1)` and both have `(10/p)=−1`, yet ord_139(10)=46 (not Artin) and ord_223(10)=222 (Artin). Their factorizations are 2·3·23 and 2·3·37.

**Fix:** Describe the signature as an empirically useful **coarse summary** of small-prime divisibility plus a truncated large-factor count. Separate divisibility information from actual power-residue tests. Delete “exactly” and the assertion that the criterion only consults the small part.

### 5. MAJOR — The consolidated conclusions assert a decomposition explicitly denied in the statistical body

**Location:** abstract 65–79; summary items 5 and 8; discussion 1495–1499; cross-base conclusion 1106–1111; compare residue-conditioning discussion around 817–863.

**Wrong:** The new discussion calls the 93–97% comparisons an “elementary decomposition showing” what the correlations are made of. The body correctly says these are different estimands on different selected populations, not a decomposition, and signed cancellation need not remove dependence. This is an internal contradiction, not a complaint that the numerical ratios are incorrectly calculated.

**Evidence:** Independent census recovers a global population of 50,847,530 versus selected populations 12,255,203 and 12,086,765. The computed magnitude ratios are 92.5411% and 96.6526%. They do not compare a common conditional risk-difference estimand on the same population. For cross-base phi, the signed mean drops from 0.03520593 to 0.00185646, but the mean-absolute scale is 0.03703981 to 0.00481207. The body itself warns that within-cell phi is not an additive component. The separate covariance calculation can support a precisely specified decomposition, but does not rescue the broad phi/risk-difference wording.

The sentence “**Any model** that treats [the events] as conditionally independent … reproduces 95%” (1109–1111) is also false as quantified: conditional independence alone supplies neither the correct conditional marginals nor the correct cell weights; independent constant-probability models are immediate counterexamples.

**Fix:** Carry the body's caveats into the abstract, summary, and discussion. Report these as selected-cell descriptive reductions. For the cross-base common-cause statement specify a model using the **empirical conditional marginals and observed cell frequencies**, on the covariance scale. Do not upgrade this to an identified causal explanation or complete arithmetic decomposition.

### 6. MINOR — Abstract says three triple-completed pairs; there are fifteen

**Location:** line 80, “the three pairs whose product class completes a triple.”

**Wrong:** The abstract's count contradicts the body and the supplied base set.

**Evidence:** Independent enumeration/reanalysis gives **15** triple-completed pairs and 51 others, as in Table `tab:residual`. There are six negative signature-conditioned pairs, and the particularly large positive (2,6) outlier. The phrase cannot be justified as the number of negative pairs either.

**Fix:** Say “the triple-completed pairs” or explicitly “the fifteen triple-completed pairs”; state accurately which residuals are concentrated there.

### 7. MINOR — The 10^8 delta is wrong at the precision printed

**Location:** line 1590 and the ensuing increment/interpolation discussion.

**Wrong:** `−0.013360` is not the independently enumerated delta at 10^8 rounded to six decimal places.

**Evidence:** Fresh sieve gives 5,761,452 primes starting at 7 and table **[2239116,1367048;1367048,788239]**, hence **−0.013363024053**, which rounds to **−0.013363**. The other three displayed deltas agree with the fresh 10^9 census or arithmetic on the archived 10^10/10^11 tables. This is small but real; appending a trailing zero to an older five-place summary overstates precision.

**Fix:** Print −0.013363 or uniformly use five-place precision, and recompute all dependent increments/interpolation coefficients if keeping six-place values.

### 8. MINOR — The headline prime population is misstated

**Location:** abstract lines 52–54 (“all 50,847,531 primes up to 10^9”); discussion 1493 (“at the scale of 10^9 primes”).

**Wrong:** There are **50,847,534** primes up to 10^9. The consecutive census has 50,847,531 because it excludes 2,3,5. The cross-base populations have different low-end exclusions, correctly explained in the body. The work is at a cutoff 10^9, not over one billion primes.

**Evidence:** The fresh sieve gives 50,847,531 primes from 7, and adding the three excluded primes gives the full population. The body itself correctly supplies these distinctions.

**Fix:** State “primes up to 10^9, with 50,847,531 primes p≥7 in the consecutive census,” and replace “10^9 primes” with “primes up to 10^9.”

### 9. MAJOR — The benchmark's 32-thread/end-to-end description does not match the timed code

**Location:** table caption 1450; `code/artin_payoff.c:221–254`; `code/results.txt` search3 comments; discussion's end-to-end audit prescription.

**Wrong:** `search3` has three entirely serial loops. Its only OpenMP pragmas are in the separate census and ce modes, so `OMP_NUM_THREADS=32` does not make this search a 32-thread computation. The small-prime sieve/setup is outside every reported timer. It is therefore not an end-to-end timing of a fresh standalone search unless that preprocessing is explicitly amortized.

**Evidence:** `build_small(sqrt(START+200000000)+2)` precedes t0. No parallel region occurs in search3. My complete-process measurements show **0.189–0.197 s** outside the three strategy timers on this machine. Charging that common setup once to each standalone strategy changes the corresponding local naive/early-exit comparison to about **1.77–1.81×**, rather than its kernel-only ratio. These local values are not offered as replacements for the author's different hardware timings; they demonstrate nonzero omitted work. The exact search counters independently reproduce.

**Fix:** Correct “32 threads” to single-threaded search on a 32-core machine. Label the recorded numbers steady-state/kernel timings after shared setup, or include preprocessing in separately benchmarked runs. Supply repeats, dispersion, compiler/options, and randomized/alternating strategy order before attributing a robust 15% gain to a subsecond ordered single run.

### 10. MINOR — A 1.149× speedup is a 13.0% time reduction, not 15% time saved

**Location:** line 1459 (“saves a further 15%”); abstract's related wording.

**Evidence:** Executed arithmetic: 0.0841/0.0732 = **1.1489071**, whereas 1−0.0732/0.0841 = **0.1296076**. The factorization reduction 1−300/376 = about 20.2% is correct.

**Fix:** Say “about 15% faster” or “about 13% less time,” not “saves 15%.” Distinguish throughput and elapsed-time percentages.

### 11. MINOR — Cross-entropy table column has two incompatible meanings

**Location:** table at 1387–1394, header “gain over marginal.”

**Wrong:** The base-residue rows show gain over marginal, but predecessor-status rows show incremental gain over residues, not marginal.

**Evidence:** The mod-120 augmented model's gain over marginal is 0.661046629240−0.241520318139, not 0.000009. The displayed 0.000009 correctly measures the *incremental* comparison.

**Fix:** Use separate “gain over marginal” and “incremental gain” columns, or relabel the current column and explicitly state the reference for each row. This is not a rounding error.

### 12. MAJOR — The package contains the old, explicitly diagnosed wrong Kummer output under the natural results path

**Location:** `results/kummer_model_comparison.json` versus `code/kummer_model_comparison.json`; data-availability claim that the package reproduces every number.

**Wrong:** The two same-named files disagree; the results-directory file contains the old subgroup-counting error described in the manuscript itself.

**Evidence:** My independent reanalysis of `results/kummer_model_comparison.json` gives mean relative error **0.0366186935%**, maximum **0.6690671644%**. Its (5,13) prediction is **0.164670982768**. The corrected code-directory file and my independent calculation give (5,13) **0.163943423197**, mean error **0.0121232378%**, maximum **0.0395651563%**. See `recomputed_tables.txt`.

**Fix:** Regenerate/replace the stale results file, or move it to a clearly marked historical directory excluded from the reproduction deliverable. Provide one authoritative output path and a manifest/check command. Do not silently ship both as current results.

### 13. MINOR — Source provenance and build instructions in the merged package are stale or missing

**Location:** `code/results.txt:2`, `code/build_submission.sh`, `assemble.py`, prose reference to `LEAN_NOTE.md` around line 407.

**Wrong/evidence:** The results header says SHA-256 `d246f70580897ff6f4ad1b57d52128d4112dfb2ddb55a244eb65ba14bbc0af94`; the supplied source actually hashes to **d4992d400b3a5f1e8da4191e24f43cce00006ca844b889de146a6830f7916b69**. `build_submission.sh` still changes into `paper/` and builds `crossbase_artin.tex`, paths absent from this consolidated package. `assemble.py` requires the two earlier papers at absolute workspace paths. The promised `LEAN_NOTE.md` is absent. This is demonstrable packaging drift, not evidence that the numerical results were fabricated.

**Fix:** Publish the exact source corresponding to each run hash, update the consolidated build script, distinguish optional assembly from standalone reproduction, and include the referenced Lean scope/build documentation. Add a portable README.

### 14. MINOR — The (2,6) residual paragraph miscounts its stated family and contradicts its own maximum

**Location:** lines 1171–1175.

**Wrong:** It first reports a cell with phi=1 and then says across the stated family phi ranges only to +0.98. It says there are 44 cells without stating a different selection rule.

**Evidence:** Independently aggregating the signature+QR tables back to signatures, applying the paper's n≥200/nondegenerate convention, gives **47** cells with v2=1 and 3 not dividing p−1, range **0.3859128022 to 1.0**. Cell 129 is exactly `[888676,0;0,887010]`, population **1,775,686**, so the diagonal example is correct. The family includes that cell. Changing to n≥5000 gives 41 cells, not 44. Full qualifying cell lists are saved in `misc_checks.txt`.

**Fix:** State the exact subset intended. Under the published eligibility rule, correct to 47 and a range +0.386 to +1.000; if excluding selected cells, name them and explain why.

### 15. MINOR — Two bibliography entries have incorrect publication coordinates

**Location:** `TinkovaWaxmanZindulka2023` at 1767–1771; `Moree2012` at 1778–1781.

**Evidence:** Crossref and the arXiv related DOI identify *Artin twin primes* as **Journal of Number Theory 245 (2023), 203–232**, DOI **10.1016/j.jnt.2022.10.006**, not 247, 274–304. Crossref and the publisher identify Moree's survey as **Integers 12(6) (2012), 1305–1416**, DOI **10.1515/integers-2012-0043**, not 12A, A13, 100 pp. The titles/authors themselves are genuine.

**Fix:** Correct the coordinates and include the verified DOIs. The citation table below records checks of every entry; I found no duplicate work masquerading under two keys.

### 16. MINOR — The Lean claim is broader than the checked theorem signatures

**Location:** prose around 399–407 and 455–459; `lean/Artin/PairExclusion.lean` and `TripleExclusion.lean`.

**Wrong:** The gate verifies character-parity theorems with a square-witness relation and **nonvanishing witness hypotheses**, not a generic squarefree-part theorem with the paper's full hypotheses automatically discharged. For example `not_both_primitiveRoot_five_ten` explicitly retains `hp5 : (5 : ZMod p) ≠ 0`. The blanket claim that nonvanishing assumptions are “never used” does not describe this instance or its proof.

**Evidence:** `not_both_primitiveRoot_of_barring_character` requires `d*u^2=a*b*v^2`, `hu`, and `hv`; the (5,10) instance uses hp5 to discharge hu. `not_all_three_nonresidue_of_primitiveRoot` takes the third character value as a hypothesis rather than a third primitive-root argument. Those facts can be bridged mathematically, and the pen-and-paper argument handles p dividing a base, but that general bridge is not the theorem signature checked by the gate. No generic sqf definition/theorem is supplied here. The gate itself passed 28 checks.

**Fix:** Precisely describe the formalized scope—genuine primitive-root-to-character bridge plus parity under nonvanishing square witnesses, and selected concrete instances—or add Lean wrappers deriving witnesses/nonvanishing and handling base divisors. Do not present this as a failure of the elementary theorem.

### 17. MINOR — Several elementary prose statements silently drop needed scope

**Location:** pair-law discussion 428–441; data validation 524–526; new baseline 1377; preliminaries 202–208.

**Evidence/problems:**

* “all three members of (3,5,15) are **Artin base 2**” reverses the terminology: **the prime 2 is Artin for each of bases 3,5,15**.
* To use the triple exclusion to rule out simultaneous success does **not** require first proving c is a primitive root; the theorem already excludes the conjunction. If one wants specifically to deduce the pair exclusion from knowledge about c, say so. Testing c at a specified p is decidable, not itself an unresolved conjecture. Positive Artin density for c also requires excluding the square cases this section deliberately allows.
* The displayed densities at 10^10 and 10^11 are **0.373955176619** and **0.373954344236**. Rounded to six places these are 0.373955 and 0.373954, not Artin's constant rounded to six places, 0.373956. “Agrees … to six decimal places” is false, particularly at 10^11.
* The new baseline uses `p mod 4d` to predict a single base a without defining d locally; the earlier d is sqf(ab). Use the conductor of a or define d=sqf(a).

**Fix:** Correct the terminology and precision; make the intended hypotheses and inferential use explicit. These do not invalidate the core proofs.

### 18. MINOR — The novelty statement contradicts the manuscript's own attribution

**Location:** introduction around 134–137 and theorem citation at 238 versus discussion 1489–1493.

**Wrong:** The introduction says the gap theorem is the base-10 instance of Tinková–Waxman–Zindulka and “we claim no priority for it.” The consolidated novelty paragraph then includes this law among ingredients “none of which we have found stated in this form.” At minimum the claim of novelty is inconsistent and invites a priority interpretation the introduction disavows.

**Evidence:** These are explicit statements in the same manuscript; the cited *Artin twin primes* abstract indeed says it identifies shift/base pairs with no simultaneous Artin primes.

**Fix:** Preserve the unambiguous no-priority attribution. Identify the census, conditional diagnostics, and explicitly limited computational audit—not the classical exclusion—as contributions.

### 19. NIT — Notation and an over-hedged research question need cleanup

**Location:** definition of Acal in introduction and preliminaries 202; `Art_n` usage throughout; “Research question (residue completeness)” in Open questions.

**Wrong/evidence:** The merge successfully avoids using `Art_a` for the cross-base set, but Acal_a is first a **set** and then Acal_a(p) is declared a **0/1 function**; conditional probabilities later use the bare set as an event. This is conventional if explicitly declared, not evidence of corrupted numerical work, but it falls short of the promised strict separation. `Art_n` is used as an event and inside another indicator `1[Art_n]` without a clean displayed definition. The residue-completeness question leaves the dependence measure, populations, order of limits, and range of x unspecified, and explicitly admits that no unambiguous formulation is known. It therefore states a research direction, not a currently falsifiable mathematical claim.

**Fix:** Define `Art_n=1_{p_n in Acal_10}` and `X_a(p)=1_{p in Acal_a}`, and use events consistently. Label the residue-completeness passage an informal direction or supply quantifiers/measure/limits. Also remove or justify six uncited bibliography entries (listed in `independent_checks.txt`).

## Citations check (table: key / verified? / problem)

Checks used live Crossref metadata (`citations.json`, `citations_supplement.json`), arXiv/DataCite for preprints, and publisher records/search results where Crossref's broad-title search returned unrelated works. “Yes” means title/authors/publication identity checked, not that I have independently proved every theorem in that reference. Online-first dates were not mistaken for final issue years. No duplicate bibliography entries were found.

| key | verified? | problem |
|---|---|---|
| Artin1965 | Yes, Cambridge-published contemporary review; publisher record for later collected-papers edition distinguished | 1965 Addison–Wesley edition, Lang/Tate editors confirmed. Springer DOI 10.1007/978-1-4612-5717-2 is a later edition, not evidence to change the cited year. Uncited in body. |
| BakerPollack2016 | Yes, Crossref: 10.1515/forum-2014-0137 | Title/authors, 28(4), 675–687 confirmed; 2015 online-first vs 2016 issue is not an error. |
| ConwayGuy1996 | Yes, Crossref/publisher: 10.1007/978-1-4612-4072-3 | Correct book, authors, 1996. Did not inspect the particular cited pages 166–171. |
| Erdos1935 | Yes, Crossref: 10.1093/qmath/os-6.1.205 | Correct author/title, old series 6, 205–213. |
| FanPollack2025 | Yes, Crossref: 10.1112/mtk.70055 | Kai (Steve) Fan and Paul Pollack; 71(4), e70055 confirmed. Uncited. |
| GarciaKahoroLuca2019 | Yes, Crossref: 10.1080/10586458.2017.1360809 | Correct final 2019 issue 28(2), 151–160; 2017 online-first date is not a contradiction. |
| GarciaLucaShiUdell2020 | Yes, Crossref: 10.1016/j.jnt.2019.08.003 | All four authors/title and 208, 400–417 confirmed. |
| GuptaMurty1984 | Yes, Crossref/publisher: 10.1007/BF01388719 | Correct title/authors, 78, 127–130. |
| HardyLittlewood1923 | Yes, Crossref: 10.1007/BF02403921 | Correct title/authors, 44, 1–70. |
| HeathBrown1986 | Yes, Crossref/OUP: 10.1093/qmath/37.1.27 | Correct title/author, 37, 27–38. Publisher labels issue 1; no.145 is historical cumulative issue numbering, not treated as a substantive error. |
| Hooley1967 | Yes, Crossref/publisher: 10.1515/crll.1967.225.209 | Correct article and pages 209–220; journal metadata represents 225 as issue under year-volume 1967. |
| KlurmanShparlinskiTeravainen2025 | Yes, Crossref and arXiv 2412.13355: 10.1112/blms.70103 | Correct title/authors, 57(8), 2429–2443 and arXiv identifier. |
| TinkovaWaxmanZindulka2023 | **Yes—incorrect coordinates** | Correct is **JNT 245 (2023), 203–232**, DOI **10.1016/j.jnt.2022.10.006**. arXiv 2010.15988 correctly points to this work. |
| LOS2016 | Yes, Crossref/publisher: 10.1073/pnas.1605366113 | Correct authors/title, 113(31), E4446–E4454. |
| Moree2012 | **Yes—incorrect coordinates** | Correct publisher citation **Integers 12(6) (2012), 1305–1416**, DOI **10.1515/integers-2012-0043**; not 12A/A13/100 pp. |
| PeruccaShparlinski2025 | Yes, Crossref: 10.1112/blms.70011 | Correct authors/title, 57(3), 978–991; manuscript omits issue but is otherwise correct. Uncited. |
| Pollack2014 | Yes, Crossref: 10.2140/ant.2014.8.1769 | Correct 8(7), 1769–1786. |
| BaldII | Yes, DataCite: 10.5281/zenodo.22865343 | Registered title/author/year match. Zenodo page access returned 403; DOI registration is verified, not the deposit's full mathematical content. |
| GoldmakherMartinPeringuey2025 | Yes, arXiv 2502.19601 and DataCite | Correct title, Leo Goldmakher/Greg Martin/Paul Péringuey, 2025. |
| JarviniemiPeruccaSgobba2025 | Yes, Crossref: 10.1007/s40993-025-00620-2 | Correct authors/title/year; **42 is article number**, not issue number; prefer “Article 42.” The claimed 19-page extent was not independently checked. |
| Kimmel2024 | Yes, Crossref: 10.4064/aa230810-29-6 | Correct title/author, 216(4), 329–348. Uncited. |
| Lenstra1977 | Yes, Crossref/publisher: 10.1007/BF01389788 | Correct title/author, 42, 201–224. Uncited. |
| MoreeStevenhagen2014 | Yes, Crossref: 10.4064/aa163-1-2 | Correct authors/title, 163(1), 15–32. |
| Matthews1976 | Yes, Crossref and original journal scan: 10.4064/aa-29-2-113-146 | Correct K. R. Matthews/title, 29(2), 113–146. |
| Sgobba2025 | Yes, arXiv 2508.08996 and DataCite | Correct title/author/year. Uncited. |

## What I could NOT verify, and why

* I did **not** independently sieve to 10^10 or 10^11. I recomputed delta and total pair counts from the archived tables, which match their reported values, but the underlying 10^10/10^11 census, shard-boundary handling and zero counts are not independently recertified here. This audit's independent consecutive census reaches 10^9.
* I did **not** regenerate the complete twelve-base 10^9 raw census. I independently enumerated it through 10^8, then independently analyzed the supplied 10^9 contingency tables. Claims requiring the authenticity of those raw 10^9 tables have that stated dependency.
* The original 0.1642/0.0841/0.0732-second measurements on the named AMD machine cannot be reproduced exactly on this two-core VM. Exact workloads/counters were independently reproduced; I inspected the timed code and ran local repeats. Hardware-dependent timing differences are not themselves a finding.
* I verified the exclusion proofs, subgroup-sum implementation and numerical density predictions, not an independent derivation of the full GRH/Kummer density theory from original sources. Nor did I assess all priority claims across the entire literature.
* The Lean gate certifies the actual formal statements checked, not the stronger prose scope or all arithmetic statistics. In particular the general squarefree-part-to-nonvanishing-witness bridge is not present as a checked theorem here.
* I inspected source, log, labels, numerical tables and assembly behavior, but did not do a page-by-page visual/typesetting audit of the PDF. No unresolved reference appears in the existing build log.
* A few publisher full-text pages were access-blocked. Metadata/arXiv/DataCite checks succeeded through alternate authoritative records as detailed above. I did not verify Conway–Guy's exact page locator or Järviniemi–Perucca–Sgobba's 19-page extent.

## Overall assessment

The audit did **not** find a counterexample to either exclusion law, nor a failure of the main 10^9 consecutive census. The independently reconstructed numbers are strong evidence that the headline anticorrelation, gap structure, omega repulsion, and entropy arithmetic are real finite-census measurements.

The manuscript is nevertheless not clean. Its main defect is **claim control after consolidation**: the new front matter and conclusion turn qualified descriptive diagnostics into exact sufficiency/decomposition statements and a universal negative algorithmic conclusion. The new section also contains a straightforward wrong sign, a mislabeled statistical experiment, and an inaccurately described timing protocol. These must be corrected before the paper can be assessed on its actual contribution. Correcting the sign alone is not enough. A revised paper should retain the verified census, explicitly distinguish empirical conditional entropy from predictive validation, constrain the computational verdict to tested workloads, and ship one internally consistent reproduction package.
