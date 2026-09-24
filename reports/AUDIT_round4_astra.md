MAJOR REVISION

# Round 4 — Astra, §15 / round-3 changes only

**Arithmetic survives. The replacement attribution is too categorical; the blind supplement still carries searchable provenance and its scrubber breaks Lean reproduction.** No theorem or census re-audit. Manuscript/build sources were not modified. Evidence and independent programs: `reports/round4_astra/` (abbreviated **E/** below). Commands run from `Prime Math/consolidated`. Input hashes: `E/hashes.txt`.

## Independent numerical evidence

I wrote a new NumPy scorer using the shipped integer tables, without importing `section15.py`. Successor margins plus the omitted first prime 7 give the per-prime residue rates. A separate C++ bit sieve regenerates the prime sequence, and an independent MT19937-64 Bernoulli simulation preserves overlapping pairs. It does **not** call the author's census, RNG or null routines. Rates, as specified by the null, come from the shipped tables. The extra ablation conditions only on `(p mod 120, 7 | p−1)`.

Exact commands:
```sh
python3 reports/round4_astra/check_tables.py
# primecounts.txt is the successor marginal by mod 840, plus prime 7:
python3 - <<'PY'
import numpy as np
z=np.loadtxt('results/joint_residue_tables_1e9.txt',dtype=np.int64); c=np.zeros((840,2),dtype=np.int64)
for m,h,a,b,u,v,w,x in z[z[:,0]==840]: c[b]+=u+w,v+x
c[7,1]+=1
np.savetxt('reports/round4_astra/primecounts.txt',c,fmt='%d')
PY
g++ -O3 -Wall reports/round4_astra/null.cpp -o reports/round4_astra/null
/usr/bin/time -p reports/round4_astra/null
```
Raw output (full precision/logs in `E/tables.log`, `E/null.log`):
```
M 120 N 50847530 shares cov,w1,w0,sym [98.68694337 96.1427736 99.21784795 97.68031078]
k 256 k/2N 2.5173297503339886e-06
mass Xconst,X0only,X1only 0.49995592706273045 0.49995592706273045 0.0
heldout A 0 to 1 Ntest 25421106 gain -3.19332286196e-06
heldout B 0 to 1 Ntest 25421106 gain  1.89134859592e-06
heldout A 1 to 0 Ntest 25426424 gain -4.8924579234e-06
heldout B 1 to 0 Ntest 25426424 gain  9.36622613076e-08
M 840 N 50847530 shares cov,w1,w0,sym [99.54512622 97.8059318 99.71034219 98.758137]
k 1599 k/2N 1.5723477620250186e-05
mass Xconst,X0only,X1only 0.5000379566126417 0.49995889672517035 7.905988747142684e-05
heldout A 0 to 1 Ntest 25421106 gain -6.11078852291e-05
heldout B 0 to 1 Ntest 25421106 gain -2.54564128692e-05
heldout A 1 to 0 Ntest 25426424 gain -6.07639243635e-05
heldout B 1 to 0 Ntest 25426424 gain -2.71449738868e-05
rate1 0.32841874197050086 other2to6 0.3831072763281244
ratio 0.8572500765796371 6/7 0.8571428571428571
independent sieve primes 50847531
mode=120 scored=120 R=30 mean=2.505186024e-06 sd=2.249023236e-07 ge=0
mode=120 scored=840 R=30 mean=1.707631757e-05 sd=5.612133127e-07 ge=1
mode=840 scored=120 R=30 mean=8.955660451e-06 sd=3.835398428e-07 ge=8
mode=840 scored=840 R=30 mean=1.714457696e-05 sd=5.457550836e-07 ge=1
mode=120_x_7divides scored=120 R=20 mean=8.825936161e-06 sd=4.723829123e-07 ge=5
mode=120_x_7divides scored=840 R=20 mean=1.684994864e-05 sd=6.086031908e-07 ge=0
real 54.04
```

## Findings (7)

### 1. MAJOR — A sufficient simulation is not an exclusive attribution

**Quotes:** §15: “The excess … is therefore residue information”; abstract/README: “not information carried by the predecessor's status”.

**Command/output:** independent null command above: `M=840`, mod-120 statistic, **8/30** exceed observed; narrower divisibility ablation **5/20**. These agree qualitatively with 24/100. “A residue-only model reproduces the magnitude” is fair. “Therefore … not information” is not: failure to reject this fitted model does not identify the real sequence's data-generating mechanism or rule out an additional predecessor effect. The predecessor label is precisely the proxy that carries the coarse-baseline information in the simulation. Say **“compatible with, and largely accounted for by, the mod-840 residue-only benchmark; no additional effect established.”** Repeat that qualification in abstract, summary and README, not only §15.

The new null genuinely preserves overlap and is a valid **parametric simulation benchmark under conditional independence**. It is not a distribution-free test of the categorical attribution. Rates are plug-in estimates from these same data. Dependence/drift beyond residues is removed rather than calibrated; overlap alone does not preserve it. There is no universal direction of bias in the coarse statistic. A concrete counterexample uses an equally weighted latent residue R with conditional Bernoulli margins .2/.8 and the same R in each pair. Independent statuses give pooled `[.34,.16,.16,.34]`; admissible negative or positive conditional dependence gives `[.30,.20,.20,.30]` or `[.50,0,0,.50]` with **unchanged residue rates**:
```sh
python3 - <<'PY'
import math
for t in [.34,.30,.50]:
 j=[t,.5-t,.5-t,t]
 print(j, sum(p*math.log(p/.25) for p in j if p))
PY
```
```
[0.34, 0.15999999999999998, 0.15999999999999998, 0.34] 0.06627772298751905
[0.3, 0.2, 0.2, 0.3] 0.020135513550688863
[0.5, 0.0, 0.0, 0.5] 0.6931471805599453
```
Thus independence can raise or lower the comparison; no demonstrated directional correction is available for these primes.

**Mechanism qualification:** 840=120×7, so there is no additional congruence coordinate other than mod 7. The full empirical rate table nevertheless permits arbitrary mod-7 distinctions/interactions and sampling noise, not just the event `7 | p−1`; it does not isolate the order-test mechanism. My restricted ablation supplies stronger evidence for that event and also reproduces the magnitude. The rate ratio is numerically very close to 6/7: **“consistent with the heuristic factor” survives**, provided it is not presented as a calibrated equality test or proof that this is the entire mechanism.

### 2. MINOR — “We do not read this either way” suppresses the direction of a real benchmark departure

**Quote:** “2 of the 100 replicates reach it … We do not read this either way”.

**Commands:** independent null above; `python3 reports/round4_astra/probes.py`.
```
mode=840 scored=840 R=30 mean=1.714457696e-05 sd=5.457550836e-07 ge=1
2/100 exact 95% interval [0.00243134 0.07038393] plus-one tail 0.0297029702970297
```
It is honest **not to infer a genuine predictive residual** from an assumption-sensitive simulation. It is inaccurate to imply the benchmark is directionless: there is a modest positive departure, worth naming as unresolved. The displayed interval is Monte Carlo uncertainty for the exceedance frequency, not a prime-sequence confidence interval; the plus-one value is descriptive here, not automatically a calibrated p-value for a plug-in null. Negative held-out scores do not erase this in-sample departure. Recommend: **“a modest positive departure from this benchmark, not established as a replicating predictive effect.”** “Neither held-out estimator gains at modulus 840” is true in **both** directions.

### 3. MINOR — Constant in the fit half does not mean splitting can only hurt held-out scoring

**Quote, new caption:** estimator B avoids a cell “whose successor status is constant in the fit half, where splitting can only add smoothing penalty”.

**Exact command:** `python3 reports/round4_astra/constantcheck.py`.
```
120 0 constant-fit cells where splitting wins 0 winning log-score 0.0
120 1 constant-fit cells where splitting wins 0 winning log-score 0.0
840 0 constant-fit cells where splitting wins 34 winning log-score 20.071714993961663
840 1 constant-fit cells where splitting wins 48 winning log-score 42.51533708163805
```
This is an actual-data counterexample to the unqualified rationale: a fit-constant cell can contain the opposite outcome in the test half, and the extra smoothing can then help. Say **“avoiding a smoothing penalty when constancy persists”**, or explicitly restrict “can only” to resubstitution. The eight reported scores themselves are correct. I found no separate sentence using B's two positive mod-120 gains as inferential evidence; their explicit disclaimer survives.

### 4. MINOR — A residual universal filter claim survives the new scope language

**Quotes:** abstract “The one genuinely usable filter”; summary/README “the only usable filter”; §15 “The only filter that does real work”.

**Exact command:**
```sh
rg -n 'one genuinely usable|only usable|only filter|small-.*early exit' FRONT.tex SECTION_content.tex README.md
```
Relevant raw text:
```
FRONT.tex:63: ... The one genuinely usable filter ...
FRONT.tex:185: the only usable filter is the elementary non-residue test ...
SECTION_content.tex:183:The only filter that does real work is the necessary condition ...
SECTION_content.tex:204: ... + small-$\ell$ early exit ...
README.md:77: ... the only usable filter is the elementary non-residue test ...
```
The adjacent workload itself reports useful additional small-ℓ rejection. Restrict this to **“the quadratic filter supplied by these exclusion laws is already elementary”**, not a universal inventory of useful filters. The newly scoped opening, discussion, model/workload disclaimer, and non-impossibility paragraph otherwise survive; I found no surviving “any threshold” claim.

### 5. MAJOR — Blind PDF improved; blind supplement still exposes searchable self-provenance

**Quote, build:** “blind supplement … audit=clean”; anonymiser promises “every DOI, every URL … THIRD-PARTY allowlist”.

**Exact commands:**
```sh
unzip -q submission/artin_correlations_blind_supplement.zip -d reports/round4_astra/blind
sed -n '13,17p' reports/round4_astra/blind/lean/Artin/PairExclusion.lean
sed -n '19,25p' reports/round4_astra/blind/lean/Artin/PrimitiveRootBridge.lean
python3 reports/round4_astra/probes.py
```
Raw output:
```
Formalization of **Theorem 3** of Paper 3 ("Cross-base correlations of Artin
status at a single prime"), the paper's headline deterministic result:
## Provenance
This proof was drafted by a local model (`qwen3:32b`, run on a GMKtec mini PC
via Ollama, no internet/Mathlib-docs access) across 6 supervised rounds:
DATACITE 200
10.5281/zenodo.22878204 ... CROSS-BASE CORRELATIONS OF ARTIN PRIMES ... ['Bald, Joshua']
10.5281/zenodo.22878205 ... CROSS-BASE CORRELATIONS OF ARTIN PRIMES ... ['Bald, Joshua']
PDF pages 28 Author '' URI annotations 0
PROBE https://research.example.org/j-b/ ORCID: 0000-0002-1825-0097 j-b@proton.me doi:10.1234/identifying-record AUDIT []
```
The DataCite request in `probes.py` searches the surviving title phrase, rather than supplying a known DOI. The supplement links its own formalization to an identifiable earlier paper; `Paper2.lean` also retains a named earlier paper and its dated audit history. This is stronger provenance than unavoidable thematic similarity of a public preprint. Workspace paths, machine nickname **jack**, and the detailed model/hardware drafting history also survive (`E/provenance.log`). Remove unnecessary source-paper lineage and operational histories from the blind package. The promised general DOI/URL allowlist **does not exist in `audit()`**: only known identifiers, Zenodo patterns and GitHub links are checked. The arbitrary-identifier probe is a detector limitation, not an assertion those injected identifiers occur in the real PDF.

I could not find a remaining author name, own DOI, repository link, ORCID, identifying PDF metadata or external URI annotation in the actual blind PDF. Its round-3 direct leak is fixed; supplement anonymity is not.

### 6. MAJOR — Anonymisation itself makes the shipped Lean package unbuildable

**Quote:** “third-party dependency URLs … are not provenance” immediately followed by replacing them with `[lean-dependency]`.

**Exact command:**
```sh
(cd reports/round4_astra/blind/lean; timeout 25 lake build); echo EXIT=$?
```
Raw output:
```
info: mathlib: cloning [lean-dependency]
info: stderr:
fatal: repository '[lean-dependency]' does not exist
error: external command 'git' exited with code 128
EXIT=1
```
This is new breakage introduced by the fix, not an old theorem/build audit. All third-party dependency URLs are erased from `lake-manifest.json`. Preserve genuine Lean/Mathlib URLs and allowlist them; they identify dependencies, not the author. Validate the **scrubbed artifact**, not just its text scanner.

### 7. MINOR — “All build dependencies” still fails a clean source extraction

**Quote, build:** source zip “including every build dependency”; “a source or reproduction archive that cannot rebuild from a clean extraction” causes failure.

**Exact commands:**
```sh
mkdir -p reports/round4_astra/clean
unzip -q submission/artin_correlations_source.zip -d reports/round4_astra/clean
(cd reports/round4_astra/clean; MATH_AUDIT_BASE="$PWD" CONSOLIDATED_DIR="$PWD" bash build_submission.sh)
```
Raw output:
```
== assemble (regenerate artin_correlations.tex from the two source papers)
FileNotFoundError: [Errno 2] No such file or directory: '.../clean/Paper 1 Full file/paper/consecutive_artin.tex'
```
`assemble.py` still requires both external source papers and defaults to the author's absolute workspace. The build's “clean-extraction check” only invokes the anonymiser and LaTeX, not `build_submission.sh`; the reproduction zip is not build-tested. The anonymiser omission is fixed, but the new completeness assertion is false. Permit a standalone build from the already assembled TeX, or include assembly inputs and portable paths.

Control: I also copied the complete consolidated tree **without previous build/submission artifacts** to `E/clean-full/`, ran `(cd reports/round4_astra/clean-full; CONSOLIDATED_DIR="$PWD" bash build_submission.sh)`, retaining access to the external source-paper defaults. Raw output (`E/full-build.log`):
```
pages=28 errors=0 overfull_hbox=0 undefined=0
blind pages=28 (named 28) sections=17 audit=clean
source zip: all dependencies present; anonymiser and LaTeX run from a clean extraction
blind supplement: 75 files, audit=clean
== done
```
So the build works with undeclared external inputs, but its successful scanner neither detects findings 5–6 nor proves archive completeness.

## Explicit verdict on the nine requested items

| Item | Verdict |
|---|---|
| 1. Withdrawal / residue-status null | Old bound withdrawal **PASS**. Null implementation/qualitative results **PASS**; categorical exclusive attribution **FAIL**, finding 1. Independence has no established one-way bias. |
| 2. Prime-7 mechanism | Rates/ratio **PASS**; “consistent with 6/7” fair descriptively. Restricted independent ablation also reproduces the magnitude. Exclusive mechanistic identification remains too strong. |
| 3. Mod-840 residual | Numerical scale **PASS**; modest positive benchmark departure should be acknowledged, not dismissed as directionless. Both estimators lose in both directions. |
| 4. Eight held-out gains / N_test | **PASS**, all independently reproduced above. No inferential reliance on B's positives found. New “only smoothing penalty” rationale **FAILS** on actual test cells. |
| 5. k / reference scale | **PASS:** 256/1599 and 2.5173e−6/1.57235e−5. “For scale only”, regular-iid caveat and explicit both-margin definition are appropriate. |
| 6. Four conventions / consistency | **PASS:** all eight shares, zero-within completion, ranges and X-constant mass. Independent identity errors ≤2.8e−17. Abstract/summary/discussion/README agree at their stated precisions (`E/shares-text.log`); regenerated merged TeX equals supplied merged TeX. |
| 7. Scope | Main new scope disclaimers **PASS**; categorical residue attribution and “only usable filter” still overclaim, findings 1/4. |
| 8. Anonymisation / clean build | Actual PDF direct identifiers **not found**. Supplement provenance and newly broken dependency URLs **FAIL**; standalone source rebuild **FAIL**, findings 5–7. |
| 9. Independent rederivation / null | **DONE:** new table scorer; new prime sieve and RNG; 30 replicates per original null, plus 20 restricted-channel ablations. 0/30 and 8/30 qualitatively confirm 0/100 and 24/100; not a claim to reproduce their exact random draws. |

## Could not falsify

- Every requested decomposition share, k, mass, held-out gain and split size.
- Correct pair-overlap preservation in the new residue-status null; its simulated magnitudes under an independent RNG.
- The mod-7 rates and descriptive 6/7 agreement; the narrower divisibility-only mechanism also suffices in simulation.
- Explicit refusal to treat estimator B's mod-120 positives as evidence; no positive mod-840 held-out score.
- Removal of the prior pseudo-bound / split-sample signal estimate / sign-heterogeneity inference and the actual PDF's identifying provenance footnote.

**Disposition:** retain the arithmetic and measured negative; qualify attribution, name the unresolved benchmark residual, correct two categorical captions/conclusions, and repair/audit the actual blind package rather than certifying it with the present scanner.
