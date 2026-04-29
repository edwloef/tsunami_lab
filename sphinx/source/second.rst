2. Finite Volume Discretization
-------------------------------

**Project Report 22.4.2026:**

* Edwin wrote the code to switch between the provided Roe solver and the f-wave solver.
* Edwin embedded our solver into GitHub Actions, and implemented the shock-shock and rare-rare problems as setups.
* Lara wrote the tests for the middle states and wrote the documentation.
* for 2.1.2:
* higher initial water height has no effect on the change in height due to the water streams meeting or parting
* higher particle velocity makes the change in water height larger
* for 2.2.2:
* a larger difference in height leads to a ripple in the wave at the dam position
* a higher particle velocity in the river leads to a wider and faster wave
* Our answer to the last question of evacuating the village: We would have 2248 seconds to do so, or 37 minutes.

Visualization:

.. image:: graphics/reservoir1.png
   :width: 400px


