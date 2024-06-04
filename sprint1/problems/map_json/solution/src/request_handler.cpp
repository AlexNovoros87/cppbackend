#include "request_handler.h"

std::string MakeOneMap(const model::Map* map){
 std::string oss;
 oss.append("  \"id\": \"").append(*map->GetId()).append("\",\n  \"name\": \"").
     append(map->GetName()).append("\",\n  \"roads\": [\n");
   auto& roads = map->GetRoads();
   for(int i=0; i< roads.size(); ++i){
    oss.append("    ").append(roads[i].GetJsonType());
    if(i+1 < roads.size()) oss.append(",");
    oss.append("\n");
  }
  oss.append("  ],\n      \"buildings\": [\n");

   auto& offices = map->GetOffices();
   for(int i=0; i< offices.size(); ++i){
   oss.append("    ").append(offices[i].GetJsonType());
    if(i+1 < offices.size()) oss.append(",");
    oss.append("\n");
   }
   oss.append("  ],\n      \"offices\": [\n");
 
   auto& buildings = map->GetBuildings();
   for(int i=0; i< buildings.size(); ++i){
     oss.append("    ").append(buildings[i].GetJsonType());
    if(i+1 < buildings.size()) oss.append(",");
    oss.append("\n");
   }
   oss.append("  ]");
   return oss;
};

std::string MakeAllMaps(const model::Game& game){
 auto& maps = game.GetMaps();
          std::string oss;
          oss.append("[");
          for (size_t i = 0; i< maps.size(); ++i){
            oss.append("{\"id\": \"" + *maps[i].GetId() + "\", \"name\": \"" + maps[i].GetName() + "\"}");
            if(i+ 1 < maps.size()) oss.append(",");
          };
          oss.append("]");
return oss;
};

std::vector<std::string>ParseTarget(std::string target){
   std::vector<std::string> targets;
   if(target.empty()) return targets;
   target = target.substr(1);
   char lastchar;  
   std::string oss; 
   for(char ch : target){
      
      if(ch == '/'){
       if(lastchar == '/') return{};
       if(!oss.empty())targets.push_back(std::move(oss));
       oss.clear();
       lastchar = ch;
      }
      else if(!isdigit(ch) && !isalpha(ch)){ //((ch< 'A' && ch> 'Z') && (ch< 'a' && ch> 'z')) && (ch!= '_' && ch!='-') 
          if(ch!= '_' && ch!='-') return{};
      }
      else{
      oss.push_back(ch);
       lastchar = ch;
      }
     }
      if(!oss.empty())targets.push_back(std::move(oss));
   
   return targets;
}

bool CheckValid(const std::vector<std::string>& container){
     if(container.size() < 3 || container.size()>4) return false;
     if(container[0] != "api") return false;
     if(container[1] != "v1") return false;
     if(container[2] != "maps" ) return false;
     return true;
};

namespace http_handler {

}  // namespace http_handler
