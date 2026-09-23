/* Round-2 independent census. No author code, prime list, or result inputs. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
typedef uint64_t U;
static U powmod(U a,U n,U p){U r=1;while(n){if(n&1)r=r*a%p;a=a*a%p;n>>=1;}return r;}
/* SplitMix64 specification, to reproduce the manuscript's declared split. */
static U hash(U x){x+=0x9e3779b97f4a7c15ULL;x=(x^(x>>30))*0xbf58476d1ce4e5b9ULL;x=(x^(x>>27))*0x94d049bb133111ebULL;return x^(x>>31);}
int main(int argc,char**argv){unsigned lim=argc>1?strtoul(argv[1],0,10):1000000000;uint16_t *spf=calloc(lim/2+1,2);if(!spf)return 1;
for(U q=3;q*q<lim;q+=2)if(!spf[q/2])for(U t=q*q;t<lim;t+=2*q)if(!spf[t/2])spf[t/2]=q;
fprintf(stderr,"independent sieve finished\n");
U *tab=calloc(840*840*8,sizeof(U));U total[4]={0},np=0,na=0,ex[3]={0};unsigned prev=0;int old=0;
for(unsigned p=7;p<lim;p+=2){if(spf[p/2])continue;np++;int art=powmod(10,(p-1)/2,p)!=1;unsigned m=(p-1);while(!(m&1))m/=2;
while(m>1 && art){unsigned q=spf[m/2];if(!q)q=m;if(powmod(10,(p-1)/q,p)==1)art=0;do{m/=q;}while(m%q==0);}
na+=art;if(prev){unsigned code=old*2+art;total[code]++;unsigned c=(prev%840)*840+p%840;tab[c*8+(hash(p)&1)*4+code]++;if((p-prev)%40==20){ex[0]++;ex[1]+=old;ex[2]+=old*art;}}prev=p;old=art;
}
printf("limit=%u primes_ge7=%llu artin=%llu table00,01,10,11=%llu,%llu,%llu,%llu exclusion_total,prev,both=%llu,%llu,%llu\n",lim,(unsigned long long)np,(unsigned long long)na,(unsigned long long)total[0],(unsigned long long)total[1],(unsigned long long)total[2],(unsigned long long)total[3],(unsigned long long)ex[0],(unsigned long long)ex[1],(unsigned long long)ex[2]);
char fname[128];sprintf(fname,"reports/round2_astra/tables_%u.bin",lim);FILE*f=fopen(fname,"wb");fwrite(tab,sizeof(U),840*840*8,f);fclose(f);free(tab);free(spf);return 0;}
