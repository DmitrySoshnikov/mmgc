/**
 * The MIT License (MIT)
 * Copyright (c) 2018-present Dmitry Soshnikov <dmitry.soshnikov@gmail.com>
 */

#include "MarkSweepGC.h"
#include "MemoryManager.h"
#include "gtest/gtest.h"

namespace {

static MarkSweepGC msgc(std::make_shared<MemoryManager>(32));

TEST(MarkSweepGC, API) {
  EXPECT_EQ(msgc.mm->getHeapSize(), 32);
}
}  // namespace
