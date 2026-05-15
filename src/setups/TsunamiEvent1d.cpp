/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Tsunami Event problem.
 **/
#define _USE_MATH_DEFINES

#include "TsunamiEvent1d.h"
#include "../io/Csv.h"

#include <cmath>
#include <fstream>
#include <string>

tsunami_lab::setups::TsunamiEvent1d::TsunamiEvent1d() {
    const std::string filename = "GEBCO_2026_sub_ice_bathy.csv";
    std::ifstream file;
    file.open(filename);
    m_b = tsunami_lab::io::Csv::read<4>(file);
}

tsunami_lab::t_real
tsunami_lab::setups::TsunamiEvent1d::getHeight(t_real i_x, t_real) const {
    t_idx idx = i_x / 440000. * m_b.size();
    return m_b[idx][3] < 0 ? std::max(-m_b[idx][3], 20.) : 0;
}

tsunami_lab::t_real
tsunami_lab::setups::TsunamiEvent1d::getMomentumX(t_real, t_real) const {
    return 0;
}

tsunami_lab::t_real
tsunami_lab::setups::TsunamiEvent1d::getMomentumY(t_real, t_real) const {
    return 0;
}

tsunami_lab::t_real
tsunami_lab::setups::TsunamiEvent1d::getBathymetry(t_real i_x, t_real) const {
    t_idx idx = i_x / 440000. * m_b.size();
    return (m_b[idx][3] < 0 ? std::min(m_b[idx][3], -20.)
                            : std::max(m_b[idx][3], 20.)) +
           (175000 < i_x && i_x < 250000
                ? 10 * std::sin((i_x - 175000.) / 37500. * M_PI + M_PI)
                : 0);
}
