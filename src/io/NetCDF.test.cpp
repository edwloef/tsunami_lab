/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Unit tests of the NetCDF-interface.
 **/

#include "NetCDF.h"
#include <catch2/catch.hpp>
#include <cstdio>
#include <netcdf.h>
#include <vector>

// Create a minimal NetCDF file with expected dimensions and coordinate vars for
// read test
static void create_read_test_input(const char *path, size_t nx, size_t ny,
                                   std::vector<float> const &x,
                                   std::vector<float> const &y,
                                   std::vector<float> const &z) {
    int ncid;
    int x_dim, y_dim;
    int x_var, y_var, z_var;

    // create file
    REQUIRE(nc_create(path, NC_CLOBBER | NC_64BIT_OFFSET, &ncid) == NC_NOERR);

    // dims
    REQUIRE(nc_def_dim(ncid, "y", ny, &y_dim) == NC_NOERR);
    REQUIRE(nc_def_dim(ncid, "x", nx, &x_dim) == NC_NOERR);

    int dim2[2] = {y_dim, x_dim};

    REQUIRE(nc_def_var(ncid, "x", NC_FLOAT, 1, &x_dim, &x_var) == NC_NOERR);
    REQUIRE(nc_def_var(ncid, "y", NC_FLOAT, 1, &y_dim, &y_var) == NC_NOERR);
    REQUIRE(nc_def_var(ncid, "z", NC_FLOAT, 2, dim2, &z_var) == NC_NOERR);

    REQUIRE(nc_enddef(ncid) == NC_NOERR);

    // write coords
    REQUIRE(nc_put_var_float(ncid, x_var, x.data()) == NC_NOERR);
    REQUIRE(nc_put_var_float(ncid, y_var, y.data()) == NC_NOERR);
    REQUIRE(nc_put_var_float(ncid, z_var, z.data()) == NC_NOERR);

    REQUIRE(nc_close(ncid) == NC_NOERR);
}

TEST_CASE("Test the NetCDF writer", "[NetCDFWriter]") {
    const char *PATH = "test_write.nc";

    const size_t NX = 6;
    const size_t NY = 4;
    const size_t STRIDE = 6;

    {
        tsunami_lab::io::NetCDF writer(PATH, false);
        writer.writeDefs(NX, NY, STRIDE);

        // prepare bathymetry: row-major with stride
        std::vector<float> b(NY * STRIDE);
        for (size_t y = 0; y < NY; ++y) {
            for (size_t x = 0; x < NX; ++x) {
                b[y * STRIDE + x] = y * 100 + x;
            }
        }

        writer.writeBathymetry(b.data());

        // write two time steps
        std::vector<float> h(NY * STRIDE), hu(NY * STRIDE), hv(NY * STRIDE);
        for (size_t t = 0; t < 2; ++t) {
            for (size_t y = 0; y < NY; ++y) {
                for (size_t x = 0; x < NX; ++x) {
                    size_t idx = y * STRIDE + x;
                    h[idx] = 1 + t + x * 0.1 + y * 0.01;
                    hu[idx] = h[idx] * 2;
                    hv[idx] = h[idx] * 3;
                }
            }

            writer.writeTimeStep(t * 0.5, h.data(), hu.data(), hv.data());
        }
    }

    std::remove(PATH);
}

TEST_CASE("Test the NetCDF reader", "[NetCDFReader]") {
    const char *const PATH = "test_read.nc";
    const size_t NX = 5;
    const size_t NY = 4;

    // create coordinate vectors
    std::vector<float> x(NX), y(NY), z(NY * NX);
    for (size_t i = 0; i < NX; ++i) {
        x[i] = i * 2;
    }
    for (size_t j = 0; j < NY; ++j) {
        y[j] = j * -1.5;
    }
    for (size_t j = 0; j < NY; ++j) {
        for (size_t i = 0; i < NX; ++i) {
            z[j * NX + i] = j * 10 + i + 0.5;
        }
    }

    create_read_test_input(PATH, NX, NY, x, y, z);

    tsunami_lab::io::NetCDF reader(PATH, true);
    reader.readDefs();

    // exact grid point
    auto v = reader.readAt(4.0, -1.5);
    REQUIRE(v.has_value());
    REQUIRE(v.value() == z[NX + 2]);

    v = reader.readAt(3.9, -1.6);
    REQUIRE(v.has_value());
    REQUIRE(v.value() == z[NX + 2]);

    // outside domain (x too large)
    v = reader.readAt(100, 0);
    REQUIRE(!v.has_value());

    // outside domain (y too small)
    v = reader.readAt(0, -100);
    REQUIRE(!v.has_value());

    std::remove(PATH);
}
