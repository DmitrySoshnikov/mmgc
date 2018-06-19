/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#include "ObjectHeader.h"
#include "gtest/gtest.h"

namespace {

TEST(Header, MarkSweep) {
  ObjectHeader header = {
      .size = 0xA,
      .used = true,
      .mark = true,
  };

  EXPECT_EQ(header.size, 0xA);
  EXPECT_EQ(header.used, true);
  EXPECT_EQ(header.mark, true);

  EXPECT_EQ(header.toInt(), 0x0101000A);
  EXPECT_EQ((uint32_t)header, header.toInt());

  header.used = false;
  EXPECT_EQ(header.toInt(), 0x0100000A);

  header.mark = false;
  EXPECT_EQ(header.toInt(), 0x0000000A);

  header.size = 0xFF;
  EXPECT_EQ(header.toInt(), 0x000000FF);
}

TEST(Header, ReferenceCounting) {
  ObjectHeader header = {
      .size = 0xA,
      .used = true,
      .rc = 10,
  };

  EXPECT_EQ(header.size, 0xA);
  EXPECT_EQ(header.used, true);
  EXPECT_EQ(header.rc, 10);

  EXPECT_EQ(header.toInt(), 0x0A01000A);
  EXPECT_EQ((uint32_t)header, header.toInt());

  header.used = false;
  EXPECT_EQ(header.toInt(), 0x0A00000A);

  header.rc++;
  EXPECT_EQ(header.toInt(), 0x0B00000A);

  header.size = 0xFF;
  EXPECT_EQ(header.toInt(), 0x0B0000FF);
}

}  // namespace