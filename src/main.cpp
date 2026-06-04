/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Entry-point for simulations.
 **/
#include "io/NetCDF.h"
#include "io/Stations.h"
#include "patches/WavePropagation2d.h"
#include "setups/TsunamiEvent2d.h"
#include "solvers/FWave.h"
#include <algorithm>
#include <assert.h>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <limits>
#include <ostream>
#include <stdarg.h>

typedef struct {
    tsunami_lab::t_real outputFreq, checkpointFreq, simLen, cellSize;
    tsunami_lab::t_idx coarseOutputSize;
    char const *displ, *bathy, *stations, *output, *checkpoint;
} Args;

enum {
    outputFreq = 256,
    checkpointFreq,
    stations,
};

static struct option options[] = {
    {"help", no_argument, 0, 'h'},
    {"output-freq", required_argument, 0, outputFreq},
    {"checkpoint-freq", required_argument, 0, checkpointFreq},
    {"simulation-length", required_argument, 0, 'l'},
    {"cell-size", required_argument, 0, 's'},
    {"coarse-output-size", required_argument, 0, 'k'},
    {"displacement", required_argument, 0, 'd'},
    {"bathymetry", required_argument, 0, 'b'},
    {"stations", required_argument, 0, stations},
    {"output", required_argument, 0, 'o'},
    {"checkpoint", required_argument, 0, 'c'},
    {0, 0, 0, 0}};

void printUsage(char *program) {
    std::cout
        << "Usage: " << program
        << " [OPTION...]\n  -h                                This help "
           "message.\n     --output-freq <seconds>        Output frequency in "
           "seconds (default 60s)\n     --checkpoint-freq <seconds>    "
           "Checkpoint frequency in seconds (default 600s)\n  -l "
           "--simulation-length <seconds>  Simulation length in seconds "
           "(default 3600s)\n  -s --cell-size <meters>           Cell size in "
           "meters (default 1000m)\n  -k --coarse-output-size <cells>   Coarse "
           "output size in cells (default 1)\n  -d --displacement <path>       "
           "   Path to displacement NetCDF file (required)\n  -b --bathymetry "
           "<path>            Path to bathymetry NetCDF file (required)\n     "
           "--stations <path>              Path to stations JSON file (default "
           "stations.nc)\n  -o --output <path>                Path to output "
           "NetCDF file (default output.nc)\n  -c --checkpoint <path>          "
           "  Path to checkpoint NetCDF file (default checkpoint.nc)"
        << std::endl;
}

Args parseArgs(int argc, char *argv[]) {
    Args args = {60.0, 600.0, 3600.0,          1000.0,      1,
                 NULL, NULL,  "stations.json", "output.nc", "checkpoint.nc"};

    int c;
    while ((c = getopt_long(argc, argv, "hl:s:d:b:o:c", options, NULL)) != -1) {
        switch (c) {
        case 'h':
            printUsage(*argv);
            exit(0);
        case outputFreq:
            args.outputFreq = std::stod(optarg);
            break;
        case checkpointFreq:
            args.checkpointFreq = std::stod(optarg);
            break;
        case 'l':
            args.simLen = std::stod(optarg);
            break;
        case 's':
            args.cellSize = std::stod(optarg);
            break;
        case 'k':
            args.coarseOutputSize = std::atoi(optarg);
            break;
        case 'd':
            args.displ = optarg;
            break;
        case 'b':
            args.bathy = optarg;
            break;
        case stations:
            args.stations = optarg;
            break;
        case 'o':
            args.output = optarg;
            break;
        case 'c':
            args.checkpoint = optarg;
            break;
        default:
            printUsage(*argv);
            exit(2);
        }
    }

    if (!args.displ || !args.bathy) {
        printUsage(*argv);
        exit(2);
    }

    return args;
}

char *strfmt(const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    int len = vsnprintf(NULL, 0, fmt, va);
    va_end(va);
    char *str = (char *)malloc(sizeof(char) * (len + 1));
    assert(str);
    va_start(va, fmt);
    len = vsnprintf(str, len + 1, fmt, va);
    va_end(va);
    return str;
}

int main(int i_argc, char *i_argv[]) {
    std::cout << "####################################" << std::endl;
    std::cout << "### Tsunami Lab                  ###" << std::endl;
    std::cout << "###                              ###" << std::endl;
    std::cout << "### https://scalable.uni-jena.de ###" << std::endl;
    std::cout << "####################################" << std::endl;

    Args l_args = parseArgs(i_argc, i_argv);

    std::cout << "runtime configuration:\n  cell size: " << l_args.cellSize
              << " meters" << std::endl;

    // construct setup
    auto l_setup =
        new tsunami_lab::setups::TsunamiEvent2d(l_args.displ, l_args.bathy);

    tsunami_lab::t_idx l_nx =
        (l_setup->maxX() - l_setup->minX()) / l_args.cellSize;
    tsunami_lab::t_idx l_ny =
        (l_setup->maxY() - l_setup->minY()) / l_args.cellSize;

    auto defaultprecision = std::cout.precision();
    std::cout.precision(0);
    std::cout << "  simulation bounds: " << std::fixed << l_setup->minX() << "/"
              << l_setup->maxX() << "/" << l_setup->minY() << "/"
              << l_setup->maxY() << std::defaultfloat
              << "\n  cell count: " << l_nx << " * " << l_ny << " = "
              << l_nx * l_ny << std::endl;
    std::cout.precision(defaultprecision);

    // construct solver
    tsunami_lab::patches::WavePropagation2d l_waveProp(
        l_nx, l_ny, tsunami_lab::solvers::FWave());

    // maximum observed height in the setup
    tsunami_lab::t_real l_hMax =
        std::numeric_limits<tsunami_lab::t_real>::lowest();

    // set up solver
    for (tsunami_lab::t_idx l_cy = 0; l_cy < l_ny; l_cy++) {
        tsunami_lab::t_real l_y = l_setup->minY() + l_cy * l_args.cellSize;

        for (tsunami_lab::t_idx l_cx = 0; l_cx < l_nx; l_cx++) {
            tsunami_lab::t_real l_x = l_setup->minX() + l_cx * l_args.cellSize;

            // get initial values of the setup
            tsunami_lab::t_real l_h = l_setup->getHeight(l_x, l_y);
            tsunami_lab::t_real l_hu = l_setup->getMomentumX(l_x, l_y);
            tsunami_lab::t_real l_hv = l_setup->getMomentumY(l_x, l_y);
            tsunami_lab::t_real l_b = l_setup->getBathymetry(l_x, l_y);

            // set initial values in wave propagation solver
            l_waveProp.setHeight(l_cx, l_cy, l_h);
            l_waveProp.setMomentumX(l_cx, l_cy, l_hu);
            l_waveProp.setMomentumY(l_cx, l_cy, l_hv);
            l_waveProp.setBathymetry(l_cx, l_cy, l_b);

            l_hMax = std::max(l_h, l_hMax);
        }
    }

    // destroy setup
    delete l_setup;

    // construct netcdf writer
    tsunami_lab::io::NetCDF netcdf(l_args.output, l_args.cellSize, l_nx, l_ny,
                                   l_waveProp.getStride());

    // construct stations
    tsunami_lab::io::Stations stations(std::ifstream{l_args.stations});

    // derive maximum wave speed in setup; the momentum is ignored
    tsunami_lab::t_real l_speedMax = std::sqrt(9.80665 * l_hMax);

    // derive constant time step; changes at simulation time are ignored
    tsunami_lab::t_real l_dt = 0.5 * l_args.cellSize / l_speedMax;

    // derive scaling for a time step
    tsunami_lab::t_real l_scaling = l_dt / l_args.cellSize;

    // set up time and output control
    tsunami_lab::t_idx l_timeStep = 0;
    tsunami_lab::t_real l_simTime = 0;
    tsunami_lab::t_real l_nextOutput = 0;
    tsunami_lab::t_real l_nextCheckpoint = 0;

    std::cout << "  time step length: " << l_dt << " seconds" << std::endl;

    netcdf.writeBathymetry(l_waveProp.getBathymetry());

    std::cout << "entering time loop..." << std::endl;

    auto dur = std::chrono::duration<double>(0);

    char *new_checkpoint = strfmt("%s.new", l_args.checkpoint);

    // iterate over time
    while (l_simTime < l_args.simLen) {
        if (l_simTime >= l_nextOutput) {
            std::cout << "  output: " << l_timeStep << " time steps, "
                      << l_simTime << " seconds" << std::endl;

            netcdf.writeTimeStep(l_simTime, l_waveProp.getHeight(),
                                 l_waveProp.getMomentumX(),
                                 l_waveProp.getMomentumY());

            stations.output(l_args.cellSize, l_simTime, &l_waveProp);

            l_nextOutput += l_args.outputFreq;
        }

        if (l_simTime >= l_nextCheckpoint) {
            std::cout << "  checkpoint: " << l_timeStep << " time steps, "
                      << l_simTime << " seconds" << std::endl;

            netcdf.writeCheckpoint(new_checkpoint, l_waveProp.getBathymetry(),
                                   l_waveProp.getHeight(),
                                   l_waveProp.getMomentumX(),
                                   l_waveProp.getMomentumY());

            std::filesystem::rename(new_checkpoint, l_args.checkpoint);

            l_nextCheckpoint += l_args.checkpointFreq;
        }

        auto now = std::chrono::high_resolution_clock::now();
        l_waveProp.setGhostOutflow();
        l_waveProp.timeStep(l_scaling);
        dur += std::chrono::high_resolution_clock::now() - now;

        l_timeStep++;
        l_simTime += l_dt;
    }

    free(new_checkpoint);

    std::filesystem::remove(l_args.checkpoint);

    std::cout << "finished time loop\n  simulation time per time step: "
              << dur.count()
              << " ms\n  simulation time per time step per cell: "
              << 1'000'000 * dur.count() / (l_nx * l_ny) << " ns" << std::endl;
}
