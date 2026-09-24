# Round-4 adversarial confirmation audit (narrow scope): opus-5.5

**Verdict: MINOR REVISION.**

- The science and the §15 text now hold up: all of F1, F2, F3 and F5 from round 3 are implemented correctly.
- One **MAJOR packaging blocker** remains. The blind supplement still identifies the author, through a new route (F1 below).
- The shipped zips are stale (F2 below).

## Scope and route

**What I audited.** The public commit `ee91f8f`. Its files are byte-identical to the local tree:

| File | sha256 prefix (local = public) |
|---|---|
| `SECTION_content.tex` | `ba52a414` |
| `FRONT.tex` | `da07f101` |
| `DISC.tex` | `569c01d0` |
| `artin_correlations.tex` | `f7afe0ad` |
| anonymous PDF | `66bfb148` |
| blind supplement | `d16a79c9` |

Evidence is in `reports/opus55_r4_evidence/`. All commands below run from there.

**Route (independent of the author's code):**
- My round-3 factor-sieve census (`fs_census.c`, no trial division) and its per-prime stream (p mod 840, status, hash bit).
- Own numpy nulls with a numpy RNG, including a **class-restricted null**: statuses depend only on p mod 120 plus the single bit [7 | p−1].
- A **control null** with a same-size split that does not involve 7.
- An exact re-implementation of the author's `smix` RNG, to check replicate independence.
- A null distribution for the held-out estimators themselves. The author has not run this.
- A clean-copy `build_submission.sh`.

**Consistency checks, all passing:**
- The author's shipped tables are byte-equal to mine: `tables_cmp.py` → `True True / True True`.
- `section15.py` rerun on them is byte-identical to `section15_1e9.json`: `s15-identical`.
- `artin_payoff resnull 1e9 100`, recompiled from the clean copy, is byte-identical to `resnull_1e9.json`: `resnull-byte-identical`.

## Findings

### F1 — MAJOR (blind package) — the blind supplement still identifies the author

**The blind PDF is clean now.** It has no DOIs, GitHub links, names, ORCID or metadata author. The only URLs left are two third-party DOIs.

**The supplement is not clean.** Command:
```
unzip blind_supplement.zip; grep -rIn "Cross-base correlations of Artin" .
./lean/Artin/TripleExclusion.lean:15:Formalization of **Theorem 2** of Paper 3 ("Cross-base correlations of Artin
./lean/Artin/PairExclusion.lean:15:Formalization of **Theorem 3** of Paper 3 ("Cross-base correlations of Artin
```

**One search of that title identifies the author:**
```
curl zenodo.org/api/records?q="Cross-base correlations of Artin"
CROSS-BASE CORRELATIONS OF ARTIN PRIMES: ENTANGLEMENT, EXCLUSION, AND THE STRUCTURE OF p − 1 | ['Bald, Joshua'] 10.5281/zenodo.22878205
```

**Other provenance the audit list does not cover** (15 files in a clean rebuild):
- `code/results.txt:1` "artin_payoff.c on jack (GMKtec, 32 cores…)".
- `lean/Artin/PrimitiveRootBridge.lean:21` "drafted by a local model (qwen3:32b, run on a GMKtec mini PC via Ollama)".
- `results/*_2026-09-11.md` "run as 96 shards on jack".
- `/home/work/.openclaw/workspace/Prime Math/Paper 1 Full file/...` in `analysis_1e9_log.txt`, `dataset_sha256.txt` and `pilot_log.txt`.
- "Paper 1/2/3" cross-references throughout the Lean sources.

**The filenames also lead to the author.**
- The files are named `artin_correlations_*`.
- A GitHub search for `artin-correlations` returns `jpbald93/artin-correlations`.
- That repository hosts the byte-identical "anonymous" PDF (sha `66bfb148…`).

**Fix:**
- Scrub the paper titles, "Paper N", host names and absolute paths from the supplement, or drop the provenance files from it.
- Add the titles of the author's own deposits to `OWN_*` in `anonymise.py`.
- Consider a neutral filename.

### F2 — MINOR — the shipped zips are stale relative to the repository and contradict §15

**Timeline:**
- The zips were built at 20:32:16–20:32:23.
- `code/results.txt` was rewritten at 20:33:29.
- `results/section15_sha256.txt` was created at 20:33.

**Evidence:**
```
unzip -p submission/artin_correlations_reproduction.zip code/results.txt | sha256sum -> 828160ce…  (repo file: c6d06ae7…)
grep 1702 <blind supplement>/code/results.txt
105: "cells": {"x_nondegenerate": 4123, "xy_nondegenerate": 1702, ...}
106: "optimism": {"k": 1702, "k_over_2N": 1.674e-05}
zipinfo diff (my clean rebuild vs shipped):  < results/section15_sha256.txt   (missing from shipped)
```

**Consequences.**
- Both shipped supplements still carry round-3's k = 1,702 and 1.674e−5. §15 correctly says 1,599 and 1.6e−5.
- My clean rebuild (`build.log`: exit 0, 28/28 pp, "audit=clean", 75 files) has 0 hits for `1702`.

**Fix:** re-run `build_submission.sh` after the last results edit, and commit the output.

### F3 — MINOR — the front matter states more than §15 does, and two unscoped universal claims survive

**Abstract (FRONT l.59) and README l.73–75:**
> "it is residue information (the prime 7 of p−1), not information carried by the predecessor's status"

§15 supports only "reproduced by a residue-only model", which means **accounted for**. A null that fits cannot show that the predecessor carries *no* information. At mod 840, §15 itself reports a residual 2/100.

**Abstract l.99:**
> "We conclude that these correlations describe arithmetic structure rather than supply algorithms"

This is unscoped. Every other occurrence is limited to "the estimators and workload tested".

**§15:**
> "The only filter that does real work is the necessary condition"

and, in the summary:
> "the only usable filter is the elementary non-residue test"

Yet `tab:filter` shows the small-ℓ early exit doing real work (15% faster). It is labelled "test-ordering", but it is still a filter that rejects on its first failure.

**Fix:**
- Replace "not information carried by" with "accounted for by residues, with no detectable predecessor-specific signal in the estimators tested".
- Scope the conclusion sentence.
- Replace "only usable filter" with "only new filter".

### F4 — MINOR — mod 840: the author's null sd is larger than in every other route, and "We do not read this either way" mildly under-claims

**Command:** `python3 nulls_r4.py 25 44` (float32 numpy RNG), plus `author_rng.py 12` (the author's exact smix seeds).
```
M840  reps=25  mod840: mean=1.6694e-05 sd=4.500e-07 ge_obs=0 z=3.63
author-RNG 12 reps mod840: mean 1.6924e-05 sd 5.00e-07 ge 1.8326e-5: 0
round-3 (30 reps): mean 1.6919e-05 sd 5.30e-07 z=2.66
author (100 reps): mean 1.7062e-05 sd 6.79e-07 2/100 z=1.86
```

**Reading.**
- Three independent routes give sd 4.5–5.3e-7. The author's 100 reps give 6.8e-7.
- A per-replicate dump is not shipped, so I cannot tell whether a few outliers inflate it.
- Across the routes the observed value sits at z ≈ 1.9–3.6, with a tail frequency of 0/67 in my own reps and 2/100 in the author's.
- That is a consistent small **in-sample** excess. It does **not** replicate held out. My held-out null (F6) at M=840 gives z = +0.35, +0.65, +0.71 and +0.33.
- "Slightly above … we do not read this either way" is defensible. More accurate would be: "a small in-sample excess (2/100; z 1.9–3.6 across independent null implementations) with no held-out counterpart".

**Fix:** also dump the per-replicate values.

### F5 — NIT — the caption's "where splitting can only add smoothing penalty" is false at mod 840

**Command:** `python3 constcells.py`
```
120 even->odd fit-Y-constant X-varying cells 256 of which test has the other Y value: 0
840 even->odd fit-Y-constant X-varying cells 1722 of which test has the other Y value: 39 test pairs in them 880
840 odd->even ... 1732 ... 42 ... 797
```

In those 81 cells, splitting changes the predicted probability of the unseen outcome. That change can help or hurt, so the penalty is not guaranteed. The caption holds at mod 120.

**Fix:** "where splitting mostly adds smoothing penalty".

### F6 — NIT — the paper's "We have not tested that held-out figure against the residue-status null": I ran that test, and it supports the text

**Command:** `python3 heldout_null.py 10`
```
null M=840: 120B eo: obs +1.891e-06 null mean +1.548e-06 sd 7.35e-07 ge_obs 3/10 z +0.47
            120B oe: obs +9.366e-08 null mean +1.450e-06 sd 1.39e-06 ge_obs 9/10 z -0.97
            840A/B (4 stats): z +0.35 +0.65 +0.71 +0.33
null M=120: 120B eo z +6.17, oe z +4.05   (so B's positive mod-120 gain is itself residue info mod 7)
```

**Reading.** Estimator B's positive mod-120 gain is exactly what the M=840 residue null produces. The author could replace the disclaimer with this result.

No sentence leans on B. B is mentioned only with that disclaimer.

### F7 — NIT — leftovers

- **`section15.py`** has the comment `# cross-check against the driver's held-out table (estimator A, even->odd), 1e9 values`, but no code follows it. (The values do agree: `results.txt` l.78–79 = A even→odd.)
- **The blind PDF** still says "The author takes responsibility …" and "reported by the author". The singular "author" is a mild hint.
- **DISC (ii)** says "at the scale of $10^9$ primes". It is 5.08×10⁷ primes below 10⁹. This may predate round 3.

## Verdict per item

1. **F1 implementation: CORRECT and FULL-STRENGTH.**
   - It is my serial null: per-prime redraw, overlap preserved, with the class rate mod M.
   - Author: M=120 gives 2.53e−6 / 2.1e−7 and 0/100; M=840 gives 8.82e−6 / 4.9e−7 and 24/100 (z 0.74).
   - Mine, round 3: z 31.8 and 0.75. Round 4: M=120 gives 2.51e−6 / 2.48e−7, 0/25, z 26.9; M=840 gives 8.93e−6 / 4.36e−7, 7/25, z 0.58.
   - The two sets are consistent. The author's RNG replicates are independent: agreement 0.8404, against 0.8404 expected.
   - "A model in which statuses depend on nothing but p mod 840 reproduces it" is **SUPPORTED** by 24/100 (the observed value is at about the 76th percentile).
2. **"The prime 7": VERIFIED, not an overclaim.**
   - Rates: 0.328419 against 0.383226, 0.383119, 0.382981, 0.383102 and 0.383109. Ratio 0.85725 against 6/7 = 0.85714.
   - The decisive test (`nulls_r4.out`): a null using only p mod 120 plus the single bit [7|p−1] reproduces the mod-120 excess (mean 8.68e−6, 4/25 ≥ obs, z 0.89).
   - A same-construction control bit (p mod 7 ∈ {2,4}) does not (z 21.8, 0/25).
3. **Mod 840: honest but slightly under-claimed.** See F4.
4. **Held-out table: all eight gains and both N_test VERIFIED.**
   - My vectorised re-implementation on my own tables (`heldout_r4.out`) gives A −3.1933e−6 / −4.8925e−6 / −6.1108e−5 / −6.0764e−5 and B +1.8913e−6 / +9.3662e−8 / −2.5456e−5 / −2.7145e−5.
   - N_test is 25,421,106 and 25,426,424.
   - The round-3 B figures at 840 (−2.51e−5 / −2.69e−5) also refused to split X-constant fit cells. The paper's B, which does not, is internally consistent.
   - No sentence leans on B. There are caption nits (F5, F6).
5. **Conventions, shares and k: VERIFIED.**
   - Shares: 96.14 / 97.81 / 99.22 / 99.71 / 97.68 / 98.76 / 98.69 / 99.55. Ranges 96.1–99.2 and 97.8–99.7. Mass 0.49996, all of it X0-only at mod 120.
   - k = 256 / 1,599, giving k/(2N) = 2.517e−6 / 1.572e−5.
   - `completion.py`: completing q0 by 0 or 1 instead of q1 would move the mod-840 w¹ share to 98.40 / 96.91. The stated completion is the one used: 97.81.
   - The abstract, summary, DISC (iii) and README agree with §15 on every share and range. **Exception:** the stale `results.txt` inside the zips (F2).
6. **Universal claims: three survive.** See F3: the abstract's conclusion sentence, "only usable filter", and "not information carried by". The "no computational use" occurrences are all scoped.
7. **Anonymiser: blind PDF CLEAN (defeat attempts failed); blind supplement DEFEATED** (F1).
   - `build_submission.sh` from a clean copy exits 0.
   - The source zip rebuilds, and the anonymiser runs from it.
   - The rebuilt anonymous PDF text is identical to the shipped one.
8. **Independent reproduction: DONE.**
   - Tables are byte-equal. Nulls reproduced with a numpy RNG, with the author's exact RNG (12 reps: M=840 mod-120 mean 8.88e−6, 3/12 ≥ obs), and with the author's C recompiled (byte-identical JSON).

## Could not falsify

- Every number in the new §15 text and in `section15_1e9.json`.
- The per-prime residue null and its preservation of the overlap.
- The mod-7 mechanism and the 6/7 ratio.
- The four decomposition identities, with the convention stated.
- The k definition and value.
- The held-out split direction and the N values.
- Front-matter agreement on all shares and ranges.
- A clean-copy build and a source-zip rebuild.
- Blind-PDF text and metadata.
- Independence of the author's null replicates.
