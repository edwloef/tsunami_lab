/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Station observers.
 **/

#ifndef TSUNAMI_LAB_IO_STATIONS_H
#define TSUNAMI_LAB_IO_STATIONS_H

#include "../constants.h"
#include "../patches/WavePropagation.h"
#include <fstream>
#include <string>
#include <vector>

namespace tsunami_lab {
namespace io {
class Stations;
}
} // namespace tsunami_lab

typedef struct {
    std::string name;
    tsunami_lab::t_real x, y;
    std::ofstream os;
} Station;

/**
 * Station observers.
 **/
class tsunami_lab::io::Stations {
  private:
    t_real m_OutputFreq;
    t_real m_lastOutput;
    std::vector<Station> m_stations;

  public:
    /**
     * Constructor.
     *
     * @param i_file configuration file
     **/
    Stations(std::ifstream i_file);

    /**
     * Outputs measurements for all stations.
     *
     * @param i_simTime the current simulation time
     * @param i_waveProp the wave solver
     *
     * @return height at the given point.
     **/
    void output(t_real i_dxy, t_real i_simTime,
                patches::WavePropagation *i_waveProp);

    /**
     * Gets the next output time.
     *
     * @return next output time in seconds.
     **/
    t_real nextOutputTime() {
        return m_lastOutput + m_OutputFreq;
    }
};

#endif
