#pragma once

#include <cstddef>

namespace spsc {
template <typename T, class Derived> class QueueBase {
public:
  // Accessors
  T front() noexcept { return derived().front(); }

  T back() const noexcept { return derived().back(); }

  // Capacity
  size_t size() const noexcept { return derived().size(); }

  bool empty() const noexcept { return derived().empty(); }

  bool full() const noexcept { return derived().full(); }

  // Modifiers
  bool push(const T &val) noexcept { return derived().push(val); }

  bool pop() noexcept { return derived().pop(); }

private:
  Derived &derived() noexcept { return static_cast<Derived &>(*this); }

  const Derived &derived() const noexcept {
    return static_cast<const Derived &>(*this);
  }
};
} // namespace spsc
