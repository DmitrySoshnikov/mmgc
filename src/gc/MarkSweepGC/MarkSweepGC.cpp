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
  auto todo = mm->getRoots();

  while (!todo.empty()) {
    auto v = todo.back();
    todo.pop_back();
    auto header = mm->getHeader(v);

    // Mark the object if it's not marked yet, and move to the child pointers.
    if (header->used && header->mark == 0) {
      header->mark = 1;
      stats->alive++;
      for (const auto& p : mm->getPointers(v)) {
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

  while (scan < mm->heap.size()) {
    auto header = mm->getHeader(scan);

    // Alive object, reset the mark bit for future collection cycles.
    if (header->mark == 1) {
      header->mark = 0;
    } else if (header->used) {
      // Garbage, reclaim.
      mm->free(scan);
      stats->reclaimed++;
    }

    // Move to the next block.
    scan += header->size + sizeof(ObjectHeader);
  }
}

/**
 * Resets the stats before the collection cycle.
 */
void MarkSweepGC::_resetStats() {
  stats->total = mm->getObjectCount();
  stats->alive = 0;
  stats->reclaimed = 0;
}