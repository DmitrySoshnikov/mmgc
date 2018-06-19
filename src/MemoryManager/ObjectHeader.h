/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#pragma once

#include <stdint.h>

/**
 * Object header.
 *
 * The header stores meta-information for the Collector, and Allocator
 * purposes. It's located in the word prior to the payload pointer.
 *
 */
struct ObjectHeader {
  /**
   * The block size.
   */
  uint16_t size;

  /**
   * Whether the block is used.
   */
  bool used;

  /**
   * Specific GC data.
   */
  union {
    /**
     * Used by Mark-Sweep GC during trace phase.
     */
    bool mark;

    /**
     * Reference counter in RC collector (up to 255 references).
     */
    uint8_t rc;
  };

  operator uint32_t() { return toInt(); }
  uint32_t toInt() { return *reinterpret_cast<uint32_t*>(this); }
};