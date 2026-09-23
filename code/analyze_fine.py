import json,math,sys,itertools
from sympy import factorint
d=json.load(open(sys.argv[1]))
N=d['n_primes']; BAS=d['bases']
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
def pooled(cells,minn=200):
    num=den=0
    for k,mm in cells.items():
        r,n=phi(mm)
        if r is not None and n>=minn: num+=r*n; den+=n
    return (num/den if den else None), den
rows=[]
for k,m in d['pairs'].items():
    a,b=map(int,k.split(','))
    g,ng=phi(m)
    ro,_=pooled(d['omega'][k])
    rs,ns=pooled(d['sig'][k])
    rows.append({'a':a,'b':b,'phi':g,'z':g*math.sqrt(ng),'phi_om':ro,'phi_sig':rs,'n_sig':ns})
mg=sum(r['phi'] for r in rows)/len(rows)
mo=sum(r['phi_om'] for r in rows)/len(rows)
ms=sum(r['phi_sig'] for r in rows if r['phi_sig'] is not None)/len([r for r in rows if r['phi_sig'] is not None])
print(f"limit={d['limit']:,}  primes={N:,}  pairs={len(rows)}")
print(f"\nmean phi  unconditional                  = {mg:.5f}")
print(f"mean phi  | omega(p-1)                    = {mo:.5f}   ({100*(1-abs(mo)/abs(mg)):.1f}% removed)")
print(f"mean phi  | fine signature (v2,mask,nlrg) = {ms:.5f}   ({100*(1-abs(ms)/abs(mg)):.1f}% removed)")
tri=set()
for a,b,c in itertools.combinations(BAS,3):
    if sqf(sqf(a)*sqf(b))==sqf(c):
        for x in ((a,b),(a,c),(b,c)): tri.add(frozenset(x))
ent=[r for r in rows if frozenset((r['a'],r['b'])) in tri]
non=[r for r in rows if frozenset((r['a'],r['b'])) not in tri]
def mm(L,k): 
    v=[r[k] for r in L if r[k] is not None]; return sum(v)/len(v)
print(f"\n--- entanglement (chi_a*chi_b=chi_c) ---")
print(f"  in triple      n={len(ent):>3}  phi={mm(ent,'phi'):.5f}  |om={mm(ent,'phi_om'):.5f}  |sig={mm(ent,'phi_sig'):.5f}")
print(f"  not in triple  n={len(non):>3}  phi={mm(non,'phi'):.5f}  |om={mm(non,'phi_om'):.5f}  |sig={mm(non,'phi_sig'):.5f}")
rows.sort(key=lambda r:-abs(r['phi']))
print(f"\n{'a':>4} {'b':>4} {'phi':>9} {'phi|om':>9} {'phi|sig':>9} {'trip':>5}")
print('-'*48)
for r in rows[:14]:
    t='YES' if frozenset((r['a'],r['b'])) in tri else ''
    print(f"{r['a']:>4} {r['b']:>4} {r['phi']:>9.5f} {r['phi_om']:>9.5f} {r['phi_sig']:>9.5f} {t:>5}")
json.dump(rows,open('crossbase_fine_summary.json','w'),indent=2)
