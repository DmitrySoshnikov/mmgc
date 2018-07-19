/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#include "MarkSweepGC.h"
#include "../../MemoryManager/ObjectHeader.h"

#include <iostream>

/**
 * Main collection cycle.
 */
std::shared_ptr<GCStats> MarkSweepGC::collect() {
  _resetStats();
  mark();
  sweep();
  return stats;
}

/**
 * Mark phase. Returns number of live objects.
 */
void MarkSweepGC::mark() {
  auto worklist = getRoots();

  while (!worklist.empty()) {
    auto v = worklist.back();
    worklist.pop_back();
    auto header = allocator->getHeader(v);

    // Mark the object if it's not marked yet, and move to the child pointers.
    if (header->mark == 0) {
      header->mark = 1;
      stats->alive++;
      for (const auto& p : allocator->getPointers(v)) {
        worklist.push_back(p->decode());
      }
    }
  }
}

/**
 * Sweep phase. Resets the mark bit, reclaims the objects by
 * adding back to the free list.
 */
void MarkSweepGC::sweep() {
  auto scan = 0 + sizeof(ObjectHeader);

  while (scan < allocator->heap->size()) {
    auto header = allocator->getHeader(scan);

    // Alive object, reset the mark bit for future collection cycles.
    if (header->mark == 1) {
      header->mark = 0;
    } else {
      // Garbage, reclaim.
      allocator->free(scan);
      stats->reclaimed++;
    }

    // Move to the next block.
    scan += header->size + sizeof(ObjectHeader);
  }
}
