#include "comparser.h"
[[nodiscard]] std::optional<LaunchParams> ParseCommandLine(int argc, const char* const argv[]) {
    namespace po = boost::program_options;

    po::options_description desc{"Allowed options"s};

    LaunchParams args;

     //VVVVVVV  ИНСТРУКЦИЯ КАК ПАРСИТЬ КОМАНДНУЮ СТРОКУ
     desc.add_options()
        // Добавляем опцию --help и её короткую версию -h
        ("help,h", "produce help message")   
        ("tick-period,t", po::value(&args.period)->value_name("milliseconds"s), "set tick period")
        ("config-file,c", po::value(&args.config_file)->value_name("file"s), "set config file path")
        ("www-root,w", po::value(&args.www_root)->value_name("dir"s), "set tick period")
        ("randomize-spawn-points" , "spawn dogs at random positions");  
       
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    
   if (vm.contains("help"s)) {
        // Если был указан параметр --help, то выводим справку и возвращаем nullopt
        std::cout << desc;
        return std::nullopt;
    }

    if (!vm.contains("config-file"s)) {
        throw std::runtime_error("JSON CONFIG WAS NOT FOUND"s);
    }
    if (!vm.contains("www-root"s)) {
        throw std::runtime_error("STATIC CONTENT WAS NOT FOUND"s);
    }
    
    if (vm.contains("randomize-spawn-points"s)) {
       args.random_respawn = true;    
    }
    return args;
} 