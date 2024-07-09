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
std::string GetExtention(std::string req_body);
bool IsSubPath(fs::path path, fs::path base);

std::string MakeOneMap(const model::Map *map);
std::string MakeAllMaps(const model::Game &game);

std::string CodeMessageBody(std::string code, std::string message);
std::string TokenIdBody(std::string token, uint64_t id);

TypeRequest GetTypeRequset(const std::vector<std::string>& vec);
std::vector<std::string> ParseTarget(std::string target);
DirectionAPI Director(const std::vector<std::string> &container);
bool CheckBaseValid(const std::vector<std::string> &container);
std::vector<std::string> ParseToken(std::string target);


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
     StringResponse Make400JSB(unsigned version, bool alive, std::string code, std::string message);
     StringResponse Make401JSB(unsigned version, bool alive, std::string code, std::string message);
     StringResponse Make404JSB(unsigned version, bool alive, std::string code, std::string message);
     StringResponse Make405JSB(unsigned version, bool alive, std::string code, std::string message, std::string allow);
}