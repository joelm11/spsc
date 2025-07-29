#pragma once
#include "queue_base.hh"
#include <cstddef>
#include <cstdlib>
#include <mutex>

namespace spsc {
template <typename T, size_t N>
class LQueue : public QueueBase<T, LQueue<T, N>> {
public:
  T front() noexcept {
    std::lock_guard<std::mutex> lock(m_);
    return data_[front_];
  }

  const T back() const noexcept {
    std::lock_guard<std::mutex> lock(m_);
    return data_[(back_ + kCapacity_ - 1) % kCapacity_];
  }

  size_t size() const noexcept {
    std::lock_guard<std::mutex> lock(m_);
    return size_is();
  }

  bool empty() const noexcept {
    std::lock_guard<std::mutex> lock(m_);
    return is_empty();
  }

  bool full() const {
    std::lock_guard<std::mutex> lock(m_);
    return is_full();
  }

  bool push(const T &val) noexcept {
    std::lock_guard<std::mutex> lock(m_);
    if (is_full()) {
      return false;
    }
    data_[back_] = val;
    back_ = (back_ + 1) % kCapacity_;
    return true;
  }

  bool pop() noexcept {
    std::lock_guard<std::mutex> lock(m_);
    if (is_empty()) {
      return false;
    }
    front_ = (front_ + 1) % kCapacity_;
    return true;
  }

  bool push_val(const T &val) noexcept {
    std::lock_guard<std::mutex> lock(m_);
    if (is_full()) {
      return false;
    }
    data_[back_] = val;
    back_ = (back_ + 1) % kCapacity_;
    return true;
  }

  bool pop_val(T &val) noexcept {
    std::lock_guard<std::mutex> lock(m_);
    if (is_empty()) {
      return false;
    }
    val = data_[front_];
    front_ = (front_ + 1) % kCapacity_;
    return true;
  }

private:
  inline size_t size_is() const noexcept {
    return back_ >= front_ ? back_ - front_ : kCapacity_ - (front_ - back_);
  }

  inline bool is_empty() const noexcept { return front_ == back_; }

  inline bool is_full() const noexcept {
    return (back_ + 1) % kCapacity_ == front_;
  }

  static constexpr size_t kCapacity_ = N + 1;
  mutable std::mutex m_;
  size_t front_ = 0, back_ = 0;
  T data_[kCapacity_];
};
} // namespace spsc