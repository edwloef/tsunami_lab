/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Two-dimensional wave propagation patch.
 **/
#include "DynamicWavePropagation2d.h"
#include "../solvers/FWave.h"
#include <cmath>

template <typename Solver>
tsunami_lab::patches::DynamicWavePropagation2d<
    Solver>::DynamicWavePropagation2d(t_idx i_x, t_idx i_y, t_real i_gamma,
                                      Solver i_solver) {
    m_gamma = i_gamma;
    m_solver = i_solver;

    m_nCellsX = i_x;
    m_nCellsY = i_y;

    m_nCells = (i_x + 2) * (i_y + 2);

    m_h = new t_real[m_nCells];
    m_hu = new t_real[m_nCells];
    m_hv = new t_real[m_nCells];
    m_b = new t_real[m_nCells];

    m_hAcc = new t_real[m_nCells];
    m_huAcc = new t_real[m_nCells];
    m_hvAcc = new t_real[m_nCells];
}

template <typename Solver>
tsunami_lab::patches::DynamicWavePropagation2d<
    Solver>::DynamicWavePropagation2d::~DynamicWavePropagation2d() {
    delete[] m_h;
    delete[] m_hu;
    delete[] m_hv;
    delete[] m_b;

    delete[] m_hAcc;
    delete[] m_huAcc;
    delete[] m_hvAcc;
}

template <typename Solver>
tsunami_lab::t_real
tsunami_lab::patches::DynamicWavePropagation2d<Solver>::timeStep(t_real i_dxy) {
    t_idx l_stride = getStride();
    t_real l_reflect = (2.0 * m_gamma - t_real(1.0));

    t_real l_maxLambda = 0;

#pragma omp parallel for schedule(guided) reduction(max : l_maxLambda)
    for (t_idx l_y = 0; l_y < m_nCellsY + 1; l_y++) {
        t_idx l_ce = l_y * l_stride;

        // init new cell quantities
        m_hAcc[l_ce] = 0;
        m_huAcc[l_ce] = 0;
        m_hvAcc[l_ce] = 0;

        for (t_idx l_x = 0; l_x < m_nCellsX + 1; l_x++) {
            // determine left and right cell-id
            t_idx l_ceL = l_ce + l_x;
            t_idx l_ceR = l_ceL + 1;

            // init new cell quantities
            m_hAcc[l_ceR] = 0;
            m_huAcc[l_ceR] = 0;
            m_hvAcc[l_ceR] = 0;

            t_real l_hL = m_h[l_ceL];
            t_real l_hR = m_h[l_ceR];
            t_real l_huL = m_hu[l_ceL];
            t_real l_huR = m_hu[l_ceR];
            t_real l_bL = m_b[l_ceL];
            t_real l_bR = m_b[l_ceR];

            bool l_dryL = !std::signbit(l_bL);
            bool l_dryR = !std::signbit(l_bR);

            // if wet <-> dry boundary, set up reflection
            if (l_dryL) {
                if (l_dryR) {
                    continue;
                } else {
                    l_hL = l_hR;
                    l_huL = l_reflect * l_huR;
                    l_bL = l_bR;
                }
            } else if (l_dryR) {
                l_hR = l_hL;
                l_huR = l_reflect * l_huL;
                l_bR = l_bL;
            }

            t_real l_netUpdates[2][2];
            t_real l_lambda[2];

            // compute net-updates
            m_solver.netUpdates(l_hL, l_hR, l_huL, l_huR, l_bL, l_bR,
                                l_netUpdates[0], l_netUpdates[1], l_lambda);

            // update the cells' quantities
            if (!l_dryL) {
                m_hAcc[l_ceL] += l_netUpdates[0][0];
                m_huAcc[l_ceL] += l_netUpdates[0][1];
                l_maxLambda = std::max(l_maxLambda, std::abs(l_lambda[0]));
            }

            if (!l_dryR) {
                m_hAcc[l_ceR] += l_netUpdates[1][0];
                m_huAcc[l_ceR] += l_netUpdates[1][1];
                l_maxLambda = std::max(l_maxLambda, std::abs(l_lambda[1]));
            }
        }
    }

    for (t_idx l_s = 0; l_s < 2; l_s++) {
#pragma omp parallel for schedule(guided) reduction(max : l_maxLambda)
        for (t_idx l_y = l_s; l_y < m_nCellsY + 1; l_y += 2) {
            t_idx l_ce = l_y * l_stride;

            for (t_idx l_x = 0; l_x < m_nCellsX + 1; l_x++) {
                // determine left and right cell-id
                t_idx l_ceL = l_ce + l_x;
                t_idx l_ceR = l_ceL + l_stride;

                t_real l_hL = m_h[l_ceL];
                t_real l_hR = m_h[l_ceR];
                t_real l_hvL = m_hv[l_ceL];
                t_real l_hvR = m_hv[l_ceR];
                t_real l_bL = m_b[l_ceL];
                t_real l_bR = m_b[l_ceR];

                bool l_dryL = !std::signbit(l_bL);
                bool l_dryR = !std::signbit(l_bR);

                // if wet <-> dry boundary, set up reflection
                if (l_dryL) {
                    if (l_dryR) {
                        continue;
                    } else {
                        l_hL = l_hR;
                        l_hvL = l_reflect * l_hvR;
                        l_bL = l_bR;
                    }
                } else if (l_dryR) {
                    l_hR = l_hL;
                    l_hvR = l_reflect * l_hvL;
                    l_bR = l_bL;
                }

                t_real l_netUpdates[2][2];
                t_real l_lambda[2];

                // compute net-updates
                m_solver.netUpdates(l_hL, l_hR, l_hvL, l_hvR, l_bL, l_bR,
                                    l_netUpdates[0], l_netUpdates[1], l_lambda);

                // update the cells' quantities
                if (!l_dryL) {
                    m_hAcc[l_ceL] += l_netUpdates[0][0];
                    m_hvAcc[l_ceL] += l_netUpdates[0][1];
                    l_maxLambda = std::max(l_maxLambda, std::abs(l_lambda[0]));
                }

                if (!l_dryR) {
                    m_hAcc[l_ceR] += l_netUpdates[1][0];
                    m_hvAcc[l_ceR] += l_netUpdates[1][1];
                    l_maxLambda = std::max(l_maxLambda, std::abs(l_lambda[1]));
                }
            }
        }
    }

    t_real l_dt = t_real(0.5) * i_dxy / l_maxLambda;
    t_real l_scaling = l_dt / i_dxy;

#pragma omp parallel for schedule(static)
    for (t_idx l_ed = 0; l_ed < m_nCells; l_ed++) {
        m_h[l_ed] -= l_scaling * m_hAcc[l_ed];
        m_hu[l_ed] -= l_scaling * m_huAcc[l_ed];
        m_hv[l_ed] -= l_scaling * m_hvAcc[l_ed];
    }

    return l_dt;
}

template <typename Solver>
void tsunami_lab::patches::DynamicWavePropagation2d<
    Solver>::setGhostBathymetry() {
    t_idx l_stride = getStride();

#pragma omp parallel for schedule(static)
    for (t_idx l_y = 1; l_y < m_nCellsY + 1; l_y++) {
        t_idx l_x = 0;

        t_idx l_i = l_y * l_stride + l_x;
        t_idx l_j = l_i + 1;

        m_b[l_i] = m_b[l_j];

        l_x = m_nCellsX + 1;

        l_i = l_y * l_stride + l_x;
        l_j = l_i - 1;

        m_b[l_i] = m_b[l_j];
    }

#pragma omp parallel for schedule(static)
    for (t_idx l_x = 1; l_x < m_nCellsX + 1; l_x++) {
        t_idx l_y = 0;

        t_idx l_i = l_y * l_stride + l_x;
        t_idx l_j = l_i + l_stride;

        m_b[l_i] = m_b[l_j];

        l_y = m_nCellsY + 1;

        l_i = l_y * l_stride + l_x;
        l_j = l_i - l_stride;

        m_b[l_i] = m_b[l_j];
    }
}

template <typename Solver>
void tsunami_lab::patches::DynamicWavePropagation2d<Solver>::setGhostOutflow() {
    t_idx l_stride = getStride();

#pragma omp parallel for schedule(static)
    for (t_idx l_y = 1; l_y < m_nCellsY + 1; l_y++) {
        t_idx l_x = 0;

        t_idx l_i = l_y * l_stride + l_x;
        t_idx l_j = l_i + 1;

        m_h[l_i] = m_h[l_j];
        m_hu[l_i] = m_hu[l_j];

        l_x = m_nCellsX + 1;

        l_i = l_y * l_stride + l_x;
        l_j = l_i - 1;

        m_h[l_i] = m_h[l_j];
        m_hu[l_i] = m_hu[l_j];
    }

#pragma omp parallel for schedule(static)
    for (t_idx l_x = 1; l_x < m_nCellsX + 1; l_x++) {
        t_idx l_y = 0;

        t_idx l_i = l_y * l_stride + l_x;
        t_idx l_j = l_i + l_stride;

        m_h[l_i] = m_h[l_j];
        m_hv[l_i] = m_hv[l_j];

        l_y = m_nCellsY + 1;

        l_i = l_y * l_stride + l_x;
        l_j = l_i - l_stride;

        m_h[l_i] = m_h[l_j];
        m_hv[l_i] = m_hv[l_j];
    }
}

template <typename Solver>
void tsunami_lab::patches::DynamicWavePropagation2d<
    Solver>::setGhostReflecting() {
    t_idx l_stride = getStride();

#pragma omp parallel for schedule(static)
    for (t_idx l_y = 1; l_y < m_nCellsY + 1; l_y++) {
        t_idx l_x = 0;

        t_idx l_i = l_y * l_stride + l_x;
        t_idx l_j = l_i + 1;

        m_h[l_i] = m_h[l_j];
        m_hu[l_i] = -m_hu[l_j];

        l_x = m_nCellsX + 1;

        l_i = l_y * l_stride + l_x;
        l_j = l_i - 1;

        m_h[l_i] = m_h[l_j];
        m_hu[l_i] = -m_hu[l_j];
    }

#pragma omp parallel for schedule(static)
    for (t_idx l_x = 1; l_x < m_nCellsX + 1; l_x++) {
        t_idx l_y = 0;

        t_idx l_i = l_y * l_stride + l_x;
        t_idx l_j = l_i + l_stride;

        m_h[l_i] = m_h[l_j];
        m_hv[l_i] = -m_hv[l_j];

        l_y = m_nCellsY + 1;

        l_i = l_y * l_stride + l_x;
        l_j = l_i - l_stride;

        m_h[l_i] = m_h[l_j];
        m_hv[l_i] = -m_hv[l_j];
    }
}

template class tsunami_lab::patches::DynamicWavePropagation2d<
    tsunami_lab::solvers::FWave>;
