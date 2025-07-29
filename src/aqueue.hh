#pragma once

#include "queue_base.hh"
#include <atomic>
#include <cstddef>

namespace spsc {

template <typename T, size_t N>
class AQueue : public QueueBase<T, AQueue<T, N>> {
public:
  AQueue() : front_(0), back_(0) {}

  // Accessors
  T front() noexcept {
    size_t f = front_.load(std::memory_order_relaxed);
    return data_[f];
  }

  const T back() const noexcept {
    size_t b = back_.load(std::memory_order_relaxed);
    size_t last = (b + kCapacity_ - 1) % kCapacity_;
    return data_[last];
  }

  // Modifiers
  bool push(const T &val) noexcept {
    size_t b = back_.load(std::memory_order_relaxed);
    size_t next = (b + 1) % kCapacity_;
    if (next == front_.load(std::memory_order_acquire))
      return false; // full

    data_[b] = val;
    back_.store(next, std::memory_order_release);
    return true;
  }

  bool pop() noexcept {
    size_t f = front_.load(std::memory_order_relaxed);
    if (f == back_.load(std::memory_order_acquire))
      return false; // empty

    front_.store((f + 1) % kCapacity_, std::memory_order_release);
    return true;
  }

  // Capacity
  size_t size() const noexcept {
    size_t b = back_.load(std::memory_order_acquire);
    size_t f = front_.load(std::memory_order_acquire);
    return b >= f ? b - f : kCapacity_ - (f - b);
  }

  bool empty() const noexcept {
    return front_.load(std::memory_order_acquire) ==
           back_.load(std::memory_order_acquire);
  }

  bool full() const noexcept {
    size_t b = back_.load(std::memory_order_relaxed);
    size_t next = (b + 1) % kCapacity_;
    return next == front_.load(std::memory_order_acquire);
  }

private:
  static constexpr size_t kCapacity_ = N + 1;
  alignas(64) std::atomic<size_t> front_;
  alignas(64) std::atomic<size_t> back_;
  alignas(64) T data_[kCapacity_];
};

} // namespace spsc
