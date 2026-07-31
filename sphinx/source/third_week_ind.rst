10.3 Individual phase: Continuation Dynamic time step
-----------------------------------------------------

We want to take another look at the implementation of the Dynamic time step.
The first try in the last week was suboptimal, we could see that the waves at the end of the simulation werent quite 
accurate, and we had problems with the water depth.

To fix this, we decided to implement new parameters in the command line:
We added gamma, which is a configurable wave reflection damping coefficient,
and delta, which is the configurable minimum water depth at our setup time.


.. code-block:: c++

    std::cout
        << "Usage: " << program << " [OPTION...]"
           "..."
           "--gamma Wave reflection damping coefficient between 0 and 1 (default 0.05)\n"
           "--delta Minimum water depth at setup time in meters (default 20m)\n"
           "..."
        << std::endl;


Since we tested our program very often this week, a lot of time was spent waiting for the simulations to finish.
But we still achieved a very good result, as we can see here with the first video being of Chile, and the second being Tohoku:

.. video:: graphics/chile_dynamic.mp4
    :width: 100%

.. video:: graphics/tohoku_dynamic.mp4
    :width: 100%