/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Unit tests of the FWave Riemann solver.
 **/

#include "FWave.h"
#include <catch2/catch.hpp>

TEST_CASE("Test the derivation of the FWave net-updates.", "[FWaveUpdates]") {
    auto solver = tsunami_lab::solvers::FWave();

    float netUpdateL[2];
    float netUpdateR[2];

    /*
     * Test case:
     *
     *      left | right
     *  h:    10 | 9
     *  u:    -3 | 3
     *  hu:  -30 | 27
     *
     * The derivation of the Roe speeds (s1, s2) and wave strengths (a1, a1)
     * is given above.
     *
     * The net-updates are given through the scaled eigenvectors.
     *
     *                      |  1 |   | 33.5590017014261447899292 |
     * update #1: s1 * a1 * |    | = |                           |
     *                      | s1 |   | -326.56631690591093200508 |
     *
     *                      |  1 |   | 23.4409982985738561366777 |
     * update #2: s2 * a2 * |    | = |                           |
     *                      | s2 |   | 224.403141905910928927533 |
     */
    solver.netUpdates(10, 9, -30, 27, netUpdateL, netUpdateR);

    REQUIRE(netUpdateL[0] == Approx(33.5590017014261447899292));
    REQUIRE(netUpdateL[1] == Approx(-326.56631690591093200508));

    REQUIRE(netUpdateR[0] == Approx(23.4409982985738561366777));
    REQUIRE(netUpdateR[1] == Approx(224.403141905910928927533));

    /*
        Test case (supersonic, eigenvalues > 0)
    */
    solver.netUpdates(4, 3, 40, 30, netUpdateL, netUpdateR);

    REQUIRE(netUpdateL[0] == Approx(0));
    REQUIRE(netUpdateL[1] == Approx(0));

    REQUIRE(netUpdateR[0] != Approx(0));
    REQUIRE(netUpdateR[1] != Approx(0));

    /*
        Test case (supersonic, eigenvalues < 0)
    */
    solver.netUpdates(4, 3, -40, -30, netUpdateL, netUpdateR);

    REQUIRE(netUpdateL[0] != Approx(0));
    REQUIRE(netUpdateL[1] != Approx(0));

    REQUIRE(netUpdateR[0] == Approx(0));
    REQUIRE(netUpdateR[1] == Approx(0));

    /*
     * Test case (dam break):
     *
     *     left | right
     *   h:  10 | 8
     *   hu:  0 | 0
     *
     * Roe speeds are given as:
     *
     *   s1 = -sqrt(9.80665 * 9)
     *   s2 =  sqrt(9.80665 * 9)
     *
     * Inversion of the matrix of right Eigenvectors:
     *
     *   wolframalpha.com query: invert {{1, 1}, {-sqrt(9.80665 * 9),
     * sqrt(9.80665
     * * 9)}}
     *
     *          | 0.5 -0.0532217 |
     *   Rinv = |                |
     *          | 0.5 -0.0532217 |
     *
     * Multiplicaton with the jump in quantities gives the wave strengths:
     *
     *        | 8 - 10 |   | -1 |   | a1 |
     * Rinv * |        | = |    | = |    |
     *        |  0 - 0 |   | -1 |   | a2 |
     *
     * The net-updates are given through the scaled eigenvectors.
     *
     *                      |  1 |   |   9.394671362 |
     * update #1: s1 * a1 * |    | = |               |
     *                      | s1 |   | -88.25985     |
     *
     *                      |  1 |   |  -9.394671362 |
     * update #2: s2 * a2 * |    | = |               |
     *                      | s2 |   | -88.25985     |
     */
    solver.netUpdates(10, 8, 0, 0, netUpdateL, netUpdateR);

    REQUIRE(netUpdateL[0] == Approx(9.394671362));
    REQUIRE(netUpdateL[1] == -Approx(88.25985));

    REQUIRE(netUpdateR[0] == -Approx(9.394671362));
    REQUIRE(netUpdateR[1] == -Approx(88.25985));

    /*
     * Test case (trivial steady state):
     *
     *     left | right
     *   h:  10 | 10
     *  hu:   0 |  0
     */
    solver.netUpdates(10, 10, 0, 0, netUpdateL, netUpdateR);

    REQUIRE(netUpdateL[0] == Approx(0));
    REQUIRE(netUpdateL[1] == Approx(0));

    REQUIRE(netUpdateR[0] == Approx(0));
    REQUIRE(netUpdateR[1] == Approx(0));
}
