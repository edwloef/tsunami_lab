/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * One-dimensional rare-rare problem.
 **/
#include "RareRare1d.h"

tsunami_lab::setups::RareRare1d::RareRare1d(t_real i_height, t_real i_momentum,
                                            t_real i_location) {
    m_height = i_height;
    m_momentum = i_momentum;
    m_location = i_location;
}

tsunami_lab::t_real tsunami_lab::setups::RareRare1d::getHeight(t_real,
                                                               t_real) const {
    return m_height;
}

tsunami_lab::t_real
tsunami_lab::setups::RareRare1d::getMomentumX(t_real i_x, t_real) const {
    return i_x > m_location ? m_momentum : -m_momentum;
}

tsunami_lab::t_real
tsunami_lab::setups::RareRare1d::getMomentumY(t_real, t_real) const {
    return 0;
}