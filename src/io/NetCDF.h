/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * IO-routines for writing a snapshot as NetCDF.
 **/
#ifndef TSUNAMI_LAB_IO_NETCDF
#define TSUNAMI_LAB_IO_NETCDF

#include "../constants.h"
#include <algorithm>
#include <optional>

namespace tsunami_lab {
namespace io {
class NetCDF;
}
} // namespace tsunami_lab

class tsunami_lab::io::NetCDF {
  private:
    t_idx nx, ny, stride, k, knx, kny, step;
    int ncid, x_dimid, y_dimid, t_dimid, x_varid, y_varid, z_varid, b_varid,
        h_varid, hu_varid, hv_varid, t_varid;
    float *buf;

    float const *downsample(t_real const *i_v, t_real *buf) {
        t_real scale = t_real(1) / (k * k);

        for (t_idx oy = 0; oy < kny; oy++) {
            t_idx iy = oy * k;
            t_idx my = std::min(iy + k, ny);

            for (t_idx ox = 0; ox < knx; ox++) {
                t_idx ix = ox * k;
                t_idx mx = std::min(ix + k, nx);

                t_real sum = 0;

                for (t_idx iy_ = iy; iy_ < my; iy_++) {
                    for (t_idx ix_ = ix; ix_ < mx; ix_++) {
                        sum += i_v[iy_ * stride + ix_];
                    }
                }

                buf[oy * knx + ox] = sum * scale;
            }
        }

        return buf;
    }

  public:
    /**
     * Constructor.
     *
     * @param i_path path to be read from.
     */
    NetCDF(char const *i_path);

    /**
     * Constructor.
     *
     * @param i_path path to be written to
     * @param i_dxy cell width in x- and y-direction
     * @param i_nx number of cells in x-direction
     * @param i_ny number of cells in y-direction
     * @param i_stride stride of the data arrays in y-direction
     * @param i_k coarse output size
     */
    NetCDF(char const *i_path, t_real i_dxy, t_idx i_nx, t_idx i_ny,
           t_idx i_stride, t_idx i_k = 1);

    /**
     * Destructor.
     */
    ~NetCDF();

    /**
     * Writes the bathymetry as NetCDF.
     *
     * @param i_b bathymetry
     **/
    void writeBathymetry(t_real const *i_b);

    /**
     * Writes the time step as NetCDF.
     *
     * @param i_h water height of the cells
     * @param i_hu momentum in x-direction of the cells
     * @param i_hv momentum in y-direction of the cells
     **/
    void writeTimeStep(t_real i_simTime, t_real const *i_h, t_real const *i_hu,
                       t_real const *i_hv);

    /**
     * Writes the checkpoint as NetCDF.
     *
     * @param i_path path to be written to
     * @param i_nx number of cells in x-direction
     * @param i_ny number of cells in y-direction
     * @param i_stride stride of the data arrays in y-direction
     * @param i_b bathymetry
     * @param i_h water height of the cells
     * @param i_hu momentum in x-direction of the cells
     * @param i_hv momentum in y-direction of the cells
     **/
    void writeCheckpoint(char const *i_path, t_real const *i_b,
                         t_real const *i_h, t_real const *i_hu,
                         t_real const *i_hv);

    /**
     * Reads the value of the z variable at the given position.
     *
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return value of z.
     **/
    std::optional<t_real> readAt(t_real i_x, t_real i_y) const;

    t_real minX() const {
        return std::min(buf[0], buf[nx - 1]);
    }

    t_real minY() const {
        return std::min(buf[nx], buf[nx + ny - 1]);
    }

    t_real maxX() const {
        return std::max(buf[0], buf[nx - 1]);
    }

    t_real maxY() const {
        return std::max(buf[nx], buf[nx + ny - 1]);
    }
};

#endif
