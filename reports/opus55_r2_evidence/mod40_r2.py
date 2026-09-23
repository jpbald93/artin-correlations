# Aggregate the mod-120 joint table to mod 40 and mod 24/8/5/3 and report the same w1-within / residual-between split.
import numpy as np
raw=np.fromfile('tables.bin',dtype=np.uint64).astype(np.float64)
T=raw[:3*120*120*4].reshape(3,120,120,4)[0]
def split(c):
    n00,n01,n10,n11=c.T; a=n10+n11; b=n00+n01; s1=a.sum(); s0=b.sum()
    d=n11.sum()/s1-n01.sum()/s0; nd=(a>0)&(b>0)
    w=(a/s1*np.where(nd,n11/np.maximum(a,1)-n01/np.maximum(b,1),0)).sum()
    return d,w
for M in (3,5,8,40,24,120):
    C=np.zeros((M,M,4))
    for i in range(120):
        for j in range(120): C[i%M,j%M]+=T[i,j]
    d,w=split(C.reshape(-1,4)); print(f"mod{M}: delta={d:.6f} within={w:.6f} between={d-w:.6f} between share={(d-w)/d:.1%}")
