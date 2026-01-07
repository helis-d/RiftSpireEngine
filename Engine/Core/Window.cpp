#include "Window.h"
#include "Logger.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace RiftSpire
{
    bool Window::s_GLFWInitialized = false;

    static void GLFWErrorCallback(int error, const char* description)
    {
        RS_ENGINE_ERROR("GLFW Error ({0}): {1}", error, description);
    }

    Window::Window(const WindowProps& props)
    {
        Init(props);
    }

    Window::~Window()
    {
        Shutdown();
    }

    void Window::Init(const WindowProps& props)
    {
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;
        m_Data.VSync = props.VSync;

        RS_ENGINE_INFO("Creating window: {} ({} x {})", props.Title, props.Width, props.Height);

        if (!s_GLFWInitialized)
        {
            int success = glfwInit();
            RS_ENGINE_ASSERT(success, "Failed to initialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
            s_GLFWInitialized = true;
        }

        // OpenGL hints
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        // Create window
        if (props.Fullscreen)
        {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            m_Window = glfwCreateWindow(mode->width, mode->height, m_Data.Title.c_str(), monitor, nullptr);
        }
        else
        {
            m_Window = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height),
                m_Data.Title.c_str(), nullptr, nullptr);
        }
 
        RS_ENGINE_ASSERT(m_Window, "Failed to create GLFW window!");
        RS_ENGINE_INFO("Window created. ShouldClose: {}", glfwWindowShouldClose(m_Window));
 
        glfwMakeContextCurrent(m_Window);
 
        // Initialize GLAD
        int gladStatus = gladLoadGL(glfwGetProcAddress);
        RS_ENGINE_ASSERT(gladStatus, "Failed to initialize GLAD!");
        RS_ENGINE_INFO("GLAD initialized. ShouldClose: {}", glfwWindowShouldClose(m_Window));
 
        RS_ENGINE_INFO("OpenGL Info:");
        RS_ENGINE_INFO("  Vendor:   {}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
        RS_ENGINE_INFO("  Renderer: {}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        RS_ENGINE_INFO("  Version:  {}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(props.VSync);
        SetupCallbacks();
    }

    void Window::Shutdown()
    {
        RS_ENGINE_INFO("Destroying window");
        if (m_Window)
        {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }
    }

    void Window::SetupCallbacks()
    {
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
        {
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            data.Width = width;
            data.Height = height;

            if (data.ResizeCallback)
                data.ResizeCallback(width, height);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
        {
            RS_ENGINE_INFO("GLFW Window Close Callback triggered!");
            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            if (data.CloseCallback)
                data.CloseCallback();
        });
    }

    void Window::OnUpdate()
    {
        glfwPollEvents();
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(m_Window);
    }

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(m_Window);
    }

    void Window::SetVSync(bool enabled)
    {
        glfwSwapInterval(enabled ? 1 : 0);
        m_Data.VSync = enabled;
    }

    void Window::SetTitle(const std::string& title)
    {
        m_Data.Title = title;
        glfwSetWindowTitle(m_Window, title.c_str());
    }

    Scope<Window> Window::Create(const WindowProps& props)
    {
        return CreateScope<Window>(props);
    }
}
