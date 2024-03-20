#include "sample/sample.hpp"
#include <gtest/gtest.h>

TEST(Sampletest, Sample) 
{
  Sample sample;
  EXPECT_EQ(1, sample.test());
}
