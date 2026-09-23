import numpy as np, sys, math
dt=np.dtype([('p','<u4'),('a','<u2'),('q','<u2'),('o','u1'),('s','<u2')])
d=np.fromfile(sys.argv[1],dtype=dt)
d=d[d['p']>=7]
p=d['p'].astype(np.int64); A=((d['a']>>5)&1).astype(np.int64)  # base 10 is index 5
Q=((d['q']>>5)&1).astype(np.int64)
print('n primes',len(p),'artin',A.sum(),A.mean())
x=A[:-1]; y=A[1:]; g=p[1:]-p[:-1]
N=len(x)
n11=int((x&y).sum()); n10=int((x&(1-y)).sum()); n01=int(((1-x)&y).sum()); n00=N-n11-n10-n01
print('pairs',N,'table',[[n00,n01],[n10,n11]])
pAA=n11/(n11+n10); pAn=n01/(n01+n00); dl=pAA-pAn
r1=n11+n10; c1=n11+n01
chi=N*(n11*n00-n10*n01)**2/(r1*(N-r1)*c1*(N-c1))
phi=(n11*n00-n10*n01)/math.sqrt(r1*(N-r1)*c1*(N-c1))
print('P(A|A)',pAA,'P(A|~A)',pAn,'delta',dl,'chi2',chi,'phi',phi,'signedsqrt',-math.sqrt(chi))
# gaps
for G in [20,40,60]:
    m=g==G; xs=x[m]; ys=y[m]
    print('g',G,'N',m.sum(),'both',(xs&ys).sum(),'P(A|A)',(xs&ys).sum()/max(xs.sum(),1),'P(A|~A)',((1-xs)&ys).sum()/max((1-xs).sum(),1))
m=(g%40==20); print('g%40==20 pairs',m.sum(),'prevA',x[m].sum(),'both',(x[m]&y[m]).sum(),'g in {20,60}',((g==20)|(g==60)).sum())
# legendre flip check
q=Q  # 1 if nonresidue
print('g%40==20 QR flip violations', (q[:-1][m]==q[1:][m]).sum(), ' g%40==0 QR preserve viol',(q[:-1][g%40==0]!=q[1:][g%40==0]).sum())
m0=g%40==0; print('g%40==0 pairs',m0.sum(),'prevA',x[m0].sum(),'both',(x[m0]&y[m0]).sum())
# delta(g) extremes over gaps with N>=1e5
res=[]
for G in np.unique(g):
    m=g==G
    if m.sum()<1e5: continue
    xs=x[m];ys=y[m]
    dd=(xs&ys).sum()/xs.sum()-((1-xs)&ys).sum()/(1-xs).sum()
    res.append((G,m.sum(),dd))
res.sort(key=lambda t:t[2]); print('min',res[0],'max',res[-1]); print('ngaps',len(res),[r[0] for r in res if r[0]>60])
w=sum(r[1] for r in res); print('pair-weighted mean delta(g)',sum(r[1]*r[2] for r in res)/w, w)
# omega corr
o=d['o'].astype(float); print('r omega',np.corrcoef(o[:-1],o[1:])[0,1])
# mod12 diag
r12=p%12
for r in [1,5,7,11]:
    mm=r12[:-1]==r; print('mod12 diag',r,(r12[1:][mm]==r).mean())
# CE
def ce(key,K):
    cnt=np.zeros((K,4),np.int64)
    np.add.at(cnt,(key,x*2+y),1)
    Nn=cnt.sum(); n1=cnt[:,1].sum()+cnt[:,3].sum(); pm=n1/Nn
    CE1=-(n1*math.log(pm)+(Nn-n1)*math.log(1-pm))/Nn
    def H(a,b):
        s=a+b; out=np.zeros_like(s,dtype=float); 
        with np.errstate(divide='ignore',invalid='ignore'):
            t=np.where(a>0,a*np.log(a/s),0)+np.where(b>0,b*np.log(b/s),0)
        return -np.nansum(t)
    CE2=H(cnt[:,1]+cnt[:,3],cnt[:,0]+cnt[:,2])/Nn
    CE3=(H(cnt[:,3],cnt[:,2])+H(cnt[:,1],cnt[:,0]))/Nn
    return CE1,CE2,CE3,CE1-CE2,CE2-CE3
for M in [120,840]:
    print('CE joint mod',M,['%.6f'%v for v in ce((p[:-1]%M)*M+p[1:]%M,M*M)])
print('CE gap mod40 only',['%.6f'%v for v in ce(g%40,40)])
print('CE p_{n+1} mod 40 only',['%.6f'%v for v in ce(p[1:]%40,40)])
print('CE g mod 40 x p_{n+1} mod 40',['%.6f'%v for v in ce((g%40)*40+p[1:]%40,1600)])
print('CE joint mod40',['%.6f'%v for v in ce((p[:-1]%40)*40+p[1:]%40,1600)])
print('CE p_{n+1} mod 120',['%.6f'%v for v in ce(p[1:]%120,120)])
print('CE p_{n+1} mod 840',['%.6f'%v for v in ce(p[1:]%840,840)])
