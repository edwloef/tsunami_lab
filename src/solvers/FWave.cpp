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
                                             t_real i_bL, t_real i_bR,
                                             t_real o_netUpdateL[2],
                                             t_real o_netUpdateR[2]) const {
    // compute wave speeds
    t_real uL = i_huL / i_hL;
    t_real uR = i_huR / i_hR;

    t_real sqrt_hL = std::sqrt(i_hL);
    t_real sqrt_hR = std::sqrt(i_hR);

    t_real u_roe = (uL * sqrt_hL + uR * sqrt_hR) / (sqrt_hL + sqrt_hR);
    t_real sqrt_g_h_roe = std::sqrt(g_half * (i_hL + i_hR));

    // compute eigenvalues
    t_real lambda_roe[2] = {u_roe - sqrt_g_h_roe, u_roe + sqrt_g_h_roe};

    // compute fluxes
    t_real fL[2] = {i_huL, i_huL * uL + g_half * i_hL * i_hL};
    t_real fR[2] = {i_huR, i_huR * uR + g_half * i_hR * i_hR};

    // flux difference
    t_real deltaF[2] = {fR[0] - fL[0], fR[1] - fL[1]};

    // bathymetry difference
    t_real deltaXPsi[2] = {0, g_half * (i_bL - i_bR) * (i_hL + i_hR)};

    // combined effect
    t_real combined[2] = {deltaF[0] - deltaXPsi[0], deltaF[1] - deltaXPsi[1]};

    // solve linear system for alphas
    t_real diff = t_real(0.5) / sqrt_g_h_roe; // 1 / (lambda2 - lambda1)
    t_real alpha[2] = {(combined[0] * lambda_roe[1] - combined[1]) * diff,
                       (combined[1] - lambda_roe[0] * combined[0]) * diff};

    // f-waves
    t_real z1[2] = {alpha[0], alpha[0] * lambda_roe[0]};
    t_real z2[2] = {alpha[1], alpha[1] * lambda_roe[1]};

    o_netUpdateL[0] = 0;
    o_netUpdateL[1] = 0;
    o_netUpdateR[0] = 0;
    o_netUpdateR[1] = 0;

    // distribute waves
    if (std::signbit(lambda_roe[0])) {
        o_netUpdateL[0] = z1[0];
        o_netUpdateL[1] = z1[1];
    } else {
        o_netUpdateR[0] = z1[0];
        o_netUpdateR[1] = z1[1];
    }

    if (std::signbit(lambda_roe[1])) {
        o_netUpdateL[0] += z2[0];
        o_netUpdateL[1] += z2[1];
    } else {
        o_netUpdateR[0] += z2[0];
        o_netUpdateR[1] += z2[1];
    }
}
