#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <vector>

namespace RiftSpire
{
    std::shared_ptr<spdlog::logger> Logger::s_EngineLogger;
    std::shared_ptr<spdlog::logger> Logger::s_GameLogger;

    void Logger::Init()
    {
        // Create sinks
        std::vector<spdlog::sink_ptr> sinks;
        sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("RiftSpire.log", true));

        // Set pattern: [timestamp] [logger name] [level] message
        sinks[0]->set_pattern("%^[%T] [%n] [%l]%$ %v");
        sinks[1]->set_pattern("[%T] [%n] [%l] %v");

        // Create engine logger
        s_EngineLogger = std::make_shared<spdlog::logger>("ENGINE", sinks.begin(), sinks.end());
        spdlog::register_logger(s_EngineLogger);
        s_EngineLogger->set_level(spdlog::level::trace);
        s_EngineLogger->flush_on(spdlog::level::trace);

        // Create game logger
        s_GameLogger = std::make_shared<spdlog::logger>("GAME", sinks.begin(), sinks.end());
        spdlog::register_logger(s_GameLogger);
        s_GameLogger->set_level(spdlog::level::trace);
        s_GameLogger->flush_on(spdlog::level::trace);

        RS_ENGINE_INFO("Logger initialized");
    }

    void Logger::Shutdown()
    {
        RS_ENGINE_INFO("Logger shutting down");
        spdlog::shutdown();
    }
}
