/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#include "MemoryManager.h"

/**
 * Resets the memory setting each word to 0.
 */
void MemoryManager::reset() {
  heap->reset();
  allocator->reset();
}

/**
 * Returns the size of the heap in bytes.
 */
uint32_t MemoryManager::getHeapSize() { return heap->size(); }

/**
 * Returns number of words.
 */
uint32_t MemoryManager::getWordsCount() { return heap->size() / getWordSize(); }

/**
 * Returns number of words.
 */
uint32_t MemoryManager::getWordSize() { return sizeof(Word); }

/**
 * Returns a byte array at offset.
 */
uint8_t* MemoryManager::asBytePointer(Word address) {
  return heap->asBytePointer(address);
}

/**
 * Returns a word array at offset.
 */
Word* MemoryManager::asWordPointer(Word address) {
  return heap->asWordPointer(address);
}

/**
 * Writes a word at address.
 */
void MemoryManager::writeWord(Word address, Word value) {
  *asWordPointer(address) = value;
}

/**
 * Reads a word at address.
 */
uint32_t MemoryManager::readWord(Word address) {
  return *asWordPointer(address);
}

/**
 * Writes a byte at word address + byte offset.
 */
void MemoryManager::writeByte(Word address, uint8_t value) {
  (*heap)[address] = value;
}

/**
 * Reads a byte at address, and offset.
 */
uint8_t MemoryManager::readByte(Word address) { return (*heap)[address]; }

/**
 * Writes a Value at address.
 */
void MemoryManager::writeValue(Word address, Word value, Type valueType) {
  writeWord(address, Value::encode(value, valueType));
}

/**
 * Writes a Value at address.
 */
void MemoryManager::writeValue(uint32_t address, Value& value) {
  if (writeBarrier_ != nullptr) {
    writeBarrier_(address, value);
  }
  writeWord(address, value);
}

/**
 * Writes a Value at address.
 */
void MemoryManager::writeValue(uint32_t address, Value&& value) {
  if (writeBarrier_ != nullptr) {
    writeBarrier_(address, value);
  }
  writeWord(address, value);
}

/**
 * Reads a Value at address.
 */
Value* MemoryManager::readValue(uint32_t address) {
  return (Value*)asWordPointer(address);
}

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
Value MemoryManager::allocate(uint32_t n) { return allocator->allocate(n); }

/**
 * Frees previously allocated block. The block should contain
 * correct object header, otherwise the result is not defined.
 */
void MemoryManager::free(Word address) { allocator->free(address); }

/**
 * Runs a collection cycle.
 */
std::shared_ptr<GCStats> MemoryManager::collect() {
  if (!collector) {
    throw std::runtime_error("Collector is not specified.");
  }

  return collector->collect();
}

/**
 * Returns object header.
 */
ObjectHeader* MemoryManager::getHeader(Word address) {
  return allocator->getHeader(address);
}

/**
 * Sizeof operator.
 */
uint16_t MemoryManager::sizeOf(Word address) {
  return getHeader(address)->size;
}

/**
 * Returns child pointers of this object.
 */
std::vector<Value*> MemoryManager::getPointers(Word address) {
  return allocator->getPointers(address);
}

/**
 * Returns total amount of objects on the heap.
 */
uint32_t MemoryManager::getObjectCount() { return allocator->getObjectCount(); }

/**
 * Prints memory dump.
 */
void MemoryManager::dump() { heap->dump(); }
