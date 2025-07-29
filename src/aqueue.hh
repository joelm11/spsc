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
    const size_t kIdx = front_.load(std::memory_order::relaxed);
    return data_[kIdx];
  }

  T back() const noexcept {
    const size_t kIdx =
        (back_.load(std::memory_order::relaxed) + kCapacity_ - 1) % kCapacity_;
    return data_[kIdx];
  }

  // Capacity
  size_t size() const noexcept {
    const size_t kFront = front_.load(std::memory_order::relaxed);
    const size_t kBack = back_.load(std::memory_order::relaxed);

    return kBack >= kFront ? kBack - kFront : kCapacity_ - (kFront - kBack);
  }

  bool empty() const noexcept {
    const size_t kFront = front_.load(std::memory_order::relaxed);
    const size_t kBack = back_.load(std::memory_order::relaxed);

    return kFront == kBack;
  }

  bool full() const noexcept {
    const size_t kFront = front_.load(std::memory_order::relaxed);
    const size_t kBack = back_.load(std::memory_order::relaxed);

    return (kBack + 1) % kCapacity_ == kFront;
  }

  // Modifiers
  bool push(const T &val) noexcept {
    const size_t kFront = front_.load(std::memory_order::relaxed);
    const size_t kBack = back_.load(std::memory_order::acquire);

    if ((kBack + 1) % kCapacity_ == kFront) {
      return false;
    }
    data_[kBack] = val;
    back_.store((kBack + 1) % kCapacity_, std::memory_order::release);
    return true;
  }

  bool pop() noexcept {
    const size_t kFront = front_.load(std::memory_order::acquire);
    const size_t kBack = back_.load(std::memory_order::relaxed);

    if (kFront == kBack) {
      return false;
    }
    front_.store((kFront + 1) % kCapacity_, std::memory_order::release);
    return true;
  }

private:
  static constexpr size_t kCapacity_ = N + 1;
  alignas(64) std::atomic<size_t> front_;
  alignas(64) std::atomic<size_t> back_;
  alignas(64) T data_[kCapacity_];
};
} // namespace spsc
