/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Two-dimensional tsunami event problem.
 **/

#include "TsunamiEvent2d.h"
#include <algorithm>

tsunami_lab::setups::TsunamiEvent2d::TsunamiEvent2d(char const *i_d,
                                                    char const *i_b)
    : d(io::NetCDF(i_d)), b(io::NetCDF(i_b)) {}

tsunami_lab::t_real
tsunami_lab::setups::TsunamiEvent2d::getHeight(t_real i_x, t_real i_y) const {
    double b_in = b.readAt(i_x, i_y).value();
    return b_in < 0 ? std::max(-b_in, 20.) : 0;
}

tsunami_lab::t_real
tsunami_lab::setups::TsunamiEvent2d::getMomentumX(t_real, t_real) const {
    return 0;
}

tsunami_lab::t_real
tsunami_lab::setups::TsunamiEvent2d::getMomentumY(t_real, t_real) const {
    return 0;
}

tsunami_lab::t_real
tsunami_lab::setups::TsunamiEvent2d::getBathymetry(t_real i_x,
                                                   t_real i_y) const {
    double b_in = b.readAt(i_x, i_y).value();
    return (b_in < 0 ? std::min(b_in, -20.) : std::max(b_in, 20.)) +
           d.readAt(i_x, i_y).value_or(0);
}
