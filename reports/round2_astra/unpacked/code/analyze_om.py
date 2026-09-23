import json,math,sys,itertools
from sympy import factorint
d=json.load(open(sys.argv[1] if len(sys.argv)>1 else 'crossbase_om_1e9.json'))
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
    return (n11*n00-n10*n01)/math.sqrt(r0*r1*c0*c1), n
BAS=d['bases']
gs=[];rs=[];rows=[]
for k,m in d['pairs'].items():
    a,b=map(int,k.split(','))
    g,ng=phi(m); gs.append(g)
    num=den=0
    for om,mm in d['omega'][k].items():
        r,n=phi(mm)
        if r is not None: num+=r*n; den+=n
    res=num/den if den else None
    rs.append(res)
    rows.append({'a':a,'b':b,'phi':g,'z':g*math.sqrt(ng),'phi_resid':res})
mg=sum(gs)/len(gs); mr=sum(rs)/len(rs)
print(f"limit={d['limit']:,}  primes={N:,}  pairs={len(gs)}")
print(f"\nmean phi, unconditional             = {mg:.5f}")
print(f"mean phi, conditioned on omega(p-1)  = {mr:.5f}")
print(f"REDUCTION = {100*(1-abs(mr)/abs(mg)):.1f}%")
# entanglement test
tri=set()
for a,b,c in itertools.combinations(BAS,3):
    if sqf(sqf(a)*sqf(b))==sqf(c):
        for x in ((a,b),(a,c),(b,c)): tri.add(frozenset(x))
ent=[r for r in rows if frozenset((r['a'],r['b'])) in tri]
non=[r for r in rows if frozenset((r['a'],r['b'])) not in tri]
print(f"\n--- entanglement test (chi_a*chi_b=chi_c) ---")
print(f"in a triple     n={len(ent):>3}  mean phi={sum(r['phi'] for r in ent)/len(ent):.5f}  mean resid={sum(r['phi_resid'] for r in ent)/len(ent):.5f}")
print(f"not in a triple n={len(non):>3}  mean phi={sum(r['phi'] for r in non)/len(non):.5f}  mean resid={sum(r['phi_resid'] for r in non)/len(non):.5f}")
rows.sort(key=lambda r:-abs(r['phi']))
print(f"\n{'a':>4} {'b':>4} {'phi':>10} {'z':>9} {'phi|omega':>11} {'triple':>7}")
print('-'*54)
for r in rows[:12]:
    t='YES' if frozenset((r['a'],r['b'])) in tri else ''
    print(f"{r['a']:>4} {r['b']:>4} {r['phi']:>10.5f} {r['z']:>9.1f} {r['phi_resid']:>11.5f} {t:>7}")
print("\nmarginal Artin densities:")
for a,c in d['marginals'].items(): print(f"   base {a:>3}: {c/N:.6f}")
json.dump(rows,open('crossbase_om_summary.json','w'),indent=2)
