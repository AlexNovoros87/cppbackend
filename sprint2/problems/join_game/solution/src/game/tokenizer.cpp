#include "tokenizer.h"
std::string NumToHex(uint64_t num){
  std::stringstream ss;
  ss << std::hex << num;
  std::stringstream ss2;
  ss2 << std::setw(16)<<std::setfill('0')<<ss.str();
  return ss2.str();
}