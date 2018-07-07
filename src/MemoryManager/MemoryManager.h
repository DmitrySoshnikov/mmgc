/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#pragma once

#include <stdint.h>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "../Value/Value.h"
#include "../util/number-util.h"

#include "Heap.h"
#include "ObjectHeader.h"

#include "../allocators/IAllocator.h"
#include "../gc/ICollector.h"

/**
 * Memory manager is a thin wrapper on top of several modules used for
 * automatic memory management.
 *
 * Encapulates:
 *
 *   - `heap`: the actual heap being operated on (allocation, gc cylces)
 *
 *   - `allocator`: a particular allocator, conforming to the
 *                  IAllocator interface
 *
 *   - `collector`: a particular garbage collector
 */
class MemoryManager {
 public:
  /**
   * Virtual heap.
   */
  std::shared_ptr<Heap> heap;

  /**
   * Accociated allocator.
   */
  std::shared_ptr<IAllocator> allocator;

  /**
   * Accociated collector.
   */
  std::shared_ptr<ICollector> collector;

  MemoryManager(
      const std::shared_ptr<Heap> heap,
      const std::shared_ptr<IAllocator> allocator,
      const std::shared_ptr<ICollector> collector = nullptr,
      std::function<void(Word, Value& value)> writeBarrier = nullptr)
      : heap(heap),
        allocator(allocator),
        collector(collector),
        writeBarrier_(writeBarrier) {
    reset();
  }

  /**
   * Template factory.
   */
  template <class Allocator, class Collector, uint32_t heapSize>
  static std::shared_ptr<MemoryManager> create(
      std::function<void(Word, Value& value)> writeBarrier = nullptr) {
    auto heap = std::make_shared<Heap>(heapSize);
    auto allocator = std::make_shared<Allocator>(heap);
    auto collector = std::make_shared<Collector>(allocator);

    return std::make_shared<MemoryManager>(heap, allocator, collector,
                                           writeBarrier);
  }

  /**
   * Template factory.
   */
  template <class Allocator, uint32_t heapSize>
  static std::shared_ptr<MemoryManager> create(
      std::function<void(Word, Value& value)> writeBarrier = nullptr) {
    auto heap = std::make_shared<Heap>(heapSize);
    auto allocator = std::make_shared<Allocator>(heap);

    return std::make_shared<MemoryManager>(heap, allocator, nullptr,
                                           writeBarrier);
  }

  /**
   * Resets the memory setting each word to 0.
   */
  void reset();

  /**
   * Returns the size of the heap in bytes.
   */
  uint32_t getHeapSize();

  /**
   * Returns number of words.
   */
  uint32_t getWordsCount();

  /**
   * Returns number of words.
   */
  uint32_t getWordSize();

  /**
   * Returns a byte array at offset.
   */
  uint8_t* asBytePointer(Word address);

  /**
   * Returns a word array at offset.
   */
  Word* asWordPointer(Word address);

  /**
   * Writes a word at address.
   */
  void writeWord(uint32_t address, uint32_t value);

  /**
   * Reads a word at address.
   */
  Word readWord(Word address);

  /**
   * Writes a byte at word address + byte offset.
   */
  void writeByte(Word address, uint8_t value);

  /**
   * Reads a byte at address, and offset.
   */
  uint8_t readByte(Word address);

  /**
   * Writes a Value at address.
   */
  void writeValue(Word address, uint32_t value, Type valueType);

  /**
   * Writes a Value at address.
   */
  void writeValue(Word address, Value& value);

  /**
   * Writes a Value at address.
   */
  void writeValue(Word address, Value&& value);

  /**
   * Reads a Value at address.
   */
  Value* readValue(Word address);

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
  Value allocate(uint32_t n);

  /**
   * Frees previously allocated block. The block should contain
   * correct object header, otherwise the result is not defined.
   */
  void free(Word address);

  /**
   * Runs a collection cycle.
   */
  std::shared_ptr<GCStats> collect();

  /**
   * Returns object header.
   */
  ObjectHeader* getHeader(Word address);

  /**
   * Sizeof operator.
   */
  uint16_t sizeOf(Word address);

  /**
   * Prints memory dump.
   */
  void dump();

  /**
   * Returns child pointers of this object.
   */
  std::vector<Value*> getPointers(Word address);

  /**
   * Returns total amount of objects on the heap.
   */
  uint32_t getObjectCount();

 private:
  /**
   * Initializes the heap to zeros.
   */
  void _resetHeap();

  /**
   * Initially the object header stored at the beginning
   * of the heap defines the whole heap as a "free block".
   */
  void _initFirstBlock();

  /**
   * Write barrier.
   *
   * Several GC algorithms use write barrier to do extra operation
   * before pointer operations. E.g. Reference Counting collector
   * updates the counter, generation collector handles inter-generational
   * pointers, etc.
   */
  std::function<void(Word, Value& value)> writeBarrier_;
};