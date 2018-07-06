/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#include "MarkSweepGC.h"
#include "MemoryManager.h"
#include "../src/allocators/SingleFreeListAllocator/SingleFreeListAllocator.h"
#include "gtest/gtest.h"

namespace {

static auto heap = std::make_shared<Heap>(32);
static auto sflAllocator = std::make_shared<SingleFreeListAllocator>(heap);
static MarkSweepGC msgc(sflAllocator);

TEST(MarkSweepGC, API) {
  EXPECT_EQ(msgc.allocator->heap->size(), 32);
}
}  // namespace
