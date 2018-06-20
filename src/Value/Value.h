/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#pragma once

#include <stdint.h>
#include <cstddef>

/**
 * Value types.
 */
enum class Type {
  Number,
  Pointer,
  Boolean,
};

/**
 * Represents a "value", which can be of different types.
 *
 * We use tagged-pointers notation here. The LSB as 1 means the
 * value is a number, 0 - a pointer.
 *
 * Fixnum 31-bit value:
 *
 * iiii iiii iiii iii1
 *
 * Pointer:
 *
 * pppp pppp pppp pp00
 *
 * Other "immediate objects" has 4 bits mask with a tag.
 *
 * Booleans, mask: 0110, actual boolean bit is 5:
 *
 * ---- ---- ---b 0110
 */
class Value {
  /**
   * Actual storage.
   */
  uint32_t value_;

 public:
  /**
   * Default constructor.
   */
  Value(uint32_t value) : value_(value) {}

  /**
   * Values constant declarations.
   */
  static const uint32_t TRUE;
  static const uint32_t FALSE;

  /**
   * Returns the type of the value.
   */
  Type getType();

  /**
   * Encodes a binary as a value.
   */
  static Value encode(uint32_t value, Type valueType);

  /**
   * Encodes a number.
   */
  static Value Number(uint32_t value);

  /**
   * Checks whether a value is a Number.
   */
  bool isNumber();

  /**
   * Encodes a pointer.
   */
  static Value Pointer(uint32_t value);
  static Value Pointer(std::nullptr_t _p);

  /**
   * Checks whether a value is a Pointer.
   */
  bool isPointer();

  /**
   * Checks whether a value is a Null Pointer.
   */
  bool isNullPointer();

  /**
   * Encodes a boolean.
   */
  static Value Boolean(uint32_t value);

  /**
   * Checks whether a value is a Boolean.
   */
  bool isBoolean();

  /**
   * Returns the decoded value from this binary.
   */
  uint32_t decode();

  /**
   * Convertion to word.
   */
  operator uint32_t() { return value_; }
  uint32_t toInt() { return value_; }

  /**
   * For convenient address comparison.
   */
  inline friend bool operator==(const Value& v, int i) {
    return i == const_cast<Value&>(v).toInt();
  }
};
