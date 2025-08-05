#include <gtest/gtest.h>
#include <errors.h>


// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(ReadReg, InvalidParams){
  std::byte buff[10];//create buffer
  EXPECT_EQ(readReg(nullptr,1,nullptr),ERR_CODE_INVALID_ARG);//test for null imputs
  EXPECT_EQ(readReg(DATA_START,0,nullptr),ERR_CODE_INVALID_ARG);//TEST FOr 0 input
  EXPECT_EQ(readReg(nullptr,1,nullptr),ERR_CODE_INVALID_ARG);//test for null imputs
  EXPECT_EQ(readReg(DATA_START,1,buff),ERR_CODE_INVALID_ARG);//test for null imputs
}
