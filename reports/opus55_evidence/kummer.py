import json, math
from itertools import combinations
from sympy import primerange, factorint
def sqf(n):
    r=1
    for q,e in factorint(n).items():
        if e%2: r*=q
    return r
def subsets(S):
    out=[1]
    for q in S: out+= [x*q for x in out]
    return out
def phi(n):
    r=1
    for q in factorint(n): r*=q-1
    return r
EUL={}
PR=list(primerange(2,3_000_000))
def euler(S,k):  # k=1 single, 2 pair
    key=(tuple(sorted(S)),k)
    if key in EUL: return EUL[key]
    pr=1.0
    for q in PR:
        if q in S: continue
        pr*= (1-1/(q*(q-1))) if k==1 else (1-(2*q-1)/(q*q*(q-1)))
    EUL[key]=pr; return pr
def ok(d,L): return d>1 and d%4==1 and L%d==0
def single(a):
    da=sqf(a); S=sorted(set([2]+list(factorint(da))))
    s=0
    for m in subsets(S):
        mu=(-1)**len(factorint(m))
        V=[1]+([da] if m%2==0 else [])
        eps=sum(1 for v in V if v==1 or ok(v,m))
        s+=mu*eps/(phi(m)*m)
    return s*euler(S,1)
def pair(a,b):
    da,db=sqf(a),sqf(b); S=sorted(set([2]+list(factorint(da))+list(factorint(db))))
    s=0
    for m in subsets(S):
        for n in subsets(S):
            L=m*n//math.gcd(m,n)
            mu=(-1)**(len(factorint(m))+len(factorint(n)))
            V={1}
            if m%2==0: V|={da}
            if n%2==0: V|={db}
            if m%2==0 and n%2==0: V|={sqf(da*db)}
            eps=sum(1 for v in V if v==1 or ok(v,L))
            s+=mu*eps/(phi(L)*m*n)
    return s*euler(S,2)
B=[2,3,5,6,7,10,11,13,15,17,21,29]
meas=json.load(open('cross_5.json'))
dens={b:single(b) for b in B}
print({b:round(v,6) for b,v in dens.items()})
errs=[]
for a,b in combinations(B,2):
    jm=meas[f"{a},{b}"]['j']; jp=pair(a,b)
    pa,pb=dens[a],dens[b]
    phm=(jp-pa*pb)/math.sqrt(pa*(1-pa)*pb*(1-pb))
    e=(jm-jp)/jp; errs.append((abs(e),a,b,jm,jp,phm))
errs.sort()
print('mean rel err %.4f%%  max %.4f%% at %s'%(100*sum(e[0] for e in errs)/len(errs),100*errs[-1][0],errs[-1][1:3]))
for e in errs:
    if (e[1],e[2]) in [(5,13),(5,10),(5,15),(3,7),(13,17),(2,3),(2,6),(7,21),(15,21),(3,15),(2,10),(21,29)]: print(e[1],e[2],'meas %.6f model %.6f rel %.3f%% model phi %.4f'%(e[3],e[4],100*(e[3]-e[4])/e[4],e[5]))
