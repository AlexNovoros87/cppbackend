#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <bitset>

#include "urlencode.h"

const size_t LOW = 32;
const size_t HIGH = 127;

std::unordered_map<char, std::string> url_enc{
    {'!',"%21"},
    {'#',"%23" },
    {'$',"%24" },
    {'&',"%26" },
    {'\'',"%27" },
    {'(',"%28" },
    { ')',"%29"},
    {' ' ,"%20" },
    {'@' ,"%40" },
    { '*' ,"%2A"},
    {'+',"%2B" },
    {',',"%2C" },
    {'/',"%2F" },
    {':',"%3A" },
    {';',"%3B" },
    {'=',"%3D" },
    {'?',"%3F" },
    { '[',"%5B"},
    {']',"%5C" }
};

std::string MakeUrlCode(char ch)
{
    std::bitset<8> bits(ch);
    std::stringstream stream;
    stream<< '%' <<std::setw(2)<<std::setfill('0')<< std::hex<< bits.to_ulong();
    return stream.str();
}

std::string UrlEncode(std::string_view str) {
    std::string decoded;
   
    for (size_t i = 0; i < str.size(); ++i)
    {
        const char ch = str[i];
        if(ch == ' '){
            decoded += '+';
        }
        else if(url_enc.count(ch)>0)
        {
           decoded.append(url_enc.at(ch));
        }
        else if ( static_cast<size_t>(ch) < LOW || static_cast<size_t>(ch) > HIGH)
        { 
            decoded.append(MakeUrlCode(ch));
        }
        else
        {
            decoded += ch;
        };
    };
    return decoded;
}
