#pragma once

namespace fs = std::filesystem;

namespace request_handler
{

    namespace beast = boost::beast;
    namespace http = beast::http;
    using StringRequest = http::request<http::string_body>;
    using StringResponse = http::response<http::string_body>;
    using FileResponse = http::response<http::file_body>;
    using VariantResponse = std::variant<StringResponse, FileResponse>;
    using RequestMethod = boost::beast::http::verb;
    using HttpHeader = http::field;

}

namespace request_handler
{

    struct IncorrectResponse
    {
        IncorrectResponse() = delete;
        constexpr static std::string_view WRONG_MAP = "{\n  \"code\": \"mapNotFound\",\n  \"message\": \"Map not found\"\n}";
        constexpr static std::string_view WRONG_REQ = "{\n  \"code\": \"badRequest\", \n  \"message\": \"Bad request\"\n}";
    };

    struct Extensions
    {
        constexpr static std::string_view htm = ".htm";
        constexpr static std::string_view html = ".html";
        constexpr static std::string_view css = ".css";
        constexpr static std::string_view txt = ".txt";
        constexpr static std::string_view json = ".json";
        constexpr static std::string_view js = ".js";
        constexpr static std::string_view xml = ".xml";
        constexpr static std::string_view png = ".png";
        constexpr static std::string_view jpg = ".jpg";
        constexpr static std::string_view jpe = ".jpe";
        constexpr static std::string_view jpeg = ".jpeg";
        constexpr static std::string_view gif = ".gif";
        constexpr static std::string_view bmp = ".bmp";
        constexpr static std::string_view ico = ".ico";
        constexpr static std::string_view tiff = ".tiff";
        constexpr static std::string_view tif = ".tif";
        constexpr static std::string_view svgz = ".svgz";
        constexpr static std::string_view svg = ".svg";
        constexpr static std::string_view mp3 = ".mp3";
        constexpr static std::string_view octet = "";
    };

    const std::unordered_map<std::string_view, std::string_view> type_content{
        {Extensions::htm, "text/html"},
        {Extensions::html, "text/html"},
        {Extensions::css, "text/css"},
        {Extensions::txt, "text/plain"},
        {Extensions::js, "text/javascript"},
        {Extensions::json, "application/json"},
        {Extensions::xml, "application/xml"},
        {Extensions::png, "image/png"},
        {Extensions::jpg, "image/jpeg"},
        {Extensions::jpe, "image/jpeg"},
        {Extensions::jpeg, "image/jpeg"},
        {Extensions::gif, "image/gif"},
        {Extensions::bmp, "image/bmp"},
        {Extensions::ico, "image/vnd.microsoft.icon"},
        {Extensions::tiff, "image/tiff"},
        {Extensions::tif, "image/tiff"},
        {Extensions::svgz, "image/svg+xml"},
        {Extensions::svg, "image/svg+xml"},
        {Extensions::mp3, "audio/mpeg"},
        {Extensions::octet, "application/octet-stream"}};

    struct req_static_str
    {
        req_static_str() = delete;
        static constexpr  boost::json::string_view badRequest = "badRequest";
        static constexpr  boost::json::string_view message = "message";
        static constexpr  boost::json::string_view code = "code";
        static constexpr  boost::json::string_view userName = "userName";
        static constexpr  boost::json::string_view mapId = "mapId";
        static constexpr  boost::json::string_view authToken = "authToken";
        static constexpr  boost::json::string_view playerId = "playerId";
        static constexpr  boost::json::string_view no_cache = "no-cache";
        static constexpr  boost::json::string_view mapNotFound = "mapNotFound";
        static constexpr  boost::json::string_view invalidArgument = "invalidArgument";
        static constexpr  boost::json::string_view invalidMethod = "invalidMethod";
        static constexpr  boost::json::string_view invalidToken = "invalidToken";
        static constexpr  boost::json::string_view unknownToken = "unknownToken";
        static constexpr  boost::json::string_view Allowed_POST = "POST";
        static constexpr  boost::json::string_view Allowed_GET_HEAD = "GET, HEAD";
        static constexpr  boost::json::string_view timeDelta = "timeDelta";
        static constexpr  boost::json::string_view lostObjects = "lostObjects";

        static constexpr  std::string_view pos = "pos";
        static constexpr  std::string_view speed = "speed";
        static constexpr  std::string_view dir = "dir";
        static constexpr  std::string_view players = "players";
        static constexpr  std::string_view move = "move";
        static constexpr  std::string_view type = "type";
        static constexpr  std::string_view bag = "bag";
        static constexpr  std::string_view id = "id";
        static constexpr  std::string_view score = "score";
    };

    struct reason_to_human
    {
        reason_to_human() = delete;
        static constexpr  boost::json::string_view API_Base_Check_Failed = "API Base Check Failed";
        static constexpr  boost::json::string_view Map_not_found = "Map not found";
        static constexpr  boost::json::string_view Invalid_name = "Invalid name";
        static constexpr  boost::json::string_view Join_game_request_parse_error = "Join game request parse error";
        static constexpr  boost::json::string_view Only_POST_method_is_expected = "Only POST method is expected";
        static constexpr  boost::json::string_view Authorization_header_is_missing = "Authorization header is missing";
        static constexpr  boost::json::string_view Player_token_has_not_been_found = "Player token has not been found";
        static constexpr  boost::json::string_view Invalid_method = "Invalid method";
        static constexpr  boost::json::string_view Players_parse_error = "Player parse error";
    };

    struct statuses
    {
        statuses() = delete;
        using responce_status = boost::beast::http::status;
        static constexpr responce_status ST200 = responce_status::ok;
        static constexpr responce_status ST400 = responce_status::bad_request;
        static constexpr responce_status ST401 = responce_status::unauthorized;
        static constexpr responce_status ST404 = responce_status::not_found;
        static constexpr responce_status ST405 = responce_status::method_not_allowed;
    };

    struct json_foo
    {
        json_foo() = delete;
        static constexpr std::string_view state = "state";
        static constexpr std::string_view maps = "maps";
        static constexpr std::string_view game = "game";
        static constexpr std::string_view players = "players";
        static constexpr std::string_view join = "join";
        static constexpr std::string_view api = "api";
        static constexpr std::string_view v1 = "v1";
        static constexpr std::string_view player = "player";
        static constexpr std::string_view action = "action";
        static constexpr std::string_view tick = "tick";
        static constexpr std::string_view records = "records";
        
    };
}

namespace request_handler
{

    const json::value obj_code_message = {{req_static_str::code, ""},
                                          {req_static_str::message, ""}};

    const json::value obj_token_id_player = {{req_static_str::authToken, ""},
                                             {req_static_str::playerId, 0}};

}