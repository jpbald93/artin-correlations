# Null calibration (mod 120): within each cell and each hash half, redraw the 2x2 table
# with fixed X- and Y-margins under conditional independence (hypergeometric),
# then compute full-sample in-sample gain and both held-out gains exactly as before.
import numpy as np
exec(open('content_r2.py').read().split('for M in (120,840):')[0])
rng=np.random.default_rng(20260923)
T=TT[840]
def redraw(c):
    n00,n01,n10,n11=c.T; a=(n10+n11).astype(np.int64); y=(n01+n11).astype(np.int64); n=(c.sum(1)).astype(np.int64)
    ok=n>0; m11=np.zeros_like(n)
    m11[ok]=rng.hypergeometric(y[ok],n[ok]-y[ok],a[ok])
    m10=a-m11; m01=y-m11; m00=n-a-y+m11
    return np.stack([m00,m01,m10,m11],1).astype(float)
res=[]
for r in range(20):
    E=redraw(T[1]); O=redraw(T[2]); F=E+O
    N,ce0,ceR,ceRP,k,kk=insample(F); gin=ceR-ceRP
    g1=score(E,O,'cells')[3]; g2=score(O,E,'cells')[3]
    res.append((gin,g1,g2))
res=np.array(res)
obs=(1.8326e-05,-6.1108e-05,-6.0764e-05)
for j,lab in enumerate(('in-sample full','held-out even->odd','held-out odd->even')):
    print(f"{lab}: null mean={res[:,j].mean():.3e} sd={res[:,j].std(ddof=1):.3e} observed={obs[j]:.3e} z={(obs[j]-res[:,j].mean())/res[:,j].std(ddof=1):.1f}")
