#define BOOST_TEST_MODULE urlencode tests
#include <boost/test/unit_test.hpp>

#include "../src/urldecode.h"

BOOST_AUTO_TEST_CASE(UrlDecode_tests) {
    using namespace std::literals;

       BOOST_TEST(UrlDecode(""sv) == ""s);
       // Напишите остальные тесты для функции UrlDecode самостоятельно
      BOOST_REQUIRE_THROW(UrlDecode("Hello+World%2Z%21"sv) , std::invalid_argument);
      BOOST_REQUIRE_THROW(UrlDecode("Hello+World%20%2"sv) , std::invalid_argument);
      BOOST_REQUIRE_THROW(UrlDecode("%2"sv) , std::invalid_argument);
      BOOST_TEST(UrlDecode("I love C%2b%2B+very+strong%21%21"sv) == "I love C++ very strong!!"s);
      BOOST_TEST(UrlDecode("Vova+Mikhailov+is+the+best%21%21%21"sv) == "Vova Mikhailov is the best!!!"s);
      BOOST_TEST(UrlDecode("Dasha+and+Denis+hate+me%21%21%21"sv) == "Dasha and Denis hate me!!!"s);
}