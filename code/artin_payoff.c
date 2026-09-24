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

/* ================= within/between decompositions (asserted) =================
 * X = predecessor Artin, Y = successor Artin, C = joint-residue cell; counts [n00,n01,n10,n11],
 * first index = X. q1_c = P(Y=1|X=1,C=c), q0_c = P(Y=1|X=0,C=c), w1_c = P(C=c|X=1),
 * w0_c = P(C=c|X=0), p_c = P(Y=1|C=c), pi_c = P(C=c), r_c = P(X=1|C=c).
 *
 * X-CONSTANT CELLS (only one predecessor status occurs) have no identifiable within-cell
 * association. Convention used throughout: they contribute ZERO to every within term. This is
 * implemented by completing the missing conditional with the observed one (q0:=q1 where no X=0
 * pair occurs, q1:=q0 where no X=1 pair occurs), so dc_c = q1_c - q0_c = 0 there.
 * With any completion the Kitagawa identities are exact, because the completed terms cancel.
 *
 *   covariance : delta = sum pi r(1-r) dc / Var(X)   + sum (w1-w0) p
 *   Kitagawa w1: delta = sum w1 dc                    + sum (w1-w0) q0
 *   Kitagawa w0: delta = sum w0 dc                    + sum (w1-w0) q1
 *   symmetric  : delta = sum (w1+w0)/2 dc             + sum (w1-w0) (q0+q1)/2
 *
 * EVERY within and between sum is accumulated independently from the cell table and each of the
 * four identities is asserted to 1e-12; any failure exits non-zero.
 * k = number of cells in which BOTH X and Y vary (the only cells where splitting on X adds an
 * identifiable response parameter). Reported for the record; not used in the paper's argument. */
static int decomp_report(ll *c, long ncells, const char *name){
    double n00=0,n01=0,n10=0,n11=0;
    for(long i=0;i<ncells;i++){ n00+=c[4*i]; n01+=c[4*i+1]; n10+=c[4*i+2]; n11+=c[4*i+3]; }
    double N=n00+n01+n10+n11, s1=n10+n11, s0=n00+n01;
    double delta=n11/s1 - n01/s0, varX=(s1/N)*(s0/N);
    double CW=0,CB=0, W1=0,B1=0, W0=0,B0=0, WS=0,BS=0, xconst=0, x0only=0, x1only=0, k=0;
    for(long i=0;i<ncells;i++){
        double a=c[4*i], b=c[4*i+1], g0=c[4*i+2], g1=c[4*i+3];
        double n=a+b+g0+g1; if(n<=0) continue;
        double m0=a+b, m1=g0+g1, t1=b+g1;
        double pi=n/N, r=m1/n, p=t1/n, w1=m1/s1, w0=m0/s0;
        double q0 = (m0>0)? b/m0 : g1/m1;      /* completion: q0:=q1 if no X=0 pair */
        double q1 = (m1>0)? g1/m1 : b/m0;      /* completion: q1:=q0 if no X=1 pair */
        double dc = q1-q0;                      /* exactly 0 in X-constant cells */
        if(m0==0||m1==0){ xconst+=pi; if(m1==0) x0only+=pi; else x1only+=pi; }
        if(m0>0 && m1>0 && t1>0 && (n-t1)>0) k+=1;
        CW += pi*r*(1-r)*dc/varX;   CB += (w1-w0)*p;
        W1 += w1*dc;                B1 += (w1-w0)*q0;
        W0 += w0*dc;                B0 += (w1-w0)*q1;
        WS += 0.5*(w1+w0)*dc;       BS += (w1-w0)*0.5*(q0+q1);
    }
    double e[4]={CW+CB-delta, W1+B1-delta, W0+B0-delta, WS+BS-delta};
    int ok=1; for(int j=0;j<4;j++) if(!(fabs(e[j])<1e-12)) ok=0;
    double sh[4]={CB/delta, B1/delta, B0/delta, BS/delta};
    double mn=sh[0],mx=sh[0]; for(int j=1;j<4;j++){ if(sh[j]<mn) mn=sh[j]; if(sh[j]>mx) mx=sh[j]; }
    printf("  \"%s\": {\"N\": %.0f, \"delta\": %.9f,\n", name, N, delta);
    printf("    \"covariance\":  {\"within\": %.9f, \"between\": %.9f, \"between_share\": %.6f, \"identity_error\": %.1e},\n", CW,CB,sh[0],e[0]);
    printf("    \"kitagawa_w1\": {\"within\": %.9f, \"between\": %.9f, \"between_share\": %.6f, \"identity_error\": %.1e},\n", W1,B1,sh[1],e[1]);
    printf("    \"kitagawa_w0\": {\"within\": %.9f, \"between\": %.9f, \"between_share\": %.6f, \"identity_error\": %.1e},\n", W0,B0,sh[2],e[2]);
    printf("    \"symmetric\":   {\"within\": %.9f, \"between\": %.9f, \"between_share\": %.6f, \"identity_error\": %.1e},\n", WS,BS,sh[3],e[3]);
    printf("    \"share_min\": %.6f, \"share_max\": %.6f,\n", mn, mx);
    printf("    \"x_constant_mass\": %.6f, \"x0_only_mass\": %.6f, \"x1_only_mass\": %.3e, \"k_both_X_and_Y_vary\": %.0f,\n", xconst,x0only,x1only,k);
    printf("    \"ASSERT\": \"%s\"},\n", ok? "all four identities hold" : "IDENTITY FAILURE");
    if(!ok){ printf("  \"ASSERT_FAIL_%s\": true\n}\n", name); exit(1); }
    return ok;
}

/* ================= in-sample MLE increment and the residue-status null =================
 * mle_inc: I(Y; X | C) in nats per pair, unsmoothed, from a table of [n00,n01,n10,n11]
 * (first index X). This is exactly the "in-sample increment" of the conditional-entropy table.
 * Residue-status null: every prime's Artin status is redrawn independently with probability
 * equal to the observed Artin rate of its residue class mod Mnull. Statuses are drawn per PRIME,
 * so each pair (X_n, Y_n) = (status of p_n, status of p_{n+1}) keeps the sequence structure
 * (Y_n = X_{n+1}). Under this null the predecessor's status carries no information beyond the
 * residue classes mod Mnull. */
static double xlogx(double x){ return x>0? x*log(x) : 0.0; }
static double mle_inc(ll *c, long ncells){
    double N=0, I=0;
    for(long i=0;i<ncells;i++){
        double a=c[4*i], b=c[4*i+1], g0=c[4*i+2], g1=c[4*i+3], n=a+b+g0+g1; if(n<=0) continue; N+=n;
        double m0=a+b, m1=g0+g1, t0=a+g0, t1=b+g1;
        /* n*H(Y|c) - sum_x n_x H(Y|c,x) = sum n_xy log n_xy - sum n_x log n_x - sum n_y log n_y + n log n */
        I += xlogx(a)+xlogx(b)+xlogx(g0)+xlogx(g1) - xlogx(m0)-xlogx(m1) - xlogx(t0)-xlogx(t1) + xlogx(n);
    }
    return I/N;
}
static inline u64 smix(u64 x){ x+=0x9E3779B97F4A7C15ULL; x=(x^(x>>30))*0xBF58476D1CE4E5B9ULL; x=(x^(x>>27))*0x94D049BB133111EBULL; return x^(x>>31); }

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

    if(!strcmp(argv[1],"dump")){
        /* Write the complete joint-residue tables (mod 120 and mod 840) for both hash halves.
         * Every Section-15 statistic is computed from this file by section15.py.
         * line: modulus half a b n00 n01 n10 n11   (a = p_{n} mod M, b = p_{n+1} mod M,
         * first index of nXY = predecessor Artin status X, second = successor status Y,
         * half = hash64(successor) & 1). */
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
        printf("# limit %llu n_primes %ld pairs %ld\n", LIMIT, n, n-1);
        for(int h=0;h<2;h++) for(long i=0;i<(long)M1*M1;i++){ ll *q=(h?K1:J1)[i];
            if(q[0]+q[1]+q[2]+q[3]) printf("120 %d %ld %ld %lld %lld %lld %lld\n",h,i/M1,i%M1,q[0],q[1],q[2],q[3]); }
        for(int h=0;h<2;h++) for(long i=0;i<(long)M2*M2;i++){ ll *q=(h?K2:J2)[i];
            if(q[0]+q[1]+q[2]+q[3]) printf("840 %d %ld %ld %lld %lld %lld %lld\n",h,i/M2,i%M2,q[0],q[1],q[2],q[3]); }
        return 0;
    }

    if(!strcmp(argv[1],"resnull")){
        u64 LIMIT = argc>2 ? strtoull(argv[2],0,10) : 1000000000ULL;
        int R = argc>3 ? atoi(argv[3]) : 100;
        long lim_small = (long)sqrt((double)LIMIT)+2; build_small(lim_small);
        long n; int *pr = sieve_primes(LIMIT,&n);
        char *art = (char*)malloc(n), *sim=(char*)malloc(n);
        #pragma omp parallel for schedule(static)
        for(long i=0;i<n;i++) art[i]=(char)artin10((u64)pr[i]);
        static ll T1[M1*M1][4], T2[M2*M2][4];
        /* observed */
        memset(T1,0,sizeof T1); memset(T2,0,sizeof T2);
        for(long i=1;i<n;i++){ int k=(art[i-1]<<1)|art[i];
            T1[(pr[i-1]%M1)*M1+pr[i]%M1][k]++; T2[(pr[i-1]%M2)*M2+pr[i]%M2][k]++; }
        double o120=mle_inc(&T1[0][0],(long)M1*M1), o840=mle_inc(&T2[0][0],(long)M2*M2);
        /* Artin rate by p mod 7 (mechanism check: does the rate depend on whether 7 | p-1?) */
        double c7[7]={0}, a7[7]={0};
        for(long i=0;i<n;i++){ c7[pr[i]%7]+=1; a7[pr[i]%7]+=art[i]; }
        printf("{\n  \"limit\": %llu, \"n_primes\": %ld, \"pairs\": %ld, \"replicates\": %d,\n", LIMIT, n, n-1, R);
        printf("  \"observed_mle_increment\": {\"mod120\": %.6e, \"mod840\": %.6e},\n", o120, o840);
        printf("  \"artin_rate_by_p_mod7\": [");
        for(int r=0;r<7;r++) printf("%s%.6f", r?", ":"", c7[r]>0? a7[r]/c7[r] : 0.0);
        printf("],\n");
        int Ms[2]={120,840};
        for(int mi=0;mi<2;mi++){
            int M=Ms[mi]; double *rate=(double*)calloc(M,sizeof(double)), *cnt=(double*)calloc(M,sizeof(double));
            for(long i=0;i<n;i++){ cnt[pr[i]%M]+=1; rate[pr[i]%M]+=art[i]; }
            for(int r=0;r<M;r++) rate[r]= cnt[r]>0? rate[r]/cnt[r] : 0.0;
            double s1=0,s2=0,t1=0,t2=0; int ge1=0, ge2=0;
            for(int rep=0;rep<R;rep++){
                u64 seed=0xA5A5A5A5ULL*(u64)(mi+1) + 0x1000003ULL*(u64)(rep+1);
                #pragma omp parallel for schedule(static)
                for(long i=0;i<n;i++){ double u=(smix(seed ^ (u64)i)>>11)*(1.0/9007199254740992.0);
                    sim[i]=(char)(u<rate[pr[i]%M]); }
                memset(T1,0,sizeof T1); memset(T2,0,sizeof T2);
                for(long i=1;i<n;i++){ int k=(sim[i-1]<<1)|sim[i];
                    T1[(pr[i-1]%M1)*M1+pr[i]%M1][k]++; T2[(pr[i-1]%M2)*M2+pr[i]%M2][k]++; }
                double g1=mle_inc(&T1[0][0],(long)M1*M1), g2=mle_inc(&T2[0][0],(long)M2*M2);
                s1+=g1; s2+=g1*g1; t1+=g2; t2+=g2*g2; if(g1>=o120) ge1++; if(g2>=o840) ge2++;
            }
            double m1=s1/R, sd1=sqrt((s2/R-m1*m1)*R/(R-1)), m2=t1/R, sd2=sqrt((t2/R-m2*m2)*R/(R-1));
            printf("  \"null_rates_mod%d\": {\"mod120_increment\": {\"null_mean\": %.6e, \"null_sd\": %.6e, \"n_null_ge_observed\": %d, \"standardized\": %.2f},\n", M, m1, sd1, ge1, (o120-m1)/sd1);
            printf("                        \"mod840_increment\": {\"null_mean\": %.6e, \"null_sd\": %.6e, \"n_null_ge_observed\": %d, \"standardized\": %.2f}}%s\n", m2, sd2, ge2, (o840-m2)/sd2, mi==0? ",":"");
            free(rate); free(cnt);
        }
        printf("}\n");
        return 0;
    }

    fprintf(stderr,"unknown mode\n"); return 2;
}
