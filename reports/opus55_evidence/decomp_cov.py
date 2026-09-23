import numpy as np
dt=np.dtype([('p','<u4'),('a','<u2'),('q','<u2'),('o','u1'),('s','<u2')])
d=np.fromfile('c1e9.bin',dtype=dt); d=d[d['p']>=7]
p=d['p'].astype(np.int64); A=((d['a']>>5)&1).astype(np.int64); del d
x=A[:-1]; y=A[1:]; N=len(x)
cov=(x*y).mean()-x.mean()*y.mean()
print('total cov',cov)
for M in [40,120,840]:
    key=(p[:-1]%M)*M+p[1:]%M; K=M*M
    c=np.bincount(key*4+x*2+y,minlength=4*K).reshape(K,4).astype(float)
    n=c.sum(1); ok=n>0
    ex=(c[:,2]+c[:,3])[ok]/n[ok]; ey=(c[:,1]+c[:,3])[ok]/n[ok]; exy=c[ok,3]/n[ok]; w=n[ok]/N
    within=(w*(exy-ex*ey)).sum(); between=(w*ex*ey).sum()-(w*ex).sum()*(w*ey).sum()
    print('M',M,'within-cell cov',within,'between',between,'share within %.2f%%'%(100*within/cov))
