10.1 Individual phase: I/O parallelization
------------------------------------------


We first wanted to improve the runtime of our project by
parallelizing the I/O.

First we implemented the class PingPong, which is our
synchronization primitive for turn-style data ownership.

We also added these two lines:

.. code-block:: c++

    static std::mutex nc_m;

Here we declare a single, shared mutex named nc_m at file level.
We enforce mutual exclusion, e.g. to guard calls into the NetCDF 
library if those calls arent thread-safe.

.. code-block:: c++

    std::unique_lock l{nc_m};    

Here we have a lock named l that locks the mutex nc_m immediately 
(by default unique_lock locks on construction).
l now owns the lock, so no other code that uses the same nc_m 
(and locks it with a mutex/unique_lock/lock_guard) can enter the 
protected section.

There are three functions: access_a, access_b, and Guard:

.. code-block:: c++

    PingPong::Guard PingPong::access_a() {
        std::unique_lock<std::mutex> lock(m_);
        cv_.wait(lock, [&] { return turn_; });
        return PingPong::Guard(*this);
    }

    PingPong::Guard PingPong::access_b() {
        std::unique_lock<std::mutex> lock(m_);
        cv_.wait(lock, [&] { return !turn_; });
        return Guard(*this);
    }

    PingPong::Guard::~Guard() {
        std::lock_guard<std::mutex> lock(this_.m_);
        this_.turn_ ^= true;
        this_.cv_.notify_one();
    }


access_a() locks the thread until ``turn_`` is true, and then does its work and destroyes Guard at the end.
access_b() works the same way. Guard() locks the thread and flips ``turn_``,
and then calls  ``this_.cv_.notify_one()`` to wake one waiting thread.


Here is our implementation of PingPong:

.. code-block:: c++

    void tsunami_lab::io::NetCDF::writeTimeStep(t_real i_simTime, t_real const *i_h,
                                            t_real const *i_hu,
                                            t_real const *i_hv) {
    auto guard = pp.access_a();
    downsample(i_h, buf);
    downsample(i_hu, buf + knx * kny);
    downsample(i_hv, buf + 2 * knx * kny);
    simTime = i_simTime;
    }


Here we call access_a(), then write into buf and updates simTime,
and when guard goes out of scope, the Guard destructor flips the turn and notifies the other 
thread that the new timestep data is ready to consume.

.. code-block:: c++

    // PingPong pp;
    t = std::thread([&] {
        while (true) {
            auto guard = pp.access_b();

            if (!t.has_value()) {
                break;
            }

            t_idx start[3] = {step, 0, 0};
            t_idx count[3] = {1, kny, knx};

            nc_try(nc_put_vara_float(ncid, h_varid, start, count, buf));
            nc_try(nc_put_vara_float(ncid, hu_varid, start, count,
                                     buf + knx * kny));
            nc_try(nc_put_vara_float(ncid, hv_varid, start, count,
                                     buf + 2 * knx * kny));
            nc_try(nc_put_var1_float(ncid, t_varid, &step, &simTime));

            step++;
        }
    });

Here we call the function access_b(), the other access function of 
the PingPong class.

Now to check if these changes improved the runtime of our code:

* Benchmark info: Tohoku event, 500m cell size, 3600s simulation time (4430 time steps), output every 60s simulation time, checkpoints disabled
* Hardware info: Intel i7-13700H (6+8 cores, 12+8 threads), Samsung 990 PRO SSD, Seagate Enterprise Capacity 3.5 HDD v5

.. code-block:: c++

    SSD, synchronous output: 2:23.21
    HDD, synchronous output: 2:31.03
    SSD, asynchronous output: 2:22.57
    HDD, asynchronous output: 2:22.26

We save almost 8 seconds with our changes, so we have succeeded in our task.

Now to test it on the draco cluster:

* Benchmark info: Tohoku event, 250m cell size, 3600s simulation time (8870 time steps), output every 60s simulation time, checkpoints disabled
* Hardware info: Draco cluster node009 (48 cores, 96 threads)

.. code-block:: c++

    /vast, synchronous output: 6:38.31
    /work, synchronous output: 6:05.68
    /vast, asynchronous output: 6:37.07
    /work, asynchronous output: 5:45.69

Here we also save a good amount of time with the changes we made.