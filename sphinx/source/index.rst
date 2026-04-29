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

To create the final csv-files where the actual simulation is shown,
type 

``./build/tsunami_lab <number_of_cells>``

in the terminal and let it run. 

You can then view the result with paraview.