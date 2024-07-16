#include "urldecode.h"

#include <charconv>
#include <stdexcept>
#include <unordered_map>

std::unordered_map<std::string, char> url_dec{
    {"%21", '!'},
    {"%23", '#'},
    {"%24", '$'},
    {"%26", '&'},
    {"%27", '\''},
    {"%28", '('},
    {"%29", ')'},
    {"%20", ' '},
    {"%40", '@'},
    {"%2A", '*'},
    {"%2B", '+'},
    {"%2C", ','},
    {"%2F", '/'},
    {"%3A", ':'},
    {"%3B", ';'},
    {"%3D", '='},
    {"%3F", '?'},
    {"%5B", '['},
    {"%5C", ']'},
    
    {"%22", '"'},
    {"%25", '%'},
    {"%27", '\''},
    {"%3C", '<'},
    {"%3E", '>'},
    {"%5C", '\\'},
    {"%5E", '^'},
    {"%60", '`'},
    {"%7B", '{'},
    {"%7C", '|'},
    {"%7D", '}'}
};

char PlusReplaceOrSymb(char ch)
{
    if (ch == '+')
        return ' ';
    return ch;
}

std::string UrlDecode(std::string_view url)
{
    std::string decoded;
   
    for (size_t i = 0; i < url.size(); ++i)
    {
        if (url[i] == '%' && i + 2 >= url.size())
        {
          throw std::invalid_argument("invalid decoding");
        }

        else if (url[i] == '%' && i + 2 < url.size())
        {
            std::string tmp ;
            for(size_t j = i; j<= i+2; ++j){
                tmp.push_back(std::toupper(url[j]));
            }  
            if(url_dec.count(tmp) == 0){
               throw std::invalid_argument("invalid decoding");  
            }
            decoded += url_dec.at(tmp); 
            i+=2;
        }
        else
        {
            decoded += PlusReplaceOrSymb(url[i]);
        };
    };
    return decoded;
}