/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Subcritical flow problem.
 **/
#ifndef TSUNAMI_LAB_SETUPS_SUBCRITICAL_1D_H
#define TSUNAMI_LAB_SETUPS_SUBCRITICAL_1D_H

#include "Setup.h"

namespace tsunami_lab {
namespace setups {
class SubCritical1d;
}
} // namespace tsunami_lab

/**
 * 1d dam break setup.
 **/
class tsunami_lab::setups::SubCritical1d : public Setup {
  public:
    /**
     * Constructor.
     **/
    SubCritical1d();

    /**
     * Gets the water height at a given point.
     *
     * @return height at the given point.
     **/
    t_real getHeight(t_real, t_real) const;

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