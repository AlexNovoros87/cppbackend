#pragma once
#include <unordered_map>

namespace http_handler
{
    namespace beast = boost::beast;
    namespace http = beast::http;

    struct IncorrectResponse
    {
        IncorrectResponse() = delete;
        constexpr static std::string_view WRONG_MAP = "{\n  \"code\": \"mapNotFound\",\n  \"message\": \"Map not found\"\n}";
        constexpr static std::string_view WRONG_REQ = "{\n  \"code\": \"badRequest\", \n  \"message\": \"Bad request\"\n}";
    };

    const std::unordered_map<std::string_view, std::string_view> type_content{
        {".htm", "text/html"},
        {".html", "text/html"},
        {".css", "text/css"},
        {".txt", "text/plain"},
        {".js", "text/javascript"},
        {".json", "application/json"},
        {".xml", "application/xml"},
        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".jpe", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".gif", "image/gif"},
        {".bmp", "image/bmp"},
        {".ico", "image/vnd.microsoft.icon"},
        {".tiff", "image/tiff"},
        {".tif", "image/tiff"},
        {".svgz", "image/svg+xml"},
        {".svg", "image/svg+xml"},
        {".mp3", "audio/mpeg"},
        {"", "application/octet-stream"}};

    enum TypeRequest
    {
        API,
        STATIC,
        ERROR
    };

}