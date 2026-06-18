/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Two-dimensional tsunami event problem.
 **/

#include "ArtificialTsunami2d.h"
#include <cmath>

tsunami_lab::t_real
tsunami_lab::setups::ArtificialTsunami2d::getHeight(t_real, t_real) const {
    return 100;
}

tsunami_lab::t_real
tsunami_lab::setups::ArtificialTsunami2d::getMomentumX(t_real, t_real) const {
    return 0;
}

tsunami_lab::t_real
tsunami_lab::setups::ArtificialTsunami2d::getMomentumY(t_real, t_real) const {
    return 0;
}

tsunami_lab::t_real
tsunami_lab::setups::ArtificialTsunami2d::getBathymetry(t_real i_x,
                                                        t_real i_y) const {
    return -100 + (std::abs(i_x) <= 500 && std::abs(i_y) <= 500
                       ? 5 * std::sin((i_x / 500 + 1) * M_PI) *
                             (1 - std::pow(i_y / 500, 2))
                       : 0);
}
