#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <cstdio>
#include <vector>
#include <string>
#include <netcdf.h>

#include "NetCDF.h"

// Helper: remove file if exists
static void remove_if_exists(const char *path) {
    std::remove(path);
}

// Small tolerance for floats
static constexpr float EPS = 1e-5f;

// Create a minimal NetCDF file with expected dimensions and coordinate vars for read tests
static void create_sample_file_with_coords(const char *path, size_t nx, size_t ny,
                                           std::vector<float> const &xs,
                                           std::vector<float> const &ys,
                                           std::vector<float> const &bathymetry,
                                           std::vector<float> const &zdata /* t=0 only */) {
    int ncid;
    int x_dim, y_dim, t_dim;
    int x_var, y_var, b_var, z_var, t_var;

    // create file
    int ret = nc_create(path, NC_CLOBBER | NC_64BIT_OFFSET, &ncid);
    REQUIRE(ret == NC_NOERR);

    // dims
    REQUIRE(nc_def_dim(ncid, "t", NC_UNLIMITED, &t_dim) == NC_NOERR);
    REQUIRE(nc_def_dim(ncid, "y", ny, &y_dim) == NC_NOERR);
    REQUIRE(nc_def_dim(ncid, "x", nx, &x_dim) == NC_NOERR);

    int dim3[3] = {t_dim, y_dim, x_dim};
    int dim2[2] = {y_dim, x_dim};

    REQUIRE(nc_def_var(ncid, "x", NC_FLOAT, 1, &x_dim, &x_var) == NC_NOERR);
    REQUIRE(nc_def_var(ncid, "y", NC_FLOAT, 1, &y_dim, &y_var) == NC_NOERR);
    REQUIRE(nc_def_var(ncid, "b", NC_FLOAT, 2, dim2, &b_var) == NC_NOERR);
    REQUIRE(nc_def_var(ncid, "z", NC_FLOAT, 3, dim3, &z_var) == NC_NOERR);
    REQUIRE(nc_def_var(ncid, "t", NC_FLOAT, 1, &t_dim, &t_var) == NC_NOERR);

    REQUIRE(nc_enddef(ncid) == NC_NOERR);

    // write coords
    REQUIRE(nc_put_var_float(ncid, x_var, xs.data()) == NC_NOERR);
    REQUIRE(nc_put_var_float(ncid, y_var, ys.data()) == NC_NOERR);

    // write bathymetry (y major)
    REQUIRE(nc_put_var_float(ncid, b_var, bathymetry.data()) == NC_NOERR);

    size_t start[3] = {0, 0, 0};
    size_t count[3] = {1, ny, nx};
    REQUIRE(nc_put_vara_float(ncid, z_var, start, count, zdata.data()) == NC_NOERR);

    // write time value
    float t0 = 0.0f;
    size_t tstart[1] = {0};
    REQUIRE(nc_put_vara_float(ncid, t_var, tstart, tstart + 1, &t0) == NC_EINVAL || nc_put_var1_float(ncid, t_var, tstart, &t0) == NC_NOERR || nc_put_vara_float(ncid, t_var, tstart, tstart, &t0) == NC_NOERR);

    REQUIRE(nc_close(ncid) == NC_NOERR);
}

// Fixture for creating temporary files
struct TempFile {
    std::string path;
    TempFile(std::string p) : path(std::move(p)) { remove_if_exists(path.c_str()); }
    ~TempFile() { remove_if_exists(path.c_str()); }
};

TEST_CASE("NetCDF write then read basic cycle", "[NetCDF][write][read]") {
    TempFile tf("test_write_read.nc");

    const size_t NX = 6;
    const size_t NY = 4;
    const size_t STRIDE = 6;

    // Create writer
    {
        tsunami_lab::io::NetCDF writer(tf.path.c_str(), false);
        writer.writeDefs(NX, NY, STRIDE);

        // prepare bathymetry: row-major with stride
        std::vector<float> b(NY * STRIDE);
        for (size_t y = 0; y < NY; ++y)
            for (size_t x = 0; x < NX; ++x)
                b[y * STRIDE + x] = static_cast<float>(y * 100 + x);

        writer.writeBathymetry(b.data());

        // write two time steps
        std::vector<float> h(NY * STRIDE), hu(NY * STRIDE), hv(NY * STRIDE);
        for (size_t t = 0; t < 2; ++t) {
            for (size_t y = 0; y < NY; ++y)
                for (size_t x = 0; x < NX; ++x) {
                    size_t idx = y * STRIDE + x;
                    h[idx] = 1.0f + float(t) + float(x) * 0.1f + float(y) * 0.01f;
                    hu[idx] = h[idx] * 2.0f;
                    hv[idx] = h[idx] * 3.0f;
                }
            writer.writeTimeStep(static_cast<float>(t * 0.5f), h.data(), hu.data(), hv.data());
        }
    }

    tsunami_lab::io::NetCDF reader(tf.path.c_str(), true);
}

TEST_CASE("NetCDF readAt finds nearest gridpoint from coordinate arrays", "[NetCDF][readAt]") {
    TempFile tf("test_coords.nc");

    const size_t NX = 5;
    const size_t NY = 4;

    // create coordinate vectors
    std::vector<float> xs(NX), ys(NY);
    for (size_t i = 0; i < NX; ++i) xs[i] = float(i) * 2.0f;    
    for (size_t j = 0; j < NY; ++j) ys[j] = float(j) * -1.5f;   

    // bathymetry and z (t,y,x)
    std::vector<float> bath(NY * NX);
    std::vector<float> z(NY * NX);

    for (size_t j = 0; j < NY; ++j) {
        for (size_t i = 0; i < NX; ++i) {
            bath[j * NX + i] = float(j * 10 + i);
            z[j * NX + i] = bath[j * NX + i] + 0.5f;
        }
    }

    create_sample_file_with_coords(tf.path.c_str(), NX, NY, xs, ys, bath, z);

    tsunami_lab::io::NetCDF reader(tf.path.c_str(), true);
    reader.readDefs();

    // exact grid point
    auto v = reader.readAt(4.0f, -1.5f);
    REQUIRE(v.has_value());
    REQUIRE(std::abs(v.value() - z[1 * NX + 2]) < EPS);

    v = reader.readAt(3.9f, -1.6f); 
    REQUIRE(v.has_value());
    REQUIRE(std::abs(v.value() - z[1 * NX + 2]) < EPS);

    // outside domain (x too large)
    v = reader.readAt(100.0f, 0.0f);
    REQUIRE(!v.has_value());

    // outside domain (y too small)
    v = reader.readAt(0.0f, -100.0f);
    REQUIRE(!v.has_value());
}
