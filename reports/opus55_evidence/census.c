// Independent census: for primes p in [5, HI], base-{2,3,5,6,7,10,11,13,15,17,21,29} Artin mask,
// quadratic-nonresidue mask, omega(p-1), fine signature. Segmented factor sieve over m=p-1.
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
typedef uint64_t u64; typedef uint32_t u32;
static const u32 B[12]={2,3,5,6,7,10,11,13,15,17,21,29};
static u32 sp[5000]; static int nsp=0;
static inline u64 pw(u64 b,u64 e,u64 m){u64 r=1;b%=m;while(e){if(e&1)r=r*b%m;b=b*b%m;e>>=1;}return r;}
int main(int argc,char**argv){
  u64 LO=strtoull(argv[1],0,10), HI=strtoull(argv[2],0,10); // m in [LO,HI) ; p=m+1
  FILE*fo=fopen(argv[3],"wb");
  // small primes up to 40000
  {char c[40001]={0}; for(int i=2;i<=40000;i++) if(!c[i]){sp[nsp++]=i; for(int j=i*i;j<=40000;j+=i)c[j]=1;}}
  const u64 S=1<<22;
  u32*rem=malloc(4*S); unsigned char*isp=malloc(S); u32*msk=malloc(4*S); /* art mask low12, bit12.. */
  unsigned char*om=malloc(S); unsigned short*sg=malloc(2*S); unsigned short*qn=malloc(2*S);
  for(u64 lo=LO; lo<HI; lo+=S){
    u64 hi=lo+S; if(hi>HI) hi=HI; u64 L=hi-lo;
    // primality of n=m+1 for m in [lo,hi)
    memset(isp,1,L);
    for(int i=0;i<nsp;i++){u64 q=sp[i]; if(q*q>hi) break; u64 st=((lo+1+q-1)/q)*q; if(st<q*q) st=q*q; for(u64 n=st;n<hi+1;n+=q) isp[n-lo-1]=0;}
    for(u64 i=0;i<L;i++){ u64 n=lo+1+i; if(n<5) isp[i]=0; }
    for(u64 i=0;i<L;i++){ rem[i]=(u32)(lo+i); msk[i]=0xFFF; om[i]=0; sg[i]=0; qn[i]=0;}
    for(u64 i=0;i<L;i++) if(isp[i]){ u64 p=lo+i+1; for(int k=0;k<12;k++){ if(B[k]%p==0){ msk[i]&=~(1u<<k);} } }
    for(int t=0;t<nsp;t++){ u64 q=sp[t]; if(q*q>hi) break;
      u64 st=((lo+q-1)/q)*q; if(st==0) st=q;
      for(u64 m=st;m<hi;m+=q){ u64 i=m-lo; if(!isp[i]) continue; u64 p=m+1;
        int v=0; while(rem[i]%q==0){rem[i]/=q; v++;}
        om[i]++;
        if(q==2){ int vv=v>4?4:v; sg[i]|=(vv-1);} 
        else if(q<=13){ int b= q==3?0:q==5?1:q==7?2:q==11?3:4; sg[i]|=1<<(2+b);} 
        else { int c=(sg[i]>>7)&3; if(c<3) c++; sg[i]=(sg[i]&0x7F)|(c<<7); }
        u64 e=(p-1)/q;
        for(int k=0;k<12;k++){ if(!(msk[i]>>k&1)) { if(q==2 && B[k]%p){ /* still need qr */ u64 r=pw(B[k],e,p); if(r==p-1) qn[i]|=1<<k; } continue;}
          u64 r=pw(B[k],e,p); if(r==1) msk[i]&=~(1u<<k); if(q==2 && r==p-1) qn[i]|=1<<k; }
      }
    }
    for(u64 i=0;i<L;i++) if(isp[i]){ u64 p=lo+i+1;
      if(rem[i]>1){ u64 q=rem[i]; om[i]++; if(q<=13){ /* only possible for tiny p */ if(q==2){sg[i]|=0;} else {int b= q==3?0:q==5?1:q==7?2:q==11?3:4; sg[i]|=1<<(2+b);} }
        else { int c=(sg[i]>>7)&3; if(c<3) c++; sg[i]=(sg[i]&0x7F)|(c<<7); }
        u64 e=(p-1)/q; for(int k=0;k<12;k++) if(msk[i]>>k&1){ if(pw(B[k],e,p)==1) msk[i]&=~(1u<<k);} }
      u32 pp=(u32)p; unsigned short a=(unsigned short)msk[i];
      fwrite(&pp,4,1,fo); fwrite(&a,2,1,fo); fwrite(&qn[i],2,1,fo); fwrite(&om[i],1,1,fo); fwrite(&sg[i],2,1,fo);
    }
  }
  fclose(fo); return 0;
}
