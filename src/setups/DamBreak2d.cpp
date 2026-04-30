/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Two-dimensional dam break problem.
 **/
#include "DamBreak2d.h"

tsunami_lab::setups::DamBreak2d::DamBreak2d() {}

tsunami_lab::t_real
tsunami_lab::setups::DamBreak2d::getHeight(t_real i_x, t_real i_y) const {
    return (i_x - 50) * (i_x - 50) + (i_y - 50) * (i_y - 50) < 100 ? 10 : 5;
}

tsunami_lab::t_real
tsunami_lab::setups::DamBreak2d::getMomentumX(t_real, t_real) const {
    return 0;
}

tsunami_lab::t_real
tsunami_lab::setups::DamBreak2d::getMomentumY(t_real, t_real) const {
    return 0;
}

tsunami_lab::t_real
tsunami_lab::setups::DamBreak2d::getBathymetry(t_real, t_real) const {
    return 0;
}
