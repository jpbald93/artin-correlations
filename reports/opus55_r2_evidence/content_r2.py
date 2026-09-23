# Round-2: in-sample gain, LR statistic, optimism, held-out (both directions), and an
# independent pooled common-odds-ratio model (Mantel-Haenszel beta, per-cell alpha by 1-D root find).
import numpy as np
from math import log
M1,M2=120,840
raw=np.fromfile('tables.bin',dtype=np.uint64).astype(np.float64)
o=3*M1*M1*4
TT={120:raw[:o].reshape(3,M1*M1,4),840:raw[o:].reshape(3,M2*M2,4)}
def xlogy(x,y): return np.where(x>0,x*np.log(np.where(y>0,y,1)),0.0)
def insample(c):
    n00,n01,n10,n11=c.T; a=n10+n11; b=n00+n01; n=a+b; N=n.sum()
    p=np.where(n>0,(n01+n11)/np.maximum(n,1),0)
    q1=np.where(a>0,n11/np.maximum(a,1),0); q0=np.where(b>0,n01/np.maximum(b,1),0)
    ceR=-(xlogy(n01+n11,p)+xlogy(n00+n10,1-p)).sum()/N
    ceRP=-(xlogy(n11,q1)+xlogy(n10,1-q1)+xlogy(n01,q0)+xlogy(n00,1-q0)).sum()/N
    y=(n01+n11).sum()/N; ce0=-(y*log(y)+(1-y)*log(1-y))
    k=((a>0)&(b>0)&(p>0)&(p<1)).sum()
    kk=((a>0)&(b>0)).sum()
    return N,ce0,ceR,ceRP,k,kk
def score(fit,test,mode,beta=None):
    f00,f01,f10,f11=fit.T; t00,t01,t10,t11=test.T; n=test.sum(1); N=n.sum()
    pR=(f01+f11+.5)/(f00+f01+f10+f11+1)
    ceR=-(xlogy(t01+t11,pR)+xlogy(t00+t10,1-pR)).sum()/N
    if mode=='cells':
        q1=(f11+.5)/(f10+f11+1); q0=(f01+.5)/(f00+f01+1)
    else:
        # per-cell alpha solving fit-half marginal: b*s(al)+a*s(al+beta) = f01+f11 (Jeffreys-adjusted)
        a=f10+f11; b=f00+f01; m=f01+f11+.5; tot=a+b+1
        lo=np.full(len(a),-30.); hi=np.full(len(a),30.)
        for _ in range(200):
            mid=(lo+hi)/2
            val=(b+.5)/(1+np.exp(-mid))+(a+.5)/(1+np.exp(-(mid+beta)))
            big=val>m; hi=np.where(big,mid,hi); lo=np.where(big,lo,mid)
        al=(lo+hi)/2; q0=1/(1+np.exp(-al)); q1=1/(1+np.exp(-(al+beta)))
    ceRP=-(xlogy(t11,q1)+xlogy(t10,1-q1)+xlogy(t01,q0)+xlogy(t00,1-q0)).sum()/N
    return N,ceR,ceRP,ceR-ceRP
def mh_beta(fit):
    f00,f01,f10,f11=fit.T; n=fit.sum(1); ok=n>0
    num=(f11*f00/np.maximum(n,1))[ok].sum(); den=(f10*f01/np.maximum(n,1))[ok].sum()
    return log(num/den)
for M in (120,840):
    T=TT[M]
    N,ce0,ceR,ceRP,k,kk=insample(T[0])
    g=ceR-ceRP
    print(f"mod{M}: CE marg {ce0:.6f} CE res {ceR:.6f} +prev {ceRP:.6f} in-sample gain {g:.4e}")
    print(f"   extra params (nondeg cells, 0<p<1)={k} (nondeg={kk});  k/(2N)={k/(2*N):.3e}  k/N={k/N:.3e}")
    G=2*N*g; print(f"   LR G=2N*gain={G:.1f} on {k} df; z=(G-k)/sqrt(2k)={(G-k)/np.sqrt(2*k):.1f};  debiased true gain ~ gain-k/(2N) = {g-k/(2*N):.3e}")
    for (fi,te,lab) in ((1,2,'fit even/test odd'),(2,1,'fit odd/test even')):
        Nt,cR,cRP,hg=score(T[fi],T[te],'cells')
        b=mh_beta(T[fi]); _,_,_,pg=score(T[fi],T[te],'pooled',b)
        print(f"   {lab}: N_test={Nt:.0f} cells-model held-out gain={hg:.4e}; expected from optimism ~ -k/(2N_fit)={-k/(2*T[fi].sum()):.2e};  MH beta={b:.5f} pooled held-out gain={pg:.4e}")
