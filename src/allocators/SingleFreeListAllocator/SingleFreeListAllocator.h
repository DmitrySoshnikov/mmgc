/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#pragma once

#include <stdint.h>
#include <list>

#include "../IAllocator.h"

class SingleFreeListAllocator {
  /**
   * Free list: linked list of all free memory chunks.
   */
  std::list<uint32_t> freeList;

 public:
  /**
   * Associated heap.
   */
  std::shared_ptr<Heap> heap;

  explicit SingleFreeListAllocator(std::shared_ptr<Heap> heap)
      : heap(heap), freeList() {
    reset();
  }

  /**
   * Allocates a memory chunk with an object header.
   * The payload pointer is set to the first byte (after the header).
   *
   * This returns a "virtual pointer" (Value::Pointer), the byte address in
   * the virtual heap. To convert it to C++ pointer, use `asWordPointer(p)` or
   * `asBytePointer(p)`.
   *
   * Value::Pointer(nullptr) payload signals OOM.
   */
  Value allocate(uint32_t n);

  /**
   * Returns the block to the allocator.
   */
  void free(Word address);

  /**
   * Resets the allocator.
   */
  void reset();

  /**
   * Returns the reference to the object header.
   */
  ObjectHeader* getHeader(Word address);

 private:
  void _resetFreeList();
};
