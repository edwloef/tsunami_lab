/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * F-Wave solver for the one-dimensional shallow water equations.
 **/

#ifndef TSUNAMI_LAB_SOLVERS_SOLVER
#define TSUNAMI_LAB_SOLVERS_SOLVER

#include "../constants.h"

namespace tsunami_lab {
namespace solvers {
class Solver;
}
} // namespace tsunami_lab

class tsunami_lab::solvers::Solver {
  public:
    /**
     * Virtual destructor for base class.
     **/
    virtual ~Solver() {};

    /**
     * Computes the net-updates.
     *
     * @param i_hL height of the left side.
     * @param i_hR height of the right side.
     * @param i_huL momentum of the left side.
     * @param i_huR momentum of the right side.
     * @param i_bL bathymetry on the left side.
     * @param i_bR bathymetry on the right side.
     * @param o_netUpdateL will be set to the net-updates for the left side; 0:
     * height, 1: momentum.
     * @param o_netUpdateR will be set to the net-updates for the right side; 0:
     * height, 1: momentum.
     **/
    virtual void netUpdates(t_real i_hL, t_real i_hR, t_real i_huL,
                            t_real i_huR, t_real i_bL, t_real i_bR,
                            t_real o_netUpdateL[2], t_real o_netUpdateR[2]) = 0;
};

#endif
