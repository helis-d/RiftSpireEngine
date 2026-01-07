#pragma once

#include "Types.h"
#include "Window.h"
#include <string>

namespace RiftSpire
{
    struct ApplicationConfig
    {
        std::string Name = "RiftSpire Application";
        u32 WindowWidth = 1920;
        u32 WindowHeight = 1080;
        bool VSync = true;
        bool Fullscreen = false;
    };

    class Application
    {
    public:
        Application(const ApplicationConfig& config = ApplicationConfig());
        virtual ~Application();

        // Non-copyable
        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        void Run();
        void Close();

        // Virtual methods for game to override
        virtual void OnInit() {}
        virtual void OnShutdown() {}
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender() {}
        virtual void OnImGuiRender() {}

        // Getters
        Window& GetWindow() { return *m_Window; }
        const Window& GetWindow() const { return *m_Window; }
        bool IsRunning() const { return m_Running; }

        static Application& Get() { return *s_Instance; }

    private:
        void InitSystems();
        void ShutdownSystems();

        void InitImGui();
        void ShutdownImGui();
        void BeginImGui();
        void EndImGui();

    private:
        ApplicationConfig m_Config;
        Scope<Window> m_Window;
        bool m_Running = true;
        bool m_Minimized = false;

        static Application* s_Instance;
    };

    // Defined by client application
    Application* CreateApplication();
}
