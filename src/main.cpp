/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#include <array>
#include <iostream>
#include <memory>

#include "gc/MarkSweepGC/MarkSweepGC.h"
#include "MemoryManager/MemoryManager.h"
#include "MemoryManager/ObjectHeader.h"
#include "Value/Value.h"

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

  MarkSweepGC msgc(mm);

  auto gcStats = msgc.collect();

  log("    total:", gcStats->total);
  log("    alive:", gcStats->alive);
  log("reclaimed:", gcStats->reclaimed);

  log("\nAfter GC:", "");
  mm->dump();

  return 0;
}
