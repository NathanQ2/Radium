#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

namespace Radium
{
    class Logger
    {
    public:
        static void init();

        inline static std::shared_ptr<spdlog::logger>& getLogger() { return s_logger; }
    private:
        static std::shared_ptr<spdlog::logger> s_logger;
    };
}

#define RA_TRACE(...) ::Radium::Logger::getLogger()->trace(__VA_ARGS__)
#define RA_INFO(...) ::Radium::Logger::getLogger()->info(__VA_ARGS__)
#define RA_WARN(...) ::Radium::Logger::getLogger()->warn(__VA_ARGS__)
#define RA_ERROR(...) ::Radium::Logger::getLogger()->error(__VA_ARGS__)
#define RA_FATAL(...) ::Radium::Logger::getLogger()->critical(__VA_ARGS__)
