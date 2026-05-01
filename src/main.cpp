/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Entry-point for simulations.
 **/
#include "io/Csv.h"
#include "io/Stations.h"
#include "patches/WavePropagation2d.h"
#include "setups/DamBreak2d.h"
#include "solvers/FWave.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <ostream>

int main(int i_argc, char *i_argv[]) {
    // number of cells in x- and y-direction
    tsunami_lab::t_idx l_nx = 0;
    tsunami_lab::t_idx l_ny = 0;

    // set cell size
    tsunami_lab::t_real l_dxy = 1;

    std::cout << "####################################" << std::endl;
    std::cout << "### Tsunami Lab                  ###" << std::endl;
    std::cout << "###                              ###" << std::endl;
    std::cout << "### https://scalable.uni-jena.de ###" << std::endl;
    std::cout << "####################################" << std::endl;

    if (i_argc != 3) {
        std::cerr << "invalid number of arguments, usage:" << std::endl;
        std::cerr << "  ./build/tsunami_lab N_CELLS_X N_CELLS_Y" << std::endl;
        std::cerr << "where N_CELLS_X is the number of cells in x-direction"
                  << std::endl;
        std::cerr << "and N_CELLS_Y is the number of cells in y-direction"
                  << std::endl;
        return EXIT_FAILURE;
    } else {
        l_nx = atoi(i_argv[1]);
        if (l_nx < 1) {
            std::cerr << "invalid number of cells in x direction" << std::endl;
            return EXIT_FAILURE;
        }
        l_ny = atoi(i_argv[2]);
        if (l_ny < 1) {
            std::cerr << "invalid number of cells in y direction" << std::endl;
            return EXIT_FAILURE;
        }
        l_dxy = 100.0 / l_nx;
    }

    std::cout << "runtime configuration" << std::endl;
    std::cout << "  number of cells in x-direction: " << l_nx << std::endl;
    std::cout << "  number of cells in y-direction: " << l_ny << std::endl;
    std::cout << "  cell size:                      " << l_dxy << std::endl;

    // construct setup
    tsunami_lab::setups::Setup *l_setup;
    l_setup = new tsunami_lab::setups::DamBreak2d();
    // construct solver
    tsunami_lab::patches::WavePropagation *l_waveProp;
    l_waveProp = new tsunami_lab::patches::WavePropagation2d(l_nx, l_ny);
    // construct stations
    tsunami_lab::io::Stations stations{std::ifstream("stations.json")};

    // maximum observed height in the setup
    tsunami_lab::t_real l_hMax =
        std::numeric_limits<tsunami_lab::t_real>::lowest();

    // set up solver
    for (tsunami_lab::t_idx l_cy = 0; l_cy < l_ny; l_cy++) {
        tsunami_lab::t_real l_y = l_cy * l_dxy;

        for (tsunami_lab::t_idx l_cx = 0; l_cx < l_nx; l_cx++) {
            tsunami_lab::t_real l_x = l_cx * l_dxy;

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

    // derive maximum wave speed in setup; the momentum is ignored
    tsunami_lab::t_real l_speedMax = std::sqrt(9.80665 * l_hMax);

    // derive constant time step; changes at simulation time are ignored
    tsunami_lab::t_real l_dt = 0.5 * l_dxy / l_speedMax;

    // derive scaling for a time step
    tsunami_lab::t_real l_scaling = l_dt / l_dxy;

    // set up time and print control
    tsunami_lab::t_idx l_timeStep = 0;
    tsunami_lab::t_idx l_nOut = 0;
    tsunami_lab::t_real l_maxTime = 25;
    tsunami_lab::t_real l_simTime = 0;

    std::cout << "entering time loop" << std::endl;

    auto solver = tsunami_lab::solvers::FWave();

    // iterate over time
    while (l_simTime < l_maxTime) {
        if (l_timeStep % 25 == 0) {
            std::cout << "  simulation time / #time steps: " << l_simTime
                      << " / " << l_timeStep << std::endl;

            std::string l_path = "solution_" + std::to_string(l_nOut) + ".csv";
            std::cout << "  writing wave field to " << l_path << std::endl;

            std::ofstream l_file;
            l_file.open(l_path);

            tsunami_lab::io::Csv::write(
                l_dxy, l_nx, l_ny, l_waveProp->getStride(),
                l_waveProp->getHeight(), l_waveProp->getMomentumX(),
                l_waveProp->getMomentumY(), l_waveProp->getBathymetry(),
                l_file);
            l_file.close();
            l_nOut++;
        }

        l_waveProp->setGhostOutflow();
        l_waveProp->timeStep(l_scaling, &solver);

        stations.output(l_dxy, l_simTime, l_waveProp);

        l_timeStep++;
        l_simTime += l_dt;
    }

    std::cout << "finished time loop" << std::endl;

    // free memory
    std::cout << "freeing memory" << std::endl;
    delete l_setup;
    delete l_waveProp;

    std::cout << "finished, exiting" << std::endl;
    return EXIT_SUCCESS;
}
