#include "Logger.h"

namespace Radium
{
    std::shared_ptr<spdlog::logger> Logger::s_logger;


    void Logger::init()
    {
        spdlog::set_pattern("%^[%T] %n: %v%$");

        s_logger = spdlog::stdout_color_mt("Radium");
        s_logger->set_level(spdlog::level::trace);
    }
}
