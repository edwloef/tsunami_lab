/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * IO-routines for writing a snapshot as NetCDF.
 **/

#include <assert.h>
#include <cstring>
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

tsunami_lab::io::NetCDF::NetCDF(char const *i_path) {
    nc_try(nc_create(i_path, NC_CLOBBER, &ncid));
}

tsunami_lab::io::NetCDF::~NetCDF() {
    nc_try(nc_close(ncid));
}

void tsunami_lab::io::NetCDF::writeDefs(t_idx i_nx, t_idx i_ny,
                                        t_idx i_stride) {
    nx = i_nx;
    ny = i_ny;
    stride = i_stride;

    nc_try(nc_def_dim(ncid, "t", NC_UNLIMITED, &t_dimid));
    nc_try(nc_def_dim(ncid, "y", ny, &y_dimid));
    nc_try(nc_def_dim(ncid, "x", nx, &x_dimid));

    int dimids[3] = {t_dimid, y_dimid, x_dimid};

    nc_try(nc_def_var(ncid, "h", NC_FLOAT, 3, dimids, &h_varid));
    nc_try(nc_def_var(ncid, "hu", NC_FLOAT, 3, dimids, &hu_varid));
    nc_try(nc_def_var(ncid, "hv", NC_FLOAT, 3, dimids, &hv_varid));
    nc_try(nc_def_var(ncid, "b", NC_FLOAT, 2, &dimids[1], &b_varid));

    nc_try(nc_put_att_text(ncid, h_varid, "units", strlen("meters"), "meters"));

    nc_try(nc_enddef(ncid));
}

void tsunami_lab::io::NetCDF::writeBathymetry(t_real const *i_b) {
    t_idx l_count[2] = {1, nx};

    for (t_idx l_iy = 0; l_iy < ny; l_iy++) {
        t_idx l_start[2] = {l_iy, 0};

        nc_try(nc_put_vara_float(ncid, b_varid, l_start, l_count,
                                 i_b + l_iy * stride));
    }
}

void tsunami_lab::io::NetCDF::writeTimeStep(t_real const *i_h,
                                            t_real const *i_hu,
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

void tsunami_lab::io::NetCDF::readDefs() {
    nc_try(nc_inq_dimid(ncid, "x", &x_dimid));
    nc_try(nc_inq_dimid(ncid, "y", &y_dimid));

    nc_try(nc_inq_dimlen(ncid, x_dimid, &nx));
    nc_try(nc_inq_dimlen(ncid, y_dimid, &ny));

    nc_try(nc_inq_varid(ncid, "x", &x_varid));
    nc_try(nc_inq_varid(ncid, "y", &y_varid));
    nc_try(nc_inq_varid(ncid, "z", &z_varid));

    t_real *x = new t_real[nx];

    nc_try(nc_get_var_float(ncid, x_varid, x));

    for (t_idx i = 0; i < nx; i++) {
        y_coords[x[i]] = i;
    }

    delete[] x;

    t_real *y = new t_real[ny];

    nc_try(nc_get_var_float(ncid, y_varid, y));

    for (t_idx i = 0; i < nx; i++) {
        y_coords[y[i]] = i;
    }

    delete[] y;
}

template <typename K, typename V> V nearest(std::map<K, V> &map, K num) {
    assert(!map.empty());

    auto it = map.upper_bound(num);

    auto k_l = *(--it++);

    if (it == std::end(map)) {
        return k_l.second;
    }

    auto k_r = *it;

    return k_l.first - num > num - k_r.first ? k_r.second : k_l.second;
}

tsunami_lab::t_real tsunami_lab::io::NetCDF::readAt(t_real i_x, t_real i_y) {
    size_t index[2] = {(size_t)nearest(x_coords, i_x),
                       (size_t)nearest(y_coords, i_y)};

    t_real val;
    nc_try(nc_get_var1_float(ncid, z_varid, index, &val));
    return val;
}
