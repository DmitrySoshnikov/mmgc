/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#include <memory>

#include "Heap.h"
#include "ObjectHeader.h"
#include "SingleFreeListAllocator.h"
#include "Value.h"

#include "gtest/gtest.h"

namespace {

static auto heap = std::make_shared<Heap>(32);
static SingleFreeListAllocator allocator(heap);

void reset() {
  heap->reset();
  *heap->asWordPointer(0) = ObjectHeader{.size = 28};

  allocator.reset();
}

TEST(SingleFreeListAllocator, allocate) {
  reset();

  auto p1 = allocator.allocate(3);
  EXPECT_EQ(p1.isPointer(), true);
  // 0-3 - header, 4 - payload:
  EXPECT_EQ(p1, 4);

  // 3 is aligned to 4:
  EXPECT_EQ(allocator.getHeader(p1)->size, 4);
  EXPECT_EQ(allocator.getHeader(p1)->used, 1);

  *heap->asWordPointer(p1) = 100;
  EXPECT_EQ(*heap->asWordPointer(p1), 100);

  auto p2 = allocator.allocate(5);
  // 8-11 - header, 12 - payload:
  EXPECT_EQ(p2, 12);

  // 5 is aligned to 8:
  EXPECT_EQ(allocator.getHeader(p2)->size, 8);
  EXPECT_EQ(allocator.getHeader(p2)->used, 1);
}

TEST(SingleFreeListAllocator, free) {
  reset();

  auto p1 = allocator.allocate(4);
  EXPECT_EQ(p1, 4);

  auto p2 = allocator.allocate(4);
  EXPECT_EQ(p2, 12);

  // Free p1, alloc p3, still bump (alloc at 20)
  allocator.free(p1);
  auto p3 = allocator.allocate(12);
  EXPECT_EQ(p3, 20);

  // Alloc p4 at 4 (from freed p 1)
  auto p4 = allocator.allocate(2);
  EXPECT_EQ(p4, 4);
}

TEST(SingleFreeListAllocator, blockSize) {
  reset();

  auto p1 = allocator.allocate(16);
  EXPECT_EQ(allocator.getHeader(p1)->size, 16);
  EXPECT_EQ(p1, 4);

  auto p2 = allocator.allocate(8);
  EXPECT_EQ(allocator.getHeader(p2)->size, 8);
  EXPECT_EQ(p2, 24);

  allocator.free(p1);
  EXPECT_EQ(allocator.getHeader(p1)->size, 16);
  EXPECT_EQ(allocator.getHeader(p1)->used, false);

  p1 = allocator.allocate(12);
  EXPECT_EQ(p1, 4);
  // Couldn't split, the block is still 16.
  EXPECT_EQ(allocator.getHeader(p1)->size, 16);
  EXPECT_EQ(allocator.getHeader(p1)->used, true);
  EXPECT_EQ(p1, 4);

  allocator.free(p1);
  p1 = allocator.allocate(8);
  EXPECT_EQ(p1, 4);
  // Can split:
  EXPECT_EQ(allocator.getHeader(p1)->size, 8);
  EXPECT_EQ(allocator.getHeader(p1)->used, true);

  p2 = allocator.allocate(4);
  EXPECT_EQ(allocator.getHeader(p2)->size, 4);
  EXPECT_EQ(p2, 16);

}

TEST(SingleFreeListAllocator, reset) {
  reset();

  auto p1 = allocator.allocate(4);
  EXPECT_EQ(p1, 4);

  allocator.reset();

  auto p2 = allocator.allocate(4);
  EXPECT_EQ(p2, 4);
}

TEST(SingleFreeListAllocator, getHeader) {
  reset();

  auto p = allocator.allocate(4);
  auto header = allocator.getHeader(p);
  EXPECT_EQ(header->size, 4);
  EXPECT_EQ(header->used, true);
}

}  // namespace