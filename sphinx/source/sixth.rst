6. Tsunami Simulations
----------------------

**Project Report 27.5.2026:**

This week our task was to simulate two real tsunami events, one from March 11 in 2011, M 9.1 Tohoku
and the other from  February 27 in 2010, M 8.8 Chile.

**6.1. 2010 M 8.8 Chile Event**

Now to our simulations for the Chile event. We used the grid resolutions 250m, 500m, and 1000m.

The 250m simulation:

.. video:: graphics/chile_250m.mp4
    :width: 100%

Our computational demands were 165174201 required cells and 11165775987600 cell updates.
It took about 3 to 4 hours in simulated time for the wave to leave the computational domain. 
This was similar to the results for the other resolutions as well.

The 500m simulation:

.. video:: graphics/chile_500m.mp4
    :width: 100%

Our computational demands were 41287101 required cells and 386894137970340000 cell updates.


The 1000m simulation:

.. video:: graphics/chile_1000m.mp4
    :width: 100%

Our computational demands were 10318551 required cells and 188829483300 cell updates.

The simulation time 

**6.2. 2011 M 9.1 Tohoku Event**

Next we have the simulations of the Tohoku event. 


The 250m simulation:

.. video:: graphics/tohoku_250m.mp4
    :width: 100%

Our computational demands were 64772402 required cells and 4721908105800 cell updates.
The simulated time for the wave was 3 to 4 hours, much like the chile event. 


The 500m simulation:

.. video:: graphics/tohoku_500m.mp4
    :width: 100%

Our computational demands were 16191601 required cells and 605565877400 cell updates.


The 1000m simulation:

.. video:: graphics/tohoku_1000m.mp4
    :width: 100%

Our computational demands were 4045801 required cells and 79702279700 cell updates.


Now we will take a look at Sõma:

.. image:: graphics/plot.svg
   :target: _images/output.svg

The x axis represents the time in seconds and the y axis the height of the water.
This is correct as it doesn't differ from our simulated tsunami arrival times.


**Our contributions**

* Edwin simulated the two tsunami events and the Sõma simulation.
* Lara implemented the NetCDF tests and wrote the documentation.
