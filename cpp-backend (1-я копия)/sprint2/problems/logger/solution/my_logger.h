#pragma once

#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <optional>
#include <mutex>
#include <shared_mutex>
#include <thread>



using namespace std::literals;

#define LOG(...) Logger::GetInstance().Log(__VA_ARGS__)

class Logger {
    auto GetTime() const {
        if (manual_ts_) {
            return *manual_ts_;
        }
        return std::chrono::system_clock::now();
    }

    auto GetTimeStamp() const {
        const auto now = GetTime();
        const auto t_c = std::chrono::system_clock::to_time_t(now);
        return std::put_time(std::localtime(&t_c), "%F %T");
    }

   

    Logger() = default;
    Logger(const Logger&) = delete;
    ~Logger(){
        log_.close();
    }

public:
    // Для имени файла возьмите дату с форматом "%Y_%m_%d"
    std::string GetFileTimeStamp() const {
        std::stringstream ss;
        const auto now_point = std::chrono::system_clock::to_time_t(GetTime());
        ss << std::put_time(std::localtime(&now_point), "%Y_%m_%d");
        return ss.str();
    };
   
    static Logger& GetInstance() {
        static Logger obj;
        return obj;
    }

    // Выведите в поток все аргументы.
    template<class... Ts>
    void Log(const Ts&... args){
        std::lock_guard guard(mtx_);
        log_ << GetTimeStamp() << ": "sv;
        ((log_ << args), ...);
        log_ << std::endl;

    };

    // Установите manual_ts_. Учтите, что эта операция может выполняться
    // параллельно с выводом в поток, вам нужно предусмотреть 
    // синхронизацию.
    void SetTimestamp(std::chrono::system_clock::time_point ts){
        std::lock_guard guard(mtx_);
        log_.close();
        manual_ts_ = ts;
        std::string log_file = std::string().append("/var/log/sample_log_")
        .append(GetFileTimeStamp()).append(".log");
        log_.open(log_file, std::ios::app);

    };

private:
    std::optional<std::chrono::system_clock::time_point> manual_ts_;
    mutable std::shared_mutex mtx_;
    std::ofstream log_;
};
