/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Entry-point for simulations.
 **/
#include "io/NetCDF.h"
#include "io/Stations.h"
#include "patches/WavePropagation2d.h"
#include "setups/ArtificialTsunami2d.h"
#include "setups/TsunamiEvent2d.h"
#include "solvers/FWave.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <ostream>

int main(int i_argc, char *i_argv[]) {
    tsunami_lab::t_real l_maxTime, l_dxy, l_sx, l_sy, l_ex, l_ey;
    char const *l_displ, *l_bathy, *l_stations, *l_solution;

    std::cout << "####################################" << std::endl;
    std::cout << "### Tsunami Lab                  ###" << std::endl;
    std::cout << "###                              ###" << std::endl;
    std::cout << "### https://scalable.uni-jena.de ###" << std::endl;
    std::cout << "####################################" << std::endl;

    if (i_argc < 7) {
        std::cerr << "invalid number of arguments, usage:\n  " << *i_argv
                  << " SIM_TIME CELL_SIZE DOMAIN_START_X "
                     "DOMAIN_START_Y DOMAIN_END_X DOMAIN_END_Y [DISPL.nc "
                     "[BATHY.nc [STATIONS.json [SOLUTION.nc]]]]"
                  << std::endl;
        return EXIT_FAILURE;
    }

    i_argv++;

    l_maxTime = std::stod(*i_argv++);
    l_dxy = std::stod(*i_argv++);
    l_sx = std::stod(*i_argv++);
    l_sy = std::stod(*i_argv++);
    l_ex = std::stod(*i_argv++);
    l_ey = std::stod(*i_argv++);

    l_displ = *i_argv ? *i_argv++ : "displ.nc";
    l_bathy = *i_argv ? *i_argv++ : "bathy.nc";
    l_stations = *i_argv ? *i_argv++ : "stations.json";
    l_solution = *i_argv ? *i_argv++ : "solution.nc";

    if (l_dxy <= 0) {
        std::cerr << "invalid cell size" << std::endl;
        return EXIT_FAILURE;
    }

    if (l_ex - l_sx < l_dxy || l_ey - l_sy < l_dxy) {
        std::cerr << "invalid computational domain" << std::endl;
        return EXIT_FAILURE;
    }

    tsunami_lab::t_idx l_nx = (l_ex - l_sx) / l_dxy;
    tsunami_lab::t_idx l_ny = (l_ey - l_sy) / l_dxy;

    std::cout << "runtime configuration:\n  number of cells in x-direction: "
              << l_nx << "\n  number of cells in y-direction: " << l_ny
              << std::endl;

    // construct setup
    tsunami_lab::setups::Setup *l_setup;
    l_setup = new tsunami_lab::setups::TsunamiEvent2d(l_displ, l_bathy);
    // construct solver
    tsunami_lab::patches::WavePropagation *l_waveProp;
    l_waveProp = new tsunami_lab::patches::WavePropagation2d(l_nx, l_ny);
    // construct stations
    tsunami_lab::io::Stations stations(std::ifstream{l_stations});
    // construct netcdf writer
    tsunami_lab::io::NetCDF netcdf(l_solution, l_dxy, l_nx, l_ny,
                                   l_waveProp->getStride());

    // maximum observed height in the setup
    tsunami_lab::t_real l_hMax =
        std::numeric_limits<tsunami_lab::t_real>::lowest();

    // set up solver
    for (tsunami_lab::t_idx l_cy = 0; l_cy < l_ny; l_cy++) {
        tsunami_lab::t_real l_y = l_sy + l_cy * l_dxy;

        for (tsunami_lab::t_idx l_cx = 0; l_cx < l_nx; l_cx++) {
            tsunami_lab::t_real l_x = l_sx + l_cx * l_dxy;

            // get initial values of the setup
            tsunami_lab::t_real l_h = l_setup->getHeight(l_x, l_y);
            tsunami_lab::t_real l_hu = l_setup->getMomentumX(l_x, l_y);
            tsunami_lab::t_real l_hv = l_setup->getMomentumY(l_x, l_y);
            tsunami_lab::t_real l_b = l_setup->getBathymetry(l_x, l_y);

            // set initial values in wave propagation solver
            l_waveProp->setHeight(l_cx, l_cy, l_h);
            l_waveProp->setMomentumX(l_cx, l_cy, l_hu);
            l_waveProp->setMomentumY(l_cx, l_cy, l_hv);
            l_waveProp->setBathymetry(l_cx, l_cy, l_b);

            l_hMax = std::max(l_h, l_hMax);
        }
    }

    delete l_setup;

    // derive maximum wave speed in setup; the momentum is ignored
    tsunami_lab::t_real l_speedMax = std::sqrt(9.80665 * l_hMax);

    // derive constant time step; changes at simulation time are ignored
    tsunami_lab::t_real l_dt = 0.5 * l_dxy / l_speedMax;

    // derive scaling for a time step
    tsunami_lab::t_real l_scaling = l_dt / l_dxy;

    // set up time and print control
    tsunami_lab::t_idx l_timeStep = 0;
    tsunami_lab::t_real l_simTime = 0;

    std::cout << "  time step length: " << l_dt << " seconds" << std::endl;

    netcdf.writeBathymetry(l_waveProp->getBathymetry());

    std::cout << "entering time loop..." << std::endl;

    tsunami_lab::solvers::FWave solver;

    auto dur = std::chrono::duration<double>(0);

    // iterate over time
    while (l_simTime < l_maxTime) {
        if (l_simTime >= stations.nextOutputTime()) {
            std::cout << "  " << l_timeStep << " time steps, " << l_simTime
                      << " seconds" << std::endl;

            netcdf.writeTimeStep(l_simTime, l_waveProp->getHeight(),
                                 l_waveProp->getMomentumX(),
                                 l_waveProp->getMomentumY());

            stations.output(l_dxy, l_simTime, l_waveProp);
        }

        auto now = std::chrono::high_resolution_clock::now();
        l_waveProp->setGhostOutflow();
        l_waveProp->timeStep(l_scaling, &solver);
        dur += std::chrono::high_resolution_clock::now() - now;

        l_timeStep++;
        l_simTime += l_dt;
    }

    std::cout << "finished time loop\n  simulation time per time step: "
              << dur.count() << " ms" << std::endl;

    delete l_waveProp;
}
