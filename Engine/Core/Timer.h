#pragma once

#include "Types.h"
#include <chrono>

namespace RiftSpire
{
    class Timer
    {
    public:
        Timer();

        void Reset();
        float Elapsed() const;      // Seconds since last reset
        float ElapsedMillis() const; // Milliseconds since last reset

    private:
        std::chrono::high_resolution_clock::time_point m_Start;
    };

    // Delta time manager
    class Time
    {
    public:
        static void Update();

        static float GetDeltaTime() { return s_DeltaTime; }
        static float GetTime() { return s_Time; }
        static u32 GetFPS() { return s_FPS; }

    private:
        static float s_DeltaTime;
        static float s_Time;
        static float s_LastFrameTime;
        static u32 s_FPS;
        static u32 s_FrameCount;
        static float s_FPSTimer;
    };
}
