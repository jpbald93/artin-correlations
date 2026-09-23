# Round-2 adversarial audit — Astra

**VERDICT: MAJOR REVISION.** The new “exact” decomposition is false, its assertion is tautological, and the new predictive-information upper bound is not established. The independently regenerated 10^9 census and both negative held-out scores do reproduce.

Audit target: `artin_correlations.tex`, SHA-256 prefix `ad5b359db5fe98d0`, and `code/artin_payoff.c`, SHA-256 `43335061ef371cff8520f982c587984c56cb026a49cf771ad31ff91eb7359448`. All commands below run from `Prime Math/consolidated`. Evidence, new independent programs, API responses and logs are under `reports/round2_astra/`. I did not modify manuscript/code sources. Running the requested build regenerated the submission artifacts.

## Independent experiment, not a rerun of the author's census

I wrote `independent.c`: a fresh odd-integer smallest-prime-factor sieve, integer modular exponentiation and primitive-root test, with no input prime list, author routine, or results file. It enumerates every prime from 7 to 10^9, records the four-cell tables for every joint residue mod 840, and independently applies the specified SplitMix64 split. Mod 120 is obtained by exact aggregation. A separate NumPy/SciPy analyzer computes entropies, both split directions, both sides of the claimed identity, and a correct covariance decomposition. Another independent Newton/Schur-complement fit checks the pooled-logit model.

Commands:
```sh
gcc -O3 -Wall reports/round2_astra/independent.c -o reports/round2_astra/independent
/usr/bin/time -v reports/round2_astra/independent 1000000000
python3 reports/round2_astra/analyze.py
python3 reports/round2_astra/pooled.py
```
Raw census output (`independent_1e9.log`):
```text
limit=1000000000 primes_ge7=50847531 artin=19016617 table00,01,10,11=19758045,12072868,12072869,6943748 exclusion_total,prev,both=2214511,984678,0
Elapsed (wall clock) time: 0:57.68
Exit status: 0
```
The census establishes delta = **−0.014140158839795136**. The main counts, entropy table and zero forbidden-gap joint events survive this round. I did not regenerate the twelve-base census or the 10^10/10^11 censuses this round; those are not independently recertified here.

## Findings (9)

### 1. MAJOR — The NEW exact identity is false; the driver never evaluates its between term

**Quote (§15):** “This is an identity, not a model, and we assert it in the driver.” The displayed formula is `delta = sum w1_c delta_c + sum (w1_c-w0_c) p_c`, with `p_c=P(Y=1|C=c)`.

**Command:** `python3 reports/round2_astra/analyze.py`.

Raw output excerpts:
```text
                              mod120                   mod840
paper_within                 -0.0005454179402152527    -0.00031024472865238056
actual_between_formula       -0.013954490546512897     -0.014075838965196374
paper_RHS                    -0.01449990848672815      -0.014386083693848755
paper_identity_error         -0.0003597496469330136    -0.0002459248540536193
paper_between_residual_share   0.9614277359685477        0.978059317991588
corrected_within             -0.0001856682932822347    -0.00006431987459877051
corrected_between_share       0.9868694336898319        0.9954512622292655
corrected_identity_error       3.469446951953614e-18    -8.673617379884035e-18
RATIONAL_COUNTEREXAMPLE [[3, 1, 1, 3], [6, 2, 1, 1]] delta 5/12 W 5/12 B 1/15 RHS 29/60 error 1/15
```
Here each rational-example cell is `[n00,n01,n10,n11]`; no empty margin or numerical approximation is involved. The formula already fails on 18 observations.

The program actually sets `double between=delta-within;` and tests `fabs((within+between)-delta)>1e-12`. This verifies subtraction, not the proposed identity. Moreover failure merely prints an `ASSERT_FAIL` key rather than terminating unsuccessfully. I compiled a harness using the unmodified author's function on the rational counterexample:
```sh
gcc -O2 reports/round2_astra/driver_harness.c -lm -o reports/round2_astra/driver_harness
reports/round2_astra/driver_harness
```
Raw output:
```text
"counterexample": {"N": 18, "delta": 0.416666667, "within": 0.416666667, "between": 0.000000000, "between_share": 0.0000, "nondegenerate_cells": 2, "extra_params": 2, "optimism_1x": 5.556e-02, "optimism_2x": 1.111e-01},
```
No failure is reported although the stated between formula is 1/15, not zero.

**Repair:** Let `pi_c=P(C=c)`, `r_c=P(X=1|C=c)` and `r=P(X=1)`. Retaining the paper's unconditional `p_c`, a genuine identity is

`delta = sum pi_c*r_c*(1-r_c)/(r*(1-r))*delta_c + sum (w1_c-w0_c)*p_c`.

This is the law of total covariance divided by `Var(X)`. Define zero-margin within contributions to be zero; their `delta_c` itself is not identifiable/defined. This gives **98.69% / 99.55%**, not 96.1% / 97.8%, between on this census. Alternatively an asymmetric Oaxaca identity uses `P(Y=1|X=0,C=c)` in the between term, with an explicit convention for unsupported cells—not the displayed `p_c`.

**Deterministic-cell claim:** If *X or Y is constant in a cell*, its conditional covariance is zero and its contribution to the correctly defined within term vanishes. Thus the intended assertion holds for this properly specified decomposition. “Deterministic exclusion” alone does not imply zero within association: e.g. `Y=1-X` is a deterministic exclusion with maximal within association. In the manuscript formula, X-degenerate cells make `delta_c` undefined until a convention is stated. The present percentages are arithmetic residuals, not the claimed composition term. Their propagation into the abstract is a new round-2 error.

### 2. MAJOR — Extra-parameter arithmetic is doubled and its nominal dimension includes boundary cells

**Quote (§15):** “512 … and 3,448 … additional probabilities, and the classical k/(2N) optimism is 1.0×10^-5 and 6.8×10^-5 nats respectively.”

**Command:** `python3 reports/round2_astra/analyze.py`.
Raw output:
```text
mod120 X_nondegenerate_cells=512 XY_nondegenerate_cells=256
claimed_k_over_2N=5.034659500667977e-06
active_k_over_2N=2.5173297503339886e-06
in_sample_gain=9.18085432780047e-06
mod840 X_nondegenerate_cells=3448 XY_nondegenerate_cells=1599
claimed_k_over_2N=3.390528507481091e-05
active_k_over_2N=1.5723477620250186e-05
in_sample_gain=1.8326383867811602e-05
```
The printed values are **k/N**, not k/(2N), at N=50,847,530. Using the half-sample N would not be the quoted full-sample diagnostic. Further, counting every cell with both X values includes cells with Y identically zero; those have no regular interior response parameter to split. There are only **256 / 1,599** cells with both margins nondegenerate. Boundary/sparse cells and dependent prime pairs also make the regular iid expansion heuristic, not a certified correction. Standard training-versus-test optimism k/N must not be conflated with the null likelihood-gain expectation k/(2N).

This error is substantive: at mod 120 the observed 9.18e-6 exceeds the interior-dimension null scale 2.52e-6, rather than approximately matching the claimed 1.0e-5. This does **not** by itself establish real predictive skill, but invalidates the paper's numerical argument.

### 3. MAJOR — Negative validation of two estimators is not an information upper bound

**Quote (§15):** “The honest statement is therefore an upper bound: … at most of order 10^-6 nats of usable predictive information … beyond the joint residues.” Abstract: “both held-out comparisons are negative, bounding any usable edge at order 10^-6 nats.”

**Commands:** `python3 reports/round2_astra/analyze.py`; `python3 reports/round2_astra/pooled.py`.
Raw output:
```text
full-sample empirical gain: mod120 9.18085432780047e-06; mod840 1.8326383867811602e-05
fit hash bit 0, test bit 1:
mod120 N_test=25421106 CE_residue=0.2414099540195472 CE_plusX=0.24141314734240912 gain=-3.1933228619163145e-06
mod840 N_test=25421106 CE_residue=0.23727992165429826 CE_plusX=0.23734102953952735 gain=-6.110788522908916e-05
independent_pooled M=120 beta=-0.009136463960550668 heldout_gain=-1.2105958624442081e-07
independent_pooled M=840 beta=-0.004693483482741791 heldout_gain=-2.395745996530696e-06
```
The negative scores reproduce, including a separately fitted pooled offset. They establish **no demonstrated improvement by these estimators on this split**. They do not bound the oracle conditional mutual information, other shrinkage models, other train/test designs, or future prime ranges. No confidence construction, population assumption, model-class bound or cost threshold supports the asserted 10^-6 number. An upper bound cannot be obtained by deducting an expected bias (already miscalculated in finding 2) from a noisy statistic without such assumptions. The correction remains too strong, not too weak. Replace it with the model/workload-limited negative finding; a numerical upper bound needs an actual derivation and uncertainty treatment.

### 4. MINOR — The held-out caption reverses training and test halves

**Quote (Table `tab:holdout`):** “Cell probabilities are fitted on pairs whose successor prime has an odd 64-bit hash and scored on the complementary half (25,421,106 pairs).”

**Command:** `python3 reports/round2_astra/analyze.py`.
Raw output for the caption's stated direction:
```text
train_hash_bit=1 test_N=25426424
mod120 CE_residue=0.24169874087440105 CE_plusX=0.2417036333323244 gain=-4.892457923344473e-06
mod840 CE_residue=0.23759007980090183 CE_plusX=0.2376508437252653 gain=-6.0763924363466115e-05
```
The table's published values instead exactly match **even-hash training, odd-hash testing** (finding 3). `cehold` puts odd hashes in K and calls `hold_report(J,K,...)`. Correct the caption to even; both directions are negative, so this does not reverse the qualitative result. The prior claim of 32-thread search has likewise not actually been removed from `code/results.txt` (finding 7).

### 5. MINOR — Correct reciprocity clarification is immediately contradicted

**Quotes (§15):** “multiplicativity alone does not fix which sign the shifted product takes”; later in the same subsection: “Both deductions are one line of multiplicativity”.

**Commands:** `sed -n '250,290p' artin_correlations.tex`; `python3 reports/round2_astra/proofs.py`.
Raw check output:
```text
gap theorem prime-pairs below1000 854 failures []
gap sign example p=23,q=43: -1 1
pair/triple checks, triple unit cases= 16338 failures= []
(2,5,10) squarefree relationship= 10 10 unit primes are p not in {2,5}
```
The exact reciprocity input is the proof sentence “By quadratic reciprocity (5 ≡ 1 mod 4), (5/p)=(p/5)”; it transfers equality of p mod 5 into equality of the 5-symbols. The second supplementary law then flips the 2-symbol under a shift of 4 mod 8. Multiplicativity combines those facts; it does not determine either shifted sign. The corrected abstract is right, as is the new negative product sign. Restrict the subsequent “one line” phrase to combining **already available characters** or to the same-prime law. I found no counterexample to either theorem.

### 6. MINOR — The local build passes, but the delivered reproduction build is broken and anonymous PDF is stale

**Quotes:** root build comment “Portable: no absolute paths”; README “assembly pipeline (reproducible)”; artifact `artin_correlations_anonymous.pdf` remains in the submission set.

**Commands:** `./build_submission.sh`; `(cd lean && ./gate.sh)`; extract both ZIPs, then run `bash code/build_submission.sh` in the reproduction extraction. For isolated assembly, set `CONSOLIDATED_DIR` and `MATH_AUDIT_BASE` to the source extraction and run its `assemble.py` (full paths/logs in `zip_assemble.log`).

Raw output:
```text
== named manuscript
   pages=28 overfull_hbox=0 undefined=0
PASS (28 theorems, standard axioms only)
reproduction.zip ROOT_FILES ['README.md', 'LEAN_NOTE.md']
BUILD_SCRIPTS ['code/build_submission.sh']
ZIP_BUILD_EXIT=1
code/build_submission.sh: line 21: cd: .../unpacked/paper: No such file or directory
ZIP_ASSEMBLE_EXIT=1
FileNotFoundError: .../source_unpack/Paper 1 Full file/paper/consecutive_artin.tex
```
The root consolidated build script is **not in either ZIP**. The reproduction ZIP instead contains the old Paper-3 build script; it requires nonexistent `paper/crossbase_artin.tex`. Assembly still needs both predecessor source trees, neither shipped. This is distinct from LaTeX portability: directly compiling the generated TeX from the source ZIP twice **succeeds**, 28 pages, no undefined refs/overfull boxes.

Commands: `pdfinfo submission/artin_correlations_anonymous.pdf`; `pdftotext ... reports/round2_astra/anon.txt`; grep the text. Raw excerpts:
```text
Pages: 25
on the joint residues ... removes 93% and 97% respectively
The correlation’s own predictive edge ... is of order 10^-5 bits.
```
The build never regenerates the anonymous PDF. It therefore silently leaves a materially different pre-fix submission alongside the 28-page named manuscript. Ship one tested build path, make source assembly optional unless its inputs are included, and regenerate/remove the stale anonymous artifact.

### 7. MINOR — Conflicting outputs and run provenance remain in the package

**Quotes (`code/results.txt`):** “Three strategies … 32 threads”; “sha256(artin_payoff.c) = 109f23d763fa4a011f2cf9c2d1b7afc06da6335981b7e1dfd2b04fb22ce447ef”; “the correlation's OWN predictive edge”; “1e9 values above are the converged ones”. README still says “the paper (25 pp)” and “both … decomposed — 93 %/97 %”.

**Commands:** `sha256sum code/artin_payoff.c`; compare same-named JSON files in code/results (`packaging.log`); inspect their first entries with Python.
Raw output:
```text
SOURCE_SHA256 43335061ef371cff8520f982c587984c56cb026a49cf771ad31ff91eb7359448
DUPLICATE code/crossbase_fine_summary.json results/crossbase_fine_summary.json DIFF
DUPLICATE code/crossbase_summary.json results/crossbase_summary.json DIFF
DUPLICATE code/kummer_model_comparison.json results/kummer_model_comparison.json MATCH
crossbase_summary (5,13): code phi=0.06691498112591443 z=18.74766234545257
                         results phi=0.06661262161056866 z=221.72033867631836
crossbase_fine_summary (5,13): code n_sig=51484; results n_sig=34957999
```
The search loops have no OpenMP parallel region; only census loops do. The paper's corrected single-thread caption is right but the distributed run log still contradicts it. The SHA identifies neither the shipped current source nor a shipped matching historical source. Same-named summaries from different bounds are not clearly distinguished by filename, and both copies are zipped. The serious round-1 Kummer-output mismatch **has** been fixed; I cannot falsify that fix. Remaining run records/README need synchronization and explicit run/bound provenance. `results/final_consistency_checks.txt` also still claims “PDF identical, 10 pages”, plainly not a current consolidated-package certificate.

### 8. MINOR — Moree bibliography coordinates still contradict the DOI metadata

**Quote:** “Integers 12A (2012), Article A13, 100 pp.; doi:10.1515/integers-2012-0043.”

**Command:** `python3 reports/round2_astra/metadata_supplement.py` (direct Crossref DOI request; response `doi_Moree2012.json`).
Raw metadata excerpt:
```json
{"title":["Artin's Primitive Root Conjecture – A Survey"],"DOI":"10.1515/integers-2012-0043","volume":"12","issue":"6","page":null,"article-number":null}
```
The linked publication is volume **12, issue 6**, not 12A/A13. Crossref's current response does not supply pages, so I do not independently certify the 1305–1416 page range reported by round 1 using this response alone. Correct the edition/coordinates with publisher evidence; adding a DOI while retaining the wrong coordinates is insufficient. Tinková–Waxman–Zindulka's correction to **245 (2023), 203–232** is confirmed by fresh metadata. Järviniemi–Perucca–Sgobba's “no.42” is an article number, not issue number (actual issue 1); label it “Article 42”.

### 9. MINOR — Lean scope note improves disclosure, but full generic theorem language still overstates signatures

**Quote (§4):** “Theorems … are also machine-checked in Lean 4 …”; LEAN_NOTE: “nonvanishing hypotheses … discharged in those instances.”

**Command:** `cd lean && ./gate.sh`; read `Artin/Check.lean`, `Bridge.lean`, `PrimitiveRootBridge.lean`, `PairExclusion.lean`, `TripleExclusion.lean`.
Raw output/signature excerpt:
```text
PASS (28 theorems, standard axioms only)
theorem not_both_primitiveRoot_five_ten
    (hp2 : p ≠ 2) (hp5 : (5 : ZMod p) ≠ 0)
    (hmod : p % 8 = 3 ∨ p % 8 = 5)
```
The gate succeeds, scans `Artin/*.lean` for prohibited placeholders/axioms/native_decide, builds, and checks axiom names printed by 28 selected declarations. They use only subsets of `propext`, `Classical.choice`, `Quot.sound`; no proof failure found. These 28 include auxiliary Paper-2 refutations/counting identities, not 28 versions of this paper's two laws.

Precisely formalized: the mod-40 character flip; its bridge to the genuine Legendre symbol using reciprocity; primitive-root ⇒ nonresidue; pair/triple parity from integer square-witness relations with nonvanishing witnesses; concrete instances. The gap theorem retains `hunit` and a cast-level shift; `not_both_artin` itself concludes non-conjunction of two character values. Generic pair/triple statements do not have the paper's `sqf` hypotheses automatically converted to witnesses. The (5,10) instance still **requires** hp5 rather than discharging it in its public theorem. The new note explicitly admits the missing generic bridge, which is good; its “discharged in those instances” sentence should distinguish supplied hypotheses from automatically proved consequences. The census, decomposition, entropy estimates, upper bound and asymptotics are **not formalized**. Qualify the main prose accordingly, or add wrappers. This is not a counterexample to the elementary mathematics.

## Explicit verdict on each requested change

| Change | Result | Evidence |
|---|---|---|
| 1. Upper-bound rewrite, extra parameters and entropy | **INCORRECT / INSUFFICIENT** | Entropies and both negative held-out scores reproduce; pooled fit also negative. k/(2N) arithmetic is off by 2, cells include response-boundary cases, and no upper-bound derivation exists (findings 2–4). |
| 2. Exact within/between decomposition, 96.1/97.8%, assertion | **INCORRECT** | Formula falsified both by 18-observation rational example and independently sieved full census. Percentages reproduce only the driver's residual subtraction, not its stated between formula. Driver assertion is tautological. Correct covariance version gives 98.69/99.55% (finding 1). |
| 3. Abstract decomposition numbers and reciprocity qualification | **MIXED: numbers INCORRECT; reciprocity CORRECT but not consistently propagated** | Abstract inherits finding 1. Gap proof uses reciprocity exactly at `(5/p)=(p/5)` plus the supplementary law for 2; same-prime parity is multiplicativity. Later “both … multiplicativity” survives (finding 5). |
| 4. Units hypothesis, including (2,5,10) | **CORRECT** | Under the surrounding odd-prime assumption and `sqf(c)=sqf(ab)`, `p ∤ abc` suffices for the character equality. `(2,5,10)` is included for p other than 2,5. For p=2 or 5 a base vanishes, so the full triple exclusion remains trivial there. Independent small-prime algebra tests found no exception. The actual theorem remains stronger and valid: no extra units hypothesis is needed merely to exclude the conjunction, since a vanishing base is not primitive. |
| 5. Merge-time citation repair and uncited entries | **CORRECT** | Rebuilt text says “proved above (Theorem [thm:exclusion]); the arbitrary-base form is [BaldII]”. The theorem reference resolves to the correct in-paper gap law and the retained preprint is cited only for the arbitrary-base form here. Parsed all citation keys: `UNCITED []`, `MISSING []`. |
| 6. Captions, thread claims, repository URL/code match | **PARTLY CORRECT / INSUFFICIENT** | Entropy gain columns and single-thread manuscript timing caption corrected. Holdout direction wrong; results.txt still says 32 threads. GitHub HTTP 200; public HEAD `715057b15be286f97b3d298907e8c9bb3e9efa8d`. Every shared C/Python source file and results.txt is byte-identical to local counterpart. Public repo deliberately lacks local compiled binary/old build script/code-side JSON copies, so repository is not identical to local ZIP. Packaging drift remains (findings 4,6,7). |

### Bibliography check — fresh metadata, all entries attempted

Commands: `python3 reports/round2_astra/metadata.py` and `metadata_supplement.py`. Crossref title search was sometimes insufficient or non-JSON; direct DOI requests and arXiv DOI DataCite records were then used. The arXiv API returned HTTP 406, **not** a missing-paper diagnosis. DataCite successfully supplied all four arXiv records, including the journal-related DOIs for 2412.13355 and 2010.15988. All raw responses are saved. Successful metadata checks confirm bibliographic identity/available fields, not theorem correctness.

| Entry | Live metadata result |
|---|---|
| Artin1965 | Crossref contemporary review `10.1112/jlms/s1-42.1.189b` confirms work identity; Crossref book `10.1007/978-1-4612-5717-2` is the **1982** edition. The full 1965 publisher/editor coordinates are **not independently confirmed** by these metadata fields. Do not substitute 1982 for the cited original edition. |
| BakerPollack2016 | `10.1515/forum-2014-0137`, title/authors, 28(4), 675–687 match; online-first 2015 is not an issue-year error. |
| ConwayGuy1996 | `10.1007/978-1-4612-4072-3`, title/authors/year match; particular cited pages not inspected. |
| Erdos1935 | `10.1093/qmath/os-6.1.205`, old-series 6, 205–213 match. |
| FanPollack2025 | Crossref DOI-filter request succeeds (`fan_filtered.json`): 71(4), e70055, October 2025; title/authors match. |
| GarciaKahoroLuca2019 | `10.1080/10586458.2017.1360809`, 28(2), 151–160 match; 2017 online-first distinguished. |
| GarciaLucaShiUdell2020 | `10.1016/j.jnt.2019.08.003`, 208, 400–417 match. |
| GuptaMurty1984 | `10.1007/BF01388719`, 78(1), 127–130 match. |
| HardyLittlewood1923 | `10.1007/BF02403921`, 44, 1–70 match. |
| HeathBrown1986 | `10.1093/qmath/37.1.27`, 37(1), 27–38 match; historical cumulative issue 145 not independently certified. |
| Hooley1967 | `10.1515/crll.1967.225.209`, title, 225, 209–220 match; current metadata omits author. |
| KlurmanShparlinskiTeravainen2025 | `10.1112/blms.70103`, 57(8), 2429–2443; DataCite arXiv 2412.13355 matches authors/title and links journal DOI. |
| TinkovaWaxmanZindulka2023 | `10.1016/j.jnt.2022.10.006`, 245, 203–232; DataCite arXiv 2010.15988 also links that DOI. Corrected. |
| LOS2016 | `10.1073/pnas.1605366113`, authors/title/113(31) match; response omits page field. |
| Moree2012 | `10.1515/integers-2012-0043`, **12(6)**; finding 8. |
| PeruccaShparlinski2025 | `10.1112/blms.70011`, 57(3), 978–991 match. |
| Pollack2014 | `10.2140/ant.2014.8.1769`, 8(7), 1769–1786 match. |
| BaldII | DataCite `10.5281/zenodo.22865343`, title/Joshua Bald/year 2026 match. Registration verified, not deposit mathematics. |
| GoldmakherMartinPeringuey2025 | DataCite `10.48550/arXiv.2502.19601`, title/all three authors/2025 match. |
| JarviniemiPeruccaSgobba2025 | `10.1007/s40993-025-00620-2`, **11(1), article 42**; 19-page extent unverified. |
| Kimmel2024 | `10.4064/aa230810-29-6`, 216(4), 329–348 match. |
| Lenstra1977 | `10.1007/BF01389788`, 42, 201–224 match. |
| MoreeStevenhagen2014 | `10.4064/aa163-1-2`, 163(1), 15–32 match. |
| Matthews1976 | `10.4064/aa-29-2-113-146`, 29(2), 113–146 match; metadata lists K. Matthews, not full middle initial. |
| Sgobba2025 | DataCite `10.48550/arXiv.2508.08996`, title/Pietro Sgobba/2025 match. |

## What the fixes broke; what remains clean

**New breakage:** the purported identity and its falsely reassuring assertion; invalid between percentages promoted into the abstract; factor-two optimism arithmetic; unsupported numeric upper bound promoted into the abstract; new heldout caption contradicting the actual split. **Not new but still unfixed:** anonymous artifact/build drift, stale thread/hash/summary files, Moree coordinates, partly overstated Lean scope.

**Clean results:** full independent 10^9 consecutive census; published in-sample entropies; both tabular heldout losses; qualitative pooled-model negative; negative gap-character product; corrected units language; reciprocity-qualified abstract; gap-law theorem reference and no uncited bibliography entries; live repository URL and matching source code; corrected duplicate Kummer output; successful Lean gate; rebuilt named and standalone-source PDFs each **28 pages, zero undefined references and zero overfull hboxes**. No exclusion-theorem counterexample found.
