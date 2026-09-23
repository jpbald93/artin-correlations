/*
 * crossbase.c — SAME-prime cross-base Artin correlation.
 *
 * Question (paper #3): is  p Artin base a  correlated with  p Artin base b,
 * for the SAME prime p?  Independent Kummer conditions would suggest near
 * independence; entanglement of Q(sqrt(a)), Q(sqrt(b)) suggests otherwise.
 *
 * For each prime p we factor p-1 once and record Artin status for all bases,
 * then accumulate a 2x2 table for every unordered pair (a,b).
 *
 * gcc -O3 -o crossbase crossbase.c -lm
 * ./crossbase <limit> <out.json>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEG_SIZE  (1 << 22)
#define SMALL_LIM 100000

static const int BASES[] = {2, 3, 5, 6, 7, 10, 11, 13, 15, 17, 21, 29};
#define NB (int)(sizeof(BASES)/sizeof(BASES[0]))

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

static long pairtab[NB][NB][2][2];   /* [a][b][art_a][art_b] */
static long marg[NB];
static long n_primes = 0;

int main(int argc, char **argv) {
    long LIMIT = (argc > 1) ? atol(argv[1]) : 100000000L;
    const char *out = (argc > 2) ? argv[2] : "crossbase.json";
    build_small();
    memset(pairtab, 0, sizeof pairtab);
    memset(marg, 0, sizeof marg);

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
                long d = small_primes[i]; if (d*d > tmp) break;
                if (tmp % d == 0) { fac[nf++] = d; while (tmp % d == 0) tmp /= d; }
            }
            if (tmp > 1) fac[nf++] = tmp;
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
                for (int j = i+1; j < NB; j++) pairtab[i][j][art[i]][art[j]]++;
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
    fprintf(f, "\n  }\n}\n");
    fclose(f);
    fprintf(stderr, "DONE n_primes=%ld -> %s\n", n_primes, out);
    return 0;
}
