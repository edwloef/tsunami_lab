/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Two-dimensional dam break problem.
 **/
#ifndef TSUNAMI_LAB_SETUPS_DAM_BREAK_2D_H
#define TSUNAMI_LAB_SETUPS_DAM_BREAK_2D_H

#include "Setup.h"

namespace tsunami_lab {
namespace setups {
class DamBreak2d;
}
} // namespace tsunami_lab

/**
 * 1d dam break setup.
 **/
class tsunami_lab::setups::DamBreak2d : public Setup {
  public:
    /**
     * Constructor.
     **/
    DamBreak2d();

    /**
     * Gets the water height at a given point.
     *
     * @return height at the given point.
     **/
    t_real getHeight(t_real i_x, t_real) const;

    /**
     * Gets the momentum in x-direction.
     *
     * @return momentum in x-direction.
     **/
    t_real getMomentumX(t_real, t_real) const;

    /**
     * Gets the momentum in y-direction.
     *
     * @return momentum in y-direction.
     **/
    t_real getMomentumY(t_real, t_real) const;

    /**
     * Gets the bathymetry.
     *
     * @return bathymetry.
     **/
    t_real getBathymetry(t_real, t_real) const;
};

#endif
