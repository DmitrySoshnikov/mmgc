/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#pragma once

#include <stdint.h>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "../../Value/Value.h"
#include "../../allocators/SingleFreeListAllocator/SingleFreeListAllocator.h"
#include "../../MemoryManager/ObjectHeader.h"

#include "../../util/number-util.h"

/**
 * Stats for the collection cycle.
 */
struct MarkSweepStats {
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
 * Mark-Sweep garbage collector.
 *
 *   - Mark phase: trace, marks reachable objects as alive
 *   - Sweep phase: reclaims memory occupied by unreachable objects
 *
 * Collects stats during collection.
 *
 */
class MarkSweepGC {
 public:
  /**
   * Associated allocator.
   */
  std::shared_ptr<SingleFreeListAllocator> allocator;

  /**
   * Stats for the collection cycle.
   */
  std::shared_ptr<MarkSweepStats> stats;

  MarkSweepGC(const std::shared_ptr<SingleFreeListAllocator>& allocator)
      : allocator(allocator),
        stats(std::make_shared<MarkSweepStats>()),
        _worklist(){};

  /**
   * Main collection cycle.
   */
  std::shared_ptr<MarkSweepStats> collect();

  /**
   * Mark phase. Returns number of live objects.
   */
  void mark();

  /**
   * Sweep phase. Resets the mark bit, reclaims the objects by
   * adding back to the free list.
   */
  void sweep();

  /**
   * Returns root objects (where GC starts analysis from).
   */
  std::vector<Word> getRoots();

 private:
  /**
   * Worklist for marking phase.
   */
  std::list<uint32_t> _worklist;

  /**
   * Resets the stats before the collection cycle.
   */
  void _resetStats();
};