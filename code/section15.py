#!/usr/bin/env python3
"""Every Section-15 number that is computed from the joint-residue cell tables.

Input : the output of `artin_payoff dump 1000000000` (one line per occupied cell:
        modulus half a b n00 n01 n10 n11; first index of nXY = predecessor status X,
        second = successor status Y; half = hash64(successor) & 1).
Output: JSON on stdout. Every value quoted in Section 15 from these tables is printed here,
        and every identity used is ASSERTED (the script exits non-zero on failure).

Conventions (stated in the paper):
  * X-constant cells (only one predecessor status occurs) contribute ZERO within-cell
    association under every decomposition convention.
  * The in-sample increment is the unsmoothed empirical conditional mutual information
    I(Y;X|C) in nats per pair (the 'incremental' column of Table tab:content).
  * Held-out scoring uses Jeffreys (+0.5) smoothing in EVERY cell. Estimator A splits every
    cell on X; estimator B does not split a cell whose successor status is constant in the
    fit half (splitting such a cell can only add smoothing penalty).
"""
import json
import math
import sys
from collections import defaultdict


def load(path):
    T = {120: [defaultdict(lambda: [0, 0, 0, 0]) for _ in range(2)],
         840: [defaultdict(lambda: [0, 0, 0, 0]) for _ in range(2)]}
    header = None
    with open(path) as f:
        for line in f:
            if line.startswith("#"):
                header = line.strip()
                continue
            m, h, a, b, n00, n01, n10, n11 = map(int, line.split())
            T[m][h][(a, b)] = [n00, n01, n10, n11]
    return header, T


def merged(halves):
    out = defaultdict(lambda: [0, 0, 0, 0])
    for h in halves:
        for k, v in h.items():
            o = out[k]
            for i in range(4):
                o[i] += v[i]
    return out


def decomp(cells):
    n00 = sum(v[0] for v in cells.values()); n01 = sum(v[1] for v in cells.values())
    n10 = sum(v[2] for v in cells.values()); n11 = sum(v[3] for v in cells.values())
    N = n00 + n01 + n10 + n11; s1 = n10 + n11; s0 = n00 + n01
    delta = n11 / s1 - n01 / s0
    varX = (s1 / N) * (s0 / N)
    S = dict(CW=0.0, CB=0.0, W1=0.0, B1=0.0, W0=0.0, B0=0.0, WS=0.0, BS=0.0)
    xconst = x0only = x1only = 0.0
    k = 0
    for a, b, g0, g1 in cells.values():
        n = a + b + g0 + g1
        if n == 0:
            continue
        m0, m1, t1 = a + b, g0 + g1, b + g1
        pi, r, p = n / N, m1 / n, t1 / n
        w1, w0 = m1 / s1, m0 / s0
        q0 = b / m0 if m0 else g1 / m1
        q1 = g1 / m1 if m1 else b / m0
        dc = q1 - q0
        if m0 == 0 or m1 == 0:
            xconst += pi
            if m1 == 0:
                x0only += pi
            else:
                x1only += pi
        if m0 and m1 and 0 < t1 < n:
            k += 1
        S["CW"] += pi * r * (1 - r) * dc / varX; S["CB"] += (w1 - w0) * p
        S["W1"] += w1 * dc;                      S["B1"] += (w1 - w0) * q0
        S["W0"] += w0 * dc;                      S["B0"] += (w1 - w0) * q1
        S["WS"] += 0.5 * (w1 + w0) * dc;         S["BS"] += (w1 - w0) * 0.5 * (q0 + q1)
    out = {"N": N, "delta": delta}
    for name, W, B in (("covariance", "CW", "CB"), ("kitagawa_w1", "W1", "B1"),
                       ("kitagawa_w0", "W0", "B0"), ("symmetric", "WS", "BS")):
        err = S[W] + S[B] - delta
        assert abs(err) < 1e-12, (name, err)
        out[name] = {"within": S[W], "between": S[B], "share": S[B] / delta, "identity_error": err}
    shares = [out[c]["share"] for c in ("covariance", "kitagawa_w1", "kitagawa_w0", "symmetric")]
    out["share_min"], out["share_max"] = min(shares), max(shares)
    out["x_constant_mass"], out["x0_only_mass"], out["x1_only_mass"] = xconst, x0only, x1only
    out["k_both_X_and_Y_vary"] = k
    return out


def xlogx(x):
    return x * math.log(x) if x > 0 else 0.0


def mle_increment(cells):
    N = I = 0.0
    for a, b, g0, g1 in cells.values():
        n = a + b + g0 + g1
        N += n
        I += (xlogx(a) + xlogx(b) + xlogx(g0) + xlogx(g1) - xlogx(a + b) - xlogx(g0 + g1)
              - xlogx(a + g0) - xlogx(b + g1) + xlogx(n))
    return I / N


def ll(y1, y0, p):
    return (y1 * math.log(p) if y1 else 0.0) + (y0 * math.log(1 - p) if y0 else 0.0)


def heldout(fit, test, split_constant_cells):
    N = ceR = ceX = 0.0
    for key, (n00, n01, n10, n11) in test.items():
        s = n00 + n01 + n10 + n11
        if s == 0:
            continue
        N += s
        f00, f01, f10, f11 = fit.get(key, [0, 0, 0, 0])
        pR = (f01 + f11 + 0.5) / (f00 + f01 + f10 + f11 + 1.0)
        qR = (f11 + 0.5) / (f10 + f11 + 1.0)
        rR = (f01 + 0.5) / (f00 + f01 + 1.0)
        if not split_constant_cells and ((f01 + f11) == 0 or (f00 + f10) == 0):
            qR = rR = pR                     # successor constant in the fit half: do not split
        ceR -= ll(n01 + n11, n00 + n10, pR)
        ceX -= ll(n11, n10, qR) + ll(n01, n00, rR)
    return {"N_test": N, "CE_residues": ceR / N, "CE_plus_X": ceX / N, "gain": (ceR - ceX) / N}


def main():
    header, T = load(sys.argv[1])
    res = {"source": header}
    for M in (120, 840):
        full = merged(T[M])
        res[f"mod{M}"] = {
            "decomposition": decomp(full),
            "mle_increment": mle_increment(full),
            "heldout": {
                "A_split_all_even_to_odd": heldout(T[M][0], T[M][1], True),
                "A_split_all_odd_to_even": heldout(T[M][1], T[M][0], True),
                "B_no_split_constant_even_to_odd": heldout(T[M][0], T[M][1], False),
                "B_no_split_constant_odd_to_even": heldout(T[M][1], T[M][0], False),
            },
        }
    # cross-check against the driver's held-out table (estimator A, even->odd), 1e9 values
    json.dump(res, sys.stdout, indent=1)
    print()


if __name__ == "__main__":
    main()
