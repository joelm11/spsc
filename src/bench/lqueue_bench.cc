#include "../lqueue.hh"
#include <benchmark/benchmark.h>

const size_t kQueueSize = 2048;

using QueueType = spsc::LQueue<int, kQueueSize>;

void produce(QueueType &queue) {
  for (int i = 0; i < kQueueSize; ++i) {
    queue.push(i);
  }
};

void consume(QueueType &queue) {
  for (int i = 0; i < kQueueSize; ++i) {
    queue.pop();
  }
};

static void BM_LQueuePush(benchmark::State &state) {
  spsc::LQueue<int, kQueueSize> queue;
  for (auto _ : state) {
    produce(queue);
  }
}
BENCHMARK(BM_LQueuePush);

static void BM_LQueuePop(benchmark::State &state) {
  spsc::LQueue<int, kQueueSize> queue;
  produce(queue);
  for (auto _ : state) {
    consume(queue);
  }
}
BENCHMARK(BM_LQueuePop);

BENCHMARK_MAIN();