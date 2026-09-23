/-
Copyright (c) 2026 J. Bald. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.
Authors: J. Bald
-/
import Mathlib.NumberTheory.LegendreSymbol.QuadraticReciprocity
import Mathlib.Tactic
import Artin.PrimitiveRootBridge

set_option linter.style.header false

/-!
# Pair exclusion law for same-prime Artin status

Formalization of **Theorem 3** of Paper 3 ("Cross-base correlations of Artin
status at a single prime"), the paper's headline deterministic result:

> Let `a, b` be positive integers and set `d = sqf(ab)`. Then no odd prime `p`
> with `(d|p) = -1` is simultaneously Artin base `a` and Artin base `b`.

Since `(d|p) = -1` is a union of residue classes mod `4d` of density `1/2`,
this bars **half of all primes** for **every** pair of bases — no third base
required, no special configuration. The triple exclusion law formalized in
`Artin/TripleExclusion.lean` is the special case in which the barring character
`χ_d` is supplied by a third base `c` of the set with `sqf(c) = sqf(ab)`:
Artin status for `c` forces `(d|p) = χ_c(p) = -1`.

## Why this file

`Artin/TripleExclusion.lean` was written first and proves the triple form
directly. This file isolates the actual content — the two-base parity step —
and re-derives the triple form from it in three lines
(`not_all_three_nonresidue_of_pair`), mirroring the structure of the paper
after its §2 was reorganised around the pair law.

## Encoding of the hypotheses

* **`d = sqf(ab)`** is expressed as `d * u ^ 2 = a * b * v ^ 2` with
  `u, v ≠ 0` mod `p`. This is equivalent to `sqf d = sqf (ab)` and avoids
  needing a `sqf` function. Witnesses are immediate in practice: for
  `(a,b) = (5,10)` one has `sqf(50) = 2` and `2 * 5 ^ 2 = 5 * 10 * 1 ^ 2`.
* **"Artin base `a`"** enters either as the raw character condition
  `legendreSym p a = -1` (the parity core, which holds for the genuine
  `legendreSym` however the `-1` arose) or as an actual `IsPrimitiveRoot`
  fact, discharged via `Artin/PrimitiveRootBridge.lean`.

## Main results

* `not_both_nonresidue_of_barring_character` — the parity core.
* `not_both_primitiveRoot_of_barring_character` — same, from `IsPrimitiveRoot`.
* `not_all_three_nonresidue_of_pair` — triple exclusion derived from the pair law.
* `not_both_primitiveRoot_five_ten` — a fully concrete instance with the
  barring condition discharged by quadratic reciprocity: bases `5` and `10` are
  never both primitive roots for `p ≡ 3, 5 (mod 8)`. Nothing is assumed about
  Legendre symbols here; the hypotheses are a congruence and two
  `IsPrimitiveRoot` facts.
-/

namespace ArtinExclusion

open ZMod

variable {p : ℕ} [Fact p.Prime]

/-- If `x` is a nonzero square modulo `p`, its Legendre symbol is `1`.
(Local copy; `TripleExclusion.lean` has its own private version.) -/
private theorem legendreSym_sq' {s : ℤ} (hs : ((s : ZMod p)) ≠ 0) :
    legendreSym p (s ^ 2) = 1 := by
  have : legendreSym p (s ^ 2) = (legendreSym p s) ^ 2 := by
    rw [sq, sq, legendreSym.mul]
  rw [this]
  rcases legendreSym.eq_one_or_neg_one p hs with h | h <;> rw [h] <;> norm_num

/-- **Pair exclusion, character-parity core (Paper 3, Theorem 3).**

If `d * u ^ 2 = a * b * v ^ 2` (that is, `sqf d = sqf (ab)`) with square
witnesses `u, v` nonvanishing mod `p`, and `d` is a quadratic non-residue mod
`p`, then `a` and `b` cannot *both* be non-residues.

By complete multiplicativity `χ_a(p) χ_b(p) = χ_d(p) = -1`, so the two symbols
have opposite signs and at least one equals `+1`.

Note the asymmetry with the triple law: here the third quantity `d` need not be
a primitive root, or even relate to the base set at all — only its quadratic
character matters, and that is a condition on `p` modulo `4d`. -/
theorem not_both_nonresidue_of_barring_character
    {a b d u v : ℤ}
    (hrel : d * u ^ 2 = a * b * v ^ 2)
    (hu : ((u : ZMod p)) ≠ 0) (hv : ((v : ZMod p)) ≠ 0)
    (hd : legendreSym p d = -1) :
    ¬ (legendreSym p a = -1 ∧ legendreSym p b = -1) := by
  rintro ⟨hA, hB⟩
  -- take Legendre symbols of both sides of the relation
  have h := congrArg (fun z : ℤ => legendreSym p z) hrel
  simp only [legendreSym.mul, legendreSym_sq' hu, legendreSym_sq' hv, mul_one] at h
  -- h : legendreSym p d = legendreSym p a * legendreSym p b
  rw [hA, hB, hd] at h
  norm_num at h

/-- **Pair exclusion from genuine primitive-root hypotheses.** Same as
`not_both_nonresidue_of_barring_character`, but "Artin base `a`" / "Artin base
`b`" are actual `IsPrimitiveRoot` facts rather than assumed character values;
the translation is discharged by
`legendreSym_eq_neg_one_of_isPrimitiveRoot`. -/
theorem not_both_primitiveRoot_of_barring_character
    (hp2 : p ≠ 2)
    {a b d u v : ℤ}
    (hrel : d * u ^ 2 = a * b * v ^ 2)
    (hu : ((u : ZMod p)) ≠ 0) (hv : ((v : ZMod p)) ≠ 0)
    (hd : legendreSym p d = -1)
    (ha : IsPrimitiveRoot ((a : ZMod p)) (p - 1))
    (hb : IsPrimitiveRoot ((b : ZMod p)) (p - 1)) :
    False :=
  not_both_nonresidue_of_barring_character hrel hu hv hd
    ⟨legendreSym_eq_neg_one_of_isPrimitiveRoot hp2 ha,
     legendreSym_eq_neg_one_of_isPrimitiveRoot hp2 hb⟩

/-- **Triple exclusion as a corollary of pair exclusion.** This is the
derivation the paper now gives: if `p` is Artin base `c` then `χ_c(p) = -1`,
and `χ_c = χ_d` for `d = sqf(ab) = sqf(c)`, so the pair law applies with `c`
itself as the barring quantity.

Compare `ArtinExclusion.not_all_three_nonresidue` in
`Artin/TripleExclusion.lean`, which proves the same statement directly. Both
are kept: the direct proof is self-contained, this one exhibits the logical
dependence. -/
theorem not_all_three_nonresidue_of_pair
    {a b c s t : ℤ}
    (hrel : c * s ^ 2 = a * b * t ^ 2)
    (hs : ((s : ZMod p)) ≠ 0) (ht : ((t : ZMod p)) ≠ 0) :
    ¬ (legendreSym p a = -1 ∧ legendreSym p b = -1 ∧ legendreSym p c = -1) := by
  rintro ⟨hA, hB, hC⟩
  exact not_both_nonresidue_of_barring_character hrel hs ht hC ⟨hA, hB⟩

/-- **A fully concrete instance, congruence hypothesis only.**

Bases `5` and `10` are never both primitive roots modulo a prime
`p ≡ 3, 5 (mod 8)`. Here `sqf(5 · 10) = 2` with witness
`2 * 5 ^ 2 = 5 * 10 * 1 ^ 2`, and the barring condition
`legendreSym p 2 = -1` is discharged from `p mod 8` by Mathlib's
`legendreSym.at_two` together with the explicit character `χ₈`.

This is the form asserted in the paper's §2 display: the excluded set is a
union of residue classes, and no Legendre symbol appears in the hypotheses. -/
theorem not_both_primitiveRoot_five_ten
    (hp2 : p ≠ 2) (hp5 : (5 : ZMod p) ≠ 0)
    (hmod : p % 8 = 3 ∨ p % 8 = 5)
    (h5 : IsPrimitiveRoot (((5 : ℤ) : ZMod p)) (p - 1))
    (h10 : IsPrimitiveRoot (((10 : ℤ) : ZMod p)) (p - 1)) :
    False := by
  have hd : legendreSym p 2 = -1 := by
    rw [legendreSym.at_two hp2, ZMod.χ₈_nat_mod_eight]
    rcases hmod with h | h <;> rw [h] <;> decide
  refine not_both_primitiveRoot_of_barring_character hp2
    (d := 2) (u := 5) (v := 1) (by ring) ?_ ?_ hd h5 h10
  · push_cast
    exact hp5
  · simp

/-- The same statement for bases `2` and `6`, barred on `p ≡ 5, 7 (mod 12)`.
Here `sqf(2 · 6) = 3`, and the barring condition is
`legendreSym p 3 = -1`, which by quadratic reciprocity is determined by
`p mod 12`. We state it with the character condition explicit rather than
re-deriving the reciprocity computation, since the pattern is exactly that of
`not_both_primitiveRoot_five_ten`. -/
theorem not_both_primitiveRoot_two_six
    (hp2 : p ≠ 2)
    (hd : legendreSym p 3 = -1)
    (h2 : IsPrimitiveRoot (((2 : ℤ) : ZMod p)) (p - 1))
    (h6 : IsPrimitiveRoot (((6 : ℤ) : ZMod p)) (p - 1)) :
    False := by
  refine not_both_primitiveRoot_of_barring_character hp2
    (d := 3) (u := 2) (v := 1) (by ring) ?_ ?_ hd h2 h6
  · -- `u = 2` is nonzero mod `p` since `p ≠ 2`
    have hp : (p : ℕ).Prime := Fact.out
    have hne : ((2 : ℕ) : ZMod p) ≠ 0 := by
      rw [Ne, ZMod.natCast_eq_zero_iff]
      intro hdvd
      exact hp2 (((Nat.prime_dvd_prime_iff_eq hp Nat.prime_two).mp hdvd))
    simpa using hne
  · simp

end ArtinExclusion
