/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Unit tests for the dynamic one-dimensional wave propagation patch.
 **/
#include "DynamicWavePropagation1d.h"
#include "../solvers/FWave.h"
#include <catch2/catch.hpp>

TEST_CASE("Test the dynamic 1d wave propagation solver.", "[DynWaveProp1d]") {
    /*
     * Test case:
     *
     *   Single dam break problem between cell 49 and 50.
     *     left | right
     *       10 | 8
     *        0 | 0
     *
     *   Elsewhere steady state.
     */

    // construct solver and setup a dambreak problem
    tsunami_lab::patches::DynamicWavePropagation1d m_waveProp(
        100, tsunami_lab::solvers::FWave());

    for (std::size_t l_ce = 0; l_ce < 50; l_ce++) {
        m_waveProp.setHeight(l_ce, 0, 10);
        m_waveProp.setMomentumX(l_ce, 0, 0);
        m_waveProp.setBathymetry(l_ce, 0, -20);
    }
    for (std::size_t l_ce = 50; l_ce < 100; l_ce++) {
        m_waveProp.setHeight(l_ce, 0, 8);
        m_waveProp.setMomentumX(l_ce, 0, 0);
        m_waveProp.setBathymetry(l_ce, 0, -20);
    }

    m_waveProp.setGhostBathymetry();

    // set outflow boundary condition
    m_waveProp.setGhostOutflow();

    // perform a time step
    m_waveProp.timeStep(0.1);

    // steady state
    for (std::size_t l_ce = 0; l_ce < 49; l_ce++) {
        REQUIRE(m_waveProp.getHeight()[l_ce] == Approx(10));
        REQUIRE(m_waveProp.getMomentumX()[l_ce] == Approx(0));
    }

    // dam-break
    REQUIRE(m_waveProp.getHeight()[49] != Approx(10));
    REQUIRE(m_waveProp.getMomentumX()[49] != Approx(0));

    REQUIRE(m_waveProp.getHeight()[50] != Approx(8));
    REQUIRE(m_waveProp.getMomentumX()[50] != Approx(0));

    // steady state
    for (std::size_t l_ce = 51; l_ce < 100; l_ce++) {
        REQUIRE(m_waveProp.getHeight()[l_ce] == Approx(8));
        REQUIRE(m_waveProp.getMomentumX()[l_ce] == Approx(0));
    }
}
