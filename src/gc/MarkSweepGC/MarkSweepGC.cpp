/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#include "MarkSweepGC.h"

#include <iostream>

/**
 * Main collection cycle.
 */
std::shared_ptr<MarkSweepStats> MarkSweepGC::collect() {
  _resetStats();
  mark();
  sweep();
  return stats;
}

/**
 * Mark phase. Returns number of live objects.
 */
void MarkSweepGC::mark() {
  _worklist.clear();
  auto todo = getRoots();

  while (!todo.empty()) {
    auto v = todo.back();
    todo.pop_back();
    auto header = allocator->getHeader(v);

    // Mark the object if it's not marked yet, and move to the child pointers.
    if (header->used && header->mark == 0) {
      header->mark = 1;
      stats->alive++;
      for (const auto& p : allocator->getPointers(v)) {
        todo.push_back(p->decode());
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
    } else if (header->used) {
      // Garbage, reclaim.
      allocator->free(scan);
      stats->reclaimed++;
    }

    // Move to the next block.
    scan += header->size + sizeof(ObjectHeader);
  }
}

/**
 * Returns the GC roots.
 */
std::vector<uint32_t> MarkSweepGC::getRoots() {
  std::vector<uint32_t> roots;
  // TODO: impelement actual roots, use first block for now.
  roots.push_back(0 + sizeof(ObjectHeader));
  return roots;
}

/**
 * Resets the stats before the collection cycle.
 */
void MarkSweepGC::_resetStats() {
  stats->total = allocator->getObjectCount();
  stats->alive = 0;
  stats->reclaimed = 0;
}