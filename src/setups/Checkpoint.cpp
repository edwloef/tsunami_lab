/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Checkpoint setup.
 **/

#include "CheckPoint.h"
#include <netcdf.h>
#include "NetCDF.h"
#include <algorithm>

tsunami_lab::setups::CheckPoint::CheckPoint(char const *i_d,
                                                    char const *i_b)
    : d(io::NetCDF(i_d)), b(io::NetCDF(i_b)) {}

tsunami_lab::t_real
tsunami_lab::setups::CheckPoint::getHeight(t_real i_x, t_real i_y) const {
    std::filesystem::path p = "checkpoint.nc";
    int ncid = -1;
    nc_try(nc_open(p, NC_WRITE, &ncid));
    int h_varid;
    float height;

    size_t index[2] = {0,0};

    nc_try(nc_get_var1_float(ncid, &h_varid, index, &height));

    nc_try(nc_close(ncid));

    return height;
}

tsunami_lab::t_real
tsunami_lab::setups::CheckPoint::getMomentumX(t_real, t_real) const {
    std::filesystem::path p = "checkpoint.nc";
    int ncid = -1;
    nc_try(nc_open(p, NC_WRITE, &ncid));
    int hu_varid;
    float mom_x;

    size_t index[2] = {0,0};

    nc_try(nc_get_var1_float(ncid, &hu_varid, index, &mom_x));

    nc_try(nc_close(ncid));

    return mom_x;
}

tsunami_lab::t_real
tsunami_lab::setups::CheckPoint::getMomentumY(t_real, t_real) const {
    std::filesystem::path p = "checkpoint.nc";
    int ncid = -1;
    nc_try(nc_open(p, NC_WRITE, &ncid));
    int hv_varid;
    float mom_y;

    size_t index[2] = {0,0};

    nc_try(nc_get_var1_float(ncid, &hv_varid, index, &mom_y));

    nc_try(nc_close(ncid));

    return mom_y;
}

tsunami_lab::t_real
tsunami_lab::setups::CheckPoint::getBathymetry(t_real i_x,
                                                   t_real i_y) const {
    std::filesystem::path p = "checkpoint.nc";
    int ncid = -1;
    nc_try(nc_open(p, NC_WRITE, &ncid));
    int b
    _varid;
    float bathy;

    size_t index[2] = {0,3};

    nc_try(nc_get_var1_float(ncid, &b_varid, index, &bathy));

    nc_try(nc_close(ncid));

    return bathy;
}
