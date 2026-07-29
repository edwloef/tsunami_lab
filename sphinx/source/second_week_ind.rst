10.2 Individual phase: Dynamic time step
----------------------------------------

Our simulation looked like this with a fixed time step:

.. video:: graphics/artificial_tsunami_fixed.mp4
    :width: 100%


So this week we wanted to take a look at implementing a dynamic time step.
We are doing this because our program currently derives a constant time-step 
size before entering the time loop. But this guess can be too large 
and especially toward the end of the simulation, it can be much smaller than necessary. 
So we want to derive a maximal time for each time step for further speed up
and avoid simulations where the result is useless due to an unlucky time-step guess.

First we implemented the maximal time for each time step.
To make this more manageable, we added a new patch: the DynamicWavePropagation2d patch.
We set the maxLambda variable to what is largest:

.. code-block:: c++

    if (!l_dryR) {
        m_hAcc[l_ceR] += l_netUpdates[1][0];
        m_huAcc[l_ceR] += l_netUpdates[1][1];
        l_maxLambda = std::max(l_maxLambda, std::abs(l_lambda[1]));
    }


We also added scaling to work properly with the dynamic sizes:

.. code-block:: c++

    t_real l_scaling = l_dt / i_dxy;
    #pragma omp parallel for schedule(static)
    for (t_idx l_ed = 0; l_ed < m_nCells; l_ed++) {
        m_h[l_ed] -= l_scaling * m_hAcc[l_ed];
        m_hu[l_ed] -= l_scaling * m_huAcc[l_ed];
        m_hv[l_ed] -= l_scaling * m_hvAcc[l_ed];
    }

If we try the first simulation again, we have a much better result:

.. video:: graphics/artificial_tsunami_dynamic.mp4
    :width: 100%