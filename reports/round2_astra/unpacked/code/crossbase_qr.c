/*
 * crossbase_qr.c — test whether the cross-base residual is the QUADRATIC
 * CHARACTER channel that the "fine signature" omitted.
 *
 * HYPOTHESIS. The fine signature conditions on the multiplicative structure of
 * p-1 (v2, which small primes divide it, how many large ones) but NOT on
 * p mod f, hence not on the values chi_a(p). Yet the l = 2 Artin condition
 * ALWAYS binds (2 | p-1 always), and it is exactly chi_a(p) = -1.
 *
 * Now sqf(a) and sqf(b) sharing a prime factor  <=>  chi_a and chi_b sharing a
 * prime-discriminant component  =>  chi_a(p), chi_b(p) are CORRELATED through
 * that shared component. E.g. chi_2 (component 8) and chi_6 = chi_{-3}chi_{-8}
 * share the 2-adic component; chi_2 and chi_10 = chi_5 chi_8 likewise.
 *
 * PREDICTION: conditioning additionally on the pair (chi_a(p), chi_b(p)) should
 * remove the residual for the sharing pairs -- including the (2,6) outlier --
 * and leave the coprime pairs unchanged (they are already null).
 *
 * If true, paper #3's residual is NOT exotic Kummer collapse: it is the QR
 * channel, i.e. the same mechanism as paper #2's exclusion law, seen sideways.
 *
 * gcc -O3 -o crossbase_qr crossbase_qr.c -lm
 * ./crossbase_qr <limit> <out.json>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEG_SIZE  (1 << 22)
#define SMALL_LIM 100000

static const int BASES[] = {2, 3, 5, 6, 7, 10, 11, 13, 15, 17, 21, 29};
/* squarefree parts, precomputed: sqf(15)=15, sqf(21)=21 etc (all squarefree here) */
static const int SQF[]   = {2, 3, 5, 6, 7, 10, 11, 13, 15, 17, 21, 29};
#define NB (int)(sizeof(BASES)/sizeof(BASES[0]))

#define NV2   5
#define NMASK 32
#define NLRG  4
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

/* [i][j][sig][chi_i][chi_j][art_i][art_j] : chi index 0 = (-1), 1 = (+1) */
static long tab[NB][NB][NSIG][2][2][2][2];
static long plain[NB][NB][2][2];
static long n_primes = 0;

int main(int argc, char **argv) {
    long LIMIT = (argc > 1) ? atol(argv[1]) : 100000000L;
    const char *out = (argc > 2) ? argv[2] : "crossbase_qr.json";
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
            long p = n; if (p < 31) continue;

            long pm1 = p-1, tmp = pm1, fac[64]; int nf = 0;
            for (int i = 0; i < n_small; i++) {
                long dd = small_primes[i]; if (dd*dd > tmp) break;
                if (tmp % dd == 0) { fac[nf++] = dd; while (tmp % dd == 0) tmp /= dd; }
            }
            if (tmp > 1) fac[nf++] = tmp;

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

            int art[NB], chi[NB];
            int skip = 0;
            for (int b = 0; b < NB; b++) {
                long a = BASES[b];
                if (a % p == 0) { skip = 1; break; }
                /* chi_a(p) via Euler on the squarefree part */
                long e = powmod(SQF[b], pm1/2, p);
                chi[b] = (e == 1) ? 1 : 0;         /* 1 means +1, 0 means -1 */
                int ok = 1;
                for (int i = 0; i < nf && ok; i++)
                    if (powmod(a, pm1/fac[i], p) == 1) ok = 0;
                art[b] = ok;
            }
            if (skip) continue;
            n_primes++;
            for (int i = 0; i < NB; i++)
                for (int j = i+1; j < NB; j++) {
                    plain[i][j][art[i]][art[j]]++;
                    tab[i][j][sig][chi[i]][chi[j]][art[i]][art[j]]++;
                }
            if (n_primes % 2000000 == 0) { fprintf(stderr, "  p=%ld n=%ld\n", p, n_primes); fflush(stderr); }
        }
    }

    FILE *f = fopen(out, "w");
    fprintf(f, "{\n  \"limit\": %ld,\n  \"n_primes\": %ld,\n  \"bases\": [", LIMIT, n_primes);
    for (int i = 0; i < NB; i++) fprintf(f, "%d%s", BASES[i], i==NB-1?"":", ");
    fprintf(f, "],\n  \"plain\": {\n");
    int first = 1;
    for (int i = 0; i < NB; i++) for (int j = i+1; j < NB; j++) {
        if (!first) fprintf(f, ",\n");
        fprintf(f, "    \"%d,%d\": [[%ld,%ld],[%ld,%ld]]", BASES[i], BASES[j],
                plain[i][j][0][0], plain[i][j][0][1], plain[i][j][1][0], plain[i][j][1][1]);
        first = 0;
    }
    /* cells: key "a,b" -> { "sig.chi_i.chi_j": [[..],[..]] } */
    fprintf(f, "\n  },\n  \"cells\": {\n");
    first = 1;
    for (int i = 0; i < NB; i++) for (int j = i+1; j < NB; j++) {
        if (!first) fprintf(f, ",\n");
        fprintf(f, "    \"%d,%d\": {", BASES[i], BASES[j]);
        int f2 = 1;
        for (int s = 0; s < NSIG; s++)
          for (int ci = 0; ci < 2; ci++)
            for (int cj = 0; cj < 2; cj++) {
                long t = tab[i][j][s][ci][cj][0][0]+tab[i][j][s][ci][cj][0][1]
                       + tab[i][j][s][ci][cj][1][0]+tab[i][j][s][ci][cj][1][1];
                if (!t) continue;
                if (!f2) fprintf(f, ", ");
                fprintf(f, "\"%d.%d.%d\": [[%ld,%ld],[%ld,%ld]]", s, ci, cj,
                    tab[i][j][s][ci][cj][0][0], tab[i][j][s][ci][cj][0][1],
                    tab[i][j][s][ci][cj][1][0], tab[i][j][s][ci][cj][1][1]);
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
