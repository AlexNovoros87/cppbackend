#include "declaration_structs.h"

void tab(){
std::cout<<"   ";
}


bool IsGreaterEqual(double a, double b) {
    return std::isgreater(a , b) || (std::fabs(a - b) <= std::numeric_limits<double>::epsilon() );
}

bool IsLessEqual(double a, double b) {
    return std::isless(a , b) || (std::fabs(a - b) <= std::numeric_limits<double>::epsilon() );
}

std::ostream& operator<<(std::ostream& os,const model::Point& pt){
   os<<"Point X: "<<pt.x<<" Y: "<<pt.y;
 return os;
}

std::ostream& operator<<(std::ostream& os,const model::DogCoordinates& pt){
   os<<"DogCoordinates " <<"X: "<<pt.x<<" Y: "<<pt.y;
 return os;
}

std::ostream& operator<<(std::ostream& os,const model::DogSpeed& ds){
   os<<"Dogspeed X: "<<ds.speed_x<<" Y: "<<ds.speed_y;
 return os;
}

std::ostream& operator<<(std::ostream& os,const model::NSWE_Direction dir){
    switch(dir){
     case model::NSWE_Direction::NORTH :
     os<<"NORTH";
     break;
     case model::NSWE_Direction::SOUTH :
     os<<"SOUTH";
     break;
     case model::NSWE_Direction::WEST :
     os<<"WEST";
     break;
     case model::NSWE_Direction::EAST :
     os<<"EAST";
     break;
     case model::NSWE_Direction::STOP :
     os<<"STOP";
     break;
     }
     return os;
}

std::ostream &operator<<(std::ostream &os, const model::Limits& lim){
 os<<"LIMITS Xmin: "<<lim.xmin<<" Xmax: "<<lim.xmax<<" Ymin: "<<lim.ymin<<" Ymax: "<<lim.ymax;
return os;
};