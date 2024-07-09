#include "tokenizer.h"



std::string PlayerTokens::GenerateHEXToken() 
    {
        uint64_t first = generator1_();
        uint64_t second = generator2_();
        return NumToHex(first) + NumToHex(second);
    }

    uint64_t PlayerTokens::RoadPositionRandom(uint64_t value)  {
       if(value <0) return 0;
       uint64_t generated = generator1_();
       uint64_t generated_to_delimeter = generator2_();
       
       
       uint64_t delimeter = 0;
       delimeter = generated_to_delimeter % 5;
       if(delimeter == 1 || delimeter ==0 ) delimeter+=10;

       while(generated > (value + 1)){
         generated /= delimeter;
       }
       return generated == 0? 0 : generated - 1;    
    }

    std::string PlayerTokens::NumToHex(uint64_t num)
    {
        std::stringstream ss;
        ss << std::hex << num;
        std::stringstream ss2;
        ss2 << std::setw(16) << std::setfill('0') << ss.str();
        return ss2.str();
    }

     model::DogCoordinates PlayerTokens::GetRandomRoadPoint(const model::Road& road){
     const auto& begin = road.GetStart();
     const auto& end = road.GetEnd(); 
     
     double min , max;
     
     bool X_equal = (begin.x == end.x);
     bool Y_equal = (begin.y == end.y);

     model::DogCoordinates result;
    
     if(X_equal || Y_equal){
      
       if(X_equal){ 
        result.x = begin.x;
        min = std::min(begin.y, end.y);
        max = std::max(begin.y, end.y);
       }
       else { result.y = begin.y;
        min = std::min(begin.x, end.x);
        max = std::max(begin.x, end.x);
       }  
       
      double random =  static_cast<double>(generator1_()) / 100;
      uint64_t to_delimeter = generator2_();

      short delimeter = 0;
      while (delimeter == 0 || delimeter == 1)
      {
        delimeter = to_delimeter %10;
        to_delimeter /= 10;
      }
      double midle = min+max/2;
      if(delimeter % 2 == 0){min = midle;}
      else { max = midle; }

      while(random > max){
        random /= static_cast<double>(delimeter);  
      };
      
      if(!X_equal) result.x = random;
      else result.y = random;
    
       return result;
    
    }
        return{0,0};
     };


     PlayerTokens Gen::GENERATOR;