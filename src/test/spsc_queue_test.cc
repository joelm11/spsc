#include "../spsc_queue.hh"
#include <cstddef>
#include <gtest/gtest.h>

using namespace spsc;

class LQueueTest : public testing::Test {
public:
  static const size_t kSz = 16;

protected:
  LQueueTest() = default;
  LQueue<int, kSz> queue;
};

TEST_F(LQueueTest, Accessors) {
  for (int i = 0; i < kSz; ++i) {
    queue.push(i);
  }

  EXPECT_EQ(queue.front(), 0);
  EXPECT_EQ(queue.back(), 15);
}

TEST_F(LQueueTest, Capacity) {
  EXPECT_TRUE(queue.empty());
  EXPECT_EQ(queue.size(), 0);

  for (int i = 0; i < kSz; ++i) {
    queue.push(i);
  }

  EXPECT_EQ(queue.size(), 16);
  EXPECT_TRUE(queue.full());
  EXPECT_FALSE(queue.empty());
}

TEST_F(LQueueTest, Modifiers) {
  for (int i = 0; i < kSz; ++i) {
    queue.push(i);
  }

  EXPECT_FALSE(queue.push(0));
  EXPECT_EQ(queue.size(), 16);
  EXPECT_TRUE(queue.full());
  EXPECT_FALSE(queue.empty());

  for (int i = 0; i < kSz; ++i) {
    queue.pop();
  }

  EXPECT_FALSE(queue.pop());
  EXPECT_TRUE(queue.empty());
  EXPECT_EQ(queue.size(), 0);
}