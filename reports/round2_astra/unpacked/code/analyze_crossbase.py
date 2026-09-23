#!/usr/bin/env python3
"""Cross-base (same-prime) Artin correlation: is it explained by entanglement?

Key prediction to test: bases a,b whose quadratic fields share structure --
specifically when ab is a perfect square times something, or when the
compositum degree drops -- should show anomalous correlation.

The cleanest signal: Q(sqrt(a)) = Q(sqrt(b)) iff sqf(a)==sqf(b) (e.g. 5 and 20).
Next: the 'multiplicative triple' case sqf(a)*sqf(b)*sqf(c) a square, e.g.
(3,5,15) -- then chi_a*chi_b = chi_c, so the three characters are DEPENDENT.
That is genuine entanglement and should show up as a 3-way effect.
"""
import json, math, sys, itertools
from sympy import factorint

PATH = sys.argv[1] if len(sys.argv) > 1 else "crossbase_2e8.json"
d = json.load(open(PATH))
N = d["n_primes"]

def sqf(n):
    s = 1
    for q, e in factorint(n).items():
        if e % 2: s *= q
    return s

def phi_coef(m):
    """Pearson phi for a 2x2 table [[n00,n01],[n10,n11]] + z."""
    n00, n01 = m[0]; n10, n11 = m[1]
    n = n00+n01+n10+n11
    r1 = n10+n11; r0 = n00+n01
    c1 = n01+n11; c0 = n00+n10
    if min(r0,r1,c0,c1) == 0: return None, None
    num = n11*n00 - n10*n01
    den = math.sqrt(r0*r1*c0*c1)
    phi = num/den
    z = phi*math.sqrt(n)
    return phi, z

rows = []
for key, m in d["pairs"].items():
    a, b = map(int, key.split(","))
    phi, z = phi_coef(m)
    da, db = sqf(a), sqf(b)
    prod = sqf(da*db)
    rows.append({"a": a, "b": b, "sqf_a": da, "sqf_b": db,
                 "same_field": da == db, "prod_sqf": prod,
                 "phi": phi, "z": z})

rows.sort(key=lambda r: -abs(r["phi"] or 0))
print(f"limit={d['limit']:,}  primes={N:,}\n")
print("Artin densities (marginals):")
for a, c in d["marginals"].items():
    print(f"   base {a:>3}: {c/N:.6f}")

print(f"\n{'a':>4} {'b':>4} {'sqf(ab)':>8} {'same field':>11} {'phi':>10} {'z':>9}")
print("-"*52)
for r in rows[:20]:
    print(f"{r['a']:>4} {r['b']:>4} {r['prod_sqf']:>8} {str(r['same_field']):>11} "
          f"{r['phi']:>10.5f} {r['z']:>9.1f}")

# summary: how many pairs are significant?
sig = [r for r in rows if abs(r["z"]) > 4]
print(f"\npairs with |z|>4: {len(sig)} of {len(rows)}")
mean_abs = sum(abs(r['phi']) for r in rows)/len(rows)
print(f"mean |phi| = {mean_abs:.5f}")

# THE ENTANGLEMENT TEST: triples with chi_a * chi_b = chi_c
print("\n--- multiplicative triples (chi_a*chi_b = chi_c) ---")
bases = d["bases"]
found = []
for a, b, c in itertools.combinations(bases, 3):
    if sqf(sqf(a)*sqf(b)) == sqf(c):
        found.append((a, b, c))
for t in found:
    print(f"   chi_{t[0]} * chi_{t[1]} = chi_{t[2]}   (sqf: {sqf(t[0])},{sqf(t[1])},{sqf(t[2])})")
if not found:
    print("   none in this base set")

json.dump(rows, open("crossbase_summary.json", "w"), indent=2)
