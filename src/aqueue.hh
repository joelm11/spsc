#pragma once

#include "queue_base.hh"
#include <atomic>
#include <cstddef>

namespace spsc {

template <typename T, size_t N>
class AQueue : public QueueBase<T, AQueue<T, N>> {
public:
  using ValueType = T;

  constexpr AQueue() : front_(0), back_(0) {}

  // Accessors
  T front() noexcept {
    const size_t kIdx = front_.load(std::memory_order::acquire);
    return data_[kIdx];
  }

  T back() const noexcept {
    const size_t kIdx =
        (back_.load(std::memory_order::acquire) + kCapacity_ - 1) & kMask_;
    return data_[kIdx];
  }

  // Capacity
  size_t size() const noexcept {
    const size_t kFront = front_.load(std::memory_order::acquire);
    const size_t kBack = back_.load(std::memory_order::acquire);
    return (kBack - kFront) & kMask_;
  }

  size_t capacity() const noexcept { return kCapacity_ - 1; }

  bool empty() const noexcept {
    const size_t kFront = front_.load(std::memory_order::acquire);
    const size_t kBack = back_.load(std::memory_order::acquire);
    return kFront == kBack;
  }

  bool full() const noexcept {
    const size_t kFront = front_.load(std::memory_order::acquire);
    const size_t kBack = back_.load(std::memory_order::acquire);
    return ((kBack + 1) & kMask_) == kFront;
  }

  // Modifiers
  bool push(const T &val) noexcept {
    const size_t kFront = front_.load(std::memory_order::relaxed);
    const size_t kBack = back_.load(std::memory_order::acquire);

    if (((kBack + 1) & kMask_) == kFront) {
      return false; // full
    }
    data_[kBack] = val;
    back_.store((kBack + 1) & kMask_, std::memory_order::release);
    return true;
  }

  bool pop() noexcept {
    const size_t kFront = front_.load(std::memory_order::acquire);
    const size_t kBack = back_.load(std::memory_order::relaxed);

    if (kFront == kBack) {
      return false; // empty
    }
    front_.store((kFront + 1) & kMask_, std::memory_order::release);
    return true;
  }

  bool push_val(const T &val) noexcept {
    const size_t kFront = front_.load(std::memory_order::relaxed);
    const size_t kBack = back_.load(std::memory_order::acquire);

    if (((kBack + 1) & kMask_) == kFront) {
      return false;
    }
    data_[kBack] = val;
    back_.store((kBack + 1) & kMask_, std::memory_order::release);
    return true;
  }

  bool pop_val(T &val) noexcept {
    const size_t kFront = front_.load(std::memory_order::acquire);
    const size_t kBack = back_.load(std::memory_order::relaxed);

    if (kFront == kBack) {
      return false;
    }
    val = data_[kFront];
    front_.store((kFront + 1) & kMask_, std::memory_order::release);
    return true;
  }

private:
  static constexpr size_t next_power_of_two(size_t n) {
    size_t power = 1;
    while (power < n) {
      power <<= 1;
    }
    return power;
  }

  static constexpr size_t kCapacity_ = next_power_of_two(N + 1);
  static constexpr size_t kMask_ = kCapacity_ - 1;

  alignas(128) std::atomic<size_t> front_;
  alignas(128) std::atomic<size_t> back_;
  alignas(128) T data_[kCapacity_];
};
} // namespace spsc
