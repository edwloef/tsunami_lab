/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Setup for the reservoir-village problem.
 **/
#include "Reservoir.h"

tsunami_lab::t_real tsunami_lab::setups::Reservoir::getHeight(t_real i_x,
                                                              t_real) const {
    return i_x < 25000 ? 14 : 3.5;
}

tsunami_lab::t_real tsunami_lab::setups::Reservoir::getMomentumX(t_real i_x,
                                                                 t_real) const {
    return i_x < 25000 ? 0 : 0.7;
}

tsunami_lab::t_real tsunami_lab::setups::Reservoir::getMomentumY(t_real,
                                                                 t_real) const {
    return 0;
}
