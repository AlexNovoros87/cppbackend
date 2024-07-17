#include <gtest/gtest.h>
#include "../src/urlencode.h"





using namespace std::literals;

TEST(UrlEncodeTestSuite, OrdinaryCharsAreNotEncoded) {
    std::string hcpp = "hello C++";
    std::string hcppch0 = hcpp + char(0);

    
    
    EXPECT_EQ(UrlEncode("hello"sv), "hello"s);
    EXPECT_EQ(UrlEncode(hcpp), "hello+C%2B%2B"s);
    EXPECT_EQ(UrlEncode(hcppch0), "hello+C%2B%2B%00"s);
    ASSERT_FALSE(hcppch0 == "hello+C%2B%2B%001");

}

/* Напишите остальные тесты самостоятельно */
