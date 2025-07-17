#include "../spsc_queue.hh"
#include <gtest/gtest.h>

using namespace spsc;

TEST(SPSCQueue, Ctor) { LQueue<int, 16> foo; }

// Test for push and front
TEST(SPSCQueue, PushAndFront) {
  LQueue<int, 16> queue;
  queue.push(42);
  EXPECT_EQ(queue.front(), 42);
}

// Test for push and back
TEST(SPSCQueue, PushAndBack) {
  LQueue<int, 16> queue;
  queue.push(42);
  EXPECT_EQ(queue.back(), 42);
}

// Test for size, empty, and full
TEST(SPSCQueue, CapacityChecks) {
  LQueue<int, 2> queue;
  EXPECT_TRUE(queue.empty());
  queue.push(1);
  EXPECT_EQ(queue.size(), 1);
  EXPECT_FALSE(queue.full());
  queue.push(2);
  EXPECT_TRUE(queue.full());
}

// Test for pop
TEST(SPSCQueue, Pop) {
  LQueue<int, 16> queue;
  queue.push(42);
  queue.push(84);
  queue.pop();
  EXPECT_EQ(queue.front(), 84);
}

// Test for wrap-around behavior
TEST(SPSCQueue, WrapAround) {
  LQueue<int, 3> queue;
  queue.push(1);
  queue.push(2);
  queue.pop();
  queue.push(3);
  EXPECT_EQ(queue.front(), 2);
  EXPECT_EQ(queue.back(), 3);
}
