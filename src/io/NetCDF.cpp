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

#define nc_try(e)                                                              \
    {                                                                          \
        int retval;                                                            \
        if ((retval = e)) {                                                    \
            std::cout << "Error: " << nc_strerror(retval) << std::endl;        \
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

    int dimids2[3] = {x_dimid, y_dimid};
    int dimids3[3] = {x_dimid, y_dimid, t_dimid};

    nc_try(nc_def_var(ncid, "h", NC_FLOAT, 3, dimids3, &h_varid));

    nc_try(nc_def_var(ncid, "hu", NC_FLOAT, 3, dimids3, &hu_varid));

    nc_try(nc_def_var(ncid, "hv", NC_FLOAT, 3, dimids3, &hv_varid));

    nc_try(nc_def_var(ncid, "b", NC_FLOAT, 2, dimids2, &b_varid));

    t_idx l_start[2] = {0, 0};
    t_idx l_count[2] = {nx, ny};
    ptrdiff_t l_stride[2] = {(ptrdiff_t)stride, 1};

    nc_try(nc_put_vars_float(ncid, b_varid, l_start, l_count, l_stride, i_b));
}

void tsunami_lab::io::NetCDF::write(t_real const *i_h, t_real const *i_hu,
                                    t_real const *i_hv) {
    t_idx l_start[3] = {0, 0, step};
    t_idx l_count[3] = {nx, ny, 1};
    ptrdiff_t l_stride[3] = {(ptrdiff_t)stride, 1, 0};

    nc_try(nc_put_vars_float(ncid, h_varid, l_start, l_count, l_stride, i_h));

    nc_try(nc_put_vars_float(ncid, hu_varid, l_start, l_count, l_stride, i_hu));

    nc_try(nc_put_vars_float(ncid, hv_varid, l_start, l_count, l_stride, i_hv));

    step++;
}
