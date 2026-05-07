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

namespace tsunami_lab {
namespace io {
class NetCDF;
}
} // namespace tsunami_lab

class tsunami_lab::io::NetCDF {
  private:
    t_idx nx, ny, stride, step;
    int ncid, x_dimid, y_dimid, t_dimid, h_varid, hu_varid, hv_varid, b_varid;

  public:
    /**
     * Constructor.
     *
     * @param i_path path to be written to.
     * @param i_nx number of cells in x-direction
     * @param i_ny number of cells in y-direction
     */
    NetCDF(char const *i_path, t_idx i_nx, t_idx i_ny, t_idx i_stride,
           t_real const *i_b);

    /**
     * Writes the data as NetCDF.
     *
     * @param i_h water height of the cells; optional
     * @param i_hu momentum in x-direction of the cells
     * @param i_hv momentum in y-direction of the cells
     **/
    void write(t_real const *i_h, t_real const *i_hu, t_real const *i_hv);
};

#endif
