#include "PingPong.h"

PingPong::Guard PingPong::access_a() {
    wait_a();
    return PingPong::Guard(*this);
}

PingPong::Guard PingPong::access_b() {
    wait_b();
    return Guard(*this);
}

void PingPong::wait_a() {
    std::unique_lock<std::mutex> lock(m_);
    cv_.wait(lock, [&] { return turn_; });
}

void PingPong::wait_b() {
    std::unique_lock<std::mutex> lock(m_);
    cv_.wait(lock, [&] { return !turn_; });
}

PingPong::Guard::~Guard() {
    std::lock_guard<std::mutex> lock(this_.m_);
    this_.turn_ ^= true;
    this_.cv_.notify_one();
}
