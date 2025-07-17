#include "../spsc_queue.hh"
#include <gtest/gtest.h>

using namespace spsc;

TEST(SPSCQueue, Ctor) { LQueue<int, 16> foo; }
