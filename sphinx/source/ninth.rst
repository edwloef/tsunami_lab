9. Parallelization
-------------------

**Project Report 17.6.2026:**

This week our task was the parallelization of our code with OpenMP. 

**1. Parallelization**

We parallelized WavePropagation2d in our solver as such:

.. code-block:: c++

    #pragma omp parallel for
    for (t_idx l_y = 0; l_y < m_nCellsY + 1; l_y++) {
        t_idx l_ce = l_y * stride;

        // init new cell quantities
        l_hNew[l_ce] = l_hOld[l_ce];
        l_huNew[l_ce] = l_huOld[l_ce];
        l_hvNew[l_ce] = l_hvOld[l_ce];

        for (t_idx l_x = 0; l_x < m_nCellsX + 1; l_x++) {
            // determine left and right cell-id
            t_idx l_ceL = l_ce + l_x;
            t_idx l_ceR = l_ceL + 1;

            ...

            // update the cells' quantities
            l_hNew[l_ceL] -= i_scaling * l_netUpdates[0][0];
            l_huNew[l_ceL] -= i_scaling * l_netUpdates[0][1];

            l_hNew[l_ceR] -= i_scaling * l_netUpdates[1][0];
            l_huNew[l_ceR] -= i_scaling * l_netUpdates[1][1];
        }
    }

    for (t_idx l_s = 0; l_s < 2; l_s++) {
    #pragma omp parallel for
        for (t_idx l_y = l_s; l_y < m_nCellsY + 1; l_y += 2) {
            t_idx l_ce = l_y * stride;

            for (t_idx l_x = 0; l_x < m_nCellsX + 1; l_x++) {
                // determine left and right cell-id
                t_idx l_ceL = l_ce + l_x;
                t_idx l_ceR = l_ceL + stride;

               ...

                // update the cells' quantities
                l_hNew[l_ceL] -= i_scaling * l_netUpdates[0][0];
                l_hvNew[l_ceL] -= i_scaling * l_netUpdates[0][1];

                l_hNew[l_ceR] -= i_scaling * l_netUpdates[1][0];
                l_hvNew[l_ceR] -= i_scaling * l_netUpdates[1][1];
            }
        }
    }


Then we had to compare the runtime of our
parallelized solver to the serial version on NVIDIA Grace.
As a benchmark, we used the Tohoku Event with a 3600 seconds simulation length, 
with an output every 60 seconds simulation time, and
checkpoint every 600 seconds simulation time:

- 1000m cell size

  - 1 thread: 2:06
  - 2 threads: 1:03 (2.00x speedup)
  - 4 threads: 0:34 (3.71x speedup)
  - 8 threads: 0:19 (6.63x speedup)
  - 16 threads: 0:11 (11.45x speedup)
  - 32 threads: 0:08 (15.75x speedup)
  - 64 threads: 0:06 (21.00x speedup)
  - 72 threads: 0:05 (25.20x speedup)
  - 144 threads: 0:46 (2.74x speedup)

- 500m cell size

  - 1 thread: 15:13
  - 2 threads: 7:45 (1.96x speedup)
  - 4 threads: 4:03 (3.76x speedup)
  - 8 threads: 2:07 (7.19x speedup)
  - 16 threads: 1:10 (13.04x speedup)
  - 32 threads: 0:41 (22.27x speedup)
  - 64 threads: 0:29 (31.48x speedup)
  - 72 threads: 0:28 (32.61x speedup)
  - 144 threads: 4:44 (3.21x speedup)

The time it took for the simulation to run with one thread is 15 minutes and
13 seconds.
We achieved a speedup of 3.21 for 144 threads. We got the most speedup
with 72 threads with 32.61.
Judging by the simulation time per time step per cell, the 
solver itself parallelizes very well, with a speedup of over 
50x with 72 threads, however a good chunk of time is spent 
doing single-threaded I/O, which leads to a lower speedup overall.


Concerning the question of wether we should parallelize 
the outer or the inner loop of our two-dimensional solver, 
the outer ones should be parallelized.

This is because the inner loop of the x-sweep accesses neighboring cells,
which raises the risk of race conditions and leads to incorrect
results if parallelized.
The outer loop of the x-sweep only initializes new cell quantities and
operates on disjoint cell regions, which doesn't cause any problems
if parallelized.
The outer loop of the x-sweep also requires fewer fork-join cycles,
and chunking doesn't help because we parallelize
row-by-row in both the x-sweeps and y-sweeps and are therefore 
working with good cache locality.

Roughly the same goes for the second number of loops.
The second loop of the y-sweep can again be parallelized,
as it only writes new data, and the inner loop of the y-sweep can't be parallelized
as it leads to race conditions because we access the neighboring cells again.
Because we also parallelize the y-sweep row-wise 
by splitting it into even and odd row pairs, and 
then simply process all even pairs in parallel 
followed by all odd pairs in parallel, we eliminate 
dependencies between different threads.

Taking a look at different scheduling strategies, we have static, where work 
is divided into equal-sized chunks ahead of time and assigned to threads in a 
fixed, repeating pattern.

Then with dynamic the iteration chunks are placed into a shared queue; threads 
repeatedly fetch the next chunk when they finish their current one.
This is good for irregular or unpredictable workloads, but we loose cache locality.

With guided we have a hybrid of the two previous versions: it initially hands out 
large chunks, then reduces chunk size exponentially (or to a minimum chunk size) 
as work proceeds. It combines the low overhead early with a good load balance near the end.

We found that dynamic scheduling was slower than static scheduling and guided
scheduling was the fastest.

When we tried to use OpenMP's first touch 
policy to perform NUMA-aware initializations, we 
implemented the code below in the constructor, but
it slowed our program down since it requires static scheduling which is slower than
guided scheduling:

.. code-block:: c++

    #pragma omp parallel for schedule(static)
    for (t_idx l_y = 0; l_y < m_nCellsY + 2; l_y++) {
        for (t_idx l_x = 0; l_x < m_nCellsX + 2; l_x++) {
            t_idx l_ce = l_y * stride + l_x;

            m_h[0][l_ce] = 0.0;
            m_h[1][l_ce] = 0.0;

            m_hu[0][l_ce] = 0.0;
            m_hu[1][l_ce] = 0.0;

            m_hv[0][l_ce] = 0.0;
            m_hv[1][l_ce] = 0.0;

            m_b[l_ce] = 0.0;
        }
    }



The last task was optional: we were tasked to benchmark our solver on the Grace 
system with the Tohoku event with 250 m resolution, using 10000 timesteps 
and with a file I/O every 100 time steps.

Our command for the following result:

``OMP_NUM_THREADS=72 OMP_PROC_BIND=close ./build/tsunami_lab -d ../tohoku_gebco20_ucsb3_250m_displ.nc -b ../tohoku_gebco20_ucsb3_250m_bath.nc -s 250 -l 4058.7 --output-freq 40.5856 --checkpoint-freq 0``

Our best time is 3 minutes and 17 seconds and 220 milliseconds.

That is 4628 million cell updates per second (mcups).

Our team name shall be "Aqua Tax Evaders", because of the prevelance of 
the name "Aqua Helden".