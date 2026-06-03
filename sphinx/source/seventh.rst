7. Checkpointing and Coarse Output
----------------------------------

**Project Report 3.6.2026:**

This week our task was to implement checkpoints to save the state 
of our simulation before a crash or other interuption 
occurs. Our NetCDF output already has some important information,
like domain size, height, bathymetry, and momentum.

**7.1. Checkpointing**

First we extended our netCDF writer to write more information 
in order to start the simulation again:

.. code-block:: c++

    nc_try(nc_def_var(ncid, "x", NC_FLOAT, 1, dimids + 2, &x_varid));
    nc_try(nc_def_var(ncid, "y", NC_FLOAT, 1, dimids + 1, &y_varid));
    nc_try(nc_def_var(ncid, "b", NC_FLOAT, 2, dimids + 1, &b_varid));
    nc_try(nc_def_var(ncid, "h", NC_FLOAT, 3, dimids, &h_varid));
    nc_try(nc_def_var(ncid, "hu", NC_FLOAT, 3, dimids, &hu_varid));
    nc_try(nc_def_var(ncid, "hv", NC_FLOAT, 3, dimids, &hv_varid));
    nc_try(nc_def_var(ncid, "t", NC_FLOAT, 1, dimids, &t_varid));


Next we made the setup CheckPoint, which uses the last available time step
in the file.

...

Then we needed to make our solver use the checkpoints if we have a crash and 
need to rely on them to continue our simulation.

...




**7.2. Coarse Output**

Next we want to make our output smaller so the files don't take up that much space.
To do this, we modified the output as such:

.. code-block:: c++

    template <typename T> T div_ceil(T &lhs, T &rhs) {
        return (lhs + rhs - 1) / rhs;
    }

We created a function to divide the cells properly and we use this
as the upper bound in every relevant loop where the cell size
is important.
The user can specify k via a command line argument:

.. code-block:: c++

    << "invalid number of arguments, usage:\n  " << *i_argv
    << " SIM_TIME CELL_SIZE DOMAIN_START_X DOMAIN_START_Y DOMAIN_END_X "
               "DOMAIN_END_Y DISPL.nc, BATHY.nc [OUTPUT_FREQ [CHECKPOINT_FREQ "
               "[STATIONS.json [SOLUTION.nc]]]]"
    << std::endl;


Our next task was the simulation of the Tohoku event with
a cell size of 50 meters. 
We had to use 100 meters for our cell size because our solver
couldn't load the 50m simulation.

.. video:: graphics/tohoku_100m_short.mp4
    :width: 100%


**Our contributions**

* Edwin extended the NetCDF solver and implemented the coarse output
* Lara wrote the documentation
* The CheckPoint task was Laras task but she couldn't finish it in time
