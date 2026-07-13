/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Two-dimensional wave propagation patch.
 **/
#ifndef TSUNAMI_LAB_PATCHES_DYNAMIC_WAVE_PROPAGATION_2D
#define TSUNAMI_LAB_PATCHES_DYNAMIC_WAVE_PROPAGATION_2D

#include "WavePropagation.h"

namespace tsunami_lab {
namespace patches {
template <typename Solver> class DynamicWavePropagation2d;
}
} // namespace tsunami_lab

template <typename Solver>
class tsunami_lab::patches::DynamicWavePropagation2d : public WavePropagation {
  private:
    t_real m_gamma;
    Solver m_solver;

    t_idx m_nCellsX = 0;
    t_idx m_nCellsY = 0;

    t_idx m_nCells = 0;

    t_real *m_h = nullptr;
    t_real *m_hu = nullptr;
    t_real *m_hv = nullptr;
    t_real *m_b = nullptr;

    t_real *m_hAcc = nullptr;
    t_real *m_huAcc = nullptr;
    t_real *m_hvAcc = nullptr;

  public:
    /**
     * Constructs the 2d wave propagation solver.
     *
     * @param i_x number of cells in x direction.
     * @param i_y number of cells in y direction.
     **/
    DynamicWavePropagation2d(t_idx i_x, t_idx i_y, t_real i_gamma,
                             Solver i_solver);

    /**
     * Destructor which frees all allocated memory.
     **/
    ~DynamicWavePropagation2d();

    /**
     * Performs a time step.
     **/
    t_real timeStep(t_real i_dxy);

    /**
     * Sets the bathymetry of the ghost cells.
     **/
    void setGhostBathymetry();

    /**
     * Sets the values of the ghost cells according to outflow boundary
     * conditions.
     **/
    void setGhostOutflow();

    /**
     * Sets the values of the ghost cells according to reflecting boundary
     * conditions.
     **/
    void setGhostReflecting();

    /**
     * Gets the stride in y-direction. x-direction is stride-1.
     *
     * @return stride in y-direction.
     **/
    t_idx getStride() const {
        return m_nCellsX + 2;
    }

    /**
     * Gets cells' water heights.
     *
     * @return water heights.
     */
    t_real const *getHeight() const {
        return m_h + getStride() + 1;
    }

    /**
     * Gets the cells' momenta in x-direction.
     *
     * @return momenta in x-direction.
     **/
    t_real const *getMomentumX() const {
        return m_hu + getStride() + 1;
    }

    /**
     * Gets the cells' momenta in y-direction.
     *
     * @return momenta in y-direction.
     **/
    t_real const *getMomentumY() const {
        return m_hv + getStride() + 1;
    }

    /**
     * Gets the cells' bathymetry.
     *
     * @return bathymetry.
     **/
    t_real const *getBathymetry() const {
        return m_b + getStride() + 1;
    }

    /**
     * Sets the height of the cell to the given value.
     *
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in y-direction.
     * @param i_h water height.
     **/
    void setHeight(t_idx i_ix, t_idx i_iy, t_real i_h) {
        m_h[(i_iy + 1) * getStride() + i_ix + 1] = i_h;
    }

    /**
     * Sets the momentum in x-direction to the given value.
     *
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in y-direction.
     * @param i_hu momentum in x-direction.
     **/
    void setMomentumX(t_idx i_ix, t_idx i_iy, t_real i_hu) {
        m_hu[(i_iy + 1) * getStride() + i_ix + 1] = i_hu;
    }

    /**
     * Sets the momentum in y-direction to the given value.
     *
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in y-direction.
     * @param i_hv momentum in y-direction.
     **/
    void setMomentumY(t_idx i_ix, t_idx i_iy, t_real i_hv) {
        m_hv[(i_iy + 1) * getStride() + i_ix + 1] = i_hv;
    };

    /**
     * Sets the bathymetry to the given value.
     *
     * @param i_ix id of the cell in x-direction.
     * @param i_iy id of the cell in y-direction.
     * @param i_b bathymetry.
     **/
    void setBathymetry(t_idx i_ix, t_idx i_iy, t_real i_b) {
        m_b[(i_iy + 1) * getStride() + i_ix + 1] = i_b;
    }
};

#endif
