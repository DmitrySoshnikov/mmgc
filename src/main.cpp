/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#include <array>
#include <iostream>
#include <memory>

#include "MemoryManager.h"
#include "ObjectHeader.h"
#include "allocators/SingleFreeListAllocator/SingleFreeListAllocator.h"
#include "Value/Value.h"
#include "MarkSweepGC.h"
#include "util/number-util.h"

#define log(title, value) std::cout << title << " " << value << std::endl

int main(int argc, char* argv[]) {
  auto mm = std::make_shared<MemoryManager>(64);

  auto p1 = mm->allocate(12);

  mm->writeWord(p1, Value::Number(1));
  mm->writeValue(p1 + 2, Value::Pointer(p1));

  auto p2 = mm->allocate(4);
  mm->writeValue(p2, Value::Number(2));
  mm->writeValue(p1 + 1, Value::Pointer(p2));

  // alloc unreachable
  mm->writeValue(mm->allocate(4), Value::Number(3));
  mm->writeValue(mm->allocate(4), Value::Number(4));
  mm->writeValue(mm->allocate(4), Value::Number(5));

  log("\nBefore GC:", "");

  mm->dump();

  auto heap = mm->heap;
  auto sflAllocator = std::make_shared<SingleFreeListAllocator>(heap);

  MarkSweepGC msgc(sflAllocator);

  auto gcStats = msgc.collect();

  log("    total:", gcStats->total);
  log("    alive:", gcStats->alive);
  log("reclaimed:", gcStats->reclaimed);

  log("\nAfter GC:", "");
  mm->dump();

  // Write barrier.
  mm = std::make_shared<MemoryManager>(32, [&](uint32_t address, Value& value) {
    auto prevValue = mm->readValue(address);

    std::cout << "\n-- Write barrier is called -- \n" << std::endl;

    log("  Address:", int_to_hex(address));

    log("\nOld value:", *prevValue);
    log("Old value is pointer:", prevValue->isPointer());

    log("\nNew value:", value);
    log("Old value is pointer:", value.isPointer());
  });

  mm->writeValue(4, Value::Pointer(8));

  mm->writeValue(4, Value::Pointer(12));


  return 0;
}
