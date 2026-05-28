/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * IO-routines for writing a snapshot as NetCDF.
 **/

#include <algorithm>
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
    nc_try(nc_open(i_path, NC_NOWRITE | NC_64BIT_OFFSET, &ncid));

    nc_try(nc_inq_dimid(ncid, "x", &x_dimid));
    nc_try(nc_inq_dimid(ncid, "y", &y_dimid));

    nc_try(nc_inq_dimlen(ncid, x_dimid, &nx));
    nc_try(nc_inq_dimlen(ncid, y_dimid, &ny));

    nc_try(nc_inq_varid(ncid, "x", &x_varid));
    nc_try(nc_inq_varid(ncid, "y", &y_varid));
    nc_try(nc_inq_varid(ncid, "z", &z_varid));

    size_t si = 0, ei;

    ei = nx - 1;
    nc_try(nc_get_var1_float(ncid, x_varid, &si, &xs));
    nc_try(nc_get_var1_float(ncid, x_varid, &ei, &xe));

    lxi = si;
    lx = xs;

    ei = ny - 1;
    nc_try(nc_get_var1_float(ncid, y_varid, &si, &ys));
    nc_try(nc_get_var1_float(ncid, y_varid, &ei, &ye));

    lyi = si;
    ly = ys;
}

tsunami_lab::io::NetCDF::NetCDF(char const *i_path, t_idx i_nx, t_idx i_ny,
                                t_idx i_stride) {
    nc_try(nc_create(i_path, NC_CLOBBER | NC_64BIT_OFFSET, &ncid));

    nx = i_nx;
    ny = i_ny;
    stride = i_stride;

    nc_try(nc_set_fill(ncid, NC_NOFILL, NULL));

    nc_try(nc_def_dim(ncid, "t", NC_UNLIMITED, &t_dimid));
    nc_try(nc_def_dim(ncid, "y", ny, &y_dimid));
    nc_try(nc_def_dim(ncid, "x", nx, &x_dimid));

    int dimids[3] = {t_dimid, y_dimid, x_dimid};

    nc_try(nc_def_var(ncid, "h", NC_FLOAT, 3, dimids, &h_varid));
    nc_try(nc_def_var(ncid, "hu", NC_FLOAT, 3, dimids, &hu_varid));
    nc_try(nc_def_var(ncid, "hv", NC_FLOAT, 3, dimids, &hv_varid));
    nc_try(nc_def_var(ncid, "b", NC_FLOAT, 2, &dimids[1], &b_varid));
    nc_try(nc_def_var(ncid, "t", NC_FLOAT, 1, dimids, &t_varid));

    nc_try(nc_put_att_text(ncid, h_varid, "units", strlen("meters"), "meters"));
    nc_try(nc_put_att_text(ncid, b_varid, "units", strlen("meters"), "meters"));
    nc_try(nc_put_att_text(ncid, t_varid, "units",
                           strlen("seconds since tsunami event"),
                           "seconds since tsunami event"));

    nc_try(nc_enddef(ncid));
}

tsunami_lab::io::NetCDF::~NetCDF() {
    nc_try(nc_close(ncid));
}

void tsunami_lab::io::NetCDF::writeBathymetry(t_real const *i_b) {
    t_idx l_count[2] = {1, nx};

    for (t_idx l_iy = 0; l_iy < ny; l_iy++) {
        t_idx l_start[2] = {l_iy, 0};

        nc_try(nc_put_vara_float(ncid, b_varid, l_start, l_count,
                                 i_b + l_iy * stride));
    }
}

void tsunami_lab::io::NetCDF::writeTimeStep(t_real i_simTime, t_real const *i_h,
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

    nc_try(nc_put_var1_float(ncid, t_varid, &step, &i_simTime));

    step++;
}

static std::optional<size_t> nc_find_index(int ncid, int varid, size_t size,
                                           float left_val, float right_val,
                                           float value) {
    size_t left = 0;
    size_t right = size - 1;

    bool increasing = right_val > left_val;

    if (increasing ? (value < left_val || value > right_val)
                   : (value > left_val || value < right_val)) {
        return std::nullopt;
    }

    while (right - left > 1) {
        if (left_val == right_val) {
            break;
        }

        float ratio = (value - left_val) / (right_val - left_val);

        size_t guess = left + (right - left) * ratio;
        guess = std::clamp(guess, left + 1, right - 1);

        float guess_val;
        nc_try(nc_get_var1_float(ncid, varid, &guess, &guess_val));

        if (increasing ? guess_val < value : guess_val > value) {
            left = guess;
            left_val = guess_val;
        } else {
            right = guess;
            right_val = guess_val;
        }
    }

    return std::abs(value - left_val) <= std::abs(right_val - value) ? left
                                                                     : right;
}

std::optional<tsunami_lab::t_real>
tsunami_lab::io::NetCDF::readAt(t_real i_x, t_real i_y) const {
    std::optional<size_t> y =
        i_y == ly ? lyi : nc_find_index(ncid, y_varid, ny, ys, ye, i_y);
    if (!y.has_value()) {
        return std::nullopt;
    }

    ly = i_y;
    lyi = y.value();

    std::optional<size_t> x =
        i_x == lx ? lxi : nc_find_index(ncid, x_varid, nx, xs, xe, i_x);
    if (!x.has_value()) {
        return std::nullopt;
    }

    lx = i_x;
    lxi = x.value();

    size_t index[2] = {y.value(), x.value()};

    t_real val;
    nc_try(nc_get_var1_float(ncid, z_varid, index, &val));
    return val;
}
