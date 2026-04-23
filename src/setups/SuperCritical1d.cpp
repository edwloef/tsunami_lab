/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Subcritical flow problem.
 **/
#include "SuperCritical1d.h"

#include <cmath>

tsunami_lab::setups::SuperCritical1d::SuperCritical1d() {}

tsunami_lab::t_real
tsunami_lab::setups::SuperCritical1d::getHeight(t_real i_x, t_real i_y) const {
    return -getBathymetry(i_x, i_y);
}

tsunami_lab::t_real
tsunami_lab::setups::SuperCritical1d::getMomentumX(t_real, t_real) const {
    return 0.18;
}

tsunami_lab::t_real
tsunami_lab::setups::SuperCritical1d::getMomentumY(t_real, t_real) const {
    return 0;
}

tsunami_lab::t_real
tsunami_lab::setups::SuperCritical1d::getBathymetry(t_real i_x, t_real) const {
    return i_x > 8 && i_x < 12 ? -0.13 - 0.05 * std::powf(i_x - 10, 2) : -0.33;
}