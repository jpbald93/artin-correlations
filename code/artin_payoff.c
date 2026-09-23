/* artin_payoff.c
 * Exploratory tool: does the consecutive-prime Artin correlation (base 10)
 * yield any usable computational payoff?
 *
 * modes:
 *   census LIMIT          - aggregate pair statistics + QR-filter soundness
 *   search START COUNT    - timed search for COUNT Artin-base-10 primes >= START,
 *                           naive vs QR-pre-filtered
 *
 * Output: JSON on stdout. ASCII only. Deterministic.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#ifdef _OPENMP
#include <omp.h>
#endif

typedef long long ll;
typedef unsigned long long u64;

static int  *sml;  static long n_sml;

static void build_small(long lim){
    char *c = (char*)calloc(lim+1,1);
    sml = (int*)malloc(sizeof(int)*(lim+2)); n_sml = 0;
    for(long i=2;i<=lim;i++)
        if(!c[i]){ sml[n_sml++]=(int)i; for(long j=i*i;j<=lim;j+=i) c[j]=1; }
    free(c);
}

/* deterministic Miller-Rabin for 64-bit (bases valid for n < 3.3e24) */
static inline u64 pw(u64 b,u64 e,u64 m){ u64 r=1%m; b%=m; while(e){ if(e&1) r=(u64)((unsigned __int128)r*b%m); b=(u64)((unsigned __int128)b*b%m); e>>=1; } return r; }
static int is_prime64(u64 n){
    if(n<2) return 0;
    static const u64 sp[12]={2,3,5,7,11,13,17,19,23,29,31,37};
    for(int i=0;i<12;i++){ if(n%sp[i]==0) return n==sp[i]; }
    u64 d=n-1; int s=0; while(!(d&1)){ d>>=1; s++; }
    for(int i=0;i<12;i++){
        u64 x=pw(sp[i],d,n);
        if(x==1||x==n-1) continue;
        int ok=0;
        for(int j=1;j<s;j++){ x=(u64)((unsigned __int128)x*x%n); if(x==n-1){ ok=1; break; } }
        if(!ok) return 0;
    }
    return 1;
}

/* fast Legendre symbol (10|p) by reciprocity: (2|p)*(5|p), O(1) */
static inline int leg10_fast(u64 p){
    int c2 = (p%8==1||p%8==7) ? 1 : -1;
    int c5 = (p%5==1||p%5==4) ? 1 : -1;
    return c2*c5;
}

/* Legendre symbol (10|p) for odd prime p: +1 / -1 */
static inline int leg10(u64 p){
    u64 e=(p-1)/2, b=10%p, r=1;
    while(e){ if(e&1) r=(u64)((unsigned __int128)r*b%p); b=(u64)((unsigned __int128)b*b%p); e>>=1; }
    if(r==1) return 1;
    if(r==p-1) return -1;
    return 0;
}

/* is 10 a primitive root mod p ?  (factor p-1, test 10^((p-1)/q) != 1) */
static inline int artin10(u64 p){
    u64 m=p-1, q[64]; int nf=0;
    for(long i=0;i<n_sml;i++){
        u64 d=(u64)sml[i];
        if(d*d>m) break;
        if(m%d==0){ q[nf++]=d; while(m%d==0) m/=d; }
    }
    if(m>1) q[nf++]=m;
    for(int i=0;i<nf;i++){
        u64 e=(p-1)/q[i], b=10%p, r=1;
        while(e){ if(e&1) r=(u64)((unsigned __int128)r*b%p); b=(u64)((unsigned __int128)b*b%p); e>>=1; }
        if(r==1) return 0;
    }
    return 1;
}

/* primes in [7, LIMIT) into a malloc'd int array (odd-only sieve) */
static int* sieve_primes(u64 LIMIT, long *out_n){
    u64 half = LIMIT/2 + 1;
    char *comp = (char*)calloc(half,1);          /* index i <-> number 2i+1 */
    for(u64 i=1; (2*i+1)*(2*i+1) < LIMIT; i++){
        if(!comp[i]){ u64 p=2*i+1; for(u64 j=(p*p-1)/2; j<half; j+=p) comp[j]=1; }
    }
    int *pr = (int*)malloc(sizeof(int)*(half/4+1024)); long n=0;
    for(u64 i=3;i<half;i++){ u64 p=2*i+1; if(p>=LIMIT) break; if(!comp[i]) pr[n++]=(int)p; }
    free(comp); *out_n=n; return pr;
}

static double now(void){ struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t); return t.tv_sec+1e-9*t.tv_nsec; }

/* cross-entropy report for a joint-cell table: cells[k][(a_prev<<1)|a_next] */
static void ce_report(ll *cells, long ncells, const char *name){
    double N=0,n1=0;
    for(long i=0;i<ncells;i++){ double s=cells[4*i]+cells[4*i+1]+cells[4*i+2]+cells[4*i+3];
        N+=s; n1+=cells[4*i+1]+cells[4*i+3]; }
    double p=n1/N;
    double CE1=-(n1*log(p)+(N-n1)*log(1-p))/N, CE2=0, CE3=0;
    for(long i=0;i<ncells;i++){
        double n00=cells[4*i],n01=cells[4*i+1],n10=cells[4*i+2],n11=cells[4*i+3];
        double s=n00+n01+n10+n11, m=n01+n11;
        if(s>0){ double pc=m/s; if(m>0) CE2-=m*log(pc); if(s-m>0) CE2-=(s-m)*log(1-pc); }
        double p1=n10+n11, p0=n00+n01;
        if(p1>0){ double q=n11/p1; if(n11>0) CE3-=n11*log(q); if(n10>0) CE3-=n10*log(1-q); }
        if(p0>0){ double r=n01/p0; if(n01>0) CE3-=n01*log(r); if(n00>0) CE3-=n00*log(1-r); }
    }
    CE2/=N; CE3/=N;
    printf("  \"%s\": {\"N\": %.0f, \"CE_marginal\": %.6f, \"CE_residues\": %.6f, \"CE_residues_plus_prevA\": %.6f, \"gain_from_residues\": %.6f, \"gain_prevA_beyond_residues\": %.6f},\n",
           name, N, CE1, CE2, CE3, CE1-CE2, CE2-CE3);
}

#define M1 120
#define M2 840
static ll J1[M1*M1][4], J2[M2*M2][4];
static ll K1[M1*M1][4], K2[M2*M2][4];

/* deterministic 64-bit mixer, for an exchangeable (drift-free) train/test split */
static inline u64 mix64(u64 x){
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

/* Held-out evaluation: fit cell probabilities on the FIT table, score the TEST table.
 * Jeffreys (0.5) smoothing, so empty fit cells fall back to 1/2.
 * heldout_gain = CE_residues - CE_residues_plus_prevA  (>0 means the augmented model wins). */
static void hold_report(ll *fit, ll *test, long ncells, const char *name){
    double N=0, ceR=0, ceRP=0;
    for(long i=0;i<ncells;i++){
        double n00=test[4*i], n01=test[4*i+1], n10=test[4*i+2], n11=test[4*i+3];
        double s=n00+n01+n10+n11; if(s<=0) continue;
        N+=s;
        double f00=fit[4*i], f01=fit[4*i+1], f10=fit[4*i+2], f11=fit[4*i+3];
        double fs=f00+f01+f10+f11;
        double pR=(f01+f11+0.5)/(fs+1.0);
        double qR=(f11+0.5)/(f10+f11+1.0);
        double rR=(f01+0.5)/(f00+f01+1.0);
        if(n01>0) ceR-=n01*log(pR); if(n00>0) ceR-=n00*log(1-pR);
        if(n11>0) ceR-=n11*log(pR); if(n10>0) ceR-=n10*log(1-pR);
        if(n11>0) ceRP-=n11*log(qR); if(n10>0) ceRP-=n10*log(1-qR);
        if(n01>0) ceRP-=n01*log(rR); if(n00>0) ceRP-=n00*log(1-rR);
    }
    ceR/=N; ceRP/=N;
    printf("  \"%s\": {\"N_test\": %.0f, \"CE_residues\": %.9f, \"CE_residues_plus_prevA\": %.9f, \"heldout_gain\": %.9e},\n",
           name, N, ceR, ceRP, ceR-ceRP);
}

/* ================= within/between decompositions (TWO conventions) =================
 * X = predecessor Artin, Y = successor Artin, C = joint-residue cell. Cell counts are
 * [n00,n01,n10,n11] with the FIRST index the X value.
 *
 * (1) KITAGAWA ordering with w1 weights:
 *       delta = sum_c w1_c*dc_c  +  sum_c (w1_c - w0_c)*q0_c
 *     w1_c = P(C=c|X=1), w0_c = P(C=c|X=0), dc_c = q1_c - q0_c,
 *     q0_c = P(Y=1|X=0,C=c), q1_c = P(Y=1|X=1,C=c).
 * (2) COVARIANCE ordering (law of total covariance, rescaled by Var(X)):
 *       delta = sum_c pi_c*r_c*(1-r_c)*dc_c / Var(X)  +  sum_c (w1_c - w0_c)*p_c
 *     pi_c = n_c/N, r_c = P(X=1|C=c), p_c = P(Y=1|C=c), Var(X)=P(X=1)P(X=0).
 *     The second term is exactly the p_c term printed in the paper.
 *
 * BOTH sums are accumulated INDEPENDENTLY from the cell table, so the two identities are
 * genuinely asserted: the assertion can fail and a failure exits non-zero. Cells with an
 * empty X margin have no identifiable within-cell association and land in the between term.
 * k counts cells with BOTH Y margins non-empty -- the only cells where splitting on X adds
 * a free response parameter (boundary cells whose successor can never be Artin gain nothing). */
static void decomp_report(ll *c, long ncells, const char *name){
    double n00=0,n01=0,n10=0,n11=0;
    for(long i=0;i<ncells;i++){ n00+=c[4*i]; n01+=c[4*i+1]; n10+=c[4*i+2]; n11+=c[4*i+3]; }
    double N=n00+n01+n10+n11, s1=n10+n11, s0=n00+n01;
    double delta=n11/s1 - n01/s0;
    double varX=(s1/N)*(s0/N);
    double K_W=0,K_B=0,C_W=0,C_B=0,W0_W=0,SYM_W=0,xonly=0,kx=0,kxy=0;
    for(long i=0;i<ncells;i++){
        double a=c[4*i], b=c[4*i+1], g0=c[4*i+2], g1=c[4*i+3];
        double n=a+b+g0+g1; if(n<=0) continue;
        double pi=n/N, m1=g0+g1, m0=a+b, t1=b+g1;
        if(m1>0) kx+=1;
        if(m1>0 && t1>0 && (n-t1)>0) kxy+=1;
        if(m1==0) xonly+=pi;
        double w1=m1/s1, w0=m0/s0, p=t1/n, r=m1/n;
        double q0=(m0>0)? b/m0 : 0.0, q1=(m1>0)? g1/m1 : 0.0;
        /* Convention: q0_c := 0 when the cell has no X=0 pairs. Then
         *   delta = sum_c w1_c*(q1_c-q0_c) + sum_c (w1_c-w0_c)*q0_c
         * holds EXACTLY for any such assignment, because the q0 terms cancel:
         *   sum a_c q1 - sum a_c q0 + sum a_c q0 - sum b_c q0 = sum a_c q1 - sum b_c q0.
         * The within sum therefore runs over every cell with m1>0 (a_c>0); cells with
         * m1=0 have a_c=0 and contribute nothing. X=0-empty cells have r_c=1, so their
         * covariance contribution vanishes automatically. */
        double dc=q1-q0;
        if(m1>0){ K_W += w1*dc; C_W += (pi*r*(1-r)*dc)/varX; W0_W += w0*dc; SYM_W += 0.5*(w1+w0)*dc; }
        K_B += (w1-w0)*q0;
        C_B += (w1-w0)*p;
    }
    double Ke=(K_W+K_B)-delta, Ce=(C_W+C_B)-delta;
    printf("  \"%s\": {\n", name);
    printf("    \"N\": %.0f, \"delta\": %.9f,\n", N, delta);
    printf("    \"kitagawa\":   {\"within\": %.9f, \"between\": %.9f, \"between_share\": %.4f, \"identity_error\": %.2e},\n",
           K_W, K_B, (delta!=0? K_B/delta:0.0), Ke);
    printf("    \"covariance\": {\"within\": %.9f, \"between\": %.9f, \"between_share\": %.4f, \"identity_error\": %.2e},\n",
           C_W, C_B, (delta!=0? C_B/delta:0.0), Ce);
    printf("    \"between_share_by_convention\": {\"w1\": %.4f, \"w0\": %.4f, \"symmetric\": %.4f, \"covariance\": %.4f},\n",
           (delta!=0? K_B/delta:0.0), (delta!=0? (delta-W0_W)/delta:0.0),
           (delta!=0? (delta-SYM_W)/delta:0.0), (delta!=0? C_B/delta:0.0));
    printf("    \"cells\": {\"x_nondegenerate\": %.0f, \"xy_nondegenerate\": %.0f, \"x_only_mass\": %.3e},\n", kx, kxy, xonly);
    printf("    \"optimism\": {\"k\": %.0f, \"k_over_2N\": %.3e},\n", kxy, kxy/(2*N));
    int ok = (fabs(Ke)<1e-12) && (fabs(Ce)<1e-12);
    printf("    \"ASSERT\": \"%s\"},\n", ok? "both identities hold" : "IDENTITY FAILURE");
    if(!ok){ printf("  \"ASSERT_FAIL_%s\": true,\n", name); exit(1); }
}

/* pooled-beta held-out model: logit P(Y=1) = alpha_cell + beta*x.
 * alpha is profiled out by Newton, but ONLY for cells whose fit-half margins are all
 * non-empty: a one-sided fit cell has alpha = -/+inf, so its Jeffreys value is retained
 * instead (an unregularised MLE there produced a 136-nat loss in a single cell). */
static void pool_report(ll *fit, ll *test, long ncells, const char *name){
    static double al[705600];
    for(long i=0;i<ncells;i++){
        double a=fit[4*i],b=fit[4*i+1],g0=fit[4*i+2],g1=fit[4*i+3];
        double m=b+g1, n=a+g0;
        double p=(m+0.5)/(m+n+1.0); al[i]=log(p/(1-p));
    }
    double beta=0;
    for(int it=0;it<200;it++){
        for(int k=0;k<3;k++){
            for(long i=0;i<ncells;i++){
                double a=fit[4*i],b=fit[4*i+1],g0=fit[4*i+2],g1=fit[4*i+3];
                if((a+b)==0||(g0+g1)==0||(b+g1)==0||(a+g0)==0) continue;   /* one-sided: keep Jeffreys alpha */
                double p0=1/(1+exp(-al[i])), p1=1/(1+exp(-(al[i]+beta)));
                double g=(b-(a+b)*p0)+(g1-(g0+g1)*p1);
                double h=(a+b)*p0*(1-p0)+(g0+g1)*p1*(1-p1)+1e-9;
                double st=g/h; if(st>2) st=2; if(st<-2) st=-2; al[i]+=st;
            }
        }
        double gb=0,hb=0;
        for(long i=0;i<ncells;i++){
            double a=fit[4*i],b=fit[4*i+1],g0=fit[4*i+2],g1=fit[4*i+3];
            if((g0+g1)==0||(b+g1)==0||(a+g0)==0) continue;
            double p1=1/(1+exp(-(al[i]+beta)));
            gb+=(g1-(g0+g1)*p1); hb+=(g0+g1)*p1*(1-p1);
        }
        beta+=gb/(hb+1e-12);
    }
    double N=0,ceR=0,ceB=0;
    for(long i=0;i<ncells;i++){
        double a=test[4*i],b=test[4*i+1],g0=test[4*i+2],g1=test[4*i+3];
        double s=a+b+g0+g1; if(s<=0) continue; N+=s;
        double f00=fit[4*i],f01=fit[4*i+1],f10=fit[4*i+2],f11=fit[4*i+3];
        double pr=(f01+f11+0.5)/(f00+f01+f10+f11+1.0);
        pr = pr<1e-12?1e-12:(pr>1-1e-12?1-1e-12:pr);
        ceR-= (b+g1)*log(pr) + (a+g0)*log(1-pr);
        double q0=1/(1+exp(-al[i])), q1=1/(1+exp(-(al[i]+beta)));
        if((f10+f11)==0||(f00+f01)==0||(f01+f11)==0||(f00+f10)==0){ q0=pr; q1=pr; }
        q0=q0<1e-12?1e-12:(q0>1-1e-12?1-1e-12:q0); q1=q1<1e-12?1e-12:(q1>1-1e-12?1-1e-12:q1);
        ceB-= b*log(q0)+a*log(1-q0) + g1*log(q1)+g0*log(1-q1);
    }
    ceR/=N; ceB/=N;
    printf("  \"%s\": {\"N_test\": %.0f, \"beta\": %.6f, \"CE_residues\": %.9f, \"CE_pooled_beta\": %.9f, \"heldout_gain\": %.3e},\n",
           name, N, beta, ceR, ceB, ceR-ceB);
}

/* ============== permutation null (fixed margins, hypergeometric draw) ==============
 * Holds both margins of every cell fixed and redraws n11 ~ Hypergeometric(n, m1, t1).
 * Mean and variance are exact; the normal shape is the right approximation for an
 * aggregate over ~10^4 cells. Reports the in-sample gain and BOTH held-out directions. */
static double gnorm(void){
    static int have=0; static double spare=0;
    if(have){ have=0; return spare; }
    double u,v,ss;
    do{ u=2.0*rand()/RAND_MAX-1.0; v=2.0*rand()/RAND_MAX-1.0; ss=u*u+v*v; }while(ss>=1.0||ss==0.0);
    double f=sqrt(-2.0*log(ss)/ss);
    spare=v*f; have=1; return u*f;
}
static void perm_table(ll *c, long ncells, int draw){
    for(long i=0;i<ncells;i++){
        double a=c[4*i],b=c[4*i+1],g0=c[4*i+2],g1=c[4*i+3];
        double n=a+b+g0+g1; if(n<2) continue;
        double m1=g0+g1, t1=b+g1, hi=(m1<t1?m1:t1), lo=(t1-(n-m1)>0?t1-(n-m1):0);
        if(hi<=lo) continue;
        double mu=m1*t1/n;
        double var=m1*t1*(n-m1)*(n-t1)/(n*n*(n-1));
        double x = draw? mu+sqrt(var)*gnorm() : mu;
        double n11r=floor(x+0.5); if(n11r<lo) n11r=lo; if(n11r>hi) n11r=hi;
        c[4*i]  = n-m1-t1+n11r;
        c[4*i+1]= t1-n11r;
        c[4*i+2]= m1-n11r;
        c[4*i+3]= n11r;
    }
}
static double gain_of(ll *c, long ncells){
    double s1=0,n11=0,n01=0,N=0,ceR=0,ceRP=0;
    for(long i=0;i<ncells;i++){ s1+=c[4*i+2]+c[4*i+3]; n11+=c[4*i+3]; n01+=c[4*i+1]; }
    double pR=(n11+0.5)/(s1+1.0);
    for(long i=0;i<ncells;i++){
        double a=c[4*i],b=c[4*i+1],g0=c[4*i+2],g1=c[4*i+3];
        double n=a+b+g0+g1; if(n<=0) continue; N+=n;
        double qR=(g1+0.5)/(g0+g1+1.0), rR=(b+0.5)/(a+b+1.0);
        ceR -= (b+g1)*log(pR) + (a+g0)*log(1-pR);
        if(g1>0) ceRP-=g1*log(qR); if(g0>0) ceRP-=g0*log(1-qR);
        if(b>0)  ceRP-=b*log(rR);  if(a>0)  ceRP-=a*log(1-rR);
    }
    return (ceR-ceRP)/N;
}
static double hold_gain(ll *fit, ll *test, long ncells){
    double N=0,ceR=0,ceRP=0;
    for(long i=0;i<ncells;i++){
        double a=test[4*i],b=test[4*i+1],g0=test[4*i+2],g1=test[4*i+3];
        double s=a+b+g0+g1; if(s<=0) continue; N+=s;
        double f00=fit[4*i],f01=fit[4*i+1],f10=fit[4*i+2],f11=fit[4*i+3];
        double pr=(f01+f11+0.5)/(f00+f01+f10+f11+1.0);
        double qR=(f11+0.5)/(f10+f11+1.0), rR=(f01+0.5)/(f00+f01+1.0);
        if((f10+f11)==0||(f00+f01)==0){ qR=pr; rR=pr; }
        pr=pr<1e-12?1e-12:(pr>1-1e-12?1-1e-12:pr);
        qR=qR<1e-12?1e-12:(qR>1-1e-12?1-1e-12:qR);
        rR=rR<1e-12?1e-12:(rR>1-1e-12?1-1e-12:rR);
        ceR -= (b+g1)*log(pr) + (a+g0)*log(1-pr);
        if(g1>0) ceRP-=g1*log(qR); if(g0>0) ceRP-=g0*log(1-qR);
        if(b>0)  ceRP-=b*log(rR);  if(a>0)  ceRP-=a*log(1-rR);
    }
    return (ceR-ceRP)/N;
}
static void nullsim_one(ll *F, ll *J, ll *K, long ncells, const char *name, int R){
    double g_obs=hold_gain(F,F,ncells);
    double h_obs=hold_gain(J,K,ncells);          /* fit = hash bit 0 (even), test = odd  */
    double hr_obs=hold_gain(K,J,ncells);         /* fit = odd, test = even               */
    ll *tmp=(ll*)malloc(sizeof(ll)*ncells*4);
    double si=0,si2=0,sh=0,sh2=0,sr=0,sr2=0;
    for(int r=0;r<R;r++){
        memcpy(tmp,F,sizeof(ll)*ncells*4); perm_table(tmp,ncells,1); double gi=hold_gain(tmp,tmp,ncells);
        si+=gi; si2+=gi*gi;
        memcpy(tmp,J,sizeof(ll)*ncells*4); perm_table(tmp,ncells,1); double gh=hold_gain(tmp,K,ncells);
        sh+=gh; sh2+=gh*gh;
        memcpy(tmp,K,sizeof(ll)*ncells*4); perm_table(tmp,ncells,1); double gr=hold_gain(tmp,J,ncells);
        sr+=gr; sr2+=gr*gr;
    }
    free(tmp);
    double mi=si/R, mh=sh/R, mr=sr/R;
    double vi=(si2/R-mi*mi)*R/(R-1), vh=(sh2/R-mh*mh)*R/(R-1), vr=(sr2/R-mr*mr)*R/(R-1);
    printf("  \"%s\": {\"replicates\": %d,\n", name, R);
    printf("    \"in_sample\":  {\"observed\": %.6e, \"null_mean\": %.6e, \"null_sd\": %.6e, \"z\": %.2f},\n",
           g_obs, mi, sqrt(vi), (g_obs-mi)/sqrt(vi));
    printf("    \"heldout_even_to_odd\": {\"observed\": %.6e, \"null_mean\": %.6e, \"null_sd\": %.6e, \"z\": %.2f},\n",
           h_obs, mh, sqrt(vh), (h_obs-mh)/sqrt(vh));
    printf("    \"heldout_odd_to_even\": {\"observed\": %.6e, \"null_mean\": %.6e, \"null_sd\": %.6e, \"z\": %.2f}},\n",
           hr_obs, mr, sqrt(vr), (hr_obs-mr)/sqrt(vr));
}

int main(int argc,char**argv){
    if(argc<2){ fprintf(stderr,"usage: %s census LIMIT | search START COUNT\n",argv[0]); return 2; }

    if(!strcmp(argv[1],"census")){
        u64 LIMIT = argc>2 ? strtoull(argv[2],0,10) : 100000000ULL;
        long lim_small = (long)sqrt((double)LIMIT)+2; build_small(lim_small);
        double t0=now();
        long n; int *pr = sieve_primes(LIMIT,&n);
        double t1=now();
        char *art = (char*)malloc(n); char *qr = (char*)malloc(n);
        #pragma omp parallel for schedule(static)
        for(long i=0;i<n;i++){ qr[i]=(char)leg10((u64)pr[i]); art[i]=(char)artin10((u64)pr[i]); }
        double t2=now();

        ll n_artin=0, n_qrneg=0, n_artin_qrpos=0;
        for(long i=0;i<n;i++){ if(art[i]) n_artin++; if(qr[i]==-1) n_qrneg++; if(art[i]&&qr[i]!=-1) n_artin_qrpos++; }

        static ll c40[40][4], c840[840][4]; ll excl_both=0, excl_tot=0, excl_prev=0;
        ll g20_pairs=0, g20_both=0, g0_pairs=0, g0_both=0, g0_prev=0;
        for(long i=1;i<n;i++){
            ll g = (ll)pr[i]-pr[i-1];
            int ap=art[i-1], an=art[i];
            c40[g%40][(ap<<1)|an]++;
            c840[g%840][(ap<<1)|an]++;
            if(g%40==20){ excl_tot++; if(ap) excl_prev++; if(ap&&an) excl_both++; }
            if(g==20){ g20_pairs++; if(ap&&an) g20_both++; }
            if(g%40==0){ g0_pairs++; if(ap) g0_prev++; if(ap&&an) g0_both++; }
        }
        double t3=now();

        printf("{\n");
        printf("  \"limit\": %llu, \"n_primes\": %ld,\n", LIMIT, n);
        printf("  \"n_artin\": %lld, \"artin_density\": %.8f,\n", n_artin, (double)n_artin/(double)n);
        printf("  \"n_qr_neg\": %lld, \"qr_neg_frac\": %.8f,\n", n_qrneg, (double)n_qrneg/(double)n);
        printf("  \"n_artin_with_qr_pos\": %lld,\n", n_artin_qrpos);
        printf("  \"filter_kept_frac\": %.8f,\n", (double)n_qrneg/(double)n);
        printf("  \"g20_pairs\": %lld, \"g20_both_artin\": %lld,\n", g20_pairs, g20_both);
        printf("  \"gmod40_20_pairs\": %lld, \"gmod40_20_prev_artin\": %lld, \"gmod40_20_both_artin\": %lld,\n",
               excl_tot, excl_prev, excl_both);
        printf("  \"gmod40_0_pairs\": %lld, \"gmod40_0_prev_artin\": %lld, \"gmod40_0_both_artin\": %lld,\n",
               g0_pairs, g0_prev, g0_both);
        printf("  \"c40\": [");
        for(int r=0;r<40;r++){ printf("[%lld,%lld,%lld,%lld]%s",c40[r][0],c40[r][1],c40[r][2],c40[r][3], r<39?",":""); }
        printf("],\n  \"c840\": [");
        for(int r=0;r<840;r++){ printf("[%lld,%lld,%lld,%lld]%s",c840[r][0],c840[r][1],c840[r][2],c840[r][3], r<839?",":""); }
        printf("],\n");
        printf("  \"t_sieve\": %.3f, \"t_status\": %.3f, \"t_agg\": %.3f, \"t_total\": %.3f\n",
               t1-t0, t2-t1, t3-t2, t3-t0);
        printf("}\n");
        return 0;
    }

    if(!strcmp(argv[1],"search")){
        u64 START = argc>2 ? strtoull(argv[2],0,10) : 1000000000000ULL;
        long COUNT = argc>3 ? atol(argv[3]) : 500;
        long lim_small = (long)sqrt((double)(START+200000000ULL))+2; build_small(lim_small);

        /* naive: primality-test every odd number, factor every prime's p-1 */
        double t0=now(); ll f_naive=0; long found=0; u64 p;
        for(p=(START|1ULL); found<COUNT; p+=2){
            if(!is_prime64(p)) continue;
            f_naive++; if(artin10(p)) found++;
        }
        double t1=now();

        /* filtered: test the free necessary condition (10|p) = -1 first */
        double t2=now(); ll f_filt=0, skipped=0; found=0;
        for(p=(START|1ULL); found<COUNT; p+=2){
            if(!is_prime64(p)) continue;
            if(leg10(p)!=-1){ skipped++; continue; }
            f_filt++; if(artin10(p)) found++;
        }
        double t3=now();

        printf("{\n  \"mode\": \"search\", \"start\": %llu, \"count\": %ld,\n", START, COUNT);
        printf("  \"naive_seconds\": %.4f, \"naive_factorizations\": %lld,\n", t1-t0, f_naive);
        printf("  \"filtered_seconds\": %.4f, \"filtered_factorizations\": %lld, \"skipped_by_qr\": %lld,\n", t3-t2, f_filt, skipped);
        printf("  \"speedup\": %.3f, \"factorization_reduction\": %.3f\n", (t1-t0)/(t3-t2), (double)f_naive/(double)f_filt);
        printf("}\n");
        return 0;
    }
    if(!strcmp(argv[1],"ce")){
        u64 LIMIT = argc>2 ? strtoull(argv[2],0,10) : 1000000000ULL;
        long lim_small = (long)sqrt((double)LIMIT)+2; build_small(lim_small);
        long n; int *pr = sieve_primes(LIMIT,&n);
        char *art = (char*)malloc(n);
        #pragma omp parallel for schedule(static)
        for(long i=0;i<n;i++) art[i]=(char)artin10((u64)pr[i]);
        for(long i=1;i<n;i++){
            ll a=(ll)pr[i-1]%M1, b=(ll)pr[i]%M1;
            J1[a*M1+b][(art[i-1]<<1)|art[i]]++;
            ll c=(ll)pr[i-1]%M2, d=(ll)pr[i]%M2;
            J2[c*M2+d][(art[i-1]<<1)|art[i]]++;
        }
        printf("{\n  \"limit\": %llu, \"n_primes\": %ld,\n", LIMIT, n);
        ce_report(&J1[0][0], (long)M1*M1, "joint_residues_mod120");
        ce_report(&J2[0][0], (long)M2*M2, "joint_residues_mod840");
        printf("  \"note\": \"gain_prevA_beyond_residues = the correlation's OWN predictive edge after the elementary joint-residue baseline\"\n}\n");
        return 0;
    }

    if(!strcmp(argv[1],"search3")){
        u64 START = argc>2 ? strtoull(argv[2],0,10) : 1000000000000ULL;
        long COUNT = argc>3 ? atol(argv[3]) : 300;
        long lim_small=(long)sqrt((double)(START+200000000ULL))+2; build_small(lim_small);
        static const u64 smalll[7]={3,5,7,11,13,17,19};
        ll fA=0,nA=0,fB=0,nB=0,fC=0,nC=0,mC=0; long found; double t0,t1,t2,t3,t4,t5;

        /* A: naive -- factor every prime's p-1, test all ell */
        t0=now(); found=0;
        for(u64 p=(START|1ULL); found<COUNT; p+=2){
            if(!is_prime64(p)) continue; nA++; fA++; if(artin10(p)) found++;
        }
        t1=now();
        /* B: Legendre-first (free by reciprocity), then factor */
        t2=now(); found=0;
        for(u64 p=(START|1ULL); found<COUNT; p+=2){
            if(!is_prime64(p)) continue; nB++;
            if(leg10_fast(p)!=-1) continue;
            fB++; if(artin10(p)) found++;
        }
        t3=now();
        /* C: early-exit on the small-ell channels of p-1, THEN factor survivors */
        t4=now(); found=0;
        for(u64 p=(START|1ULL); found<COUNT; p+=2){
            if(!is_prime64(p)) continue; nC++;
            if(leg10_fast(p)!=-1) continue;
            int rej=0;
            for(int i=0;i<7;i++){ u64 l=smalll[i]; if((p-1)%l==0){ mC++; if(pw(10,(p-1)/l,p)==1){ rej=1; break; } } }
            if(rej) continue;
            fC++; if(artin10(p)) found++;
        }
        t5=now();
        printf("{\n  \"mode\": \"search3\", \"start\": %llu, \"count\": %ld,\n", START, COUNT);
        printf("  \"A_naive\":    {\"sec\": %.4f, \"primes\": %lld, \"factorizations\": %lld},\n", t1-t0, nA, fA);
        printf("  \"B_legendre\": {\"sec\": %.4f, \"primes\": %lld, \"factorizations\": %lld},\n", t3-t2, nB, fB);
        printf("  \"C_earlyexit\":{\"sec\": %.4f, \"primes\": %lld, \"factorizations\": %lld, \"modexps_small_ell\": %lld},\n", t5-t4, nC, fC, mC);
        printf("  \"speedup_A_over_B\": %.3f, \"speedup_A_over_C\": %.3f, \"speedup_B_over_C\": %.3f,\n", (t1-t0)/(t3-t2), (t1-t0)/(t5-t4), (t3-t2)/(t5-t4));
        printf("  \"factorizations_saved_vs_B\": %.4f\n}\n", 1.0-(double)fC/(double)fB);
        return 0;
    }

    if(!strcmp(argv[1],"cehold")){
        u64 LIMIT = argc>2 ? strtoull(argv[2],0,10) : 1000000000ULL;
        long lim_small = (long)sqrt((double)LIMIT)+2; build_small(lim_small);
        long n; int *pr = sieve_primes(LIMIT,&n);
        char *art = (char*)malloc(n);
        #pragma omp parallel for schedule(static)
        for(long i=0;i<n;i++) art[i]=(char)artin10((u64)pr[i]);
        u64 SPLIT = LIMIT/2;
        for(long i=1;i<n;i++){
            /* split by a hash of the successor prime: both halves then cover the whole
             * range, so they are exchangeable. A range split is NOT valid here: the
             * Artin density drifts with x, which penalises the finer model spuriously. */
            int t = (int)(mix64((u64)pr[i]) & 1ULL);
            ll a=(ll)pr[i-1]%M1, b=(ll)pr[i]%M1;
            (t?K1:J1)[a*M1+b][(art[i-1]<<1)|art[i]]++;
            ll c=(ll)pr[i-1]%M2, d=(ll)pr[i]%M2;
            (t?K2:J2)[c*M2+d][(art[i-1]<<1)|art[i]]++;
        }
        printf("{\n  \"limit\": %llu, \"split\": \"hash64(successor) & 1\", \"n_primes\": %ld,\n", LIMIT, n);
        hold_report(&J1[0][0], &K1[0][0], (long)M1*M1, "holdout_mod120");
        hold_report(&J2[0][0], &K2[0][0], (long)M2*M2, "holdout_mod840");
        printf("  \"smoothing\": \"Jeffreys 0.5\", \"note\": \"fit on one hash half, scored on the other; heldout_gain = CE_residues - CE_residues_plus_prevA\"\n}\n");
        return 0;
    }

    if(!strcmp(argv[1],"decomp")){
        u64 LIMIT = argc>2 ? strtoull(argv[2],0,10) : 1000000000ULL;
        long lim_small = (long)sqrt((double)LIMIT)+2; build_small(lim_small);
        long n; int *pr = sieve_primes(LIMIT,&n);
        char *art = (char*)malloc(n);
        #pragma omp parallel for schedule(static)
        for(long i=0;i<n;i++) art[i]=(char)artin10((u64)pr[i]);
        for(long i=1;i<n;i++){
            ll a=(ll)pr[i-1]%M1, b=(ll)pr[i]%M1;
            J1[a*M1+b][(art[i-1]<<1)|art[i]]++;
            ll c=(ll)pr[i-1]%M2, d=(ll)pr[i]%M2;
            J2[c*M2+d][(art[i-1]<<1)|art[i]]++;
        }
        printf("{\n  \"limit\": %llu, \"n_primes\": %ld,\n", LIMIT, n);
        decomp_report(&J1[0][0], (long)M1*M1, "decomp_mod120");
        decomp_report(&J2[0][0], (long)M2*M2, "decomp_mod840");
        printf("  \"note\": \"delta = within + between; between_share is the fraction of delta attributable to cell composition rather than within-cell association\"\n}\n");
        return 0;
    }

    if(!strcmp(argv[1],"cepool")){
        u64 LIMIT = argc>2 ? strtoull(argv[2],0,10) : 1000000000ULL;
        long lim_small = (long)sqrt((double)LIMIT)+2; build_small(lim_small);
        long n; int *pr = sieve_primes(LIMIT,&n);
        char *art = (char*)malloc(n);
        #pragma omp parallel for schedule(static)
        for(long i=0;i<n;i++) art[i]=(char)artin10((u64)pr[i]);
        for(long i=1;i<n;i++){
            int t=(int)(mix64((u64)pr[i])&1ULL);
            ll a=(ll)pr[i-1]%M1, b=(ll)pr[i]%M1;
            (t?K1:J1)[a*M1+b][(art[i-1]<<1)|art[i]]++;
            ll c=(ll)pr[i-1]%M2, d=(ll)pr[i]%M2;
            (t?K2:J2)[c*M2+d][(art[i-1]<<1)|art[i]]++;
        }
        printf("{\n  \"limit\": %llu, \"n_primes\": %ld, \"split\": \"hash64(successor) & 1\",\n", LIMIT, n);
        pool_report(&J1[0][0], &K1[0][0], (long)M1*M1, "pooled_mod120");
        pool_report(&J2[0][0], &K2[0][0], (long)M2*M2, "pooled_mod840");
        printf("  \"note\": \"one pooled predecessor logit offset beta on top of the residue cells; alpha_cell profiled out by Newton\"\n}\n");
        return 0;
    }

    if(!strcmp(argv[1],"all")){
        /* one sieve, every statistic: the paper's §15 numbers in a single pass */
        u64 LIMIT = argc>2 ? strtoull(argv[2],0,10) : 1000000000ULL;
        int R = argc>3 ? atoi(argv[3]) : 200;
        long lim_small = (long)sqrt((double)LIMIT)+2; build_small(lim_small);
        long n; int *pr = sieve_primes(LIMIT,&n);
        char *art = (char*)malloc(n);
        #pragma omp parallel for schedule(static)
        for(long i=0;i<n;i++) art[i]=(char)artin10((u64)pr[i]);
        static ll F1[M1*M1][4], F2[M2*M2][4];
        for(long i=1;i<n;i++){
            int t=(int)(mix64((u64)pr[i])&1ULL);
            ll a=(ll)pr[i-1]%M1, b=(ll)pr[i]%M1;
            (t?K1:J1)[a*M1+b][(art[i-1]<<1)|art[i]]++;
            ll c=(ll)pr[i-1]%M2, d=(ll)pr[i]%M2;
            (t?K2:J2)[c*M2+d][(art[i-1]<<1)|art[i]]++;
            F1[a*M1+b][(art[i-1]<<1)|art[i]]++;
            F2[c*M2+d][(art[i-1]<<1)|art[i]]++;
        }
        srand(20260923);
        printf("{\n  \"mode\": \"all\", \"limit\": %llu, \"n_primes\": %ld,\n", LIMIT, n);
        decomp_report(&F1[0][0], (long)M1*M1, "decomp_mod120");
        decomp_report(&F2[0][0], (long)M2*M2, "decomp_mod840");
        hold_report(&J1[0][0], &K1[0][0], (long)M1*M1, "holdout_mod120_even_to_odd");
        hold_report(&J2[0][0], &K2[0][0], (long)M2*M2, "holdout_mod840_even_to_odd");
        hold_report(&K1[0][0], &J1[0][0], (long)M1*M1, "holdout_mod120_odd_to_even");
        hold_report(&K2[0][0], &J2[0][0], (long)M2*M2, "holdout_mod840_odd_to_even");
        pool_report(&J1[0][0], &K1[0][0], (long)M1*M1, "pooled_mod120");
        pool_report(&J2[0][0], &K2[0][0], (long)M2*M2, "pooled_mod840");
        nullsim_one(&F1[0][0], &J1[0][0], &K1[0][0], (long)M1*M1, "nullsim_mod120", R);
        nullsim_one(&F2[0][0], &J2[0][0], &K2[0][0], (long)M2*M2, "nullsim_mod840", R);
        printf("  \"split\": \"hash64(successor) & 1 : 0 = fit (even), 1 = test (odd)\",\n");
        printf("  \"note\": \"decomp identities asserted; held-out direction given in each key name\"\n}\n");
        return 0;
    }

    if(!strcmp(argv[1],"nullsim")){
        u64 LIMIT = argc>2 ? strtoull(argv[2],0,10) : 1000000000ULL;
        int R = argc>3 ? atoi(argv[3]) : 200;
        long lim_small = (long)sqrt((double)LIMIT)+2; build_small(lim_small);
        long n; int *pr = sieve_primes(LIMIT,&n);
        char *art = (char*)malloc(n);
        #pragma omp parallel for schedule(static)
        for(long i=0;i<n;i++) art[i]=(char)artin10((u64)pr[i]);
        for(long i=1;i<n;i++){
            int t=(int)(mix64((u64)pr[i])&1ULL);
            ll a=(ll)pr[i-1]%M1, b=(ll)pr[i]%M1;
            (t?K1:J1)[a*M1+b][(art[i-1]<<1)|art[i]]++;
            ll c=(ll)pr[i-1]%M2, d=(ll)pr[i]%M2;
            (t?K2:J2)[c*M2+d][(art[i-1]<<1)|art[i]]++;
        }
        srand(20260923);
        printf("{\n  \"limit\": %llu, \"n_primes\": %ld, \"split\": \"hash64(successor) & 1 (0 = fit, 1 = test)\",\n", LIMIT, n);
        ll *F1=(ll*)malloc(sizeof(ll)*M1*M1*4), *F2=(ll*)malloc(sizeof(ll)*M2*M2*4);
        for(long i=0;i<(long)M1*M1*4;i++) F1[i]=(&J1[0][0])[i]+(&K1[0][0])[i];
        for(long i=0;i<(long)M2*M2*4;i++) F2[i]=(&J2[0][0])[i]+(&K2[0][0])[i];
        nullsim_one(F1,&J1[0][0],&K1[0][0],(long)M1*M1,"nullsim_mod120",R);
        nullsim_one(F2,&J2[0][0],&K2[0][0],(long)M2*M2,"nullsim_mod840",R);
        printf("  \"note\": \"margins fixed per cell; n11 drawn hypergeometric (normal shape); held-out direction fit->test\"\n}\n");
        return 0;
    }

    fprintf(stderr,"unknown mode\n"); return 2;
}
