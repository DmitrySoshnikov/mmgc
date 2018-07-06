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
    freeList.remove(free);

    auto rawPayload = ((Word*)header) + 1;
    auto payload = heap->asVirtualAddress(rawPayload);

    // See if we can split the larger block, reserving at least
    // one word with a header.
    auto canSplit = (size >= n + (sizeof(Word) * 2));

    if (canSplit) {
      // This block becomes of size `n`.
      header->size = n;

      // Split the new block.
      auto nextHeaderP = payload + n;
      auto nextSize = (uint16_t)(size - n - sizeof(ObjectHeader));
      *heap->asWordPointer(nextHeaderP) = ObjectHeader{.size = nextSize};
      freeList.push_back(nextHeaderP);
    }

    // Update total object count.
    _objectCount++;

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

  // Update total object count.
  _objectCount--;
}

/**
 * Returns the reference to the object header.
 */
ObjectHeader* SingleFreeListAllocator::getHeader(Word address) {
  return (ObjectHeader*)(heap->asWordPointer(address) - 1);
}

/**
 * Returns child pointers of this object.
 */
std::vector<Value*> SingleFreeListAllocator::getPointers(Word address) {
  std::vector<Value*> pointers;

  auto header = getHeader(address);
  auto words = header->size / sizeof(Word);

  while (words-- > 0) {
    auto v = (Value*)heap->asWordPointer(address);
    address += sizeof(Word);
    if (!v->isPointer()) {
      continue;
    }
    pointers.push_back(v);
  }

  return pointers;
}

/**
 * Returns total amount of objects on the heap.
 */
uint32_t SingleFreeListAllocator::getObjectCount() { return _objectCount; }

/**
 * Resets the allocator.
 */
void SingleFreeListAllocator::reset() {
  _resetFreeList();
  _objectCount = 0;
}

void SingleFreeListAllocator::_resetFreeList() {
  freeList.clear();
  freeList.push_back(0);
}
