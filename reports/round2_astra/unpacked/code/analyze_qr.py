import json,math,sys
from sympy import factorint
d=json.load(open(sys.argv[1]))
N=d['n_primes']
def sqf(n):
    s=1
    for q,e in factorint(n).items():
        if e%2: s*=q
    return s
def phi(m):
    n00,n01=m[0];n10,n11=m[1];n=n00+n01+n10+n11
    r0,r1,c0,c1=n00+n01,n10+n11,n00+n10,n01+n11
    if min(r0,r1,c0,c1)==0 or n==0: return None,0
    return (n11*n00-n10*n01)/math.sqrt(r0*r1*c0*c1),n
def pooled(cells,keyf,minn=200):
    num=den=0
    for k,mm in cells.items():
        if not keyf(k): continue
        r,n=phi(mm)
        if r is not None and n>=minn: num+=r*n;den+=n
    return (num/den if den else None),den
rows=[]
for k,m in d['plain'].items():
    a,b=map(int,k.split(','))
    g,_=phi(m)
    cells=d['cells'][k]
    # collapse chi -> condition on sig only
    sig_only={}
    for ck,mm in cells.items():
        s=ck.split('.')[0]
        t=sig_only.setdefault(s,[[0,0],[0,0]])
        for x in (0,1):
            for y in (0,1): t[x][y]+=mm[x][y]
    r_sig,_=pooled(sig_only,lambda k:True)
    r_qr,nqr=pooled(cells,lambda k:True)   # sig AND chi pair
    rows.append({'a':a,'b':b,'gcd':math.gcd(sqf(a),sqf(b)),'phi':g,
                 'phi_sig':r_sig,'phi_sigqr':r_qr})
sh=[r for r in rows if r['gcd']>1]; co=[r for r in rows if r['gcd']==1]
def M(L,k):
    v=[r[k] for r in L if r[k] is not None]; return sum(v)/len(v)
print(f"limit={d['limit']:,} primes={N:,} pairs={len(rows)}\n")
print(f"{'group':<22}{'phi':>10}{'|sig':>10}{'|sig+QR':>10}")
print('-'*52)
print(f"{'ALL (66)':<22}{M(rows,'phi'):>10.5f}{M(rows,'phi_sig'):>10.5f}{M(rows,'phi_sigqr'):>10.5f}")
print(f"{'sqf share a factor':<22}{M(sh,'phi'):>10.5f}{M(sh,'phi_sig'):>10.5f}{M(sh,'phi_sigqr'):>10.5f}   n={len(sh)}")
print(f"{'sqf coprime':<22}{M(co,'phi'):>10.5f}{M(co,'phi_sig'):>10.5f}{M(co,'phi_sigqr'):>10.5f}   n={len(co)}")
print(f"\n{'a':>4} {'b':>4} {'gcd':>4} {'phi':>9} {'phi|sig':>9} {'phi|sig+QR':>11}")
print('-'*50)
sh.sort(key=lambda r:r['phi_sig'])
for r in sh: print(f"{r['a']:>4} {r['b']:>4} {r['gcd']:>4} {r['phi']:>9.5f} {r['phi_sig']:>9.5f} {r['phi_sigqr']:>11.5f}")
json.dump(rows,open('crossbase_qr_summary.json','w'),indent=2)
