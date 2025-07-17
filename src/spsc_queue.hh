#pragma once
#include <cstddef>
#include <cstdlib>

namespace spsc {
template <typename T, size_t N> class LQueue {
public:
  // Accessors
  T &front() noexcept { return data_[front_]; }
  const T &front() const noexcept { return data_[front_]; }

  T &back() noexcept { return data_[back_]; }
  const T &back() const noexcept { return data_[back_]; }

  // Capacity
  size_t size() const { return std::abs(front_ - back_); }
  bool empty() const { return front_ == back_; }
  bool full() const { return (back_ + 1) % kCapacity_ == front_; }

  // Modifiers
  void push(const T &val) {
    data_[back_] = val;
    back_ = (back_ + 1) % kCapacity_;
  }

  void pop() { front_ = (front_ + 1) % kCapacity_; }

private:
  static constexpr size_t kCapacity_ = N + 1;
  size_t front_ = 0, back_ = 0;
  T data_[kCapacity_];
};
} // namespace spsc