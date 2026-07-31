/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Checkpoint setup.
 **/
#ifndef TSUNAMI_LAB_SETUPS_CHECKPOINT_H
#define TSUNAMI_LAB_SETUPS_CHECKPOINT_H

#include "../io/NetCDF.h"
#include "Setup.h"

namespace tsunami_lab {
namespace setups {
class Checkpoint;
}
} // namespace tsunami_lab

/**
 * Checkpoint setup.
 **/
class tsunami_lab::setups::Checkpoint : public Setup {
  private:
    char const *nc;
    io::NetCDF h, hu, hv, b;

  public:
    /**
     * Constructor.
     **/
    Checkpoint(char const *i_nc);

    /**
     * Gets the water height at a given point.
     *
     * @return height at the given point.
     **/
    t_real getHeight(t_real, t_real) const;

    /**
     * Gets the momentum in x-direction.
     *
     * @return momentum in x-direction.
     **/
    t_real getMomentumX(t_real, t_real) const;

    /**
     * Gets the momentum in y-direction.
     *
     * @return momentum in y-direction.
     **/
    t_real getMomentumY(t_real, t_real) const;

    /**
     * Gets the bathymetry.
     *
     * @return bathymetry.
     **/
    t_real getBathymetry(t_real, t_real) const;

    t_real minX() const {
        return b.minX();
    }

    t_real minY() const {
        return b.minY();
    }

    t_real maxX() const {
        return b.maxX();
    }

    t_real maxY() const {
        return b.maxY();
    }

    t_real t() const {
        int ncid, t_varid;
        float t;

        nc_try(nc_open(nc, NC_NETCDF4, &ncid));
        nc_try(nc_inq_varid(ncid, "t", &t_varid));
        nc_try(nc_get_var1_float(ncid, t_varid, NULL, &t));
        nc_try(nc_close(ncid));

        return t;
    }

    t_idx step() const {
        int ncid, step_varid, step;

        nc_try(nc_open(nc, NC_NETCDF4, &ncid));
        nc_try(nc_inq_varid(ncid, "step", &step_varid));
        nc_try(nc_get_var1_int(ncid, step_varid, NULL, &step));
        nc_try(nc_close(ncid));

        return step;
    }
};

#endif
