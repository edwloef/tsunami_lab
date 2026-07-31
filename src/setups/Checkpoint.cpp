/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Checkpoint setup.
 **/

#include "Checkpoint.h"

tsunami_lab::setups::Checkpoint::Checkpoint(char const *i_nc)
    : nc(i_nc), h(io::NetCDF(i_nc, "h")), hu(io::NetCDF(i_nc, "hu")),
      hv(io::NetCDF(i_nc, "hv")), b(io::NetCDF(i_nc, "b")) {}

tsunami_lab::t_real
tsunami_lab::setups::Checkpoint::getHeight(t_real i_x, t_real i_y) const {
    return h.readAt(i_x, i_y).value();
}

tsunami_lab::t_real
tsunami_lab::setups::Checkpoint::getMomentumX(t_real i_x, t_real i_y) const {
    return hu.readAt(i_x, i_y).value();
}

tsunami_lab::t_real
tsunami_lab::setups::Checkpoint::getMomentumY(t_real i_x, t_real i_y) const {
    return hv.readAt(i_x, i_y).value();
}

tsunami_lab::t_real
tsunami_lab::setups::Checkpoint::getBathymetry(t_real i_x, t_real i_y) const {
    return b.readAt(i_x, i_y).value();
}
