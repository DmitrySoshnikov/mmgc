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
  std::vector<uint8_t> storage;

  Heap(uint32_t size) : storage(size, 0) {}

  uint8_t& operator[](int offset) { return storage[offset]; }

  /**
   * Returns the size of the heap.
   */
  uint32_t size() { return storage.size(); }

  /**
   * Returns an actual Word pointer for the virtual pointer address.
   */
  Word* asWordPointer(Word address) { return (Word*)&storage[address]; }

  /**
   * Returns an actual byte pointer for the virtual pointer address.
   */
  uint8_t* asBytePointer(Word address) { return &storage[address]; }

  /**
   * Converts an actual Word pointer to the virtual address.
   */
  Word asVirtualAddress(Word* address) {
    return (uint8_t*)address - asBytePointer(0);
  }

  /**
   * Resets this heap.
   */
  void reset() {
    memset(&storage[0], 0, storage.size());
  }
};