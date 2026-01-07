#include <RiftSpire.h>
#include <Core/EntryPoint.h>

#include <imgui.h>

#include "EditorTheme.h"
#include "EditorLayout.h"

#include "Panels/SceneHierarchyPanel.h"
#include "Panels/InspectorPanel.h"
#include "Panels/ViewportPanel.h"
#include "Panels/ConsolePanel.h"
#include "Panels/BlockPalettePanel.h"
#include "Panels/BlockCanvasPanel.h"
#include "Panels/BlockInspectorPanel.h"
#include "Panels/GameViewPanel.h"

#include <Scripting/Scripting.h>
#include <Scripting/Core/BlockScript.h>

namespace RiftSpire
{
    class RiftSpireEditor : public Application
    {
    public:
        RiftSpireEditor()
            : Application(GetConfig())
        {
        }

        static ApplicationConfig GetConfig()
        {
            ApplicationConfig config;
            config.Name = "RiftSpire Editor";
            config.WindowWidth = 1920;
            config.WindowHeight = 1080;
            config.VSync = true;
            return config;
        }

        void OnInit() override
        {
            RS_INFO("RiftSpire Editor initialized!");

            // Initialize RiftScripting system
            InitScripting();
            
            // Apply modern dark theme
            ApplyModernDarkTheme();
            RS_INFO("Applied modern dark theme");

            // Initialize renderer
            Renderer::Init();
            Renderer::SetClearColor({ 0.1f, 0.1f, 0.12f, 1.0f });
            RS_INFO("Renderer Init done.");

            // Create a default scene
            m_ActiveScene = CreateScope<Scene>();
            m_SceneHierarchyPanel.SetContext(m_ActiveScene.get());
            RS_INFO("Scene creation done.");

            // Create default block script
            m_ActiveBlockScript = std::make_shared<BlockScript>("New Script");
            m_BlockCanvasPanel.SetActiveScript(m_ActiveBlockScript);

            RS_INFO("End of OnInit successfully reached");
        }

        void OnShutdown() override
        {
            RS_INFO("RiftSpire Editor shutting down...");
            Renderer::Shutdown();
        }

        void OnUpdate(float deltaTime) override
        {
            // Update scene
            if (m_ActiveScene)
            {
                m_ActiveScene->OnUpdate(deltaTime);
            }
        }

        void OnRender() override
        {
            Renderer::Clear();

            // Scene rendering (will be to framebuffer later)
            if (m_ActiveScene)
            {
                m_ActiveScene->OnRender();
            }
        }

        void OnImGuiRender() override
        {
            // Main Menu Bar
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                    {
                        m_ActiveScene = CreateScope<Scene>();
                        m_SceneHierarchyPanel.SetContext(m_ActiveScene.get());
                    }
                    ImGui::Separator();
                    if (ImGui::MenuItem("Exit"))
                    {
                        Close();
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Entity"))
                {
                    if (ImGui::MenuItem("Create Empty"))
                    {
                        m_ActiveScene->CreateEntity("Empty Entity");
                    }
                    if (ImGui::MenuItem("Create Champion"))
                    {
                        auto entity = m_ActiveScene->CreateEntity("New Champion");
                        entity.AddComponent<HealthComponent>();
                        entity.AddComponent<ManaComponent>();
                        entity.AddComponent<ChampionComponent>();
                        entity.AddComponent<MovementComponent>();
                    }
                    if (ImGui::MenuItem("Create Minion"))
                    {
                        auto entity = m_ActiveScene->CreateEntity("Minion");
                        entity.AddComponent<HealthComponent>();
                        entity.AddComponent<TeamComponent>();
                    }
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("View"))
                {
                    ImGui::MenuItem("Scene Hierarchy", nullptr, &m_ShowSceneHierarchy);
                    ImGui::MenuItem("Inspector", nullptr, &m_ShowInspector);
                    ImGui::MenuItem("Viewport", nullptr, &m_ShowViewport);
                    ImGui::MenuItem("Console", nullptr, &m_ShowConsole);
                    ImGui::Separator();
                    ImGui::MenuItem("Block Palette", nullptr, &m_ShowBlockPalette);
                    ImGui::MenuItem("Block Canvas", nullptr, &m_ShowBlockCanvas);
                    ImGui::MenuItem("Block Inspector", nullptr, &m_ShowBlockInspector);
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Blocks"))
                {
                    if (ImGui::MenuItem("New Script", "Ctrl+Shift+N"))
                    {
                        m_ActiveBlockScript = std::make_shared<BlockScript>("New Script");
                        m_BlockCanvasPanel.SetActiveScript(m_ActiveBlockScript);
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }

            //=================================================================
            // TOP TOOLBAR - Runtime Controls (Fixed: 0, 26, 1920, 40)
            //=================================================================
            RenderToolbar();

            //=================================================================
            // FIXED LAYOUT PANELS (1920x1080)
            //=================================================================
            using namespace EditorLayout;

            // Scene Hierarchy (Top-Left)
            if (m_ShowSceneHierarchy)
            {
                ImGui::SetNextWindowPos(ImVec2((float)SceneHierarchy::X, (float)SceneHierarchy::Y), ImGuiCond_Always);
                ImGui::SetNextWindowSize(ImVec2((float)SceneHierarchy::WIDTH, (float)SceneHierarchy::HEIGHT), ImGuiCond_Always);
                m_SceneHierarchyPanel.OnImGuiRender();
            }

            // Block Palette (Bottom-Left)
            if (m_ShowBlockPalette)
            {
                ImGui::SetNextWindowPos(ImVec2((float)BlockPalette::X, (float)BlockPalette::Y), ImGuiCond_Always);
                ImGui::SetNextWindowSize(ImVec2((float)BlockPalette::WIDTH, (float)BlockPalette::HEIGHT), ImGuiCond_Always);
                m_BlockPalettePanel.OnImGuiRender();
            }

            // Block Canvas (Center - LARGEST)
            if (m_ShowBlockCanvas)
            {
                ImGui::SetNextWindowPos(ImVec2((float)BlockCanvas::X, (float)BlockCanvas::Y), ImGuiCond_Always);
                ImGui::SetNextWindowSize(ImVec2((float)BlockCanvas::WIDTH, (float)BlockCanvas::HEIGHT), ImGuiCond_Always);
                m_BlockCanvasPanel.OnImGuiRender();
                
                // Update inspector with selected block
                if (m_BlockCanvasPanel.GetSelectedBlock().IsValid())
                {
                    auto block = m_ActiveBlockScript->GetBlock(m_BlockCanvasPanel.GetSelectedBlock());
                    m_BlockInspectorPanel.SetSelectedBlock(block.get(), m_ActiveBlockScript);
                }
            }

            // Inspector (Right)
            if (m_ShowInspector)
            {
                ImGui::SetNextWindowPos(ImVec2((float)Inspector::X, (float)Inspector::Y), ImGuiCond_Always);
                ImGui::SetNextWindowSize(ImVec2((float)Inspector::WIDTH, (float)Inspector::HEIGHT), ImGuiCond_Always);
                m_InspectorPanel.SetSelectedEntity(m_SceneHierarchyPanel.GetSelectedEntity());
                m_InspectorPanel.OnImGuiRender();
            }

            // Console (Bottom-Center)
            if (m_ShowConsole)
            {
                ImGui::SetNextWindowPos(ImVec2((float)Console::X, (float)Console::Y), ImGuiCond_Always);
                ImGui::SetNextWindowSize(ImVec2((float)Console::WIDTH, (float)Console::HEIGHT), ImGuiCond_Always);
                m_ConsolePanel.OnImGuiRender();
            }

            // Game View (Bottom-Right)
            if (m_ShowGameView)
            {
                // Update FPS (simple moving average)
                static float s_LastFrameTime = 0.016f;
                s_LastFrameTime = s_LastFrameTime * 0.9f + 0.016f * 0.1f;  // Smooth FPS
                m_GameViewPanel.SetFPS(1.0f / s_LastFrameTime);
                m_GameViewPanel.OnImGuiRender();
            }

            // Block Inspector (merged into main Inspector for now)
            if (m_ShowBlockInspector)
            {
                // Block inspector now integrated with main inspector
                m_BlockInspectorPanel.OnImGuiRender();
            }
        }

        void RenderToolbar()
        {
            using namespace EditorLayout;
            
            ImGui::SetNextWindowPos(ImVec2((float)Toolbar::X, (float)Toolbar::Y), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2((float)Toolbar::WIDTH, (float)Toolbar::HEIGHT), ImGuiCond_Always);
            
            ImGuiWindowFlags toolbarFlags = ImGuiWindowFlags_NoTitleBar |
                                            ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoScrollbar |
                                            ImGuiWindowFlags_NoScrollWithMouse |
                                            ImGuiWindowFlags_NoCollapse;
            
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 6));
            ImGui::PushStyleColor(ImGuiCol_WindowBg, EditorColors::TitleBar);
            
            if (ImGui::Begin("##Toolbar", nullptr, toolbarFlags))
            {
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 6));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));
                
                // Center the toolbar buttons
                float buttonAreaWidth = 4 * Toolbar::BUTTON_WIDTH + 3 * Toolbar::BUTTON_SPACING;
                float startX = (Toolbar::WIDTH - buttonAreaWidth) * 0.5f;
                ImGui::SetCursorPosX(startX);
                
                // Play button
                bool isPlaying = m_GameViewPanel.IsPlaying();
                bool isPaused = m_GameViewPanel.IsPaused();
                
                if (isPlaying && !isPaused)
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, EditorColors::Success);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorColors::Success);
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, EditorColors::SurfaceLight);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorColors::SurfaceHover);
                }
                
                if (ImGui::Button(reinterpret_cast<const char*>(u8"\u25B6 Play"), ImVec2((float)Toolbar::BUTTON_WIDTH, (float)Toolbar::BUTTON_HEIGHT)))
                {
                    m_GameViewPanel.SetPlaying(true);
                    m_GameViewPanel.SetPaused(false);
                    m_GameViewPanel.SetGameState("Running");
                }
                ImGui::PopStyleColor(2);
                
                ImGui::SameLine();
                
                // Pause button
                if (isPaused)
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, EditorColors::Warning);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorColors::Warning);
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, EditorColors::SurfaceLight);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorColors::SurfaceHover);
                }
                
                if (ImGui::Button(reinterpret_cast<const char*>(u8"\u23F8 Pause"), ImVec2((float)Toolbar::BUTTON_WIDTH, (float)Toolbar::BUTTON_HEIGHT)))
                {
                    if (isPlaying)
                    {
                        m_GameViewPanel.SetPaused(!isPaused);
                        m_GameViewPanel.SetGameState(isPaused ? "Running" : "Paused");
                    }
                }
                ImGui::PopStyleColor(2);
                
                ImGui::SameLine();
                
                // Step button
                ImGui::PushStyleColor(ImGuiCol_Button, EditorColors::SurfaceLight);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorColors::SurfaceHover);
                
                if (ImGui::Button(reinterpret_cast<const char*>(u8"\u23ED Step"), ImVec2((float)Toolbar::BUTTON_WIDTH, (float)Toolbar::BUTTON_HEIGHT)))
                {
                    if (isPlaying && isPaused)
                    {
                        m_GameViewPanel.SetGameState("Stepping");
                    }
                }
                ImGui::PopStyleColor(2);
                
                ImGui::SameLine();
                
                // Restart button
                ImGui::PushStyleColor(ImGuiCol_Button, EditorColors::SurfaceLight);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorColors::Error);
                
                if (ImGui::Button("Restart", ImVec2((float)Toolbar::BUTTON_WIDTH, (float)Toolbar::BUTTON_HEIGHT)))
                {
                    m_GameViewPanel.SetPlaying(false);
                    m_GameViewPanel.SetPaused(false);
                    m_GameViewPanel.SetGameState("Idle");
                }
                ImGui::PopStyleColor(2);
                
                ImGui::PopStyleVar(2);
            }
            ImGui::End();
            
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }

    private:
        Scope<Scene> m_ActiveScene;

        // Panels
        SceneHierarchyPanel m_SceneHierarchyPanel;
        InspectorPanel m_InspectorPanel;
        ViewportPanel m_ViewportPanel;
        ConsolePanel m_ConsolePanel;
        GameViewPanel m_GameViewPanel;
        
        // RiftBlocks panels
        BlockPalettePanel m_BlockPalettePanel;
        BlockCanvasPanel m_BlockCanvasPanel;
        BlockInspectorPanel m_BlockInspectorPanel;
        BlockScriptPtr m_ActiveBlockScript;

        // Panel visibility
        bool m_ShowSceneHierarchy = true;
        bool m_ShowInspector = true;
        bool m_ShowViewport = false;  // Viewport replaced by Block Canvas
        bool m_ShowConsole = true;
        bool m_ShowBlockPalette = true;
        bool m_ShowBlockCanvas = true;
        bool m_ShowBlockInspector = false;  // Merged into Inspector
        bool m_ShowGameView = true;
    };

    Application* CreateApplication()
    {
        return new RiftSpireEditor();
    }
}
