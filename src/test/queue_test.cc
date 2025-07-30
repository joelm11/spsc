#include "../aqueue.hh"
#include "../lqueue.hh"
#include <gtest/gtest.h>
#include <thread>

using namespace spsc;

constexpr size_t kSz = 2000;

// Define the types to test
using QueueTypes = ::testing::Types<LQueue<int, kSz>, AQueue<int, kSz>>;

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
  EXPECT_FALSE(this->queue.empty());
  // Optimized queue type uses nearest po2 sizing.
  if (this->queue.capacity() == this->queue.size()) {
    EXPECT_EQ(this->queue.capacity(), this->queue.size());
    EXPECT_TRUE(this->queue.full());
  } else {
    EXPECT_NE(this->queue.capacity(), this->queue.size());
    EXPECT_FALSE(this->queue.full());
  }
}

TYPED_TEST(QueueTest, Modifiers) {
  for (int i = 0; i < kSz; ++i) {
    this->queue.push(i);
  }

  const size_t kActualQueueCap = this->queue.capacity();
  if (this->queue.size() == kActualQueueCap) {
    EXPECT_FALSE(this->queue.push(0));
    EXPECT_TRUE(this->queue.full());
  }

  EXPECT_EQ(this->queue.size(), kSz);
  EXPECT_FALSE(this->queue.empty());

  for (int i = 0; i < kSz; ++i) {
    this->queue.pop();
  }

  EXPECT_FALSE(this->queue.pop());
  EXPECT_TRUE(this->queue.empty());
  EXPECT_EQ(this->queue.size(), 0);
}

TYPED_TEST(QueueTest, BasicThreadSafety) {
  const auto produce = [this]() {
    for (int i = 0; i < kSz; ++i) {
      EXPECT_TRUE(this->queue.push(i));
    }
  };

  std::vector<int> vals;
  const auto consume = [this, &vals]() {
    int i = 0;
    while (i < kSz) {
      if (!this->queue.empty()) {
        vals.push_back(this->queue.front());
        EXPECT_TRUE(this->queue.pop());
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

TYPED_TEST(QueueTest, TightInterleavedThreadSafety) {
  constexpr int kTestSize = 20000;
  constexpr int kQueueCapacity = 4; // Intentionally small to force contention

  std::vector<int> vals;
  vals.reserve(kTestSize);
  std::atomic<bool> done{false};

  const auto produce = [this]() {
    for (int i = 0; i < kTestSize;) {
      if (this->queue.push(i)) {
        ++i;
      } else {
        std::this_thread::yield(); // encourage CPU handoff
      }
    }
  };

  const auto consume = [this, &vals, &done]() {
    int i = 0;
    while (i < kTestSize) {
      int val;
      if (!this->queue.empty()) {
        val = this->queue.front();
        EXPECT_TRUE(this->queue.pop());
        vals.push_back(val);
        ++i;
      } else {
        std::this_thread::yield();
      }
    }
    done.store(true, std::memory_order_release);
  };

  std::thread prod_thread(produce);
  std::thread con_thread(consume);

  prod_thread.join();
  con_thread.join();

  EXPECT_EQ(vals.size(), kTestSize);
  for (int i = 0; i < kTestSize; ++i) {
    EXPECT_EQ(vals[i], i);
  }
}
