8. Optimization
---------------

**Project Report 10.6.2026:**

This week we were tasked with optimizing our code.
For this we had to use the dedicated compute cluster draco
and compare two different compilers, the GNU's C++ compiler and the LLVM project's
Clang compiler.

**1. Draco**

First we had to check if we could reproduce the results of chapter 6 when running 
the code on the cluster. We tried it and it looked exactly like the simulation
before.

When we ran our code on our own computer
versus the draco clusters, we found that our computer 
was faster:

.. code-block:: c++

    g++ -O3 -flto=auto -fno-math-errno 
    finished time loop (local):
        simulation time per time step: 12.7347 ms
        simulation time per time step per cell: 12.6036 ns

.. code-block:: c++

    finished time loop (draco)
        simulation time per time step: 28.2843 ms
        simulation time per time step per cell: 27.9931 ns



**2. Compilers**

Next we compared the two different compilers, 
GNU's C++ compiler (g++) and the LLVM project's Clang compiler (clang++).
First we added support for generic compilers 
using the environment variable CXX:

.. code-block:: c++

    # set cxx
    cxx = os.environ.get("CXX", "")
    if cxx:
        env["CXX"] = cxx


Next we were tasked to recompile our code with the recent versions of both compilers,
we saw that the clang compiler was just a bit faster:

.. code-block:: c++

    g++ -O3:
    finished time loop
        simulation time per time step: 14.6622 ms
        simulation time per time step per cell: 14.5113 ns
    clang++ -O3:
    finished time loop
        simulation time per time step: 19.5751 ms
        simulation time per time step per cell: 19.3736 ns


Then we tried different optimization switches, we used O3, flto-auto, 
and fno-math-errno:

.. code-block:: c++

    g++ -O3:
    finished time loop
        simulation time per time step: 14.6622 ms
        simulation time per time step per cell: 14.5113 ns
    clang++ -O3:
    finished time loop
        simulation time per time step: 19.5751 ms
        simulation time per time step per cell: 19.3736 ns
    g++ -O3 -flto=auto:
    finished time loop
        simulation time per time step: 13.2804 ms
        simulation time per time step per cell: 13.1437 ns
    clang++ -O3 -flto=auto:
    finished time loop
        simulation time per time step: 16.6008 ms
        simulation time per time step per cell: 16.4299 ns
    g++ -O3 -flto=auto -fno-math-errno:
    finished time loop
        simulation time per time step: 12.7347 ms
        simulation time per time step per cell: 12.6036 ns
    clang++ -O3 -flto=auto -fno-math-errno:
    finished time loop
        simulation time per time step: 15.7371 ms
        simulation time per time step per cell: 15.5751 ns

The flags we use don't impact numerical accuracy, as -fno-math-errno 
only allows the compiler to skip updating errno, and -O3 only speeds up our code.
The fastest was the g++ compiler with the flags -O3, -flto=auto, and -fno-math-errno.
The slowest simulation time was the clang compiler with the O3 flag.

Then we added clang's optimization-remark options:

.. code-block:: c++

    remarks_flags = []
    if "remarks" in env["mode"]:
        remarks_flags = ["-Rpass=.*", "-Rpass-missed=.*", "-Rpass-analysis=.*"]


**3. Instrumentation and Performance Counters**

Not counting the time spent setting up the simulation, the time expenditure 
of a single-threaded 1km-resolution tohoku simulation splits up as follows:


* 98% timeStep

    * 40% of 98% = 39% netUpdates

        * 16% of 39% = 6.4% std::sqrt

            * this is likely incorrect, as the function is never called, rather the sqrtss instruction is used

    * 4.7% of 98% = 4.7% std::memcpy

* 1.5% writeTimestep

    * 100% of 1.5% = 1.5% downsample

        * 3.3% of 1.5% = 0.0% std::memset

* 0.0% writeCheckpoint

We made use of the LLVM Machine Code Analyzer to make compiler-guided 
adjustments to our F-Wave solver implementation, leading to an improvement 
from about 114.3 cycles per iteration to about 101.25 cycles per iteration, 
or about a 10% improvement.

**Our contributions**

* Edwin implemented the compiler setup, and added the optimization remark options
* Lara implemented the documentation and ran the simulations
