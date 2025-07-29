#include "../aqueue.hh"
#include "../lqueue.hh"
#include <atomic>
#include <benchmark/benchmark.h>
#include <thread>

const size_t kQueueSize = 2048;

// Template for push benchmark
template <typename QueueType>
static void BM_QueuePush(benchmark::State &state) {
  QueueType queue;
  typename QueueType::ValueType val{};
  for (auto _ : state) {
    for (int i = 0; i < kQueueSize; ++i) {
      queue.push(val);
    }
  }
}

// Template for pop benchmark
template <typename QueueType> static void BM_QueuePop(benchmark::State &state) {
  QueueType queue;
  typename QueueType::ValueType val{};
  for (int i = 0; i < kQueueSize; ++i) {
    queue.push(val);
  }
  for (auto _ : state) {
    for (int i = 0; i < kQueueSize; ++i) {
      queue.pop();
    }
  }
}

// Template for push_val benchmark
template <typename QueueType>
static void BM_QueuePushVal(benchmark::State &state) {
  QueueType queue;
  typename QueueType::ValueType val{};
  for (auto _ : state) {
    for (int i = 0; i < kQueueSize; ++i) {
      queue.push_val(val);
    }
  }
}

// Template for pop_val benchmark
template <typename QueueType>
static void BM_QueuePopVal(benchmark::State &state) {
  QueueType queue;
  typename QueueType::ValueType init_val{};
  for (int i = 0; i < kQueueSize; ++i) {
    queue.push_val(init_val);
  }
  for (auto _ : state) {
    for (int i = 0; i < kQueueSize; ++i) {
      typename QueueType::ValueType val;
      queue.pop_val(val);
    }
  }
}

// Template for produce-consume with values benchmark
template <typename QueueType>
static void BM_QueueProduceConsumeVal(benchmark::State &state) {
  for (auto _ : state) {
    QueueType queue;
    std::atomic<bool> start_flag{false};

    auto producer = [&queue, &start_flag]() {
      while (!start_flag.load(std::memory_order_acquire)) {
        std::this_thread::yield();
      }
      typename QueueType::ValueType val{};
      for (int i = 0; i < kQueueSize; ++i) {
        queue.push_val(val);
      }
    };

    auto consumer = [&queue, &start_flag]() {
      size_t items_popped = 0;
      typename QueueType::ValueType val;
      while (!start_flag.load(std::memory_order_acquire)) {
        std::this_thread::yield();
      }
      while (items_popped < kQueueSize) {
        if (queue.pop_val(val)) {
          ++items_popped;
        }
      }
    };

    std::thread producer_thread(producer);
    std::thread consumer_thread(consumer);

    start_flag.store(true, std::memory_order_release);

    producer_thread.join();
    consumer_thread.join();
  }
}

// Template for produce-consume benchmark
template <typename QueueType>
static void BM_QueueProduceConsume(benchmark::State &state) {
  for (auto _ : state) {
    QueueType queue;
    std::atomic<bool> start_flag{false};

    auto producer = [&queue, &start_flag]() {
      while (!start_flag.load(std::memory_order_acquire)) {
        std::this_thread::yield();
      }
      typename QueueType::ValueType val{};
      for (int i = 0; i < kQueueSize; ++i) {
        queue.push(val);
      }
    };

    auto consumer = [&queue, &start_flag]() {
      size_t items_popped = 0;
      while (!start_flag.load(std::memory_order_acquire)) {
        std::this_thread::yield();
      }
      while (items_popped < kQueueSize) {
        if (queue.pop()) {
          ++items_popped;
        }
      }
    };

    std::thread producer_thread(producer);
    std::thread consumer_thread(consumer);

    start_flag.store(true, std::memory_order_release);

    producer_thread.join();
    consumer_thread.join();
  }
}

// Register benchmarks for queue types
using LQueueType = spsc::LQueue<std::string, kQueueSize>;
using AQueueType = spsc::AQueue<std::string, kQueueSize>;

BENCHMARK_TEMPLATE(BM_QueuePush, LQueueType);
BENCHMARK_TEMPLATE(BM_QueuePop, LQueueType);
BENCHMARK_TEMPLATE(BM_QueueProduceConsume, LQueueType);
// BENCHMARK_TEMPLATE(BM_QueuePushVal, LQueueType);
// BENCHMARK_TEMPLATE(BM_QueuePopVal, LQueueType);
// BENCHMARK_TEMPLATE(BM_QueueProduceConsumeVal, LQueueType);

BENCHMARK_TEMPLATE(BM_QueuePush, AQueueType);
BENCHMARK_TEMPLATE(BM_QueuePop, AQueueType);
BENCHMARK_TEMPLATE(BM_QueueProduceConsume, AQueueType);
// BENCHMARK_TEMPLATE(BM_QueuePushVal, AQueueType);
// BENCHMARK_TEMPLATE(BM_QueuePopVal, AQueueType);
// BENCHMARK_TEMPLATE(BM_QueueProduceConsumeVal, AQueueType);

BENCHMARK_MAIN();