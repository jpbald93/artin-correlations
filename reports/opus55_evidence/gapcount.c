// count consecutive prime pairs (p_n,p_{n+1}) with 7<=p_n, p_{n+1}<=HI and gap in {20,60}; also pi count
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
typedef uint64_t u64;
int main(int c,char**v){ u64 LO=strtoull(v[1],0,10), HI=strtoull(v[2],0,10);
 u64 r=(u64)sqrtl((long double)HI)+2; char*s=calloc(r+1,1); u64*P=malloc(sizeof(u64)*r/2+100); long np=0;
 for(u64 i=2;i<=r;i++) if(!s[i]){ if(i>2)P[np++]=i; for(u64 j=i*i;j<=r;j+=i)s[j]=1;}
 const u64 S=1<<24; unsigned char*seg=malloc(S); u64 last=0, cnt=0, g2060=0, gm40=0, first=0;
 for(u64 lo=LO; lo<=HI; lo+=2*S){ // odd numbers lo+2k
   u64 hi=lo+2*S; if(hi>HI+1) hi=HI+1;
   memset(seg,1,S);
   for(long i=0;i<np;i++){ u64 q=P[i]; if(q*q>=hi) break; u64 st=q*q; if(st<lo){ st=((lo+q-1)/q)*q; if(!(st&1)) st+=q; }
     for(u64 m=st;m<hi;m+=2*q) seg[(m-lo)/2]=0; }
   for(u64 k=0; lo+2*k<hi; k++) if(seg[k]){ u64 p=lo+2*k; if(p<7) continue; cnt++; if(!first) first=p; if(last){ u64 g=p-last; if(g==20||g==60) g2060++; if(g%40==20) gm40++; } last=p; }
 }
 printf("LO %llu HI %llu first %llu last %llu count %llu gaps20_60 %llu gmod40_20 %llu\n",(unsigned long long)LO,(unsigned long long)HI,(unsigned long long)first,(unsigned long long)last,(unsigned long long)cnt,(unsigned long long)g2060,(unsigned long long)gm40);
}
