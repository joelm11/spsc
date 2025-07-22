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
  for (auto _ : state) {
    for (int i = 0; i < kQueueSize; ++i) {
      queue.push(i);
    }
  }
}

// Template for pop benchmark
template <typename QueueType> static void BM_QueuePop(benchmark::State &state) {
  QueueType queue;
  for (int i = 0; i < kQueueSize; ++i) {
    queue.push(i);
  }
  for (auto _ : state) {
    for (int i = 0; i < kQueueSize; ++i) {
      queue.pop();
    }
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
      for (int i = 0; i < kQueueSize; ++i) {
        queue.push(i);
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

// Register benchmarks for LQueue
using LQueueType = spsc::LQueue<int, kQueueSize>;
using AQueueType = spsc::AQueue<int, kQueueSize>;
BENCHMARK_TEMPLATE(BM_QueuePush, LQueueType);
BENCHMARK_TEMPLATE(BM_QueuePop, LQueueType);
BENCHMARK_TEMPLATE(BM_QueueProduceConsume, LQueueType);
BENCHMARK_TEMPLATE(BM_QueuePush, AQueueType);
BENCHMARK_TEMPLATE(BM_QueuePop, AQueueType);
BENCHMARK_TEMPLATE(BM_QueueProduceConsume, AQueueType);

BENCHMARK_MAIN();