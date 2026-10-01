import Artin

/-! Satisfiability certificates (outside the library; not imported by it).
For every theorem with hypotheses: a Lean-checked example showing those hypotheses can all be met
simultaneously by concrete values. Theorems whose conclusion is `False` assert that their hypotheses
are jointly impossible; for those we certify that every hypothesis but the last is satisfiable,
so the impossibility is not caused by a trivially inconsistent subset. -/

set_option linter.unusedVariables false
set_option linter.unnecessarySeqFocus false
set_option linter.style.longLine false

-- hypotheses of ArtinExclusion.cast_prime_ne_zero are satisfiable
example : ∃ (p : ℕ) (_ : Fact p.Prime) (q : ℕ) (hq : q.Prime) (hne : p ≠ q), True :=
  ⟨13, ⟨by norm_num⟩, 7, by norm_num, by decide, trivial⟩

-- hypotheses of ArtinExclusion.legendreSym_two_eq are satisfiable
example : ∃ (p : ℕ) (_ : Fact p.Prime) (hp2 : p ≠ 2), True :=
by
  refine ⟨13, ⟨by norm_num⟩, by decide, trivial⟩

-- hypotheses of ArtinExclusion.isSquare_cast_five are satisfiable
example : ∃ (n : ℕ) (hn : n % 5 ≠ 0), True :=
by
  refine ⟨1, by decide, trivial⟩

-- hypotheses of ArtinExclusion.legendreSym_five_eq are satisfiable
example : ∃ (p : ℕ) (_ : Fact p.Prime) (hp5 : p ≠ 5) (hp2 : p ≠ 2), True :=
by
  refine ⟨7, ⟨by norm_num⟩, by decide, by decide, trivial⟩

-- hypotheses of ArtinExclusion.chi10_eq_legendreSym are satisfiable
example : ∃ (p : ℕ) (_ : Fact p.Prime) (hp2 : p ≠ 2) (hp5 : p ≠ 5), True :=
by
  refine ⟨7, ⟨by norm_num⟩, by decide, by decide, trivial⟩

-- hypotheses of ArtinExclusion.legendreSym_flip_of_shift_twenty are satisfiable
example : ∃ (p p' : ℕ) (_ : Fact p.Prime) (_ : Fact p'.Prime) (hp2 : p ≠ 2) (hp5 : p ≠ 5) (hq2 : p' ≠ 2) (hq5 : p' ≠ 5) (hshift : (p' : ZMod 40) = (p : ZMod 40) + 20) (hunit : IsUnit ((p : ZMod 40))), True :=
  ⟨3, 23, ⟨by norm_num⟩, ⟨by norm_num⟩, by decide, by decide, by decide, by decide, by decide, (ZMod.isUnit_iff_coprime 3 40).mpr (by norm_num), trivial⟩

-- hypotheses of ArtinExclusion.not_both_artin are satisfiable
example : ∃ (p p' : ℕ) (_ : Fact p.Prime) (_ : Fact p'.Prime) (hp2 : p ≠ 2) (hp5 : p ≠ 5) (hq2 : p' ≠ 2) (hq5 : p' ≠ 5) (hshift : (p' : ZMod 40) = (p : ZMod 40) + 20) (hunit : IsUnit ((p : ZMod 40))), True :=
  ⟨3, 23, ⟨by norm_num⟩, ⟨by norm_num⟩, by decide, by decide, by decide, by decide, by decide, (ZMod.isUnit_iff_coprime 3 40).mpr (by norm_num), trivial⟩

-- hypotheses of ArtinExclusion.chi10_shift_of_gap are satisfiable
example : ∃ (r g : ZMod 40) (hr : IsUnit r) (hg : g = 20), True :=
  ⟨1, 20, isUnit_one, rfl, trivial⟩

-- hypotheses of ArtinExclusion.legendreSym_sq' are satisfiable
example : ∃ (p : ℕ) (_ : Fact p.Prime) (s : ℤ) (hs : ((s : ZMod p)) ≠ 0), True :=
by
  refine ⟨13, ⟨by norm_num⟩, 2, by decide⟩

-- hypotheses of ArtinExclusion.not_both_nonresidue_of_barring_character are satisfiable
example : ∃ (p : ℕ) (_ : Fact p.Prime) (a b d u v : ℤ) (hrel : d * u ^ 2 = a * b * v ^ 2) (hu : ((u : ZMod p)) ≠ 0) (hv : ((v : ZMod p)) ≠ 0) (hd : legendreSym p d = -1), True :=
  ⟨3, ⟨by norm_num⟩, 2, 1, 2, 1, 1, by norm_num, by decide, by decide, (by rw [legendreSym.eq_neg_one_iff]; decide), trivial⟩

-- hypotheses of ArtinExclusion.not_both_primitiveRoot_of_barring_character (all except `hb`) are satisfiable; with `hb` they are jointly impossible, which is the theorem
example : ∃ (p : ℕ) (_ : Fact p.Prime) (hp2 : p ≠ 2) (a b d u v : ℤ) (hrel : d * u ^ 2 = a * b * v ^ 2) (hu : ((u : ZMod p)) ≠ 0) (hv : ((v : ZMod p)) ≠ 0) (hd : legendreSym p d = -1) (ha : IsPrimitiveRoot ((a : ZMod p)) (p - 1)), True :=
  ⟨3, ⟨by norm_num⟩, by decide, 2, 1, 2, 1, 1, by norm_num, by decide, by decide, (by rw [legendreSym.eq_neg_one_iff]; decide), IsPrimitiveRoot.mk_of_lt _ (by norm_num) (by decide) (by intro l h0 h1; interval_cases l <;> decide), trivial⟩

-- hypotheses of ArtinExclusion.not_all_three_nonresidue_of_pair are satisfiable
example : ∃ (p : ℕ) (_ : Fact p.Prime) (a b c s t : ℤ) (hrel : c * s ^ 2 = a * b * t ^ 2) (hs : ((s : ZMod p)) ≠ 0) (ht : ((t : ZMod p)) ≠ 0), True :=
by
  refine ⟨13, ⟨by norm_num⟩, 2, 3, 6, 1, 1, by decide, by decide⟩

-- hypotheses of ArtinExclusion.not_both_primitiveRoot_five_ten (all except `h10`) are satisfiable; with `h10` they are jointly impossible, which is the theorem
example : ∃ (p : ℕ) (_ : Fact p.Prime) (hp2 : p ≠ 2) (hp5 : (5 : ZMod p) ≠ 0) (hmod : p % 8 = 3 ∨ p % 8 = 5) (h5 : IsPrimitiveRoot (((5 : ℤ) : ZMod p)) (p - 1)), True :=
  ⟨3, ⟨by norm_num⟩, by decide, by decide, by decide, IsPrimitiveRoot.mk_of_lt _ (by norm_num) (by decide) (by intro l h0 h1; interval_cases l <;> decide), trivial⟩

-- hypotheses of ArtinExclusion.not_both_primitiveRoot_two_six (all except `h6`) are satisfiable; with `h6` they are jointly impossible, which is the theorem
example : ∃ (p : ℕ) (_ : Fact p.Prime) (hp2 : p ≠ 2) (hd : legendreSym p 3 = -1) (h2 : IsPrimitiveRoot (((2 : ℤ) : ZMod p)) (p - 1)), True :=
  ⟨5, ⟨by norm_num⟩, by decide, (by rw [legendreSym.eq_neg_one_iff]; decide), IsPrimitiveRoot.mk_of_lt _ (by norm_num) (by decide) (by intro l h0 h1; interval_cases l <;> decide), trivial⟩

-- hypotheses of Paper2Rebuild.four_mul_indicator are satisfiable
example : ∃ (x y : ℤ) (hx : x = -1 ∨ x = 1) (hy : y = -1 ∨ y = 1), True :=
by
  refine ⟨1, 1, by decide, by decide, trivial⟩

-- hypotheses of Paper2Rebuild.main_identity are satisfiable
example : ∃ (α : Type) (_ : DecidableEq α) (U : Finset α) (f h : α → ℤ) (hf : ∀ r ∈ U, f r = -1 ∨ f r = 1) (hh : ∀ r ∈ U, h r = -1 ∨ h r = 1), True :=
  ⟨ℕ, inferInstance, ∅, fun _ => 1, fun _ => 1, by simp, by simp, trivial⟩

-- hypotheses of Paper2Rebuild.counting_identity are satisfiable
example : ∃ (α : Type) (_ : DecidableEq α) (U : Finset α) (f h : α → ℤ) (hf : ∀ r ∈ U, f r = -1 ∨ f r = 1) (hh : ∀ r ∈ U, h r = -1 ∨ h r = 1) (N T A B S : ℤ) (hN : N = ((U.filter fun r => f r = -1 ∧ h r = -1).card : ℤ)) (hT : T = (U.card : ℤ)) (hA : A = ∑ r ∈ U, f r) (hB : B = ∑ r ∈ U, h r) (hS : S = ∑ r ∈ U, f r * h r), True :=
  ⟨ℕ, inferInstance, ∅, fun _ => 1, fun _ => 1, by simp, by simp, _, _, _, _, _, rfl, rfl, rfl, rfl, rfl, trivial⟩

-- hypotheses of isPrimitiveRoot_imp_legendreSym_eq_neg_one are satisfiable
example : ∃ (p : ℕ) (hp_fact : Fact p.Prime) (hp2 : p ≠ 2) (a : ZMod p) (ha : IsPrimitiveRoot a (p - 1)), True :=
  ⟨3, ⟨by norm_num⟩, by decide, 2, IsPrimitiveRoot.mk_of_lt _ (by norm_num) (by decide) (by intro l h0 h1; interval_cases l <;> decide), trivial⟩

-- hypotheses of legendreSym_eq_neg_one_of_isPrimitiveRoot are satisfiable
example : ∃ (p : ℕ) (_ : Fact p.Prime) (hp2 : p ≠ 2) (a : ℤ) (ha : IsPrimitiveRoot ((a : ZMod p)) (p - 1)), True :=
  ⟨3, ⟨by norm_num⟩, by decide, 2, IsPrimitiveRoot.mk_of_lt _ (by norm_num) (by decide) (by intro l h0 h1; interval_cases l <;> decide), trivial⟩

-- hypotheses of ArtinExclusion.legendreSym_sq are satisfiable
example : ∃ (p : ℕ) (_ : Fact p.Prime) (s : ℤ) (hs : ((s : ZMod p)) ≠ 0), True :=
by
  refine ⟨13, ⟨by norm_num⟩, 2, by decide⟩

-- hypotheses of ArtinExclusion.not_all_three_nonresidue are satisfiable
example : ∃ (p : ℕ) (_ : Fact p.Prime) (a b c s t : ℤ) (hrel : c * s ^ 2 = a * b * t ^ 2) (hs : ((s : ZMod p)) ≠ 0) (ht : ((t : ZMod p)) ≠ 0), True :=
by
  refine ⟨13, ⟨by norm_num⟩, 2, 3, 6, 1, 1, by decide, by decide⟩

-- hypotheses of ArtinExclusion.legendreSym_third_eq_one are satisfiable
example : ∃ (p : ℕ) (_ : Fact p.Prime) (a b c s t : ℤ) (hrel : c * s ^ 2 = a * b * t ^ 2) (hs : ((s : ZMod p)) ≠ 0) (ht : ((t : ZMod p)) ≠ 0) (hA : legendreSym p a = -1) (hB : legendreSym p b = -1), True :=
  ⟨5, ⟨by norm_num⟩, 2, 3, 6, 1, 1, by norm_num, by decide, by decide, (by rw [legendreSym.eq_neg_one_iff]; decide), (by rw [legendreSym.eq_neg_one_iff]; decide), trivial⟩

-- hypotheses of ArtinExclusion.not_all_three_nonresidue_two_five_ten are satisfiable
example : ∃ (p : ℕ) (_ : Fact p.Prime), True :=
by
  refine ⟨13, ⟨by norm_num⟩, ⟨⟩⟩

-- hypotheses of ArtinExclusion.not_all_three_nonresidue_of_primitiveRoot (all except `hC`) are satisfiable; with `hC` they are jointly impossible, which is the theorem
example : ∃ (p : ℕ) (_ : Fact p.Prime) (hp2 : p ≠ 2) (a b c s t : ℤ) (hrel : c * s ^ 2 = a * b * t ^ 2) (hs : ((s : ZMod p)) ≠ 0) (ht : ((t : ZMod p)) ≠ 0) (ha : IsPrimitiveRoot ((a : ZMod p)) (p - 1)) (hb : IsPrimitiveRoot ((b : ZMod p)) (p - 1)), True :=
  ⟨5, ⟨by norm_num⟩, by decide, 2, 3, 6, 1, 1, by norm_num, by decide, by decide, IsPrimitiveRoot.mk_of_lt _ (by norm_num) (by decide) (by intro l h0 h1; interval_cases l <;> decide), IsPrimitiveRoot.mk_of_lt _ (by norm_num) (by decide) (by intro l h0 h1; interval_cases l <;> decide), trivial⟩

-- hypotheses of ArtinExclusion.legendreSym_third_eq_one_of_primitiveRoot are satisfiable
example : ∃ (p : ℕ) (_ : Fact p.Prime) (hp2 : p ≠ 2) (a b c s t : ℤ) (hrel : c * s ^ 2 = a * b * t ^ 2) (hs : ((s : ZMod p)) ≠ 0) (ht : ((t : ZMod p)) ≠ 0) (ha : IsPrimitiveRoot ((a : ZMod p)) (p - 1)) (hb : IsPrimitiveRoot ((b : ZMod p)) (p - 1)), True :=
  ⟨5, ⟨by norm_num⟩, by decide, 2, 3, 6, 1, 1, by norm_num, by decide, by decide, IsPrimitiveRoot.mk_of_lt _ (by norm_num) (by decide) (by intro l h0 h1; interval_cases l <;> decide), IsPrimitiveRoot.mk_of_lt _ (by norm_num) (by decide) (by intro l h0 h1; interval_cases l <;> decide), trivial⟩
