/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * IO-routines for writing a snapshot as NetCDF.
 **/

#include <iostream>
#include <netcdf.h>
#include <stdlib.h>

#include "NetCDF.h"

#define nc_try(expr)                                                           \
    {                                                                          \
        int err;                                                               \
        if ((err = expr)) {                                                    \
            std::cout << __FILE__ << ":" << __LINE__ << ": "                   \
                      << nc_strerror(err) << std::endl;                        \
            exit(2);                                                           \
        }                                                                      \
    }

tsunami_lab::io::NetCDF::NetCDF(char const *i_path, t_idx i_nx, t_idx i_ny,
                                t_idx i_stride, t_real const *i_b) {
    nx = i_nx;
    ny = i_ny;
    stride = i_stride;
    step = 0;

    nc_try(nc_create(i_path, NC_CLOBBER, &ncid));

    nc_try(nc_def_dim(ncid, "t", NC_UNLIMITED, &t_dimid));
    nc_try(nc_def_dim(ncid, "y", ny, &y_dimid));
    nc_try(nc_def_dim(ncid, "x", nx, &x_dimid));

    int dimids[3] = {t_dimid, y_dimid, x_dimid};

    nc_try(nc_def_var(ncid, "h", NC_FLOAT, 3, dimids, &h_varid));
    nc_try(nc_def_var(ncid, "hu", NC_FLOAT, 3, dimids, &hu_varid));
    nc_try(nc_def_var(ncid, "hv", NC_FLOAT, 3, dimids, &hv_varid));

    nc_try(nc_def_var(ncid, "b", NC_FLOAT, 2, &dimids[1], &b_varid));

    nc_try(nc_enddef(ncid));

    t_idx l_count[2] = {1, nx};

    for (t_idx l_iy = 0; l_iy < ny; l_iy++) {
        t_idx l_start[2] = {l_iy, 0};

        nc_try(nc_put_vara_float(ncid, b_varid, l_start, l_count,
                                 i_b + l_iy * stride));
    }
}

tsunami_lab::io::NetCDF::~NetCDF() {
    nc_try(nc_close(ncid));
}

void tsunami_lab::io::NetCDF::write(t_real const *i_h, t_real const *i_hu,
                                    t_real const *i_hv) {
    t_idx l_count[3] = {1, 1, nx};

    for (t_idx l_iy = 0; l_iy < ny; l_iy++) {
        t_idx l_start[3] = {step, l_iy, 0};

        nc_try(nc_put_vara_float(ncid, h_varid, l_start, l_count,
                                 i_h + l_iy * stride));
        nc_try(nc_put_vara_float(ncid, hu_varid, l_start, l_count,
                                 i_hu + l_iy * stride));
        nc_try(nc_put_vara_float(ncid, hv_varid, l_start, l_count,
                                 i_hv + l_iy * stride));
    }

    step++;
}
