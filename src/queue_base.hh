#pragma once

#include <cstddef>

namespace spsc {
template <typename T, class Derived> class QueueBase {
public:
  // Accessors
  T front() noexcept { return derived().front_impl(); }

  const T back() const noexcept { return derived().back_impl(); }

  // Capacity
  size_t size() const { return derived().size_impl(); }

  bool empty() const { return derived().empty_impl(); }

  bool full() const { return derived().full_impl(); }

  // Modifiers
  bool push(const T &val) { return derived().push_impl(val); }

  bool pop() { return derived().pop_impl(); }

private:
  Derived &derived() { return static_cast<Derived &>(*this); }

  const Derived &derived() const { return static_cast<const Derived &>(*this); }
};
} // namespace spsc
