/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#include "SingleFreeListAllocator.h"

#include "../../util/number-util.h"

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
Value SingleFreeListAllocator::allocate(uint32_t n) {
  n = align<uint32_t>(n);

  for (const auto& free : freeList) {
    auto header = (ObjectHeader*)(heap->asWordPointer(free));
    auto size = header->size;

    // Too small block, move further.
    if (size < n) {
      continue;
    }

    // Found block of a needed size:

    header->used = true;
    header->size = n;

    freeList.remove(free);

    auto rawPayload = ((Word*)header) + 1;
    auto payload = heap->asVirtualAddress(rawPayload);

    auto nextHeaderP = payload + n;

    // Reserver 2 words (for header, and at least one data word).
    if (nextHeaderP <= heap->size() - (sizeof(Word) * 2)) {
      auto nextHeader = (ObjectHeader*)(heap->asWordPointer(nextHeaderP));
      if (!nextHeader->used) {
        auto nextSize = (uint16_t)(size - n - sizeof(ObjectHeader));
        *heap->asWordPointer(nextHeaderP) = ObjectHeader{.size = nextSize};
        freeList.push_back(nextHeaderP);
      }
    }

    return Value::Pointer(payload);
  }

  return Value::Pointer(nullptr);
}

/**
 * Returns the block to the allocator.
 */
void SingleFreeListAllocator::free(Word address) {
  auto header = getHeader(address);

  // Avoid "double-free".
  if (!header->used) {
    return;
  }

  header->used = 0;
  freeList.push_back((uint8_t*)header - heap->asBytePointer(0));
}

/**
 * Returns the reference to the object header.
 */
ObjectHeader* SingleFreeListAllocator::getHeader(Word address) {
  return (ObjectHeader*)(heap->asWordPointer(address) - 1);
}

/**
 * Resets the allocator.
 */
void SingleFreeListAllocator::reset() { _resetFreeList(); }

void SingleFreeListAllocator::_resetFreeList() {
  freeList.clear();
  freeList.push_back(0);
}