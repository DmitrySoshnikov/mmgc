/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#pragma once

#include <stdint.h>
#include <vector>

/**
 * 32 bit machine word.
 */
using Word = uint32_t;

/**
 * Virtual heap storage with convenient methods of converting
 * between physical, and virtual pointers.
 */
struct Heap {
 public:
  std::vector<uint8_t> storage;

  Heap(uint8_t size): storage(size, 0) {}
  ~Heap() {}

  uint8_t& operator [](int offset) {
    return storage[offset];
  }

  Word* asWordPointer(Word address) {
    return (Word*)&storage[address];
  }

  uint8_t* asBytePointer(Word address) {
    return &storage[address];
  }

  Word asVirtualAddress(Word* address) {
    return (uint8_t*)address - asBytePointer(0);
  }
};