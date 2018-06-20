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
  if (_value & 1) {
    return Type::Number;
  } else if (_value == Value::TRUE || _value == Value::FALSE) {
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
bool Value::isNumber() { return getType() == Type::Number; }

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
bool Value::isPointer() { return getType() == Type::Pointer; }

/**
 * Checks whether a value is a Null Pointer.
 */
bool Value::isNullPointer() { return isPointer() && _value == 0; }

/**
 * Encodes a boolean.
 */
Value Value::Boolean(uint32_t value) { return encode(value, Type::Boolean); }

/**
 * Checks whether a value is a Boolean.
 */
bool Value::isBoolean() { return getType() == Type::Boolean; }

/**
 * Returns the decoded value from this binary.
 */
uint32_t Value::decode() {
  switch (getType()) {
    case Type::Number:
      return _value >> 1;
    case Type::Boolean:
      return (_value >> 4) & 1;
    case Type::Pointer:
      return _value;
    default:
      throw std::invalid_argument("Value::decode: unknown type.");
  }
}

/**
 * Enforces the pointer.
 */
inline void Value::_enforcePointer() {
  if (!isPointer()) {
    throw std::invalid_argument("Value is not a Pointer.");
  }
}

/**
 * Pointer arithmetics: p + i
 */
Value Value::operator +(int i) {
  _enforcePointer();
  return _value + (i * sizeof(uint32_t));
}

/**
 * Pointer arithmetics: p += i
 */
Value Value::operator +=(int i) {
  _enforcePointer();
  _value += (i * sizeof(uint32_t));
  return *this;
}

/**
 * Pointer arithmetics: ++p
 */
Value& Value::operator ++() {
  _enforcePointer();
  _value += sizeof(uint32_t);
  return *this;
}

/**
 * Pointer arithmetics: p++
 */
Value Value::operator ++(int) {
  _enforcePointer();
  auto prev = _value;
  _value += sizeof(uint32_t);
  return prev;
}

/**
 * Pointer arithmetics: p - i
 */
Value Value::operator -(int i) {
  _enforcePointer();
  return _value - (i * sizeof(uint32_t));
}

/**
 * Pointer arithmetics: p -= i
 */
Value Value::operator -=(int i) {
  _enforcePointer();
  _value -= (i * sizeof(uint32_t));
  return *this;
}

/**
 * Pointer arithmetics: --p
 */
Value& Value::operator --() {
  _enforcePointer();
  _value -= sizeof(uint32_t);
  return *this;
}

/**
 * Pointer arithmetics: p--
 */
Value Value::operator --(int) {
  _enforcePointer();
  auto prev = _value;
  _value -= sizeof(uint32_t);
  return prev;
}

/**
 * Encoded boolean values.
 */
const uint32_t Value::TRUE = 0b10110;
const uint32_t Value::FALSE = 0b00110;
