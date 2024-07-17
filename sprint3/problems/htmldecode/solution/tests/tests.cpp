#include <catch2/catch_test_macros.hpp>

#include "../src/htmldecode.h"

using namespace std::literals;

TEST_CASE("Text without mnemonics", "[HtmlDecode]") {
    CHECK(HtmlDecode(""sv) == ""s);
    CHECK(HtmlDecode("hello"sv) == "hello"s);
    REQUIRE( HtmlDecode("Johnson&Johnson")  == "Johnson&Johnson");
    REQUIRE( HtmlDecode("Johnson&ampJohnson")  == "Johnson&Johnson");
    REQUIRE( HtmlDecode("Johnson&ampJohnson")  == "Johnson&Johnson");
    REQUIRE( HtmlDecode("Johnson&AMP;Johnson")  == "Johnson&Johnson");
    REQUIRE( HtmlDecode("Johnson&AMPJohnson")  == "Johnson&Johnson");
    REQUIRE( HtmlDecode("&abracadabra")  == "&abracadabra");
    REQUIRE( HtmlDecode("M&amp;M&APOSs")  == "M&M's");
    REQUIRE( HtmlDecode("&amp;lt;")  == "&lt;");
    REQUIRE( HtmlDecode("&amp;lt;")  != "<");
    REQUIRE( HtmlDecode("&AMPJohnson&AMPJohnson&AMP")  == "&Johnson&Johnson&");
    REQUIRE( HtmlDecode("&AMP;Johnson&AMP;Johnson&AMP;")  == "&Johnson&Johnson&");
    REQUIRE( HtmlDecode("&AM;Johnson&AM;Johnson&AM;")  == "&AM;Johnson&AM;Johnson&AM;");
    REQUIRE( HtmlDecode("&AmP;Johnson&AmP;Johnson&AmP;")  == "&AmP;Johnson&AmP;Johnson&AmP;");
    REQUIRE( HtmlDecode("&amp;Johnson&amp;Johnson&amp;")  == "&Johnson&Johnson&");
    REQUIRE( HtmlDecode("&amp;Johnson&amp;Johnson&am;")  == "&Johnson&Johnson&am;");
}

// Напишите недостающие тесты самостоятельно
