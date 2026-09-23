import sympy as s, math, json, pathlib, re, itertools, hashlib
R=pathlib.Path(__file__).resolve().parents[2]
out=[]
def log(*a): out.append(' '.join(map(str,a)));print(*a,flush=True)
def sf(n):return math.prod(int(q) for q,e in s.factorint(n).items() if e%2)
def sig(p):f=s.factorint(p-1);return (min(f.get(2,0),4),tuple(q for q in (3,5,7,11,13) if q in f),min(sum(q>13 for q in f),3))
# counterexamples to exact sufficiency of the signature, and omitted large-prime channel
seen={}; example=False
for p in s.primerange(7,100000):
 f=s.factorint(p-1);good=s.is_primitive_root(10,p);qr=int(s.legendre_symbol(10,p));key=(sig(p),qr)
 if key in seen and seen[key][1]!=good and not example:
  log('SAME SIGNATURE + QR BUT DIFFERENT ARTIN:',seen[key],(p,good),'signature=',key,'orders=',s.n_order(10,seen[key][0]),s.n_order(10,p));example=True
 seen[key]=(p,good)
 if qr==-1 and all(pow(10,(p-1)//q,p)!=1 for q in f if q<=13) and not good:
  log('SMALL CHANNELS PASS BUT LARGE CHANNEL FAILS:',p,'factorization=',f,'order=',s.n_order(10,p));break
# independent prime-search exact counters
visited=kept=survivors=cheap=found=0;p=10**15;last=None
while found<300:
 p=int(s.nextprime(p));visited+=1
 nr=s.legendre_symbol(10,p)==-1
 if nr:kept+=1
 passes=bool(nr)
 if passes:
  for q in (3,5,7,11,13,17,19):
   if (p-1)%q==0:
    cheap+=1
    if pow(10,(p-1)//q,p)==1:passes=False;break
 if passes:survivors+=1
 if s.is_primitive_root(10,p):found+=1
 last=p
log('SEARCH COUNTS',visited,kept,survivors,cheap,found,last)
log('timing ratios',.1642/.0841,.1642/.0732,.0841/.0732,'time saving fraction',1-.0732/.0841)
log('EXPLICIT GAP',23,43,int(s.legendre_symbol(10,23)),int(s.legendre_symbol(10,43)), 'product',int(s.legendre_symbol(10,23)*s.legendre_symbol(10,43)))
log('D=1 examples',[(p,s.n_order(2,p),s.n_order(8,p)) for p in (3,5,11,29,53)])
log('p=2 triple',[(a,bool(s.is_primitive_root(a,2))) for a in (3,5,15)])
# theorem scan with square bases, square factors and all small p
pairbad=triplebad=0
for p in s.primerange(3,100):
 for a in range(1,26):
  for b in range(1,26):
   d=sf(a*b);aa=a%p!=0 and s.is_primitive_root(a,p);bb=b%p!=0 and s.is_primitive_root(b,p)
   pairbad+=bool(aa and bb and s.legendre_symbol(d,p)==-1)
   for c in (d,4*d):triplebad+=bool(aa and bb and c%p!=0 and s.is_primitive_root(c,p))
log('SMALL GENERAL PAIR/TRIPLE violations',pairbad,triplebad)
# references
tex=(R/'artin_correlations.tex').read_text();labs=re.findall(r'\\label\{([^}]+)\}',tex);refs=re.findall(r'\\(?:eq)?ref\{([^}]+)\}',tex);bib=re.findall(r'\\bibitem\{([^}]+)\}',tex);cites=[x.strip() for z in re.findall(r'\\cite(?:\[[^]]*\])?\{([^}]+)\}',tex) for x in z.split(',')]
log('undefined refs',set(refs)-set(labs),'duplicate labels',[k for k in set(labs) if labs.count(k)>1],'undefined citations',set(cites)-set(bib),'unused bib',set(bib)-set(cites))
log('source hash',hashlib.sha256((R/'code/artin_payoff.c').read_bytes()).hexdigest())
(R/'reports/astra_evidence/independent_checks.txt').write_text('\n'.join(out)+'\n')
