import numpy as np
from scipy.special import expit
raw=np.fromfile('reports/round2_astra/tables_1000000000.bin',dtype=np.uint64).reshape(840,840,2,4)
for M in [120,840]:
 a=(raw.reshape(7,120,7,120,2,4).sum(axis=(0,2)) if M==120 else raw).reshape(-1,2,4).astype(float)
 a=a[a.sum(axis=(1,2))>0];fit=a[:,0];test=a[:,1];n0=fit[:,:2].sum(1);n1=fit[:,2:].sum(1);y=fit[:,1]+fit[:,3];n=n0+n1;pr=(y+.5)/(n+1);active=(y>0)&(y<n)
 alpha=np.log(pr/(1-pr));beta=0
 # Simultaneous Newton using exact diagonal alpha block and Schur complement.
 for it in range(30):
  q0=expit(alpha);q1=expit(alpha+beta);v0=n0*q0*(1-q0);v1=n1*q1*(1-q1);h=v0+v1;ga=y-n0*q0-n1*q1;gb=(fit[:,3]-n1*q1)[active].sum()
  db=(gb-(v1[active]*ga[active]/h[active]).sum())/(v1[active]-v1[active]**2/h[active]).sum()
  da=(ga[active]-v1[active]*db)/h[active];alpha[active]+=da;beta+=db
  if abs(db)<1e-12 and max(abs(da))<1e-10:break
 q0=expit(alpha);q1=expit(alpha+beta);q0[~active]=pr[~active];q1[~active]=pr[~active]
 loss0=-(test[:,[0,2]].sum(1)*np.log1p(-pr)+test[:,[1,3]].sum(1)*np.log(pr)).sum()/test.sum()
 loss1=-(test[:,0]*np.log1p(-q0)+test[:,1]*np.log(q0)+test[:,2]*np.log1p(-q1)+test[:,3]*np.log(q1)).sum()/test.sum()
 print('independent_pooled M=',M,'beta=',beta,'iterations=',it+1,'baseline=',loss0,'pooled=',loss1,'heldout_gain=',loss0-loss1)
