/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#include "Value.h"
#include <stdexcept>
#include <string>

/**
 * Returns the type of the value.
 */
Type Value::getType() {
  if (value_ & 1) {
    return Type::Number;
  } else if (value_ == Value::TRUE || value_ == Value::FALSE) {
    return Type::Boolean;
  }
  return Type::Pointer;
}

/**
 * Encodes a binary as a value.
 */
Value Value::encode(uint32_t value, Type valueType) {
  switch (valueType) {
    case Type::Number:
      return Value((value << 1) | 1);
    case Type::Boolean:
      return Value(value == 1 ? TRUE : FALSE);
    case Type::Pointer:
      return Value(value);
    default:
      throw std::invalid_argument("Value::encode: unknown type.");
  }
}

/**
 * Encodes a number.
 */
Value Value::Number(uint32_t value) { return encode(value, Type::Number); }

/**
 * Checks whether a value is a Number.
 */
bool Value::isNumber() {
  return getType() == Type::Number;
}

/**
 * Encodes a pointer.
 */
Value Value::Pointer(uint32_t value) { return encode(value, Type::Pointer); }

/**
 * Encodes a nullptr.
 */
Value Value::Pointer(std::nullptr_t _p) { return encode(0, Type::Pointer); }

/**
 * Checks whether a value is a Pointer.
 */
bool Value::isPointer() {
  return getType() == Type::Pointer;
}

/**
 * Checks whether a value is a Null Pointer.
 */
bool Value::isNullPointer() {
  return isPointer() && value_ == 0;
}

/**
 * Encodes a boolean.
 */
Value Value::Boolean(uint32_t value) { return encode(value, Type::Boolean); }

/**
 * Checks whether a value is a Boolean.
 */
bool Value::isBoolean() {
  return getType() == Type::Boolean;
}

/**
 * Returns the decoded value from this binary.
 */
uint32_t Value::decode() {
  switch (getType()) {
    case Type::Number:
      return value_ >> 1;
    case Type::Boolean:
      return (value_ >> 4) & 1;
    case Type::Pointer:
      return value_;
    default:
      throw std::invalid_argument("Value::decode: unknown type.");
  }
}

/**
 * Encoded boolean values.
 */
const uint32_t Value::TRUE = 0b10110;
const uint32_t Value::FALSE = 0b00110;
