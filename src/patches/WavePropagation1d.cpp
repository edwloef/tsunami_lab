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

    for (unsigned short l_st = 0; l_st < 2; l_st++) {
        m_h[l_st] = new t_real[m_nCells + 2];
        m_hu[l_st] = new t_real[m_nCells + 2];
    }

    m_b = new t_real[m_nCells + 2];
}

template <typename Solver>
tsunami_lab::patches::WavePropagation1d<
    Solver>::WavePropagation1d::~WavePropagation1d() {
    for (unsigned short l_st = 0; l_st < 2; l_st++) {
        delete[] m_h[l_st];
        delete[] m_hu[l_st];
    }

    delete[] m_b;
}

template <typename Solver>
tsunami_lab::t_real
tsunami_lab::patches::WavePropagation1d<Solver>::timeStep(t_real i_dxy) {
    // pointers to old and new data
    t_real *l_hOld = m_h[m_step];
    t_real *l_huOld = m_hu[m_step];

    m_step = 1 - m_step;
    t_real *l_hNew = m_h[m_step];
    t_real *l_huNew = m_hu[m_step];

    tsunami_lab::t_real l_speedMax = std::sqrt(9.80665 * m_hMax);
    t_real l_dt = t_real(0.5) * i_dxy / l_speedMax;
    t_real l_scaling = l_dt / i_dxy;

    // init new cell quantities
    l_hNew[0] = l_hOld[0];
    l_huNew[0] = l_huOld[0];

    // iterate over edges and update with Riemann solutions
    for (t_idx l_ed = 0; l_ed < m_nCells + 1; l_ed++) {
        // determine left and right cell-id
        t_idx l_ceL = l_ed;
        t_idx l_ceR = l_ceL + 1;

        // init new cell quantities
        l_hNew[l_ceR] = l_hOld[l_ceR];
        l_huNew[l_ceR] = l_huOld[l_ceR];

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
            l_hNew[l_ceL] -= l_scaling * l_netUpdates[0][0];
            l_huNew[l_ceL] -= l_scaling * l_netUpdates[0][1];
        }

        if (!l_dryR) {
            l_hNew[l_ceR] -= l_scaling * l_netUpdates[1][0];
            l_huNew[l_ceR] -= l_scaling * l_netUpdates[1][1];
        }
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
    t_real *l_h = m_h[m_step];
    t_real *l_hu = m_hu[m_step];

    // set left boundary
    l_h[0] = l_h[1];
    l_hu[0] = l_hu[1];

    // set right boundary
    l_h[m_nCells + 1] = l_h[m_nCells];
    l_hu[m_nCells + 1] = l_hu[m_nCells];
}

template <typename Solver>
void tsunami_lab::patches::WavePropagation1d<Solver>::setGhostReflecting() {
    t_real *l_h = m_h[m_step];
    t_real *l_hu = m_hu[m_step];

    // set left boundary
    l_h[0] = l_h[1];
    l_hu[0] = -l_hu[1];

    // set right boundary
    l_h[m_nCells + 1] = l_h[m_nCells];
    l_hu[m_nCells + 1] = -l_hu[m_nCells];
}

template class tsunami_lab::patches::WavePropagation1d<
    tsunami_lab::solvers::FWave>;
