/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * F-Wave solver for the one-dimensional shallow water equations.
 **/
#ifndef TSUNAMI_LAB_SOLVERS_FWAVE
#define TSUNAMI_LAB_SOLVERS_FWAVE

#include "../constants.h"

namespace tsunami_lab {
namespace solvers {
class FWave;
}
} // namespace tsunami_lab

class tsunami_lab::solvers::FWave {
  private:
    //! gravity
    static t_real constexpr g = t_real(9.80665);
    //! half gravity
    static t_real constexpr g_half = t_real(0.5) * g;

  public:
    /**
     * Computes the net-updates.
     *
     * @param i_hL height of the left side.
     * @param i_hR height of the right side.
     * @param i_huL momentum of the left side.
     * @param i_huR momentum of the right side.
     * @param o_netUpdateL will be set to the net-updates for the left side;
     * 0: height, 1: momentum.
     * @param o_netUpdateR will be set to the net-updates for the right side;
     * 0: height, 1: momentum.
     **/
    static void netUpdates(t_real i_hL, t_real i_hR, t_real i_huL, t_real i_huR,
                           t_real o_netUpdateL[2], t_real o_netUpdateR[2]);
};

#endif
