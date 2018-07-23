/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#pragma once

#include <list>
#include <memory>

#include "../ICollector.h"

#include "../../Value/Value.h"
#include "../../allocators/IAllocator.h"

/**
 * Mark-Compact garbage collector.
 *
 *   - Mark phase: trace, marks reachable objects as alive
 *   - Compact phase: relocates objects in place using Lisp2 algorithm.
 *
 * Collects stats during collection.
 *
 */
class MarkCompactGC : public ICollector {
 public:
  MarkCompactGC(const std::shared_ptr<IAllocator>& allocator)
      : ICollector(allocator){};

  /**
   * Main collection cycle.
   */
  std::shared_ptr<GCStats> collect();

  /**
   * Mark phase. Returns number of live objects.
   */
  void mark();

  /**
   * Compact phase. Resets the mark bit, relocates objects to
   * one side of the heap, doing a defragmentation, using Lisp2 algorithm.
   */
  void compact();

 private:
  /**
   * Computes new locations for the objects.
   */
  void _computeLocations();

  /**
   * Updates child references of the object according
   * to the new locations.
   */
  void _updateReferences();

  /**
   * Relocates the objects to the new locations.
   */
  void _relocate();
};