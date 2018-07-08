/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#pragma once

#include <stdint.h>
#include <memory>
#include <vector>

#include "../allocators/IAllocator.h"

#include "../MemoryManager/Heap.h"
#include "../MemoryManager/ObjectHeader.h"

/**
 * Stats for the collection cycle.
 */
struct GCStats {
  /**
   * The snapshot of the total objects on the heap before collection.
   */
  uint32_t total;

  /**
   * Number of alive (reachable) objects.
   */
  uint32_t alive;

  /**
   * Number of reclaimed objects.
   */
  uint32_t reclaimed;
};

/**
 * Abstract collector class.
 *
 * Used as a base class for all Garbage collection classes.
 *
 * Works in pair with the associated allocator, and shares the heap
 * with this allocator.
 */
class ICollector {
 public:
  /**
   * Associated allocator.
   */
  std::shared_ptr<IAllocator> allocator;

  /**
   * Stats for the collection cycle.
   */
  std::shared_ptr<GCStats> stats;

  ICollector(std::shared_ptr<IAllocator> allocator)
      : allocator(allocator), stats(std::make_shared<GCStats>()) {}

  virtual ~ICollector() {}

  /**
   * Executes a collection cycle.
   */
  virtual std::shared_ptr<GCStats> collect() = 0;

  /**
   * Returns GC roots.
   */
  std::vector<Word> getRoots() {
    std::vector<Word> roots;
    // TODO: impelement actual roots, use first block for now.
    roots.push_back(0 + sizeof(ObjectHeader));
    return roots;
  }

  /**
   * Initializes the collector for the cycle.
   */
  void init() {
    _resetStats();
  }

 protected:
  /**
   * Resets the GC stats.
   */
  void _resetStats() {
    stats->total = allocator->getObjectCount();
    stats->alive = 0;
    stats->reclaimed = 0;
  }
};
