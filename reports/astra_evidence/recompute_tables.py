import json,math,pathlib,itertools,numpy as np
R=pathlib.Path(__file__).resolve().parents[2];D=json.loads((R/'results/crossbase_fine_1e9.json').read_text());Q=json.loads((R/'results/crossbase_qr_1e9.json').read_text());B=D['bases']
def phi(t):
 a,b,c,d=np.array(t,dtype=float).flatten();den=(a+b)*(c+d)*(a+c)*(b+d);return (a*d-b*c)/math.sqrt(den) if den else None
def pool(tables):
 vals=[(sum(map(sum,t)),phi(t)) for t in tables];vals=[(n,p) for n,p in vals if n>=200 and p is not None];return sum(n*p for n,p in vals)/sum(n for n,p in vals)
def sf(n):
 ans=1;q=2
 while q*q<=n:
  e=0
  while n%q==0:n//=q;e^=1
  if e:ans*=q
  q+=1
 return ans*n
def factor(n):return [q for q in range(2,n+1) if n%q==0 and all(q%j for j in range(2,math.isqrt(q)+1))]
def cond(n):return n if n%4==1 else 4*n
rows=[]
for k,t in D['pairs'].items():
 a,b=map(int,k.split(','));rows.append((a,b,phi(t),pool(D['omega'][k].values()),pool(D['sig'][k].values()),pool(Q['cells'][k].values()),sf(a*b) in B))
print('MEANS',np.mean([r[2:6] for r in rows],axis=0));print('positive',sum(r[2]>0 for r in rows));print('negative',[(r[0:2],r[2]) for r in rows if r[2]<0]);print('meanABS',np.mean(np.abs([r[2:5] for r in rows]),axis=0))
for tc in (True,False):
 subset=[r for r in rows if r[-1]==tc];print('group',tc,len(subset),'mean',np.mean([r[2:6] for r in subset],axis=0),'abs sig',np.mean([abs(r[4]) for r in subset]));
print('TC excluding2,6',np.mean([abs(r[4]) for r in rows if r[-1] and r[:2]!=(2,6)]))
T=Q['cells']['2,6'];ss={}
for k,t in T.items():
 sig=k.split('.')[0];ss.setdefault(sig,np.zeros((2,2),dtype=int));ss[sig]+=np.array(t)
vals=[(int(k),sum(map(sum,v)),phi(v)) for k,v in ss.items() if int(k)//128==1 and not ((int(k)//4)%32&1) and phi(v) is not None and sum(map(sum,v))>=200]
print('2,6 family',len(vals),'min/max',min(x[2] for x in vals),max(x[2] for x in vals),'129',ss.get('129'))
# independent finite Kummer sum: enumerate 4 choices per ramified prime, form quadratic span by XOR prime masks.
lim=3000000;sieve=np.ones(lim+1,dtype=bool);sieve[:2]=False
for p in range(2,math.isqrt(lim)+1):
 if sieve[p]:sieve[p*p::p]=False
pr=np.flatnonzero(sieve);local={int(q):1-(2*int(q)-1)/(int(q)**2*(int(q)-1)) for q in pr};prod=math.prod(local.values())
errs=[]
for a,b,*_ in rows:
 S=sorted(set([2]+factor(a)+factor(b)));total=0
 for config in itertools.product(range(4),repeat=len(S)):
  m=n=L=ph=1;sign=1
  for p,state in zip(S,config):
   if state: L*=p;ph*=p-1
   if state&1:m*=p;sign=-sign
   if state&2:n*=p;sign=-sign
  classes=[1]
  if m%2==0:classes.append(a)
  if n%2==0:classes += [sf(v*b) for v in classes]
  eps=sum(L%cond(c)==0 for c in set(classes));total+=sign*eps/(ph*m*n)
 pred=total*prod/math.prod(local[p] for p in S);t=D['pairs'][f'{a},{b}'];obs=t[1][1]/D['n_primes'];err=abs(pred-obs)/obs;errs.append((err,a,b,pred,obs))
print('MODEL mean/max relative %',100*np.mean([v[0] for v in errs]),max(errs));
for f in ('results/kummer_model_comparison.json','code/kummer_model_comparison.json'):
 z=json.loads((R/f).read_text());ee=[abs(x['model_joint']-x['meas_joint'])/x['meas_joint'] for x in z];print('ARCHIVED',f,'mean_relative_percent',100*np.mean(ee),'max_relative_percent',100*max(ee),'first',z[0])
print('MODMODEL',[(a,b,p,o) for e,a,b,p,o in errs if (a,b) in [(5,13),(2,6),(21,29)]])
