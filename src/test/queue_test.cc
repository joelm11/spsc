#include "../lqueue.hh"
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

using namespace spsc;

constexpr size_t kSz = 2048;

// Define the types to test
using QueueTypes = ::testing::Types<LQueue<int, kSz>>;

// Typed test fixture
template <typename T> class QueueTest : public ::testing::Test {
public:
protected:
  T queue;
};

TYPED_TEST_SUITE(QueueTest, QueueTypes);

TYPED_TEST(QueueTest, Accessors) {
  for (int i = 0; i < kSz; ++i) {
    this->queue.push(i);
  }

  EXPECT_EQ(this->queue.front(), 0);
  EXPECT_EQ(this->queue.back(), kSz - 1);
}

TYPED_TEST(QueueTest, Capacity) {
  EXPECT_TRUE(this->queue.empty());
  EXPECT_EQ(this->queue.size(), 0);

  for (int i = 0; i < kSz; ++i) {
    this->queue.push(i);
  }

  EXPECT_EQ(this->queue.size(), kSz);
  EXPECT_TRUE(this->queue.full());
  EXPECT_FALSE(this->queue.empty());
}

TYPED_TEST(QueueTest, Modifiers) {
  for (int i = 0; i < kSz; ++i) {
    this->queue.push(i);
  }

  EXPECT_FALSE(this->queue.push(0));
  EXPECT_EQ(this->queue.size(), kSz);
  EXPECT_TRUE(this->queue.full());
  EXPECT_FALSE(this->queue.empty());

  for (int i = 0; i < kSz; ++i) {
    this->queue.pop();
  }

  EXPECT_FALSE(this->queue.pop());
  EXPECT_TRUE(this->queue.empty());
  EXPECT_EQ(this->queue.size(), 0);
}

TYPED_TEST(QueueTest, ThreadSafety) {
  const auto produce = [this]() {
    for (int i = 0; i < kSz; ++i) {
      this->queue.push(i);
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  };

  std::vector<int> vals;
  const auto consume = [this, &vals]() {
    int i = 0;
    while (i < kSz) {
      if (!this->queue.empty()) {
        vals.push_back(this->queue.front());
        this->queue.pop();
        ++i;
      }
    }
  };

  std::thread prod_thread(produce);
  std::thread con_thread(consume);

  prod_thread.join();
  con_thread.join();

  EXPECT_EQ(vals.size(), kSz);
  for (int i = 0; i < vals.size(); ++i) {
    EXPECT_EQ(vals[i], i);
  }
}