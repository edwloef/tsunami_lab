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
#include <optional>

namespace tsunami_lab {
namespace io {
class NetCDF;
}
} // namespace tsunami_lab

class tsunami_lab::io::NetCDF {
  private:
    t_idx nx, ny, stride, k, step;
    int ncid, x_dimid, y_dimid, t_dimid, x_varid, y_varid, z_varid, h_varid,
        hu_varid, hv_varid, b_varid, t_varid;
    t_real xs, xe, ys, ye;
    mutable t_idx lxi, lyi;
    mutable t_real lx, ly, lv;

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
     * @param i_nx number of cells in x-direction
     * @param i_ny number of cells in y-direction
     * @param i_stride stride of the data arrays in y-direction
     * @param i_k coarse output size
     */
    NetCDF(char const *i_path, t_idx i_nx, t_idx i_ny, t_idx i_stride,
           t_idx i_k = 1);

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
     * Reads the value of the z variable at the given position.
     *
     * @param i_x x-coordinate of the queried point.
     * @param i_y y-coordinate of the queried point.
     * @return value of z.
     **/
    std::optional<t_real> readAt(t_real i_x, t_real i_y) const;
};

#endif
