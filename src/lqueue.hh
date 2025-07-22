#pragma once
#include "queue_base.hh"
#include <cstddef>
#include <cstdlib>
#include <mutex>

namespace spsc {
template <typename T, size_t N>
class LQueue : public QueueBase<T, LQueue<T, N>> {
public:
  // Implementation of QueueBase methods
  T front_impl() noexcept {
    std::lock_guard<std::mutex> lock(m_);
    return data_[front_];
  }

  const T back_impl() const noexcept {
    std::lock_guard<std::mutex> lock(m_);
    return data_[(back_ + kCapacity_ - 1) % kCapacity_];
  }

  size_t size_impl() const {
    std::lock_guard<std::mutex> lock(m_);
    return size_is();
  }

  bool empty_impl() const {
    std::lock_guard<std::mutex> lock(m_);
    return is_empty();
  }

  bool full_impl() const {
    std::lock_guard<std::mutex> lock(m_);
    return is_full();
  }

  bool push_impl(const T &val) {
    std::lock_guard<std::mutex> lock(m_);
    if (is_full()) {
      return false;
    } else {
      data_[back_] = val;
      back_ = (back_ + 1) % kCapacity_;
      return true;
    }
  }

  bool pop_impl() {
    std::lock_guard<std::mutex> lock(m_);
    if (is_empty()) {
      return false;
    } else {
      front_ = (front_ + 1) % kCapacity_;
      return true;
    }
  }

private:
  size_t size_is() const {
    return back_ >= front_ ? back_ - front_ : kCapacity_ - (front_ - back_);
  }
  bool is_empty() const { return front_ == back_; }
  bool is_full() const { return (back_ + 1) % kCapacity_ == front_; }

  static constexpr size_t kCapacity_ = N + 1;
  mutable std::mutex m_;
  size_t front_ = 0, back_ = 0;
  T data_[kCapacity_];
};
} // namespace spsc