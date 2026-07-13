/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional wave propagation patch.
 **/
#include "WavePropagation1d.h"
#include "../solvers/FWave.h"
#include <cmath>

template <typename Solver>
tsunami_lab::patches::WavePropagation1d<Solver>::WavePropagation1d(
    t_idx i_nCells, Solver i_solver) {
    m_solver = i_solver;
    m_nCells = i_nCells;

    m_h = new t_real[m_nCells + 2];
    m_hu = new t_real[m_nCells + 2];
    m_b = new t_real[m_nCells + 2];

    m_hAcc = new t_real[m_nCells + 2];
    m_huAcc = new t_real[m_nCells + 2];
}

template <typename Solver>
tsunami_lab::patches::WavePropagation1d<
    Solver>::WavePropagation1d::~WavePropagation1d() {
    delete[] m_h;
    delete[] m_hu;
    delete[] m_b;

    delete[] m_hAcc;
    delete[] m_huAcc;
}

template <typename Solver>
tsunami_lab::t_real
tsunami_lab::patches::WavePropagation1d<Solver>::timeStep(t_real i_dxy) {
    t_real l_maxLambda = 0;

    // init new cell quantities
    m_hAcc[0] = 0;
    m_huAcc[0] = 0;

    // iterate over edges and update with Riemann solutions
    for (t_idx l_ed = 0; l_ed < m_nCells + 1; l_ed++) {
        // determine left and right cell-id
        t_idx l_ceL = l_ed;
        t_idx l_ceR = l_ceL + 1;

        // init new cell quantities
        m_hAcc[l_ceR] = 0;
        m_huAcc[l_ceR] = 0;

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

    t_real l_dt = t_real(0.5) * i_dxy / l_maxLambda;
    t_real l_scaling = l_dt / i_dxy;

    for (t_idx l_ed = 0; l_ed < m_nCells + 2; l_ed++) {
        m_h[l_ed] -= l_scaling * m_hAcc[l_ed];
        m_hu[l_ed] -= l_scaling * m_huAcc[l_ed];
    }

    return l_dt;
}

template <typename Solver>
void tsunami_lab::patches::WavePropagation1d<Solver>::setGhostBathymetry() {
    // set left boundary
    m_b[0] = m_b[1];

    // set right boundary
    m_b[m_nCells + 1] = m_b[m_nCells];
}

template <typename Solver>
void tsunami_lab::patches::WavePropagation1d<Solver>::setGhostOutflow() {
    // set left boundary
    m_h[0] = m_h[1];
    m_hu[0] = m_hu[1];

    // set right boundary
    m_h[m_nCells + 1] = m_h[m_nCells];
    m_hu[m_nCells + 1] = m_hu[m_nCells];
}

template <typename Solver>
void tsunami_lab::patches::WavePropagation1d<Solver>::setGhostReflecting() {
    // set left boundary
    m_h[0] = m_h[1];
    m_hu[0] = -m_hu[1];

    // set right boundary
    m_h[m_nCells + 1] = m_h[m_nCells];
    m_hu[m_nCells + 1] = -m_hu[m_nCells];
}

template class tsunami_lab::patches::WavePropagation1d<
    tsunami_lab::solvers::FWave>;
