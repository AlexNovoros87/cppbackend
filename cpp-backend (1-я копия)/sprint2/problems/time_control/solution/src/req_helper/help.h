#pragma once
//#include"request_handler.h"
#include <sstream>
#include <string>
#include <filesystem>
#include <algorithm>

#include "../server/http_server.h"
#include "../game/game.h"
#include "req_static_objects.h"



namespace request_handler{

bool IsSubPath(fs::path path, fs::path base);
bool CheckBaseValid(const std::vector<std::string> &container);
bool IsCorrectWay(std::string_view way);

std::string MakeOneMap(const model::Map *map);
std::string MakeAllMaps(const model::Game &game);
std::string GetExtention(std::string req_body);
std::string CodeMessageBody(std::string code, std::string message);
std::string TokenIdBody(std::string token, uint64_t id);

std::vector<std::string> ParseToken(std::string target);
std::vector<std::string> ParseTarget(std::string target);
TypeRequest GetTypeRequset(const std::vector<std::string>& vec);
DirectionAPI Director(const std::vector<std::string> &container);
TimePoint GetNow();
}

namespace request_handler{

StringResponse Make400(unsigned version, bool alive);
StringResponse Make404(unsigned version, bool alive);

}

namespace request_handler{
     //JSB - JSon-Body
     StringResponse Template(unsigned version, bool alive, bool control);
     StringResponse Make200JSB(unsigned version, bool alive, std::string token, uint32_t id);
     StringResponse Make200JSB(unsigned version, bool alive, std::string body);
     StringResponse Make200State(unsigned version, bool alive, std::shared_ptr<model::GameSession> session);
     StringResponse Make400JSB(unsigned version, bool alive, std::string code, std::string message);
     StringResponse Make401JSB(unsigned version, bool alive, std::string code, std::string message);
     StringResponse Make404JSB(unsigned version, bool alive, std::string code, std::string message);
     StringResponse Make405JSB(unsigned version, bool alive, std::string code, std::string message, std::string allow);
     std::string MakeStateBody(std::shared_ptr<model::GameSession> session_);
     
}