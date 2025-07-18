#include "../lqueue.hh"
#include <atomic>
#include <benchmark/benchmark.h>
#include <thread>

const size_t kQueueSize = 2048;

using QueueType = spsc::LQueue<int, kQueueSize>;

static void BM_LQueuePush(benchmark::State &state) {
  spsc::LQueue<int, kQueueSize> queue;
  for (auto _ : state) {
    for (int i = 0; i < kQueueSize; ++i) {
      queue.push(i);
    }
  }
}
BENCHMARK(BM_LQueuePush);

static void BM_LQueuePop(benchmark::State &state) {
  spsc::LQueue<int, kQueueSize> queue;
  for (int i = 0; i < kQueueSize; ++i) {
    queue.push(i);
  }
  for (auto _ : state) {
    for (int i = 0; i < kQueueSize; ++i) {
      queue.pop();
    }
  }
}
BENCHMARK(BM_LQueuePop);

static void BM_LQueueProduceConsume(benchmark::State &state) {
  for (auto _ : state) {
    spsc::LQueue<int, kQueueSize> queue;
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
BENCHMARK(BM_LQueueProduceConsume);

BENCHMARK_MAIN();