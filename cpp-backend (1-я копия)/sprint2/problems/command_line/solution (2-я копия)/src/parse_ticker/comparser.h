#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <optional>
#include <vector>

using namespace std::literals; 

struct LaunchParams {
    
    int64_t period = std::numeric_limits<uint64_t>::max();
    std::string config_file;
    std::string www_root;
    bool random_respawn = false;  

}; 

[[nodiscard]] std::optional<LaunchParams> ParseCommandLine(int argc, const char* const argv[]);