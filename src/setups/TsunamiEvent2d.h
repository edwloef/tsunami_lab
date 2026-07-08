/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Two-dimensional tsunami event problem.
 **/
#ifndef TSUNAMI_LAB_SETUPS_TSUNAMI_EVENT_2D_H
#define TSUNAMI_LAB_SETUPS_TSUNAMI_EVENT_2D_H

#include "../io/NetCDF.h"
#include "Setup.h"

namespace tsunami_lab {
namespace setups {
class TsunamiEvent2d;
}
} // namespace tsunami_lab

/**
 * 2d tsunami event setup.
 **/
class tsunami_lab::setups::TsunamiEvent2d : public Setup {
  private:
    double m_delta;
    io::NetCDF d, b;

  public:
    /**
     * Constructor.
     **/
    TsunamiEvent2d(char const *i_d, char const *i_b, t_real i_delta);

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

    t_real minX() const {
        return b.minX();
    }

    t_real minY() const {
        return b.minY();
    }

    t_real maxX() const {
        return b.maxX();
    }

    t_real maxY() const {
        return b.maxY();
    }
};

#endif
