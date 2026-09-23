from sympy import primerange,factorint,legendre_symbol,is_primitive_root
from math import gcd
P=list(primerange(3,1000));fails=[];n=0
for p in P:
 for q in P:
  if p>5 and q>p and (q-p)%40==20:
   n+=1
   if legendre_symbol(10,q)!=-legendre_symbol(10,p):fails.append((p,q))
print('gap theorem prime-pairs below1000',n,'failures',fails)
print('gap sign example p=23,q=43:',int(legendre_symbol(10,23)),int(legendre_symbol(10,43)))
def sqf(a):
 d=1
 for p,e in factorint(a).items():
  if e%2:d*=p
 return d
n=0;fail=[]
for p in primerange(3,150):
 for a in range(1,21):
  for b in range(1,21):
   d=sqf(a*b)
   if (a*b)%p and legendre_symbol(d,p)==-1 and is_primitive_root(a,p) and is_primitive_root(b,p):fail.append((p,a,b))
   for c in range(1,21):
    if sqf(c)!=d or (a*b*c)%p==0:continue
    n+=1
    if legendre_symbol(c,p)!=legendre_symbol(a,p)*legendre_symbol(b,p):fail.append((p,a,b,c))
print('pair/triple checks, triple unit cases=',n,'failures=',fail)
print('(2,5,10) squarefree relationship=',sqf(10),sqf(2*5),'unit primes are p not in {2,5}')
