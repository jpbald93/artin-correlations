# Round-2 decomposition check from td_census tables (cells x [n00,n01,n10,n11], index = 2*X+Y)
import numpy as np
M1,M2=120,840
raw=np.fromfile('tables.bin',dtype=np.uint64).astype(np.float64)
o=3*M1*M1*4
T1=raw[:o].reshape(3,M1*M1,4); T2=raw[o:].reshape(3,M2*M2,4)
def dec(c,name):
    n00,n01,n10,n11=c.T; a=n10+n11; b=n00+n01; n=a+b
    N=n.sum(); s1=a.sum(); s0=b.sum()
    d=n11.sum()/s1-n01.sum()/s0
    nd=(a>0)&(b>0)
    q1=np.where(a>0,n11/np.maximum(a,1),0); q0=np.where(b>0,n01/np.maximum(b,1),0)
    pc=np.where(n>0,(n01+n11)/np.maximum(n,1),0)
    w1=a/s1; w0=b/s0
    dc=np.where(nd,q1-q0,0)
    within_w1=(w1*dc).sum()                      # author's within
    resid=d-within_w1                            # author's 'between' (defined as residual)
    between_formula_pc=((w1-w0)*pc).sum()        # the paper's printed between term
    between_q0=((w1-w0)*q0).sum()                # correct Kitagawa between (w1 within)
    # alternative Kitagawa ordering: within with w0 weights
    within_w0=(w0*dc).sum()
    # covariance (law of total covariance) share, as in round 1
    X=a/np.maximum(n,1); 
    cov=n11.sum()/N-(s1/N)*((n01+n11).sum()/N)
    within_cov=(n*(n11/np.maximum(n,1)-X*pc)).sum()/N
    # delta-scale version of cov: delta = cov/Var(X)
    varX=(s1/N)*(1-s1/N)
    print(f"{name}: N={N:.0f} delta={d:.9f} nondeg={nd.sum()}")
    print(f"   author-within(w1)={within_w1:.6f} residual-between={resid:.6f} share_between={resid/d:.4f}")
    print(f"   printed-formula between sum(w1-w0)p_c={between_formula_pc:.6f}  within+printed={within_w1+between_formula_pc:.9f}  (identity gap {d-within_w1-between_formula_pc:.3e})")
    print(f"   correct Kitagawa between sum(w1-w0)q0_c={between_q0:.6f}   identity gap {d-within_w1-between_q0:.3e}")
    print(f"   alt ordering: within(w0)={within_w0:.6f} share_between={(d-within_w0)/d:.4f}")
    print(f"   cov={cov:.7f} within-cov={within_cov:.4e} within share={within_cov/cov:.4%} between share={1-within_cov/cov:.4%}")
    return nd
for T,M in ((T1,120),(T2,840)):
    dec(T[0],f"mod{M} all")
