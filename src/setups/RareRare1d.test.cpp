/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Tests the rare-rare setup.
 **/
#include "RareRare1d.h"
#include <catch2/catch.hpp>

TEST_CASE("Test the one-dimensional rare-rare setup.", "[RareRare1d]") {
    tsunami_lab::setups::RareRare1d l_rareRare(25, 55, 3);

    // left side
    REQUIRE(l_rareRare.getHeight(2, 0) == 25);

    REQUIRE(l_rareRare.getMomentumX(2, 0) == -55);

    REQUIRE(l_rareRare.getMomentumY(2, 0) == 0);

    REQUIRE(l_rareRare.getHeight(2, 5) == 25);

    REQUIRE(l_rareRare.getMomentumX(2, 5) == -55);

    REQUIRE(l_rareRare.getMomentumY(2, 2) == 0);

    // right side
    REQUIRE(l_rareRare.getHeight(4, 0) == 25);

    REQUIRE(l_rareRare.getMomentumX(4, 0) == 55);

    REQUIRE(l_rareRare.getMomentumY(4, 0) == 0);

    REQUIRE(l_rareRare.getHeight(4, 5) == 25);

    REQUIRE(l_rareRare.getMomentumX(4, 5) == 55);

    REQUIRE(l_rareRare.getMomentumY(4, 2) == 0);
}
