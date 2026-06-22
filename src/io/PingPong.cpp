#include "PingPong.h"

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
