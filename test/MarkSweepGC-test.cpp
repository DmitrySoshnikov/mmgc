/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#include "MarkSweepGC.h"
#include "MemoryManager.h"
#include "../src/allocators/SingleFreeListAllocator/SingleFreeListAllocator.h"
#include "gtest/gtest.h"

#include <iostream>
#define log(x, y) std::cout << x << " " << y << std::endl

namespace {

static auto heap = std::make_shared<Heap>(32);
static auto allocator = std::make_shared<SingleFreeListAllocator>(heap);
static MarkSweepGC msgc(allocator);

void reset() {
  heap->reset();
  *heap->asWordPointer(0) = ObjectHeader{.size = 28};

  allocator.reset();
}


TEST(MarkSweepGC, API) {
  EXPECT_EQ(msgc.allocator->heap->size(), 32);
}

TEST(MarkSweepGC, collect) {
  reset();

  auto p1 = msgc.allocator->allocate(4);
  auto p2 = msgc.allocator->allocate(4);

  *msgc.allocator->heap->asWordPointer(p1) = p2;

  // Allocate unreachable.
  msgc.allocator->allocate(4);
  msgc.allocator->allocate(4);

  msgc.init();
  EXPECT_EQ(msgc.stats->total, 4);

  msgc.collect();
  EXPECT_EQ(msgc.stats->total, 4);
  EXPECT_EQ(msgc.stats->alive, 2);
  EXPECT_EQ(msgc.stats->reclaimed, 2);

  msgc.init();
  EXPECT_EQ(msgc.stats->total, 2);
}

}  // namespace
