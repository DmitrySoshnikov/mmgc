/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#pragma once

#include <stdint.h>
#include <vector>
#include <memory>

#include "../MemoryManager/Heap.h"
#include "../MemoryManager/ObjectHeader.h"
#include "../Value/Value.h"

/**
 * Abstract allocator class.
 *
 * Used by garbage collectors to synchronize on Object header structure,
 * and also my Memory Manager to do actual allocation.
 */
struct IAllocator {
  /**
   * Associated heap.
   */
  std::shared_ptr<Heap> heap;

  IAllocator(std::shared_ptr<Heap> heap): heap(heap) {}

  virtual ~IAllocator() {}

  /**
   * Total object count on the heap.
   */
  uint32_t _objectCount;

  /**
   * Allocates `n` bytes (aligned by the size of the machine word).
   *
   * Returns a virtual pointer (Value::Pointer) to the payload.
   */
  virtual Value allocate(uint32_t n) = 0;

  /**
   * Returns the block to the allocator.
   */
  virtual void free(Word address) = 0;

  /**
   * Resets the allocator.
   */
  virtual void reset() = 0;

  /**
   * Returns the pointer to the object header.
   *
   * Depending on the allocator type, the header
   * can be placed in different positions.
   */
  virtual ObjectHeader* getHeader(Word address) = 0;

  /**
   * Returns total amount of objects on the heap.
   */
  virtual uint32_t getObjectCount() = 0;

  /**
   * Returns child pointers of this object.
   */
  virtual std::vector<Value*> getPointers(Word address) = 0;
};
