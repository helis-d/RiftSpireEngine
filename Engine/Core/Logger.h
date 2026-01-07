#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>

namespace RiftSpire
{
    class Logger
    {
    public:
        static void Init();
        static void Shutdown();

        static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return s_EngineLogger; }
        static std::shared_ptr<spdlog::logger>& GetGameLogger() { return s_GameLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_EngineLogger;
        static std::shared_ptr<spdlog::logger> s_GameLogger;
    };
}

// Engine logging macros
#define RS_ENGINE_TRACE(...)    ::RiftSpire::Logger::GetEngineLogger()->trace(__VA_ARGS__)
#define RS_ENGINE_DEBUG(...)    ::RiftSpire::Logger::GetEngineLogger()->debug(__VA_ARGS__)
#define RS_ENGINE_INFO(...)     ::RiftSpire::Logger::GetEngineLogger()->info(__VA_ARGS__)
#define RS_ENGINE_WARN(...)     ::RiftSpire::Logger::GetEngineLogger()->warn(__VA_ARGS__)
#define RS_ENGINE_ERROR(...)    ::RiftSpire::Logger::GetEngineLogger()->error(__VA_ARGS__)
#define RS_ENGINE_CRITICAL(...) ::RiftSpire::Logger::GetEngineLogger()->critical(__VA_ARGS__)

// Game logging macros
#define RS_TRACE(...)    ::RiftSpire::Logger::GetGameLogger()->trace(__VA_ARGS__)
#define RS_DEBUG(...)    ::RiftSpire::Logger::GetGameLogger()->debug(__VA_ARGS__)
#define RS_INFO(...)     ::RiftSpire::Logger::GetGameLogger()->info(__VA_ARGS__)
#define RS_WARN(...)     ::RiftSpire::Logger::GetGameLogger()->warn(__VA_ARGS__)
#define RS_ERROR(...)    ::RiftSpire::Logger::GetGameLogger()->error(__VA_ARGS__)
#define RS_CRITICAL(...) ::RiftSpire::Logger::GetGameLogger()->critical(__VA_ARGS__)

// Assertions
#ifdef RS_DEBUG
    #define RS_ASSERT(condition, ...) \
        if (!(condition)) { \
            RS_ENGINE_CRITICAL("Assertion failed: {}", __VA_ARGS__); \
            __debugbreak(); \
        }
    #define RS_ENGINE_ASSERT(condition, ...) \
        if (!(condition)) { \
            RS_ENGINE_CRITICAL("Assertion failed: {}", __VA_ARGS__); \
            __debugbreak(); \
        }
#else
    #define RS_ASSERT(condition, ...)
    #define RS_ENGINE_ASSERT(condition, ...)
#endif
