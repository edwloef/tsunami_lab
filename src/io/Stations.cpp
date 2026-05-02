/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Station observers.
 **/

#include "Stations.h"
#include "../patches/WavePropagation.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ostream>

tsunami_lab::io::Stations::Stations(std::ifstream i_file) {
    nlohmann::json data = nlohmann::json::parse(i_file);

    m_outputFreq = data["outputFreq"];

    for (auto &[_, value] : data["stations"].items()) {
        std::string name = value["name"];
        t_real x = value["x"];
        t_real y = value["y"];
        Station s = Station{name, x, y, std::ofstream(name + ".csv")};
        s.os << "height" << std::endl;
        m_stations.push_back(std::move(s));
    }
}

void tsunami_lab::io::Stations::output(t_real i_dxy, t_real i_simTime,
                                       patches::WavePropagation *i_waveProp) {
    if (i_simTime - m_lastOutput < m_outputFreq) {
        return;
    }

    for (Station &station : m_stations) {
        t_idx x = station.x / i_dxy;
        t_idx y = station.y / i_dxy;
        station.os << i_waveProp->getHeight()[(y * i_waveProp->getStride()) + x]
                   << std::endl;
    }

    m_lastOutput = i_simTime;
}
