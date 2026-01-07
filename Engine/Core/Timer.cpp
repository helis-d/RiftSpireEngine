#include "Timer.h"
#include <GLFW/glfw3.h>

namespace RiftSpire
{
    // Timer implementation
    Timer::Timer()
    {
        Reset();
    }

    void Timer::Reset()
    {
        m_Start = std::chrono::high_resolution_clock::now();
    }

    float Timer::Elapsed() const
    {
        return std::chrono::duration<float>(
            std::chrono::high_resolution_clock::now() - m_Start).count();
    }

    float Timer::ElapsedMillis() const
    {
        return Elapsed() * 1000.0f;
    }

    // Time class static members
    float Time::s_DeltaTime = 0.0f;
    float Time::s_Time = 0.0f;
    float Time::s_LastFrameTime = 0.0f;
    u32 Time::s_FPS = 0;
    u32 Time::s_FrameCount = 0;
    float Time::s_FPSTimer = 0.0f;

    void Time::Update()
    {
        float currentTime = static_cast<float>(glfwGetTime());
        s_DeltaTime = currentTime - s_LastFrameTime;
        s_LastFrameTime = currentTime;
        s_Time = currentTime;

        // FPS calculation
        s_FrameCount++;
        s_FPSTimer += s_DeltaTime;
        if (s_FPSTimer >= 1.0f)
        {
            s_FPS = s_FrameCount;
            s_FrameCount = 0;
            s_FPSTimer = 0.0f;
        }
    }
}
