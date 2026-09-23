#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
typedef unsigned long long u64; typedef unsigned __int128 u128;
static u64 mulm(u64 a,u64 b,u64 m){return (u128)a*b%m;}
static u64 pw(u64 b,u64 e,u64 m){u64 r=1;b%=m;while(e){if(e&1)r=mulm(r,b,m);b=mulm(b,b,m);e>>=1;}return r;}
static int mr(u64 n){ if(n<2)return 0; u64 B[]={2,3,5,7,11,13,17,19,23,29,31,37}; for(int i=0;i<12;i++){if(n%B[i]==0)return n==B[i];}
 u64 d=n-1;int s=0;while(!(d&1)){d>>=1;s++;}
 for(int i=0;i<12;i++){u64 x=pw(B[i],d,n); if(x==1||x==n-1)continue; int ok=0; for(int j=1;j<s;j++){x=mulm(x,x,n); if(x==n-1){ok=1;break;}} if(!ok)return 0;} return 1;}
// pollard rho factor
static u64 gcd(u64 a,u64 b){while(b){u64 t=a%b;a=b;b=t;}return a;}
static u64 rho(u64 n){ if(n%2==0) return 2; for(u64 c=1;;c++){ u64 x=2,y=2,d=1; while(d==1){ x=(mulm(x,x,n)+c)%n; y=(mulm(y,y,n)+c)%n; y=(mulm(y,y,n)+c)%n; d=gcd(x>y?x-y:y-x,n);} if(d!=n) return d; } }
static int nf; static u64 fs[64];
static void fac(u64 n){ if(n==1)return; if(mr(n)){ for(int i=0;i<nf;i++) if(fs[i]==n) return; fs[nf++]=n; return;} for(u64 q=2;q<1000;q++) if(n%q==0){ fac(q); while(n%q==0)n/=q; fac(n); return;} u64 d=rho(n); fac(d); while(n%d==0) n/=d; fac(n); }
int main(){
  u64 p=1000000000000000ULL|1; int found=0, primes=0, qnr=0, smallrej=0, surv=0, survfail=0, modexps=0;
  u64 sl[7]={3,5,7,11,13,17,19};
  while(found<300){
    if(mr(p)){ primes++;
      int leg = pw(10,(p-1)/2,p)==p-1;
      nf=0; fac(p-1); int art=1; for(int i=0;i<nf;i++) if(pw(10,(p-1)/fs[i],p)==1){art=0;break;}
      if(art && !leg){printf("BUG\n");}
      if(leg){ qnr++; int rej=0; for(int i=0;i<7;i++) if((p-1)%sl[i]==0){ modexps++; if(pw(10,(p-1)/sl[i],p)==1){rej=1;break;}}
        if(rej) smallrej++; else { surv++; if(!art){ survfail++; printf("survivor not artin: %llu\n",p);} } }
      if(art) found++;
    }
    p+=2;
  }
  printf("last p %llu primes %d qnr %d smallrej %d survivors %d survfail %d modexps %d\n",p-2,primes,qnr,smallrej,surv,survfail,modexps);
}
