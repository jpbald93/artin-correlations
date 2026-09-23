# Model-free optimism check: in-sample gain on a half vs held-out gain on the other half.
# E[g_in] ~ I + opt, E[g_out] ~ I - opt  =>  I ~ (g_in+g_out)/2, opt ~ (g_in-g_out)/2.
import numpy as np
exec(open('content_r2.py').read().split('for M in (120,840):')[0])
for M in (120,840):
    T=TT[M]
    for fi,te in ((1,2),(2,1)):
        N,ce0,ceR,ceRP,k,kk=insample(T[fi]); gin=ceR-ceRP
        _,_,_,gout=score(T[fi],T[te],'cells')
        print(f"mod{M} fit={fi}: N_fit={N:.0f} g_in={gin:.3e} g_out={gout:.3e}  ->  I_est={(gin+gout)/2:.3e}  empirical optimism={(gin-gout)/2:.3e}  k_eff/(2N_fit)={k/(2*N):.3e} (k_eff={k})  author k/(2N)-claim uses 512|3448 over full N")
    # also the author's claimed numbers
N=50847530
for k in (512,3448,256,1599): print(k, 'k/(2N)=%.3e'%(k/(2*N)), 'k/N=%.3e'%(k/N))
