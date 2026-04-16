/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional rare-rare problem.
 **/
#ifndef TSUNAMI_LAB_SETUPS_RARE_RARE_1D_H
#define TSUNAMI_LAB_SETUPS_RARE_RARE_1D_H

#include "Setup.h"

namespace tsunami_lab {
namespace setups {
class RareRare1d;
}
} // namespace tsunami_lab

/**
 * 1d rare-rare setup.
 **/
class tsunami_lab::setups::RareRare1d : public Setup {
  private:
    //! height
    t_real m_height = 0;

    //! momentum
    t_real m_momentum = 0;

    //! location where the water streams part
    t_real m_location = 0;

  public:
    /**
     * Constructor.
     *
     * @param i_height water height on the left side of the dam.
     * @param i_momentum water height on the right side of the dam.
     * @param i_location location (x-coordinate) of the dam.
     **/
    RareRare1d(t_real i_height, t_real i_momentum, t_real i_location);

    /**
     * Gets the water height at a given point.
     *
     * @param i_x x-coordinate of the queried point.
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
};

#endif
