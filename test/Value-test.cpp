/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#include "Value.h"
#include "gtest/gtest.h"

namespace {

TEST(Value, getType) {
  EXPECT_EQ(Value(0b0).getType(), Type::Pointer);
  EXPECT_EQ(Value(0b10).getType(), Type::Pointer);
  EXPECT_EQ(Value(0b100).getType(), Type::Pointer);
  EXPECT_EQ(Value(0b1000).getType(), Type::Pointer);

  EXPECT_EQ(Value(0b111).getType(), Type::Number);
  EXPECT_EQ(Value(0b001).getType(), Type::Number);
  EXPECT_EQ(Value(0b101001).getType(), Type::Number);

  EXPECT_EQ(Value(0b10110).getType(), Type::Boolean);
  EXPECT_EQ(Value(0b00110).getType(), Type::Boolean);
}

TEST(Value, encode) {
  EXPECT_EQ(Value::encode(0b0, Type::Pointer).toInt(), 0b0);
  EXPECT_EQ(Value::encode(0b10, Type::Pointer).toInt(), 0b10);
  EXPECT_EQ(Value::encode(0b100, Type::Pointer).toInt(), 0b100);
  EXPECT_EQ(Value::encode(0b1000, Type::Pointer).toInt(), 0b1000);

  EXPECT_EQ(Value::encode(0b11, Type::Number).toInt(), 0b111);
  EXPECT_EQ(Value::encode(0b0, Type::Number).toInt(), 0b01);
  EXPECT_EQ(Value::encode(0b10100, Type::Number).toInt(), 0b101001);

  EXPECT_EQ(Value::encode(1, Type::Boolean).toInt(), Value::TRUE);
  EXPECT_EQ(Value::encode(0, Type::Boolean).toInt(), Value::FALSE);
}

TEST(Value, Number) {
  EXPECT_EQ(Value::Number(0b11).toInt(),
            Value::encode(0b11, Type::Number).toInt());
}

TEST(Value, Pointer) {
  EXPECT_EQ(Value::Pointer(0b10).toInt(),
            Value::encode(0b10, Type::Pointer).toInt());

  // nullptr is always 0.
  EXPECT_EQ(Value::Pointer(nullptr).toInt(), 0);
}

TEST(Value, Boolean) {
  EXPECT_EQ(Value::Boolean(1).toInt(),
            Value::encode(1, Type::Boolean).toInt());
}

TEST(Value, isNumber) {
  EXPECT_EQ(Value::Number(0b11).isNumber(), true);
  EXPECT_EQ(Value::Pointer(0b10).isNumber(), false);
}

TEST(Value, isPointer) {
  EXPECT_EQ(Value::Pointer(0b10).isPointer(), true);
  EXPECT_EQ(Value::Number(0b11).isPointer(), false);
}

TEST(Value, isNullPointer) {
  EXPECT_EQ(Value::Pointer(0b10).isPointer(), true);
  EXPECT_EQ(Value::Pointer(0b10).isNullPointer(), false);

  EXPECT_EQ(Value::Pointer(nullptr).isPointer(), true);
  EXPECT_EQ(Value::Pointer(nullptr).isNullPointer(), true);
}

TEST(Value, PointerArithmetics) {
  auto p = Value::Pointer(8);

  // Word aligned arithmetics.
  EXPECT_EQ(p + 1, 12);
  EXPECT_EQ(p + 2, 16);

  // Prefix ++.
  EXPECT_EQ(++p, 12);
  EXPECT_EQ(p, 12);

  // // Postfix ++.
  EXPECT_EQ(p++, 12);
  EXPECT_EQ(p, 16);

  EXPECT_EQ(p - 1, 12);
  EXPECT_EQ(p - 2, 8);

  // Prefix --.
  EXPECT_EQ(--p, 12);
  EXPECT_EQ(p, 12);

  // // Postfix --.
  EXPECT_EQ(p--, 12);
  EXPECT_EQ(p, 8);
}

TEST(Value, isBoolean) {
  EXPECT_EQ(Value::Boolean(1).isBoolean(), true);
  EXPECT_EQ(Value::Number(1).isBoolean(), false);
}

TEST(Value, decode) {
  EXPECT_EQ(Value(0b0).decode(), 0b0);
  EXPECT_EQ(Value(0b10).decode(), 0b10);
  EXPECT_EQ(Value(0b100).decode(), 0b100);
  EXPECT_EQ(Value(0b1000).decode(), 0b1000);

  EXPECT_EQ(Value(0b111).decode(), 0b11);
  EXPECT_EQ(Value(0b001).decode(), 0b0);
  EXPECT_EQ(Value(0b101001).decode(), 0b10100);

  EXPECT_EQ(Value(Value::TRUE).decode(), 1);
  EXPECT_EQ(Value(Value::FALSE).decode(), 0);
}

}  // namespace