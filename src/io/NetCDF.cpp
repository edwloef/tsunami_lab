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
    nc_try(nc_open(i_path, NC_NETCDF4, &ncid));

    nc_try(nc_inq_dimid(ncid, "x", &x_dimid));
    nc_try(nc_inq_dimid(ncid, "y", &y_dimid));

    nc_try(nc_inq_dimlen(ncid, x_dimid, &nx));
    nc_try(nc_inq_dimlen(ncid, y_dimid, &ny));

    nc_try(nc_inq_varid(ncid, "x", &x_varid));
    nc_try(nc_inq_varid(ncid, "y", &y_varid));
    nc_try(nc_inq_varid(ncid, "z", &z_varid));

    buf = new t_real[nx + ny];

    nc_try(nc_get_var_float(ncid, x_varid, buf));
    nc_try(nc_get_var_float(ncid, y_varid, buf + nx));
}

template <typename T> T div_ceil(T &lhs, T &rhs) {
    return (lhs + rhs - 1) / rhs;
}

tsunami_lab::io::NetCDF::NetCDF(char const *i_path, t_real i_dxy, t_idx i_nx,
                                t_idx i_ny, t_idx i_stride, t_idx i_k) {
    nc_try(nc_create(i_path, NC_NETCDF4 | NC_NOFILL, &ncid));

    nx = i_nx;
    ny = i_ny;
    stride = i_stride;
    k = i_k;
    knx = div_ceil(nx, k);
    kny = div_ceil(ny, k);
    step = 0;
    buf = new t_real[knx * kny];

    nc_try(nc_def_dim(ncid, "t", NC_UNLIMITED, &t_dimid));
    nc_try(nc_def_dim(ncid, "y", kny, &y_dimid));
    nc_try(nc_def_dim(ncid, "x", knx, &x_dimid));

    int dimids[3] = {t_dimid, y_dimid, x_dimid};

    nc_try(nc_def_var(ncid, "x", NC_FLOAT, 1, dimids + 2, &x_varid));
    nc_try(nc_def_var(ncid, "y", NC_FLOAT, 1, dimids + 1, &y_varid));
    nc_try(nc_def_var(ncid, "b", NC_FLOAT, 2, dimids + 1, &b_varid));
    nc_try(nc_def_var(ncid, "h", NC_FLOAT, 3, dimids, &h_varid));
    nc_try(nc_def_var(ncid, "hu", NC_FLOAT, 3, dimids, &hu_varid));
    nc_try(nc_def_var(ncid, "hv", NC_FLOAT, 3, dimids, &hv_varid));
    nc_try(nc_def_var(ncid, "t", NC_FLOAT, 1, dimids, &t_varid));

    nc_try(nc_def_var_deflate(ncid, x_varid, true, true, 3));
    nc_try(nc_def_var_deflate(ncid, y_varid, true, true, 3));
    nc_try(nc_def_var_deflate(ncid, b_varid, true, true, 3));
    nc_try(nc_def_var_deflate(ncid, h_varid, true, true, 3));
    nc_try(nc_def_var_deflate(ncid, hu_varid, true, true, 3));
    nc_try(nc_def_var_deflate(ncid, hv_varid, true, true, 3));
    nc_try(nc_def_var_deflate(ncid, t_varid, true, true, 3));

    nc_try(nc_put_att_text(ncid, x_varid, "units", strlen("meters"), "meters"));
    nc_try(nc_put_att_text(ncid, y_varid, "units", strlen("meters"), "meters"));
    nc_try(nc_put_att_text(ncid, b_varid, "units", strlen("meters"), "meters"));
    nc_try(nc_put_att_text(ncid, h_varid, "units", strlen("meters"), "meters"));
    nc_try(nc_put_att_text(ncid, t_varid, "units",
                           strlen("seconds since tsunami event"),
                           "seconds since tsunami event"));

    nc_try(nc_enddef(ncid));

    for (t_idx l_ix = 0; l_ix < knx; l_ix++) {
        buf[l_ix] = (l_ix + 0.5) * k * i_dxy;
    }

    nc_try(nc_put_var_float(ncid, x_varid, buf));

    for (t_idx l_iy = 0; l_iy < kny; l_iy++) {
        buf[l_iy] = (l_iy + 0.5) * k * i_dxy;
    }

    nc_try(nc_put_var_float(ncid, y_varid, buf));
}

tsunami_lab::io::NetCDF::~NetCDF() {
    nc_try(nc_close(ncid));
    delete[] buf;
}

void tsunami_lab::io::NetCDF::writeBathymetry(t_real const *i_b) {
    nc_try(nc_put_var_float(ncid, b_varid, downsample(i_b)));
}

void tsunami_lab::io::NetCDF::writeTimeStep(t_real i_simTime, t_real const *i_h,
                                            t_real const *i_hu,
                                            t_real const *i_hv) {
    t_idx start[3] = {step, 0, 0};
    t_idx count[3] = {1, kny, knx};

    nc_try(nc_put_vara_float(ncid, h_varid, start, count, downsample(i_h)));
    nc_try(nc_put_vara_float(ncid, hu_varid, start, count, downsample(i_hu)));
    nc_try(nc_put_vara_float(ncid, hv_varid, start, count, downsample(i_hv)));
    nc_try(nc_put_var1_float(ncid, t_varid, &step, &i_simTime));

    step++;
}

static std::optional<size_t> interpolation_search(float *buf, size_t size,
                                                  float value) {
    size_t left = 0;
    size_t right = size - 1;

    float left_val = buf[0];
    float right_val = buf[size - 1];

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
        float guess_val = buf[guess];

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
    std::optional<size_t> y = interpolation_search(buf + nx, ny, i_y);
    if (!y.has_value()) {
        return std::nullopt;
    }

    std::optional<size_t> x = interpolation_search(buf, nx, i_x);
    if (!x.has_value()) {
        return std::nullopt;
    }

    size_t index[2] = {y.value(), x.value()};

    t_real val;
    nc_try(nc_get_var1_float(ncid, z_varid, index, &val));
    return val;
}
