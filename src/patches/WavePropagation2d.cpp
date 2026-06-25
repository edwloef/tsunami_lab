/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Two-dimensional wave propagation patch.
 **/
#include "WavePropagation2d.h"
#include "../solvers/FWave.h"
#include <cmath>

template <typename Solver>
tsunami_lab::patches::WavePropagation2d<Solver>::WavePropagation2d(
    t_idx i_x, t_idx i_y, Solver i_solver) {
    m_solver = i_solver;

    m_nCellsX = i_x;
    m_nCellsY = i_y;

    m_nCells = (i_x + 2) * (i_y + 2);

    for (unsigned short l_st = 0; l_st < 2; l_st++) {
        m_h[l_st] = new t_real[m_nCells];
        m_hu[l_st] = new t_real[m_nCells];
        m_hv[l_st] = new t_real[m_nCells];
    }

    m_b = new t_real[m_nCells];
}

template <typename Solver>
tsunami_lab::patches::WavePropagation2d<
    Solver>::WavePropagation2d::~WavePropagation2d() {
    for (unsigned short l_st = 0; l_st < 2; l_st++) {
        delete[] m_h[l_st];
        delete[] m_hu[l_st];
        delete[] m_hv[l_st];
    }

    delete[] m_b;
}

template <typename Solver>
void tsunami_lab::patches::WavePropagation2d<Solver>::timeStep(
    t_real i_scaling) {
    // pointers to old and new data
    t_real *l_hOld = m_h[m_step];
    t_real *l_huOld = m_hu[m_step];
    t_real *l_hvOld = m_hv[m_step];

    m_step = 1 - m_step;
    t_real *l_hNew = m_h[m_step];
    t_real *l_huNew = m_hu[m_step];
    t_real *l_hvNew = m_hv[m_step];

    t_idx stride = getStride();

    t_real l_maxLambda = 0;

#pragma omp parallel for schedule(guided) reduction(max : l_maxLambda)
    for (t_idx l_y = 0; l_y < m_nCellsY + 1; l_y++) {
        t_idx l_ce = l_y * stride;

        // init new cell quantities
        l_hNew[l_ce] = l_hOld[l_ce];
        l_huNew[l_ce] = l_huOld[l_ce];
        l_hvNew[l_ce] = l_hvOld[l_ce];

        for (t_idx l_x = 0; l_x < m_nCellsX + 1; l_x++) {
            // determine left and right cell-id
            t_idx l_ceL = l_ce + l_x;
            t_idx l_ceR = l_ceL + 1;

            // init new cell quantities
            l_hNew[l_ceR] = l_hOld[l_ceR];
            l_huNew[l_ceR] = l_huOld[l_ceR];
            l_hvNew[l_ceR] = l_hvOld[l_ceR];

            t_real l_hL = l_hOld[l_ceL];
            t_real l_hR = l_hOld[l_ceR];
            t_real l_huL = l_huOld[l_ceL];
            t_real l_huR = l_huOld[l_ceR];
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
                    l_huL = -l_huR;
                    l_bL = l_bR;
                }
            } else if (l_dryR) {
                l_hR = l_hL;
                l_huR = -l_huL;
                l_bR = l_bL;
            }

            t_real l_netUpdates[2][2];
            t_real l_lambda[2];

            // compute net-updates
            m_solver.netUpdates(l_hL, l_hR, l_huL, l_huR, l_bL, l_bR,
                                l_netUpdates[0], l_netUpdates[1], l_lambda);

            // update the cells' quantities
            if (!l_dryL) {
                l_hNew[l_ceL] -= i_scaling * l_netUpdates[0][0];
                l_huNew[l_ceL] -= i_scaling * l_netUpdates[0][1];
                l_maxLambda = std::max(l_maxLambda, std::abs(l_lambda[0]));
            }

            if (!l_dryR) {
                l_hNew[l_ceR] -= i_scaling * l_netUpdates[1][0];
                l_huNew[l_ceR] -= i_scaling * l_netUpdates[1][1];
                l_maxLambda = std::max(l_maxLambda, std::abs(l_lambda[1]));
            }
        }
    }

    for (t_idx l_s = 0; l_s < 2; l_s++) {
#pragma omp parallel for schedule(guided) reduction(max : l_maxLambda)
        for (t_idx l_y = l_s; l_y < m_nCellsY + 1; l_y += 2) {
            t_idx l_ce = l_y * stride;

            for (t_idx l_x = 0; l_x < m_nCellsX + 1; l_x++) {
                // determine left and right cell-id
                t_idx l_ceL = l_ce + l_x;
                t_idx l_ceR = l_ceL + stride;

                t_real l_hL = l_hOld[l_ceL];
                t_real l_hR = l_hOld[l_ceR];
                t_real l_hvL = l_hvOld[l_ceL];
                t_real l_hvR = l_hvOld[l_ceR];
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
                        l_hvL = -l_hvR;
                        l_bL = l_bR;
                    }
                } else if (l_dryR) {
                    l_hR = l_hL;
                    l_hvR = -l_hvL;
                    l_bR = l_bL;
                }

                t_real l_netUpdates[2][2];
                t_real l_lambda[2];

                // compute net-updates
                m_solver.netUpdates(l_hL, l_hR, l_hvL, l_hvR, l_bL, l_bR,
                                    l_netUpdates[0], l_netUpdates[1], l_lambda);

                // update the cells' quantities
                if (!l_dryL) {
                    l_hNew[l_ceL] -= i_scaling * l_netUpdates[0][0];
                    l_hvNew[l_ceL] -= i_scaling * l_netUpdates[0][1];
                    l_maxLambda = std::max(l_maxLambda, std::abs(l_lambda[0]));
                }

                if (!l_dryR) {
                    l_hNew[l_ceR] -= i_scaling * l_netUpdates[1][0];
                    l_hvNew[l_ceR] -= i_scaling * l_netUpdates[1][1];
                    l_maxLambda = std::max(l_maxLambda, std::abs(l_lambda[1]));
                }
            }
        }
    }
}

template <typename Solver>
void tsunami_lab::patches::WavePropagation2d<Solver>::setGhostOutflow() {
    t_real *l_h = m_h[m_step];
    t_real *l_hu = m_hu[m_step];
    t_real *l_hv = m_hv[m_step];

    t_idx stride = getStride();

#pragma omp parallel for schedule(static)
    for (t_idx l_x = 1; l_x < m_nCellsX + 1; l_x++) {
        t_idx l_y = 0;

        t_idx l_i = l_y * stride + l_x;
        t_idx l_j = (l_y + 1) * stride + l_x;

        l_h[l_i] = l_h[l_j];
        l_hu[l_i] = l_hu[l_j];
        l_hv[l_i] = l_hv[l_j];
        m_b[l_i] = m_b[l_j];

        l_y = m_nCellsY + 1;

        l_i = l_y * stride + l_x;
        l_j = (l_y - 1) * stride + l_x;

        l_h[l_i] = l_h[l_j];
        l_hu[l_i] = l_hu[l_j];
        l_hv[l_i] = l_hv[l_j];
        m_b[l_i] = m_b[l_j];
    }

#pragma omp parallel for schedule(static)
    for (t_idx l_y = 1; l_y < m_nCellsY + 1; l_y++) {
        t_idx l_x = 0;

        t_idx l_i = l_y * stride + l_x;
        t_idx l_j = l_y * stride + l_x + 1;

        l_h[l_i] = l_h[l_j];
        l_hu[l_i] = l_hu[l_j];
        l_hv[l_i] = l_hv[l_j];
        m_b[l_i] = m_b[l_j];

        l_x = m_nCellsX + 1;

        l_i = l_y * stride + l_x;
        l_j = l_y * stride + l_x - 1;

        l_h[l_i] = l_h[l_j];
        l_hu[l_i] = l_hu[l_j];
        l_hv[l_i] = l_hv[l_j];
        m_b[l_i] = m_b[l_j];
    }
}

template <typename Solver>
void tsunami_lab::patches::WavePropagation2d<Solver>::setGhostReflecting() {
    t_idx stride = getStride();

#pragma omp parallel for schedule(static)
    for (t_idx l_x = 1; l_x < m_nCellsX + 1; l_x++) {
        m_b[l_x] = 20.;
        m_b[(m_nCellsY + 1) * stride + l_x] = 20.;
    }

#pragma omp parallel for schedule(static)
    for (t_idx l_y = 1; l_y < m_nCellsY + 1; l_y++) {
        m_b[l_y * stride] = 20.;
        m_b[l_y * stride + m_nCellsX + 1] = 20.;
    }
}

template class tsunami_lab::patches::WavePropagation2d<
    tsunami_lab::solvers::FWave>;
