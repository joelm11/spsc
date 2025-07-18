#include "../lqueue.hh"
#include <chrono>
#include <cstddef>
#include <gtest/gtest.h>
#include <thread>

using namespace spsc;

class LQueueTest : public testing::Test {
public:
  static const size_t kSz = 2048;

protected:
  LQueueTest() = default;
  LQueue<int, kSz> queue;
};

const size_t LQueueTest::kSz;

TEST_F(LQueueTest, Accessors) {
  for (int i = 0; i < kSz; ++i) {
    queue.push(i);
  }

  EXPECT_EQ(queue.front(), 0);
  EXPECT_EQ(queue.back(), kSz - 1);
}

TEST_F(LQueueTest, Capacity) {
  EXPECT_TRUE(queue.empty());
  EXPECT_EQ(queue.size(), 0);

  for (int i = 0; i < kSz; ++i) {
    queue.push(i);
  }

  EXPECT_EQ(queue.size(), kSz);
  EXPECT_TRUE(queue.full());
  EXPECT_FALSE(queue.empty());
}

TEST_F(LQueueTest, Modifiers) {
  for (int i = 0; i < kSz; ++i) {
    queue.push(i);
  }

  EXPECT_FALSE(queue.push(0));
  EXPECT_EQ(queue.size(), kSz);
  EXPECT_TRUE(queue.full());
  EXPECT_FALSE(queue.empty());

  for (int i = 0; i < kSz; ++i) {
    queue.pop();
  }

  EXPECT_FALSE(queue.pop());
  EXPECT_TRUE(queue.empty());
  EXPECT_EQ(queue.size(), 0);
}

// Basic thread safety test. More comprehensive coverage is done in
// benchmarking
TEST_F(LQueueTest, ThreadSafety) {
  const auto produce = [this]() {
    for (int i = 0; i < kSz; ++i) {
      queue.push(i);
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  };

  std::vector<int> vals;
  const auto consume = [this, &vals]() {
    // Take a value off the queue when one is ready
    int i = 0;
    while (i < kSz) {
      if (!queue.empty()) {
        vals.push_back(queue.front());
        queue.pop();
        ++i;
      }
    }
  };

  std::thread prod_thread(produce), con_thread(consume);

  prod_thread.join();
  con_thread.join();

  EXPECT_EQ(vals.size(), kSz);
  for (int i = 0; i < vals.size(); ++i) {
    EXPECT_EQ(vals[i], i);
  }
}