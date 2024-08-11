#pragma once
#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <optional>
#include <vector>
#include <filesystem>

using namespace std::literals; 


struct LaunchParams {
    
    int64_t period = std::numeric_limits<uint64_t>::max();
    std::string config_file;
    std::string www_root;
    bool random_respawn = false; 
    std::optional<std::filesystem::path> path_to_save;
    std::optional<std::chrono::milliseconds> save_period; 

}; 
void PrintLaunchParams(const LaunchParams& pars);

[[nodiscard]] std::optional<LaunchParams> ParseCommandLine(int argc, const char* const argv[]);