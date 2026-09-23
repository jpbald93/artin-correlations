# Lean scope note (consolidated paper)

`lean/` contains the Lean 4 development for the two exclusion laws, merged from the two source
papers (it is the cross-base development, which is a superset of the consecutive-prime one).

Run the gate:

```bash
cd lean && ./gate.sh      # -> PASS (28 theorems, standard axioms only)
```

**What the gate does check:** the build succeeds; no `sorry`/`admit`/`axiom`/`native_decide`;
every audited theorem depends only on a subset of `propext`, `Classical.choice`, `Quot.sound`.

**What it does *not* check** — stated here so the prose claim is not over-read:

- It formalises the **character-parity core** of the exclusion laws plus **selected concrete
  instances**. The statements retain explicit nonvanishing hypotheses on the bases (e.g.
  `hp5 : (5 : ZMod p) ≠ 0` in `not_both_primitiveRoot_five_ten`), discharged in those instances.
- There is **no generic squarefree-part-to-nonvanishing-witness bridge** formalised here, and no
  generic `sqf` theorem with the paper's hypotheses automatically discharged.
- The asymptotics and the census statistics are not formalised; they are code-verified.

Targeted Mathlib imports are used (not a bare `import Mathlib`), and the shared Mathlib cache is
expected as a symlink at `lean/.lake/packages`.
