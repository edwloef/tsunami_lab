/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Two-dimensional wave propagation patch.
 **/
#include "WavePropagation2d.h"

tsunami_lab::patches::WavePropagation2d::WavePropagation2d(t_idx i_x,
                                                           t_idx i_y) {
    m_nCellsX = i_x;
    m_nCellsY = i_y;

    m_nCells = (i_x + 2) * (i_y + 2);

    for (unsigned short l_st = 0; l_st < 2; l_st++) {
        m_h[l_st] = new t_real[m_nCells];
        m_hu[l_st] = new t_real[m_nCells];
        m_hv[l_st] = new t_real[m_nCells];
    }

    for (unsigned short l_st = 0; l_st < 2; l_st++) {
        for (t_idx l_ce = 0; l_ce < m_nCells; l_ce++) {
            m_h[l_st][l_ce] = 0;
            m_hu[l_st][l_ce] = 0;
            m_hv[l_st][l_ce] = 0;
        }
    }

    m_b = new t_real[m_nCells];

    for (t_idx l_ce = 0; l_ce < m_nCells; l_ce++) {
        m_b[l_ce] = 0;
    }
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
    for (t_idx l_ce = 0; l_ce < m_nCells; l_ce++) {
        l_hNew[l_ce] = l_hOld[l_ce];
        l_huNew[l_ce] = l_huOld[l_ce];
        l_hvNew[l_ce] = l_hvOld[l_ce];
    }

    for (t_idx l_y = 0; l_y < m_nCellsY + 1; l_y++) {
        for (t_idx l_x = 0; l_x < m_nCellsX + 1; l_x++) {
            t_real l_netUpdates[2][2];

            t_idx l_i = l_y * getStride() + l_x;
            t_idx l_h = l_y * getStride() + l_x + 1;
            t_idx l_v = (l_y + 1) * getStride() + l_x;

            t_real l_hI = l_hOld[l_i];
            t_real l_hH = l_hOld[l_h];
            t_real l_huI = l_huOld[l_i];
            t_real l_huH = l_huOld[l_h];
            t_real l_bI = m_b[l_i];
            t_real l_bH = m_b[l_h];

            // if wet <-> dry boundary, set up reflection
            if (l_bI >= 0 && l_bH < 0) {
                l_hI = l_hH;
                l_huI = -l_huH;
                l_bI = l_bH;
            } else if (l_bH >= 0 && l_bI < 0) {
                l_hH = l_hI;
                l_huH = -l_huI;
                l_bH = l_bI;
            }

            // compute net-updates
            solver->netUpdates(l_hI, l_hH, l_huI, l_huH, l_bI, l_bH,
                               l_netUpdates[0], l_netUpdates[1]);

            // update the cells' quantities
            l_hNew[l_i] -= i_scaling * l_netUpdates[0][0];
            l_huNew[l_i] -= i_scaling * l_netUpdates[0][1];

            l_hNew[l_h] -= i_scaling * l_netUpdates[1][0];
            l_huNew[l_h] -= i_scaling * l_netUpdates[1][1];

            l_hI = l_hOld[l_i];
            t_real l_hV = l_hOld[l_v];
            t_real l_hvI = l_hvOld[l_i];
            t_real l_hvV = l_hvOld[l_v];
            l_bI = m_b[l_i];
            t_real l_bV = m_b[l_v];

            // if wet <-> dry boundary, set up reflection
            if (l_bI >= 0 && l_bV < 0) {
                l_hI = l_hV;
                l_hvI = -l_hvV;
                l_bI = l_bV;
            } else if (l_bV >= 0 && l_bI < 0) {
                l_hV = l_hI;
                l_hvV = -l_hvI;
                l_bV = l_bI;
            }

            // compute net-updates
            solver->netUpdates(l_hI, l_hV, l_hvI, l_hvV, l_bI, l_bV,
                               l_netUpdates[0], l_netUpdates[1]);

            // update the cells' quantities
            l_hNew[l_i] -= i_scaling * l_netUpdates[0][0];
            l_hvNew[l_i] -= i_scaling * l_netUpdates[0][1];

            l_hNew[l_v] -= i_scaling * l_netUpdates[1][0];
            l_hvNew[l_v] -= i_scaling * l_netUpdates[1][1];
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
    t_real *l_h = m_h[m_step];
    t_real *l_hu = m_hu[m_step];
    t_real *l_hv = m_hv[m_step];

    for (t_idx l_x = 1; l_x < m_nCellsX + 1; l_x++) {
        t_idx l_y = 0;

        t_idx l_i = l_y * getStride() + l_x;
        t_idx l_j = (l_y + 1) * getStride() + l_x;

        l_h[l_i] = l_h[l_j];
        l_hu[l_i] = -l_hu[l_j];
        l_hv[l_i] = -l_hv[l_j];
        m_b[l_i] = m_b[l_j];

        l_y = m_nCellsY + 1;

        l_i = l_y * getStride() + l_x;
        l_j = (l_y - 1) * getStride() + l_x;

        l_h[l_i] = l_h[l_j];
        l_hu[l_i] = -l_hu[l_j];
        l_hv[l_i] = -l_hv[l_j];
        m_b[l_i] = m_b[l_j];
    }

    for (t_idx l_y = 1; l_y < m_nCellsY + 1; l_y++) {
        t_idx l_x = 0;

        t_idx l_i = l_y * getStride() + l_x;
        t_idx l_j = l_y * getStride() + l_x + 1;

        l_h[l_i] = l_h[l_j];
        l_hu[l_i] = -l_hu[l_j];
        l_hv[l_i] = -l_hv[l_j];
        m_b[l_i] = m_b[l_j];

        l_x = m_nCellsX + 1;

        l_i = l_y * getStride() + l_x;
        l_j = l_y * getStride() + l_x - 1;

        l_h[l_i] = l_h[l_j];
        l_hu[l_i] = -l_hu[l_j];
        l_hv[l_i] = -l_hv[l_j];
        m_b[l_i] = m_b[l_j];
    }
}
