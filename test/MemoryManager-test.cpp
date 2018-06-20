/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#include "MemoryManager.h"
#include "Value.h"
#include "gtest/gtest.h"

namespace {

static MemoryManager mm(32);

TEST(MemoryManager, API) {
  EXPECT_EQ(mm.getHeapSize(), 32);
  EXPECT_EQ(mm.heap.size(), 32);
  EXPECT_EQ(mm.getWordSize(), 4);
  EXPECT_EQ(mm.getWordsCount(), 8);
}

TEST(MemoryManager, reset) {
  mm.reset();

  // Firt object header.
  EXPECT_EQ(mm.readWord(0), 0x0000001C);

  // All other heap elements are reset.
  for (size_t i = 1; i < 8; i++) {
    EXPECT_EQ(mm.heap[i], 0x0);
  }
}

TEST(MemoryManager, readWord) {
  mm.heap[0] = 255;
  EXPECT_EQ(mm.readWord(0), 255);
}

TEST(MemoryManager, writeWord) {
  mm.reset();
  mm.writeWord(0, 255);
  EXPECT_EQ(mm.readWord(0), 255);
}

TEST(MemoryManager, asBytePointer) {
  mm.writeWord(0, 0xCCDDEEFF);

  uint8_t* byteArray = mm.asBytePointer(0);

  EXPECT_EQ(byteArray[0], 0xFF);
  EXPECT_EQ(byteArray[1], 0xEE);
  EXPECT_EQ(byteArray[2], 0xDD);
  EXPECT_EQ(byteArray[3], 0xCC);
}

TEST(MemoryManager, writeByte) {
  mm.reset();

  mm.writeByte(0, 0xFF);
  mm.writeByte(1, 0xEE);
  mm.writeByte(2, 0xDD);
  mm.writeByte(3, 0xCC);

  EXPECT_EQ(mm.readWord(0), 0xCCDDEEFF);
}

TEST(MemoryManager, readByte) {
  mm.reset();
  mm.writeWord(0, 0xCCDDEEFF);

  EXPECT_EQ(mm.readByte(0), 0xFF);
  EXPECT_EQ(mm.readByte(1), 0xEE);
  EXPECT_EQ(mm.readByte(2), 0xDD);
  EXPECT_EQ(mm.readByte(3), 0xCC);

  EXPECT_EQ(mm.readWord(0), 0xCCDDEEFF);
}

TEST(MemoryManager, writeAndReadValue) {
  mm.reset();

  mm.writeValue(0, 3, Type::Number);
  EXPECT_EQ(mm.readValue(0)->decode(), 3);

  mm.writeValue(4, 1, Type::Boolean);
  EXPECT_EQ(mm.readValue(4)->decode(), 1);

  mm.writeValue(8, 0x10, Type::Pointer);
  EXPECT_EQ(mm.readValue(8)->decode(), 0x10);

  mm.writeValue(12, Value::Number(10));
  EXPECT_EQ(mm.readValue(12)->decode(), 10);

  auto v = Value::Number(20);
  mm.writeValue(16, v);
  EXPECT_EQ(mm.readValue(16)->decode(), 20);
}

TEST(MemoryManager, allocate) {
  mm.reset();

  auto p1 = mm.allocate(3);
  EXPECT_EQ(p1.isPointer(), true);
  // 0-3 - header, 4 - payload:
  EXPECT_EQ(p1, 4);

  // 3 is aligned to 4:
  EXPECT_EQ(mm.getHeader(p1)->size, 4);
  EXPECT_EQ(mm.getHeader(p1)->used, 1);

  mm.writeWord(p1, 100);
  EXPECT_EQ(mm.readWord(p1), 100);

  auto p2 = mm.allocate(5);
  // 8-11 - header, 12 - payload:
  EXPECT_EQ(p2, 12);

  // 5 is aligned to 8:
  EXPECT_EQ(mm.getHeader(p2)->size, 8);
  EXPECT_EQ(mm.getHeader(p2)->used, 1);
}

TEST(MemoryManager, sizeOf) {
  mm.reset();

  EXPECT_EQ(mm.sizeOf(mm.allocate(9)), 12);
  EXPECT_EQ(mm.sizeOf(mm.allocate(1)), 4);
  EXPECT_EQ(mm.sizeOf(mm.allocate(4)), 4);
}

TEST(MemoryManager, free) {
  mm.reset();

  auto p1 = mm.allocate(4);
  EXPECT_EQ(p1, 4);

  auto p2 = mm.allocate(4);
  EXPECT_EQ(p2, 12);

  // Free p1, alloc p3, still bump (alloc at 20)
  mm.free(p1);
  auto p3 = mm.allocate(12);
  EXPECT_EQ(p3, 20);

  // Alloc p4 at 4 (from freed p 1)
  auto p4 = mm.allocate(2);
  EXPECT_EQ(p4, 4);
}

TEST(MemoryManager, getPointers) {
  mm.reset();

  auto p1 = mm.allocate(16);
  mm.writeValue(p1, Value::Number(1));
  mm.writeValue(p1 + 1, Value::Pointer(20));
  mm.writeValue(p1 + 2, Value::Boolean(1));
  mm.writeValue(p1 + 3, Value::Pointer(24));

  auto p1Pointers = mm.getPointers(p1);

  EXPECT_EQ(p1Pointers.size(), 2);
  EXPECT_EQ(p1Pointers[0]->decode(), 20);
  EXPECT_EQ(p1Pointers[1]->decode(), 24);
}

TEST(MemoryManager, getObjectCount) {
  mm.reset();

  auto p1 = mm.allocate(8);
  auto p2 = mm.allocate(8);

  EXPECT_EQ(mm.getObjectCount(), 2);

  // Linked list: number value, and next pointer.
  mm.writeValue(p1, Value::Number(1));
  mm.writeValue(p1 + 1, Value::Pointer(p2));

  mm.writeValue(p2, Value::Number(2));
  mm.writeValue(p2 + 1, Value::Pointer(nullptr));

  auto p3 = mm.allocate(4);
  mm.writeValue(p3, Value::Number(10));

  EXPECT_EQ(mm.getObjectCount(), 3);

  mm.free(p3);
  EXPECT_EQ(mm.getObjectCount(), 2);
}

}  // namespace