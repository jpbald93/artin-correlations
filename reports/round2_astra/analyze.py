import numpy as np,json,sys
from scipy.special import xlogy,expit
raw=np.fromfile('reports/round2_astra/tables_1000000000.bin',dtype=np.uint64).reshape(840,840,2,4)
output={}
for M in [120,840]:
 if M==120:
  a=raw.reshape(7,120,7,120,2,4).sum(axis=(0,2)).reshape(-1,2,4).astype(float)
 else:a=raw.reshape(-1,2,4).astype(float)
 c=a.sum(axis=1);n=c.sum(1);x0=c[:,:2].sum(1);x1=c[:,2:].sum(1);y=c[:,1]+c[:,3];N=n.sum();S1=x1.sum();S0=x0.sum();R=S1/N
 keep=n>0;c=c[keep];a=a[keep];n=n[keep];x0=x0[keep];x1=x1[keep];y=y[keep]
 pc=y/n;w1=x1/S1;w0=x0/S0;rc=x1/n;dc=np.zeros(len(n));both=(x1>0)&(x0>0);dc[both]=c[both,3]/x1[both]-c[both,1]/x0[both]
 delta=c[:,3].sum()/S1-c[:,1].sum()/S0
 paperW=(w1*dc).sum();B=((w1-w0)*pc).sum();W=(n/N*rc*(1-rc)/(R*(1-R))*dc).sum()
 def entropy(s,t):return -(xlogy(s,np.divide(s,t,out=np.zeros_like(s),where=t>0))+xlogy(t-s,np.divide(t-s,t,out=np.zeros_like(s),where=t>0))).sum()/N
 H=entropy(np.array([y.sum()]),np.array([N]));Hc=entropy(y,n);Hcx=entropy(c[:,1],x0)+entropy(c[:,3],x1)
 pred=[]
 for trainbit in [0,1]:
  fit=a[:,trainbit];test=a[:,1-trainbit];nt=test.sum();pr=(fit[:,1]+fit[:,3]+.5)/(fit.sum(1)+1);p0=(fit[:,1]+.5)/(fit[:,:2].sum(1)+1);p1=(fit[:,3]+.5)/(fit[:,2:].sum(1)+1)
  lossR=-(test[:,[0,2]].sum(1)*np.log1p(-pr)+test[:,[1,3]].sum(1)*np.log(pr)).sum()/nt
  lossX=-(test[:,0]*np.log1p(-p0)+test[:,1]*np.log(p0)+test[:,2]*np.log1p(-p1)+test[:,3]*np.log(p1)).sum()/nt
  pred.append(dict(train_hash_bit=trainbit,test_N=int(nt),CE_residue=lossR,CE_plusX=lossX,gain=lossR-lossX))
 active=both&(y>0)&(y<n)
 r=dict(N=int(N),table=c.sum(0).astype(int).tolist(),delta=delta,CE_marginal=H,CE_residues=Hc,CE_plusX=Hcx,in_sample_gain=Hc-Hcx,X_nondegenerate_cells=int(both.sum()),XY_nondegenerate_cells=int(active.sum()),claimed_k_over_2N=both.sum()/(2*N),active_k_over_2N=active.sum()/(2*N),paper_within=paperW,paper_between_residual=delta-paperW,paper_between_residual_share=(delta-paperW)/delta,actual_between_formula=B,paper_RHS=paperW+B,paper_identity_error=paperW+B-delta,corrected_within=W,corrected_between_share=B/delta,corrected_identity_error=W+B-delta,holdout=pred)
 output[M]=r
 print('MOD',M);print(json.dumps(r,indent=2))
json.dump(output,open('reports/round2_astra/independent_stats.json','w'),indent=2)
# Small rational counterexample independent of the census
from fractions import Fraction as F
cells=[[3,1,1,3],[6,2,1,1]]
S1=sum(v[2]+v[3] for v in cells);S0=sum(v[0]+v[1] for v in cells)
delta=F(sum(v[3] for v in cells),S1)-F(sum(v[1] for v in cells),S0)
W=sum(F(v[2]+v[3],S1)*(F(v[3],v[2]+v[3])-F(v[1],v[0]+v[1])) for v in cells)
B=sum((F(v[2]+v[3],S1)-F(v[0]+v[1],S0))*F(v[1]+v[3],sum(v)) for v in cells)
print('RATIONAL_COUNTEREXAMPLE',cells,'delta',delta,'W',W,'B',B,'RHS',W+B,'error',W+B-delta)
