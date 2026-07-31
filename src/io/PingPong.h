/**
 * @author Edwin Löffler (edwin.loeffler AT uni-jena.de)
 * @author Lara Schwarze (lara.schwarze AT uni-jena.de)
 *
 * @section DESCRIPTION
 * Synchronization primitive for turn-style data ownership.
 **/
#ifndef TSUNAMI_LAB_SETUPS_PING_PONG_H
#define TSUNAMI_LAB_SETUPS_PING_PONG_H

#include <condition_variable>
#include <mutex>

class PingPong {
  public:
    class Guard {
      public:
        Guard(const Guard &) = delete;
        Guard &operator=(const Guard &) = delete;

        ~Guard();

      private:
        friend PingPong;

        Guard(PingPong &this_) : this_(this_) {}

        PingPong &this_;
    };

    PingPong() = default;

    PingPong(const PingPong &) = delete;
    PingPong &operator=(const PingPong &) = delete;

    Guard access_a();
    Guard access_b();

    void wait_a();
    void wait_b();

  private:
    std::mutex m_;
    std::condition_variable cv_;

    bool turn_ = true;
};

#endif
