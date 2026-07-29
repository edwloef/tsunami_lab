.. Tsunami documentation master file, created by
   sphinx-quickstart on Fri Apr 10 15:27:16 2026.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   first
   second
   third
   fourth
   fifth
   sixth
   seventh
   eighth
   ninth
   individual_phase
   first_week_ind
   second_week_ind
   third_week_ind


Tsunami documentation
=====================


Documentation
-------------

Link to our code: 
https://github.com/edwloef/tsunami_lab

Compile this project by typing scons in the terminal.
To run the tests, enter 

``./build/tests.exe``

In order to work with the GEBCO files and data, download the zip file 
and unzip it: 

``curl -Lo GEBCO_2026_sub_ice.zip https://dap.ceda.ac.uk/bodc/gebco/global/gebco_2026/sub_ice_topography_bathymetry/netcdf/GEBCO_2026_sub_ice.zip
unzip GEBCO_2026_sub_ice.zip``

To create the csv-file with the bathymetry data, run the prompt below in the terminal:

``gmt project -C141.024949/37.316569 -E146.0/37.316569 -G250e -Q | gmt grdtrack -GGEBCO_2026_sub_ice.nc | awk 'BEGIN {print "lon,lat,distance_m,elevation_m"} {print $1","$2","$3","$4}' > GEBCO_2026_sub_ice_bathy.csv``

To generate the result, enter the path to the tsunami file and simply add the required arguments:

.. code-block:: c++

   Usage: build/tsunami_lab [OPTION...]
      -h                                This help message.
         --output-freq <seconds>        Output frequency in seconds (default 60s)
         --checkpoint-freq <seconds>    Checkpoint frequency in seconds (default 600s)
      -l --simulation-length <seconds>  Simulation length in seconds (default 3600s)
      -s --cell-size <meters>           Cell size in meters (default 1000m)
      -k --coarse-output-size <cells>   Coarse output size in cells (default 1)
      -d --displacement <path>          Path to displacement NetCDF file (required)
      -b --bathymetry <path>            Path to bathymetry NetCDF file (required)
         --stations <path>              Path to stations JSON file (default stations.nc)
      -o --output <path>                Path to output NetCDF file (default output.nc)
      -c --checkpoint <path>            Path to checkpoint NetCDF file (default checkpoint.nc)

The only files that are required are the bathymetry file and the displacement file, the
rest have a default setting.

You can then view the result with paraview.