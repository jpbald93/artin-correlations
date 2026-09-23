import numpy as np, math, re
dt=np.dtype([('p','<u4'),('a','<u2'),('q','<u2'),('o','u1'),('s','<u2')])
d=np.fromfile('c1e9.bin',dtype=dt); d=d[d['p']>=7]
p=d['p'].astype(np.int64); A=((d['a']>>5)&1).astype(np.int64); Q=((d['q']>>5)&1).astype(np.int64); del d
x=A[:-1]; y=A[1:]; g=p[1:]-p[:-1]; N=len(x)
# gap table
tex=open('/home/work/.openclaw/workspace/Prime Math/consolidated/artin_correlations.tex').read()
rows=re.findall(r'^\s*(\d+)\s*&\s*([\d{},]+)\s*&\s*(?:\\textbf\{)?([\d.]+)\}?\s*&\s*([\d.]+)\s*&\s*\$([+-][\d.]+)\$\s*&\s*(\d+)\s*&\s*([\d{},]+)\s*&\s*(?:\\textbf\{)?([\d.]+)\}?\s*&\s*([\d.]+)\s*&\s*\$([+-][\d.]+)\$',tex,re.M)
bad=0; cnt=0
for r in rows:
    for G,n,paa,pan,dd in [r[0:5],r[5:10]]:
        G=int(G); n=int(n.replace('{,}',''))
        m=g==G; xs=x[m]; ys=y[m]
        PAA=(xs&ys).sum()/xs.sum(); PAN=((1-xs)&ys).sum()/(1-xs).sum()
        ok = m.sum()==n and abs(round(PAA,3)-float(paa))<1e-9 and abs(round(PAN,3)-float(pan))<1e-9 and abs(round(PAA-PAN,3)-float(dd))<1e-9
        cnt+=1
        if not ok: bad+=1; print('MISMATCH g',G,n,m.sum(),paa,PAA,pan,PAN,dd,PAA-PAN)
print('gap rows checked',cnt,'mismatches',bad)
sm=p[1:]<10**7
print('pairs <1e7 with g%40==20:',((g%40==20)&sm).sum(), ' (p_{n+1}<1e7)', ((g%40==20)&(p[:-1]<10**7)).sum(),'(p_n<1e7)')
# residue conditioning
for M,th in [(120,5000),(840,2000)]:
    key=(p[:-1]%M)*M+p[1:]%M; K=M*M
    c=np.bincount(key*4+x*2+y,minlength=4*K).reshape(K,4).astype(float)
    n=c.sum(1); r1=c[:,2]+c[:,3]; c1=c[:,1]+c[:,3]
    ok=(n>=th)&(r1>0)&(r1<n)&(c1>0)&(c1<n)
    with np.errstate(divide='ignore',invalid='ignore'):
        dl=c[:,3]/r1-c[:,1]/(n-r1)
        chi=n*(c[:,3]*c[:,0]-c[:,2]*c[:,1])**2/(r1*(n-r1)*c1*(n-c1))
    print('M',M,'cells',ok.sum(),'pairs',int(n[ok].sum()),'%.2f%%'%(100*n[ok].sum()/N),'sumchi2 %.1f'%chi[ok].sum(),'delta_res %.6f'%((n[ok]*dl[ok]).sum()/n[ok].sum()))
    # split sample mod 120
    if M==120:
        for part in [p[:-1]<5*10**8, p[:-1]>=5*10**8]:
            cc=np.bincount(key[part]*4+x[part]*2+y[part],minlength=4*K).reshape(K,4).astype(float)
            n2=cc.sum(1); r2=cc[:,2]+cc[:,3]; c2=cc[:,1]+cc[:,3]
            ok2=(n2>=th)&(r2>0)&(r2<n2)&(c2>0)&(c2<n2)
            with np.errstate(divide='ignore',invalid='ignore'): d2=cc[:,3]/r2-cc[:,1]/(n2-r2)
            print('  split',ok2.sum(),'%.5f'%((n2[ok2]*d2[ok2]).sum()/n2[ok2].sum()))
dnr=((Q[:-1]==1)&(Q[1:]==1)).mean(); print('doubly nonresidue fraction',dnr)
print('1-res/global', 1-0.001055/0.014140, 1-0.000473/0.014140)
