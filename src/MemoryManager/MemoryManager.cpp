/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#include "MemoryManager.h"

/**
 * Resets the memory setting each word to 0.
 */
void MemoryManager::reset() {
  _resetHeap();
  _resetFreeList();
  _resetHeader();
  _objectCount = 0;
}

/**
 * Returns the size of the heap in bytes.
 */
uint32_t MemoryManager::getHeapSize() { return _heapSize; }

/**
 * Returns number of words.
 */
uint32_t MemoryManager::getWordsCount() { return _heapSize / getWordSize(); }

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
void MemoryManager::writeWord(Word address, uint32_t value) {
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
uint8_t MemoryManager::readByte(uint32_t address) { return (*heap)[address]; }

/**
 * Writes a Value at address.
 */
void MemoryManager::writeValue(uint32_t address, uint32_t value,
                               Type valueType) {
  writeWord(address, Value::encode(value, valueType));
}

/**
 * Writes a Value at address.
 */
void MemoryManager::writeValue(uint32_t address, Value& value) {
  if (_writeBarrier != nullptr) {
    _writeBarrier(address, value);
  }
  writeWord(address, value);
}

/**
 * Writes a Value at address.
 */
void MemoryManager::writeValue(uint32_t address, Value&& value) {
  if (_writeBarrier != nullptr) {
    _writeBarrier(address, value);
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
Value MemoryManager::allocate(uint32_t n) {
  n = align<uint32_t>(n);

  for (const auto& free : freeList) {
    auto header = (ObjectHeader*)(asWordPointer(free));
    auto size = header->size;

    // Too small block, move further.
    if (size < n) {
      continue;
    }

    // Found block of a needed size:

    header->used = true;
    header->size = n;

    freeList.remove(free);

    auto rawPayload = ((uint32_t*)header) + 1;
    auto payload = heap->asVirtualAddress(rawPayload);

    auto nextHeaderP = payload + n;

    // Reserver 2 words (for header, and at least one data word).
    if (nextHeaderP <= _heapSize - (getWordSize() * 2)) {
      auto nextHeader = (ObjectHeader*)(asWordPointer(nextHeaderP));
      if (!nextHeader->used) {
        auto nextSize = (uint16_t)(size - n - sizeof(ObjectHeader));
        writeWord(nextHeaderP, ObjectHeader{.size = nextSize});
        freeList.push_back(nextHeaderP);
      }
    }

    // Update total object count.
    _objectCount++;

    return Value::Pointer(payload);
  }

  return Value::Pointer(nullptr);
}

/**
 * Frees previously allocated block. The block should contain
 * correct object header, otherwise the result is not defined.
 */
void MemoryManager::free(Word address) {
  auto header = getHeader(address);

  // Avoid "double-free".
  if (!header->used) {
    return;
  }

  header->used = 0;
  freeList.push_back((uint8_t*)header - asBytePointer(0));

  // Update total object count.
  _objectCount--;
}

/**
 * Returns object header.
 */
ObjectHeader* MemoryManager::getHeader(Word address) {
  return (ObjectHeader*)(asWordPointer(address) - 1);
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
  std::vector<Value*> pointers;

  auto header = getHeader(address);
  auto wordSize = getWordSize();
  auto words = header->size / wordSize;

  while (words-- > 0) {
    auto v = readValue(address);
    address += wordSize;
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
uint32_t MemoryManager::getObjectCount() { return _objectCount; }

/**
 *
 */
std::vector<uint32_t> MemoryManager::getRoots() {
  std::vector<uint32_t> roots;
  // TODO: impelement actual roots, use first block for now.
  roots.push_back(0 + sizeof(ObjectHeader));
  return roots;
}

/**
 * Prints memory dump.
 */
void MemoryManager::dump() {
  std::cout << "\n Memory dump:\n";
  std::cout << "------------------------\n\n";

  int address = 0;
  std::string row = "";

  auto words = asWordPointer(0);

  for (auto i = 0; i < getWordsCount(); i++) {
    auto v = words[i];
    row += int_to_hex(address) + " : ";
    auto value = int_to_hex(v, /*usePrefix*/ false);
    insert_delimeter(value, 2, " ");
    row += value;
    address += getWordSize();
    std::cout << row << std::endl;
    row.clear();
  }

  std::cout << std::endl;
}

/**
 * Initializes the heap to zeros.
 */
void MemoryManager::_resetHeap() {
  memset(&(*heap)[0], 0, getHeapSize());
}

/**
 * Initially the object header stored at the beginning
 * of the heap defines the whole heap as a "free block".
 */
void MemoryManager::_resetHeader() {
  writeWord(0, ObjectHeader{
                   .size = (uint16_t)(_heapSize - sizeof(ObjectHeader)),
                   .used = false,
                   .mark = false,
               });
}

void MemoryManager::_resetFreeList() {
  freeList.clear();
  freeList.push_back(0);
}
