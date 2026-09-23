/*
 * crossbase_fine.c — cross-base Artin correlation with FINE conditioning.
 *
 * Paper #3 follow-up. At 1e9, conditioning on omega(p-1) removed only 55% of
 * the same-prime cross-base correlation. Question: is the residual real, or is
 * omega just a crude proxy for the actual confounder?
 *
 * The true Kummer condition depends on WHICH small primes divide p-1 (and on
 * v_2(p-1)), not merely on how many. So we condition on the "signature"
 *
 *     sig(p) = ( min(v_2(p-1),4),  bitmask of which of 3,5,7,11,13 divide p-1,
 *                min(omega_large, 3) )
 *
 * where omega_large counts prime factors of p-1 above 13. This is a much finer
 * partition than omega alone (5*32*4 = 640 cells) and is exactly the data the
 * Artin conditions actually see for small bases.
 *
 * If the correlation survives THIS, it is not a p-1 structure artifact.
 *
 * gcc -O3 -o crossbase_fine crossbase_fine.c -lm
 * ./crossbase_fine <limit> <out.json>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEG_SIZE  (1 << 22)
#define SMALL_LIM 100000

static const int BASES[] = {2, 3, 5, 6, 7, 10, 11, 13, 15, 17, 21, 29};
#define NB (int)(sizeof(BASES)/sizeof(BASES[0]))

/* signature dimensions */
#define NV2   5      /* min(v2,4)            */
#define NMASK 32     /* subset of {3,5,7,11,13} */
#define NLRG  4      /* min(#large factors,3)   */
#define NSIG  (NV2 * NMASK * NLRG)

static char small_composite[SMALL_LIM + 1];
static long small_primes[10000];
static int  n_small = 0;

static void build_small(void) {
    for (long i = 2; i <= SMALL_LIM; i++)
        if (!small_composite[i]) {
            small_primes[n_small++] = i;
            for (long j = i*i; j <= SMALL_LIM; j += i) small_composite[j] = 1;
        }
}
static long powmod(long b, long e, long m) {
    long r = 1; b %= m; if (b < 0) b += m;
    while (e > 0) {
        if (e & 1) r = (long)(((__int128)r * b) % m);
        b = (long)(((__int128)b * b) % m);
        e >>= 1;
    }
    return r;
}

static long pairtab[NB][NB][2][2];
static long ompair[NB][NB][12][2][2];               /* by omega(p-1)   */
static long sigpair[NB][NB][NSIG][2][2];            /* by fine signature */
static long marg[NB];
static long n_primes = 0;

int main(int argc, char **argv) {
    long LIMIT = (argc > 1) ? atol(argv[1]) : 200000000L;
    const char *out = (argc > 2) ? argv[2] : "crossbase_fine.json";
    build_small();

    char *seg = malloc(SEG_SIZE);
    for (long lo = 2; lo <= LIMIT; lo += SEG_SIZE) {
        long hi = lo + SEG_SIZE - 1; if (hi > LIMIT) hi = LIMIT;
        memset(seg, 0, SEG_SIZE);
        for (int i = 0; i < n_small; i++) {
            long q = small_primes[i]; if (q*q > hi) break;
            long st = (lo + q - 1)/q*q; if (st < q*q) st = q*q;
            for (long j = st; j <= hi; j += q) seg[j-lo] = 1;
        }
        for (long n = lo; n <= hi; n++) {
            if (seg[n-lo]) continue;
            long p = n; if (p < 5) continue;

            long pm1 = p-1, tmp = pm1, fac[64]; int nf = 0;
            for (int i = 0; i < n_small; i++) {
                long dd = small_primes[i]; if (dd*dd > tmp) break;
                if (tmp % dd == 0) { fac[nf++] = dd; while (tmp % dd == 0) tmp /= dd; }
            }
            if (tmp > 1) fac[nf++] = tmp;

            /* signature */
            int v2 = 0; { long t = pm1; while ((t & 1) == 0) { v2++; t >>= 1; } }
            if (v2 > 4) v2 = 4;
            int mask = 0, nlarge = 0;
            static const int SMALLSET[5] = {3, 5, 7, 11, 13};
            for (int i = 0; i < nf; i++) {
                int isSmall = 0;
                for (int k = 0; k < 5; k++)
                    if (fac[i] == SMALLSET[k]) { mask |= (1 << k); isSmall = 1; break; }
                if (!isSmall && fac[i] != 2) nlarge++;
            }
            if (nlarge > 3) nlarge = 3;
            int sig = (v2 * NMASK + mask) * NLRG + nlarge;

            int om = nf; if (om > 11) om = 11;

            int art[NB];
            for (int b = 0; b < NB; b++) {
                long a = BASES[b];
                if (a % p == 0) { art[b] = 0; continue; }
                int ok = 1;
                for (int i = 0; i < nf && ok; i++)
                    if (powmod(a, pm1/fac[i], p) == 1) ok = 0;
                art[b] = ok;
            }
            n_primes++;
            for (int i = 0; i < NB; i++) {
                marg[i] += art[i];
                for (int j = i+1; j < NB; j++) {
                    pairtab[i][j][art[i]][art[j]]++;
                    ompair[i][j][om][art[i]][art[j]]++;
                    sigpair[i][j][sig][art[i]][art[j]]++;
                }
            }
            if (n_primes % 2000000 == 0) { fprintf(stderr, "  p=%ld n=%ld\n", p, n_primes); fflush(stderr); }
        }
    }

    FILE *f = fopen(out, "w");
    fprintf(f, "{\n  \"limit\": %ld,\n  \"n_primes\": %ld,\n  \"bases\": [", LIMIT, n_primes);
    for (int i = 0; i < NB; i++) fprintf(f, "%d%s", BASES[i], i==NB-1?"":", ");
    fprintf(f, "],\n  \"marginals\": {");
    for (int i = 0; i < NB; i++) fprintf(f, "\"%d\": %ld%s", BASES[i], marg[i], i==NB-1?"":", ");
    fprintf(f, "},\n  \"pairs\": {\n");
    int first = 1;
    for (int i = 0; i < NB; i++) for (int j = i+1; j < NB; j++) {
        if (!first) fprintf(f, ",\n");
        fprintf(f, "    \"%d,%d\": [[%ld,%ld],[%ld,%ld]]", BASES[i], BASES[j],
                pairtab[i][j][0][0], pairtab[i][j][0][1],
                pairtab[i][j][1][0], pairtab[i][j][1][1]);
        first = 0;
    }
    fprintf(f, "\n  },\n  \"omega\": {\n");
    first = 1;
    for (int i = 0; i < NB; i++) for (int j = i+1; j < NB; j++) {
        if (!first) fprintf(f, ",\n");
        fprintf(f, "    \"%d,%d\": {", BASES[i], BASES[j]);
        int f2 = 1;
        for (int om = 0; om < 12; om++) {
            long t = ompair[i][j][om][0][0]+ompair[i][j][om][0][1]
                   + ompair[i][j][om][1][0]+ompair[i][j][om][1][1];
            if (!t) continue;
            if (!f2) fprintf(f, ", ");
            fprintf(f, "\"%d\": [[%ld,%ld],[%ld,%ld]]", om,
                ompair[i][j][om][0][0], ompair[i][j][om][0][1],
                ompair[i][j][om][1][0], ompair[i][j][om][1][1]);
            f2 = 0;
        }
        fprintf(f, "}");
        first = 0;
    }
    fprintf(f, "\n  },\n  \"sig\": {\n");
    first = 1;
    for (int i = 0; i < NB; i++) for (int j = i+1; j < NB; j++) {
        if (!first) fprintf(f, ",\n");
        fprintf(f, "    \"%d,%d\": {", BASES[i], BASES[j]);
        int f2 = 1;
        for (int s = 0; s < NSIG; s++) {
            long t = sigpair[i][j][s][0][0]+sigpair[i][j][s][0][1]
                   + sigpair[i][j][s][1][0]+sigpair[i][j][s][1][1];
            if (!t) continue;
            if (!f2) fprintf(f, ", ");
            fprintf(f, "\"%d\": [[%ld,%ld],[%ld,%ld]]", s,
                sigpair[i][j][s][0][0], sigpair[i][j][s][0][1],
                sigpair[i][j][s][1][0], sigpair[i][j][s][1][1]);
            f2 = 0;
        }
        fprintf(f, "}");
        first = 0;
    }
    fprintf(f, "\n  }\n}\n");
    fclose(f);
    fprintf(stderr, "DONE n_primes=%ld -> %s\n", n_primes, out);
    return 0;
}
