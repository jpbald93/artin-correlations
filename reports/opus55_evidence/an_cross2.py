import numpy as np, sys, math, json
from itertools import combinations
dt=np.dtype([('p','<u4'),('a','<u2'),('q','<u2'),('o','u1'),('s','<u2')])
d=np.fromfile(sys.argv[1],dtype=dt)
pmin=int(sys.argv[2])
d=d[d['p']>=pmin]
am=d['a'].copy(); qm=d['q'].copy(); om=d['o'].astype(np.int32); sg=d['s'].astype(np.int32); del d
B=[2,3,5,6,7,10,11,13,15,17,21,29]
N=len(am); print('N',N)
bit=lambda m,k: ((m>>k)&1).astype(np.int32)
for k in range(12): print('density',B[k],bit(am,k).mean())
def stats(c):  # c: (K,4) counts for xy=00,01,10,11
    n=c.sum(1).astype(float); nx=(c[:,2]+c[:,3]).astype(float); ny=(c[:,1]+c[:,3]).astype(float); nxy=c[:,3].astype(float)
    with np.errstate(divide='ignore',invalid='ignore'):
        mx=nx/n; my=ny/n; cov=nxy/n-mx*my; ph=cov/np.sqrt(mx*(1-mx)*my*(1-my))
    return n,cov,ph,mx,my
def condphi(c,minn=200):
    n,cov,ph,mx,my=stats(c)
    ok=(n>=minn)&(mx>0)&(mx<1)&(my>0)&(my<1)
    return (n[ok]*ph[ok]).sum()/n[ok].sum(), np.nansum(n*cov)/n.sum(), n[ok].sum()/N
res={}
for i,j in combinations(range(12),2):
    x=bit(am,i); y=bit(am,j); qi=bit(qm,i); qj=bit(qm,j)
    k=(sg*16+qi*8+qj*4+x*2+y)
    c=np.bincount(k,minlength=512*16).reshape(512,4,4)  # [sig][qiqj][xy]
    tot=c.sum((0,1))
    n,cov,ph,_,_=stats(tot[None,:]); 
    csig=c.sum(1)
    ps,cps,fr=condphi(csig)
    pq,cpq,frq=condphi(c.reshape(2048,4))
    co=np.bincount(om*4+x*2+y,minlength=64*4).reshape(64,4)
    po,cpo,_=condphi(co)
    viol=int(c[:,1,3].sum()+c[:,2,3].sum())   # qi!=qj and both artin
    same=c[:,0,:].sum()+c[:,3,:].sum(); samejoint=c[:,0,3].sum()+c[:,3,3].sum()
    res[(B[i],B[j])]=dict(phi=float(ph[0]),j=float(tot[3]/N),om=po,sig=ps,qr=pq,cov=float(cov[0]),covsig=cps,covom=cpo,fr=fr,frq=frq,viol=viol,same=int(same),samejoint=int(samejoint),PAgivenB=float(tot[3]/(tot[2]+tot[3])),PAgivennotB=float(tot[1]/(tot[0]+tot[1])))
    # diag cell for (2,6)
    if (B[i],B[j])==(2,6):
        cell=0|(0)|(1<<7)  # v2=1 -> 0; no q<=13; one large factor
        print('(2,6) cell v2=1,none<=13,omega>13=1:',csig[cell].tolist())
        ph26=[]
        for s in range(512):
            if (s&3)==0 and not (s>>2)&1:
                nn,cv,pp,mx,my=stats(csig[s][None,:])
                if nn[0]>=200 and 0<mx[0]<1 and 0<my[0]<1: ph26.append(pp[0])
        print('(2,6) v2=1,3!|p-1 cells',len(ph26),min(ph26),max(ph26))
json.dump({f"{a},{b}":v for (a,b),v in res.items()},open(f'cross_{pmin}.json','w'),indent=0)
ph=np.array([v['phi'] for v in res.values()])
print('pairs',len(ph),'positive',(ph>0).sum(),'mean',ph.mean(),'neg',[k for k,v in res.items() if v['phi']<0])
mo=np.mean([v['om'] for v in res.values()]); ms=np.mean([v['sig'] for v in res.values()]); mq=np.mean([v['qr'] for v in res.values()])
print('mean phi|om',mo,'removed',1-mo/ph.mean(),'mean phi|sig',ms,'removed',1-ms/ph.mean(),'mean phi|sig,qr',mq)
mc=np.mean([v['cov'] for v in res.values()])
print('cov scale removal om',1-np.mean([v['covom'] for v in res.values()])/mc,'sig',1-np.mean([v['covsig'] for v in res.values()])/mc)
print('mean abs removal om',1-np.mean([abs(v['om']) for v in res.values()])/np.mean(abs(ph)),'sig',1-np.mean([abs(v['sig']) for v in res.values()])/np.mean(abs(ph)))
print('frac eligible sig',np.mean([v['fr'] for v in res.values()]),'qr',np.mean([v['frq'] for v in res.values()]))
def sqf(n):
    r=1;q=2
    while q*q<=n:
        e=0
        while n%q==0: n//=q;e+=1
        if e%2:r*=q
        q+=1
    return r*n
TC=[k for k in res if sqf(k[0]*k[1]) in B]; NC=[k for k in res if k not in TC]
for nm,S in [('TC',TC),('NC',NC)]:
    print(nm,len(S),'phi',np.mean([res[k]['phi'] for k in S]),'sig',np.mean([res[k]['sig'] for k in S]),'abs sig',np.mean([abs(res[k]['sig']) for k in S]),'qr',np.mean([res[k]['qr'] for k in S]))
print('TC abs sig excl (2,6)',np.mean([abs(res[k]['sig']) for k in TC if k!=(2,6)]))
print('TC positive sig count',sum(res[k]['sig']>0 for k in TC))
neg=sorted([(v['sig'],k) for k,v in res.items() if v['sig']<-0.004]); print('sig<-0.004',[(k,round(s,4),round(res[k]['qr'],4)) for s,k in neg])
print('all negative sig',sorted([(k,round(v['sig'],5)) for k,v in res.items() if v['sig']<0]))
print('(2,6)',res[(2,6)])
print('violations total',sum(v['viol'] for v in res.values()))
for k,v in sorted(res.items(),key=lambda kv:-abs(kv[1]['phi']))[:10]: print('top',k,round(v['phi'],4),round(v['om'],4),round(v['sig'],4))
print('(2,10) P(10|2), P(10|not2)',res[(2,10)]['PAgivennotB'] if False else '', )
print('joint density on same-char stratum pooled',sum(v['samejoint'] for v in res.values())/sum(v['same'] for v in res.values()))
print('mean j per pair on same stratum',np.mean([v['samejoint']/v['same'] for v in res.values()]))
for k in [(5,13),(5,10),(5,15),(3,7),(13,17),(2,3),(2,6),(7,21),(15,21),(3,15),(2,10),(21,29)]: print('j',k,round(res[k]['j'],6),round(res[k]['phi'],4))
