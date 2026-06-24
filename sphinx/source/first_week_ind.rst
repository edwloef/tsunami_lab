10.1 Individual phase: I/O parallelization
------------------------------------------


We first wanted to improve the runtime of our project by
parallelizing the I/O.

First we implemented the class PingPong, which is our
synchronization primitive for turn-style data ownership.

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


access_a() locks the thread until turn_ is true, and then does its work and destroyes Guard at the end.
access_b() works the same way. Guard() locks the thread and flips turn_,
and then calls  ``this_.cv_.notify_one()`` to wake one waiting thread.


We use this class in our NetCDF class for our parallelization but we also 
utilize OpenMP i.e. in this loop:

.. code-block:: c++

    #pragma omp parallel for collapse(2) schedule(static)
        for (t_idx oy = 0; oy < kny; oy++) {
            for (t_idx ox = 0; ox < knx; ox++) {
                t_idx iy = oy * k;
                t_idx my = std::min(iy + k, ny);

                t_idx ix = ox * k;
                t_idx mx = std::min(ix + k, nx);

                t_real sum = 0;

                for (t_idx iy_ = iy; iy_ < my; iy_++) {
                    for (t_idx ix_ = ix; ix_ < mx; ix_++) {
                        sum += i_v[iy_ * stride + ix_];
                    }
                }

                buf[oy * knx + ox] = sum * scale;
            }
        }

        return buf;
    }

Here we can collapse the two loops because each (oy, ox) tile computes an 
independent output element and there are no data dependencies between
different iterations of ox and oy.

We cant parallelize the two inner for loops since they both access the variable sum,
leading to wrong results if parallelized.


Now we take a look at our implementation of PingPong:

.. code-block:: c++

    tsunami_lab::io::NetCDF::~NetCDF() {
    std::optional<std::thread> l_t = std::nullopt;
    {
        auto guard = pp.access_a();
        t.swap(l_t);
    }

    if (l_t.has_value()) {
        l_t.value().join();

        nc_try(nc_close(ncid));
    }

    delete[] buf;
    }

Here we aquire the a-side guard and swap out t into l_t.

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

Here we call the function access_b(), the other access function of the PingPong class.