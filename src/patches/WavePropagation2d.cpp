/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Two-dimensional wave propagation patch.
 **/
#include "WavePropagation2d.h"
#include <cstring>

tsunami_lab::patches::WavePropagation2d::WavePropagation2d(t_idx i_x,
                                                           t_idx i_y) {
    m_nCellsX = i_x;
    m_nCellsY = i_y;

    m_nCells = (i_x + 2) * (i_y + 2);

    for (unsigned short l_st = 0; l_st < 2; l_st++) {
        m_h[l_st] = new t_real[m_nCells]{0};
        m_hu[l_st] = new t_real[m_nCells]{0};
        m_hv[l_st] = new t_real[m_nCells]{0};
    }

    m_b = new t_real[m_nCells]{0};
}

tsunami_lab::patches::WavePropagation2d::~WavePropagation2d() {
    for (unsigned short l_st = 0; l_st < 2; l_st++) {
        delete[] m_h[l_st];
        delete[] m_hu[l_st];
        delete[] m_hv[l_st];
    }

    delete[] m_b;
}

void tsunami_lab::patches::WavePropagation2d::timeStep(
    t_real i_scaling, solvers::Solver *solver) {
    // pointers to old and new data
    t_real *l_hOld = m_h[m_step];
    t_real *l_huOld = m_hu[m_step];
    t_real *l_hvOld = m_hv[m_step];

    m_step = (m_step + 1) % 2;
    t_real *l_hNew = m_h[m_step];
    t_real *l_huNew = m_hu[m_step];
    t_real *l_hvNew = m_hv[m_step];

    // init new cell quantities
    std::memcpy(l_hNew, l_hOld, m_nCells * sizeof(t_real));
    std::memcpy(l_huNew, l_huOld, m_nCells * sizeof(t_real));
    std::memcpy(l_hvNew, l_hvOld, m_nCells * sizeof(t_real));

    for (t_idx l_y = 0; l_y < m_nCellsY + 1; l_y++) {
        for (t_idx l_x = 0; l_x < m_nCellsX + 1; l_x++) {
            // determine left and right cell-id
            t_idx l_ceL = l_y * getStride() + l_x;
            t_idx l_ceR = l_y * getStride() + l_x + 1;

            t_real l_hL = l_hOld[l_ceL];
            t_real l_hR = l_hOld[l_ceR];
            t_real l_huL = l_huOld[l_ceL];
            t_real l_huR = l_huOld[l_ceR];
            t_real l_bL = m_b[l_ceL];
            t_real l_bR = m_b[l_ceR];

            // if wet <-> dry boundary, set up reflection
            if (l_bL > 0) {
                if (l_bR > 0) {
                    continue;
                } else {
                    l_hL = l_hR;
                    l_huL = -l_huR;
                    l_bL = l_bR;
                }
            } else if (l_bR > 0) {
                l_hR = l_hL;
                l_huR = -l_huL;
                l_bR = l_bL;
            }

            t_real l_netUpdates[2][2];

            // compute net-updates
            solver->netUpdates(l_hL, l_hR, l_huL, l_huR, l_bL, l_bR,
                               l_netUpdates[0], l_netUpdates[1]);

            // update the cells' quantities
            l_hNew[l_ceL] -= i_scaling * l_netUpdates[0][0];
            l_huNew[l_ceL] -= i_scaling * l_netUpdates[0][1];

            l_hNew[l_ceR] -= i_scaling * l_netUpdates[1][0];
            l_huNew[l_ceR] -= i_scaling * l_netUpdates[1][1];
        }
    }

    for (t_idx l_y = 0; l_y < m_nCellsY + 1; l_y++) {
        for (t_idx l_x = 0; l_x < m_nCellsX + 1; l_x++) {
            // determine left and right cell-id
            t_idx l_ceL = l_y * getStride() + l_x;
            t_idx l_ceR = (l_y + 1) * getStride() + l_x;

            t_real l_hL = l_hOld[l_ceL];
            t_real l_hR = l_hOld[l_ceR];
            t_real l_hvL = l_hvOld[l_ceL];
            t_real l_hvR = l_hvOld[l_ceR];
            t_real l_bL = m_b[l_ceL];
            t_real l_bR = m_b[l_ceR];

            // if wet <-> dry boundary, set up reflection
            if (l_bL > 0) {
                if (l_bR > 0) {
                    continue;
                } else {
                    l_hL = l_hR;
                    l_hvL = -l_hvR;
                    l_bL = l_bR;
                }
            } else if (l_bR > 0) {
                l_hR = l_hL;
                l_hvR = -l_hvL;
                l_bR = l_bL;
            }

            t_real l_netUpdates[2][2];

            // compute net-updates
            solver->netUpdates(l_hL, l_hR, l_hvL, l_hvR, l_bL, l_bR,
                               l_netUpdates[0], l_netUpdates[1]);

            // update the cells' quantities
            l_hNew[l_ceL] -= i_scaling * l_netUpdates[0][0];
            l_hvNew[l_ceL] -= i_scaling * l_netUpdates[0][1];

            l_hNew[l_ceR] -= i_scaling * l_netUpdates[1][0];
            l_hvNew[l_ceR] -= i_scaling * l_netUpdates[1][1];
        }
    }
}

void tsunami_lab::patches::WavePropagation2d::setGhostOutflow() {
    t_real *l_h = m_h[m_step];
    t_real *l_hu = m_hu[m_step];
    t_real *l_hv = m_hv[m_step];

    for (t_idx l_x = 1; l_x < m_nCellsX + 1; l_x++) {
        t_idx l_y = 0;

        t_idx l_i = l_y * getStride() + l_x;
        t_idx l_j = (l_y + 1) * getStride() + l_x;

        l_h[l_i] = l_h[l_j];
        l_hu[l_i] = l_hu[l_j];
        l_hv[l_i] = l_hv[l_j];
        m_b[l_i] = m_b[l_j];

        l_y = m_nCellsY + 1;

        l_i = l_y * getStride() + l_x;
        l_j = (l_y - 1) * getStride() + l_x;

        l_h[l_i] = l_h[l_j];
        l_hu[l_i] = l_hu[l_j];
        l_hv[l_i] = l_hv[l_j];
        m_b[l_i] = m_b[l_j];
    }

    for (t_idx l_y = 1; l_y < m_nCellsY + 1; l_y++) {
        t_idx l_x = 0;

        t_idx l_i = l_y * getStride() + l_x;
        t_idx l_j = l_y * getStride() + l_x + 1;

        l_h[l_i] = l_h[l_j];
        l_hu[l_i] = l_hu[l_j];
        l_hv[l_i] = l_hv[l_j];
        m_b[l_i] = m_b[l_j];

        l_x = m_nCellsX + 1;

        l_i = l_y * getStride() + l_x;
        l_j = l_y * getStride() + l_x - 1;

        l_h[l_i] = l_h[l_j];
        l_hu[l_i] = l_hu[l_j];
        l_hv[l_i] = l_hv[l_j];
        m_b[l_i] = m_b[l_j];
    }
}

void tsunami_lab::patches::WavePropagation2d::setGhostReflecting() {
    for (t_idx l_x = 1; l_x < m_nCellsX + 1; l_x++) {
        m_b[l_x] = 20.;
        m_b[(m_nCellsY + 1) * getStride() + l_x] = 20.;
    }

    for (t_idx l_y = 1; l_y < m_nCellsY + 1; l_y++) {
        m_b[l_y * getStride()] = 20.;
        m_b[l_y * getStride() + m_nCellsX + 1] = 20.;
    }
}
