# held-out test with a 1-parameter pooled predecessor effect on top of residue cells (logit offset)
import numpy as np, math
dt=np.dtype([('p','<u4'),('a','<u2'),('q','<u2'),('o','u1'),('s','<u2')])
d=np.fromfile('c1e9.bin',dtype=dt); d=d[d['p']>=7]
p=d['p'].astype(np.int64); A=((d['a']>>5)&1).astype(np.int64); del d
x=A[:-1]; y=A[1:]
def mix(z):
    z=(z+0x9e3779b97f4a7c15)&0xFFFFFFFFFFFFFFFF
    z=((z^(z>>30))*0xbf58476d1ce4e5b9)&0xFFFFFFFFFFFFFFFF
    z=((z^(z>>27))*0x94d049bb133111eb)&0xFFFFFFFFFFFFFFFF
    return z^(z>>31)
succ=p[1:].astype(np.uint64)
with np.errstate(over='ignore'):
    z=succ+np.uint64(0x9e3779b97f4a7c15); z=(z^(z>>np.uint64(30)))*np.uint64(0xbf58476d1ce4e5b9); z=(z^(z>>np.uint64(27)))*np.uint64(0x94d049bb133111eb); z=z^(z>>np.uint64(31))
t=(z&np.uint64(1)).astype(bool)
for M in [120,840]:
    key=(p[:-1]%M)*M+p[1:]%M; K=M*M
    for fitmask in [t,~t]:
        te=~fitmask
        c=np.bincount(key[fitmask]*4+x[fitmask]*2+y[fitmask],minlength=4*K).reshape(K,4).astype(float)
        # fit logit model: logit P = alpha_cell + beta*x, by Newton on beta with alphas profiled (few iterations of coordinate ascent)
        beta=0.0
        n0=c[:,0]+c[:,1]; y0=c[:,1]; n1=c[:,2]+c[:,3]; y1=c[:,3]
        alpha=np.log((y0+y1+0.5)/(n0+n1-y0-y1+0.5))
        for it in range(60):
            # update alpha per cell (Newton)
            for _ in range(3):
                p0=1/(1+np.exp(-alpha)); p1=1/(1+np.exp(-(alpha+beta)))
                g=(y0-n0*p0)+(y1-n1*p1); h=n0*p0*(1-p0)+n1*p1*(1-p1)+1e-9
                alpha=alpha+np.clip(g/h,-5,5)
            p1=1/(1+np.exp(-(alpha+beta)))
            gb=(y1-n1*p1).sum(); hb=(n1*p1*(1-p1)).sum()
            beta+=gb/hb
        # deterministic cells (y never 1): alpha -> -inf; clip
        kk=key[te]; xx=x[te]; yy=y[te]
        cc=np.bincount(key[te]*4+x[te]*2+y[te],minlength=4*K).reshape(K,4).astype(float)
        pr=(c[:,1]+c[:,3]+0.5)/(c.sum(1)+1)
        def ce(pp,cnt1,cnt0):
            pp=np.clip(pp,1e-12,1-1e-12); return -(cnt1*np.log(pp)+cnt0*np.log(1-pp)).sum()
        Nt=cc.sum()
        ceR=(ce(pr,cc[:,1]+cc[:,3],cc[:,0]+cc[:,2]))/Nt
        q0=1/(1+np.exp(-alpha)); q1=1/(1+np.exp(-(alpha+beta)))
        # keep deterministic cells at smoothed baseline prob
        det=(c[:,1]+c[:,3])==0
        q0=np.where(det,pr,q0); q1=np.where(det,pr,q1)
        ceB=(ce(q0,cc[:,1],cc[:,0])+ce(q1,cc[:,3],cc[:,2]))/Nt
        print('M',M,'beta %.5f'%beta,'heldout CE R %.9f  R+beta %.9f  gain %.3e'%(ceR,ceB,ceR-ceB))
