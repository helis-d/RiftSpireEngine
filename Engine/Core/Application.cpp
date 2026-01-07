#include "Application.h"
#include "Logger.h"
#include "Input.h"
#include "Timer.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace RiftSpire
{
    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationConfig& config)
        : m_Config(config)
    {
        RS_ENGINE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;

        InitSystems();
    }

    Application::~Application()
    {
        ShutdownSystems();
        s_Instance = nullptr;
    }

    void Application::InitSystems()
    {
        // Initialize logging first
        Logger::Init();
        RS_ENGINE_INFO("RiftSpire Engine v0.1.0 starting...");

        // Create window
        WindowProps windowProps;
        windowProps.Title = m_Config.Name;
        windowProps.Width = m_Config.WindowWidth;
        windowProps.Height = m_Config.WindowHeight;
        windowProps.VSync = m_Config.VSync;
        windowProps.Fullscreen = m_Config.Fullscreen;

        m_Window = Window::Create(windowProps);

        // Setup window callbacks
        m_Window->SetCloseCallback([this]() {
            Close();
        });

        m_Window->SetResizeCallback([this](u32 width, u32 height) {
            if (width == 0 || height == 0)
            {
                m_Minimized = true;
                return;
            }
            m_Minimized = false;
            glViewport(0, 0, width, height);
        });

        // Initialize input
        Input::Init(m_Window->GetNativeWindow());

        InitImGui();

        RS_ENGINE_INFO("Engine systems initialized");
    }

    void Application::ShutdownSystems()
    {
        RS_ENGINE_INFO("Shutting down engine systems...");
        ShutdownImGui();
        m_Window.reset();
        Logger::Shutdown();
    }

    void Application::Run()
    {
        RS_ENGINE_INFO("Starting main loop");
        
        try
        {
            OnInit();
        }
        catch (const std::exception& e)
        {
            RS_ENGINE_ERROR("Exception in OnInit: {}", e.what());
            return;
        }
        catch (...)
        {
            RS_ENGINE_ERROR("Unknown exception in OnInit");
            return;
        }

        while (m_Running && !m_Window->ShouldClose())
        {
            Time::Update();
            float deltaTime = Time::GetDeltaTime();

            Input::Update();
            m_Window->OnUpdate();

            if (!m_Minimized)
            {
                OnUpdate(deltaTime);
                OnRender();

                BeginImGui();
                OnImGuiRender();
                EndImGui();
            }

            m_Window->SwapBuffers();

            // Check for escape key to close
            if (Input::IsKeyPressed(KeyCode::Escape))
            {
                RS_ENGINE_INFO("Escape key pressed, closing...");
                Close();
            }
        }

        OnShutdown();
        RS_ENGINE_INFO("Main loop ended. m_Running: {}, WindowShouldClose: {}", m_Running, m_Window->ShouldClose());
    }

    void Application::Close()
    {
        RS_ENGINE_INFO("Application::Close() called");
        m_Running = false;
    }

    void Application::InitImGui()
    {
        RS_ENGINE_INFO("Initializing ImGui");
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(m_Window->GetNativeWindow()), true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void Application::ShutdownImGui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Application::BeginImGui()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Application::EndImGui()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}
