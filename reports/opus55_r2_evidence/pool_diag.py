# Port of the author's pool_report (MLE per-cell alpha, Newton, fallback only when f01+f11==0)
# to locate where its mod-840 held-out loss of -2.35e-4 comes from.
import numpy as np
M1,M2=120,840
raw=np.fromfile('tables.bin',dtype=np.uint64).astype(np.float64); o=3*M1*M1*4
TT={120:raw[:o].reshape(3,M1*M1,4),840:raw[o:].reshape(3,M2*M2,4)}
def author_pool(fit,test):
    a,b,g0,g1=fit.T  # n00,n01,n10,n11
    m=b+g1; n=a+g0; p=(m+.5)/(m+n+1); al=np.log(p/(1-p)); beta=0.
    for it in range(200):
        for k in range(3):
            p0=1/(1+np.exp(-al)); p1=1/(1+np.exp(-(al+beta)))
            g=(b-(a+b)*p0)+(g1-(g0+g1)*p1); h=(a+b)*p0*(1-p0)+(g0+g1)*p1*(1-p1)+1e-9
            al+=np.clip(g/h,-5,5)
        p1=1/(1+np.exp(-(al+beta))); beta+=(g1-(g0+g1)*p1).sum()/(((g0+g1)*p1*(1-p1)).sum()+1e-12)
    t00,t01,t10,t11=test.T
    pr=np.clip((b+g1+.5)/(a+b+g0+g1+1),1e-12,1-1e-12)
    q0=1/(1+np.exp(-al)); q1=1/(1+np.exp(-(al+beta)))
    fb=(b+g1)==0; q0=np.where(fb,pr,q0); q1=np.where(fb,pr,q1)
    q0=np.clip(q0,1e-12,1-1e-12); q1=np.clip(q1,1e-12,1-1e-12)
    lR=-((t01+t11)*np.log(pr)+(t00+t10)*np.log(1-pr))
    lB=-(t01*np.log(q0)+t00*np.log(1-q0)+t11*np.log(q1)+t10*np.log(1-q1))
    N=test.sum(); d=lR-lB
    return beta,d.sum()/N,d,fit,test
for M in (120,840):
    beta,gain,d,fit,test=author_pool(TT[M][1],TT[M][2])
    print(f"mod{M}: author-algorithm beta={beta:.6f} heldout_gain={gain:.4e}")
    idx=np.argsort(d)[:5]
    for i in idx: print("   worst cell",divmod(i,M),"fit",fit[i].astype(int),"test",test[i].astype(int),"loss nats",round(-d[i],2))
