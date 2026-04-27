/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Tsunami event.
 **/
#ifndef TSUNAMI_LAB_SETUPS_TSUNAMI_EVENT_1D_H
#define TSUNAMI_LAB_SETUPS_TSUNAMI_EVENT_1D_H

#include "Setup.h"
#include <array>
#include <vector>

namespace tsunami_lab {
namespace setups {
class TsunamiEvent1d;
}
} // namespace tsunami_lab

/**
 * 1d dam break setup.
 **/
class tsunami_lab::setups::TsunamiEvent1d : public Setup {
  private:
    std::vector<std::array<double, 4>> m_b;

  public:
    /**
     * Constructor.
     **/
    TsunamiEvent1d();

    /**
     * Destructor.
     **/
    virtual ~TsunamiEvent1d() = default;

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
