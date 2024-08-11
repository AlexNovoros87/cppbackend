#include "comparser.h"
[[nodiscard]] std::optional<LaunchParams> ParseCommandLine(int argc, const char *const argv[])
{
    namespace po = boost::program_options;

    po::options_description desc{"Allowed options"s};

    LaunchParams args;

    std::string pth;
    uint32_t save_period;

    // VVVVVVV  ИНСТРУКЦИЯ КАК ПАРСИТЬ КОМАНДНУЮ СТРОКУ
    desc.add_options()
        // Добавляем опцию --help и её короткую версию -h
        ("help,h", "produce help message")
        ("tick-period,t", po::value(&args.period)->value_name("milliseconds"s), "set tick period")
        ("config-file,c", po::value(&args.config_file)->value_name("file"s), "set config file path")
        ("www-root,w", po::value(&args.www_root)->value_name("dir"s), "set tick period")
        ("state-file", po::value(&pth)->value_name("path"s), "it is file where server wwill save his")
        ("save-state-period", po::value(&save_period)->value_name("save per"), "period on server saving")
        ("randomize-spawn-points", "spawn dogs at random positions");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.contains("help"s))
    {
        // Если был указан параметр --help, то выводим справку и возвращаем nullopt
        std::cout << desc;
        return std::nullopt;
    }

    if (!vm.contains("config-file"s))
    {
        throw std::runtime_error("JSON CONFIG WAS NOT FOUND"s);
    }
    if (!vm.contains("www-root"s))
    {
        throw std::runtime_error("STATIC CONTENT WAS NOT FOUND"s);
    }

    if (vm.contains("randomize-spawn-points"s))
    {
        args.random_respawn = true;
    }

    if (vm.contains("state-file"s))
    {
        args.path_to_save = std::filesystem::path(std::move(pth));
        if (vm.contains("save-state-period"s))
        {
            args.save_period = std::chrono::milliseconds(save_period);
        }
    }

    return args;
}

void PrintLaunchParams(const LaunchParams& pars){
    std::cout<<"TICK PERIOD:   "<<pars.period<<std::endl;
    std::cout<<"CONFIG FILE:   "<<pars.config_file<<std::endl;
    std::cout<<"ROOT:          "<<pars.www_root<<std::endl;
    std::cout<<"RANDOM:        "<<std::boolalpha<< pars.random_respawn<<std::endl;
    std::cout<<"PATH TO SAVE:  ";
    if(pars.path_to_save.has_value()) std::cout <<*pars.path_to_save;
    std::cout<<std::endl;
    std::cout<<"SAVE PERIOD:  ";
    if(pars.save_period.has_value()) std::cout << pars.save_period.value().count();
    std::cout<<std::endl;

}