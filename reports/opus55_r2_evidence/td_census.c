// Round-2 independent census (different route from round 1):
//  - primes by segmented bitless Eratosthenes over odd n
//  - Artin(10) by per-prime TRIAL DIVISION of p-1 (32-bit), with a
//    primitivity test 10^((p-1)/q) != 1 at each distinct prime q | p-1
//  - no per-prime record file; streams straight into joint-cell count tables
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
typedef uint64_t u64; typedef uint32_t u32;
static u32 sp[7000]; static int nsp;
static inline u32 pw(u32 b,u32 e,u32 m){u64 r=1,x=b%m;while(e){if(e&1)r=r*x%m;x=x*x%m;e>>=1;}return (u32)r;}
static int artin10(u32 p){
  if(p<7) return 0;
  u32 n=p-1, m=n;
  for(int i=0;i<nsp;i++){ u32 q=sp[i]; if((u64)q*q>m) break;
    if(m%q==0){ if(pw(10,n/q,p)==1) return 0; do m/=q; while(m%q==0); } }
  if(m>1 && pw(10,n/m,p)==1) return 0;
  return 1;
}
static inline u64 mix64(u64 x){ x+=0x9e3779b97f4a7c15ULL; x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL; x=(x^(x>>27))*0x94d049bb133111ebULL; return x^(x>>31);}
#define M1 120
#define M2 840
static u64 T1[3][M1*M1][4], T2[3][M2*M2][4]; // [0]=all,[1]=hash even,[2]=hash odd
int main(int argc,char**argv){
  u64 LIM=strtoull(argv[1],0,10);
  {static char c[65536]; for(int i=2;i<65536;i++) if(!c[i]){ sp[nsp++]=i; for(long j=(long)i*i;j<65536;j+=i)c[j]=1; } }
  const u64 S=1<<24; unsigned char*sv=malloc(S); u32*pr=malloc(4*S); unsigned char*ar=malloc(S);
  u32 prevp=0; int preva=0; u64 np=0, nA=0; u64 g20=0,g20A=0,g20AA=0;
  u64 j[4]={0}; u64 cp8=0; u64 j8[4]; int done8=0;
  for(u64 lo=0; lo<=LIM; lo+=S){
    u64 hi=lo+S; if(hi>LIM+1) hi=LIM+1; u64 L=hi-lo;
    memset(sv,1,L);
    for(int i=0;i<nsp;i++){ u64 q=sp[i]; if(q*q>=hi) break; u64 st=((lo+q-1)/q)*q; if(st<q*q) st=q*q; for(u64 x=st;x<hi;x+=q) sv[x-lo]=0; }
    int k=0; for(u64 i=0;i<L;i++){ u64 x=lo+i; if(x>=7 && sv[i]) pr[k++]=(u32)x; }
    #pragma omp parallel for schedule(dynamic,4096)
    for(int i=0;i<k;i++) ar[i]=(unsigned char)artin10(pr[i]);
    for(int i=0;i<k;i++){
      u32 p=pr[i]; int a=ar[i]; np++; nA+=a;
      if(!done8 && p>100000000ULL){ memcpy(j8,j,sizeof j); done8=1; }
      if(prevp){
        int s=(preva<<1)|a; j[s]++;
        u64 c1=(u64)(prevp%M1)*M1+p%M1, c2=(u64)(prevp%M2)*M2+p%M2;
        int h=1+(int)(mix64(p)&1);
        T1[0][c1][s]++; T1[h][c1][s]++; T2[0][c2][s]++; T2[h][c2][s]++;
        u32 g=p-prevp; if(g%40==20){ g20++; g20A+=preva; g20AA+=(preva&a); }
      }
      prevp=p; preva=a;
    }
  }
  printf("primes>=7 %lu artin %lu last %u\n",np,nA,prevp);
  printf("joint00 %lu 01 %lu 10 %lu 11 %lu\n",j[0],j[1],j[2],j[3]);
  if(done8) printf("joint_1e8 %lu %lu %lu %lu\n",j8[0],j8[1],j8[2],j8[3]);
  printf("g20mod40 %lu predA %lu both %lu\n",g20,g20A,g20AA);
  FILE*f=fopen("tables.bin","wb"); fwrite(T1,sizeof T1,1,f); fwrite(T2,sizeof T2,1,f); fclose(f);
  return 0;
}
