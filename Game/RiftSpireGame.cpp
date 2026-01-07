#include <RiftSpire.h>
#include <Core/EntryPoint.h>

class RiftSpireGame : public RiftSpire::Application
{
public:
    RiftSpireGame()
        : Application(GetConfig())
    {
    }

    static RiftSpire::ApplicationConfig GetConfig()
    {
        RiftSpire::ApplicationConfig config;
        config.Name = "RiftSpire - MOBA";
        config.WindowWidth = 1920;
        config.WindowHeight = 1080;
        config.VSync = true;
        return config;
    }

    void OnInit() override
    {
        RS_INFO("RiftSpire MOBA Game initialized!");
        
        // Initialize renderer
        RiftSpire::Renderer::Init();
        RiftSpire::Renderer::SetClearColor({ 0.1f, 0.1f, 0.15f, 1.0f });
    }

    void OnShutdown() override
    {
        RS_INFO("RiftSpire shutting down...");
        RiftSpire::Renderer::Shutdown();
    }

    void OnUpdate(float deltaTime) override
    {
        // Game logic here
        
        // Camera movement example
        if (RiftSpire::Input::IsKeyHeld(RiftSpire::KeyCode::W))
        {
            // Move camera up
        }
        if (RiftSpire::Input::IsKeyHeld(RiftSpire::KeyCode::S))
        {
            // Move camera down
        }
    }

    void OnRender() override
    {
        RiftSpire::Renderer::Clear();
        
        // Render game objects
    }
};

RiftSpire::Application* RiftSpire::CreateApplication()
{
    return new RiftSpireGame();
}
