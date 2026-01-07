#pragma once

#include "Types.h"
#include <string>
#include <functional>

struct GLFWwindow;

namespace RiftSpire
{
    struct WindowProps
    {
        std::string Title;
        u32 Width;
        u32 Height;
        bool VSync;
        bool Fullscreen;

        WindowProps(
            const std::string& title = "RiftSpire Engine",
            u32 width = 1920,
            u32 height = 1080,
            bool vsync = true,
            bool fullscreen = false)
            : Title(title), Width(width), Height(height), VSync(vsync), Fullscreen(fullscreen)
        {
        }
    };

    class Window
    {
    public:
        using ResizeCallback = std::function<void(u32, u32)>;
        using CloseCallback = std::function<void()>;

        Window(const WindowProps& props = WindowProps());
        ~Window();

        // Non-copyable
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        void OnUpdate();
        void SwapBuffers();

        // Getters
        u32 GetWidth() const { return m_Data.Width; }
        u32 GetHeight() const { return m_Data.Height; }
        float GetAspectRatio() const { return static_cast<float>(m_Data.Width) / static_cast<float>(m_Data.Height); }
        bool IsVSync() const { return m_Data.VSync; }
        bool ShouldClose() const;
        GLFWwindow* GetNativeWindow() const { return m_Window; }

        // Setters
        void SetVSync(bool enabled);
        void SetTitle(const std::string& title);

        // Callbacks
        void SetResizeCallback(const ResizeCallback& callback) { m_Data.ResizeCallback = callback; }
        void SetCloseCallback(const CloseCallback& callback) { m_Data.CloseCallback = callback; }

        // Static
        static Scope<Window> Create(const WindowProps& props = WindowProps());

    private:
        void Init(const WindowProps& props);
        void Shutdown();
        void SetupCallbacks();

    private:
        GLFWwindow* m_Window = nullptr;

        struct WindowData
        {
            std::string Title;
            u32 Width = 0;
            u32 Height = 0;
            bool VSync = false;

            ResizeCallback ResizeCallback;
            CloseCallback CloseCallback;
        };

        WindowData m_Data;
        static bool s_GLFWInitialized;
    };
}
