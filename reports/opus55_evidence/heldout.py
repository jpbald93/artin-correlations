import numpy as np, math, sys
dt=np.dtype([('p','<u4'),('a','<u2'),('q','<u2'),('o','u1'),('s','<u2')])
d=np.fromfile(sys.argv[1],dtype=dt); d=d[d['p']>=7]
p=d['p'].astype(np.int64); A=((d['a']>>5)&1).astype(np.int64); del d
x=A[:-1]; y=A[1:]; N=len(x)
for M in [120,840]:
    key=(p[:-1]%M)*M+p[1:]%M
    K=M*M
    c=np.bincount(key*4+x*2+y,minlength=4*K).reshape(K,4)
    # informative cells: both prev and next vary
    nprev1=c[:,2]+c[:,3]; nprev0=c[:,0]+c[:,1]; ny1=c[:,1]+c[:,3]
    inf=(nprev1>0)&(nprev0>0)&(ny1>0)&(ny1<c.sum(1))
    print('M',M,'populated cells',(c.sum(1)>0).sum(),'informative cells',inf.sum(),'naive bias k/(2N)',inf.sum()/(2*N))
    # held-out: train on first half, test on second
    h=N//2
    for tr,te in [(slice(0,h),slice(h,N)),(slice(h,N),slice(0,h))]:
        ct=np.bincount(key[tr]*4+x[tr]*2+y[tr],minlength=4*K).reshape(K,4).astype(float)
        a=0.5
        # model R: P(y|cell); model RA: P(y|cell,prev)
        pR=(ct[:,1]+ct[:,3]+a)/(ct.sum(1)+2*a)
        pR1=(ct[:,3]+a)/(ct[:,2]+ct[:,3]+2*a); pR0=(ct[:,1]+a)/(ct[:,0]+ct[:,1]+2*a)
        # deterministic zero cells: if train never had y=1 -> keep smoothing
        kk=key[te]; xx=x[te]; yy=y[te]
        def ll(pp):
            pp=np.clip(pp,1e-12,1-1e-12); return -np.mean(yy*np.log(pp)+(1-yy)*np.log(1-pp))
        ceR=ll(pR[kk]); ceRA=ll(np.where(xx==1,pR1[kk],pR0[kk]))
        print('   held-out CE R %.6f  R+prev %.6f  gain %.7f'%(ceR,ceRA,ceR-ceRA))
