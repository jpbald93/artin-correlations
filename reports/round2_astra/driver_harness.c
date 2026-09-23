#define main author_main
#include "../../code/artin_payoff.c"
#undef main
int main(void){ll c[]={3,1,1,3,6,2,1,1};decomp_report(c,2,"counterexample");return 0;}
