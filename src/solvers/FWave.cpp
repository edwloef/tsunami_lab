/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * F-Wave solver for the one-dimensional shallow water equations.
 **/

#include "FWave.h"
#include <cmath>

void tsunami_lab::solvers::FWave::netUpdates(t_real i_hL, t_real i_hR,
                                             t_real i_huL, t_real i_huR,
                                             t_real o_netUpdateL[2],
                                             t_real o_netUpdateR[2]) {
    t_real uL = i_huL / i_hL;
    t_real uR = i_huR / i_hR;

    // compute wave speeds (eigenvalues)
    t_real sqrt_hL = std::sqrt(i_hL);
    t_real sqrt_hR = std::sqrt(i_hR);
    t_real u_roe = (uL * sqrt_hL + uR * sqrt_hR) / (sqrt_hL + sqrt_hR);

    t_real h_roe = t_real(0.5) * (i_hL + i_hR);

    t_real sqrt_g_h_roe = std::sqrt(g * h_roe);

    t_real delta1 = u_roe - sqrt_g_h_roe;
    t_real delta2 = u_roe + sqrt_g_h_roe;

    // compute fluxes
    t_real fL[2] = {i_huL, i_huL * uL + g_half * i_hL * i_hL};
    t_real fR[2] = {i_huR, i_huR * uR + g_half * i_hR * i_hR};

    // flux difference deltaF
    t_real deltaF[2] = {fR[0] - fL[0], fR[1] - fL[1]};

    // solve linear system for alphas:
    // deltaF = alpha1 * delta1 + alpha2 * delta2
    t_real alpha1 = (delta2 * deltaF[0] - deltaF[1]) / (delta2 - delta1);
    t_real alpha2 = (deltaF[1] - delta1 * deltaF[0]) / (delta2 - delta1);

    // f-waves
    t_real Z1[2] = {alpha1, alpha1 * delta1};
    t_real Z2[2] = {alpha2, alpha2 * delta2};

    // distribute waves
    o_netUpdateL[0] = t_real(0.0);
    o_netUpdateL[1] = t_real(0.0);
    o_netUpdateR[0] = t_real(0.0);
    o_netUpdateR[1] = t_real(0.0);

    if (delta1 < t_real(0.0)) {
        o_netUpdateL[0] += Z1[0];
        o_netUpdateL[1] += Z1[1];
    } else if (delta1 > t_real(0.0)) {
        o_netUpdateR[0] += Z1[0];
        o_netUpdateR[1] += Z1[1];
    }

    if (delta2 < t_real(0.0)) {
        o_netUpdateL[0] += Z2[0];
        o_netUpdateL[1] += Z2[1];
    } else if (delta2 > t_real(0.0)) {
        o_netUpdateR[0] += Z2[0];
        o_netUpdateR[1] += Z2[1];
    }
}
