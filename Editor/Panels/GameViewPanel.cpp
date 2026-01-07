#include "GameViewPanel.h"
#include "../EditorLayout.h"
#include "../EditorTheme.h"
#include <imgui.h>

namespace RiftSpire
{
    void GameViewPanel::OnImGuiRender()
    {
        using namespace EditorLayout;
        
        // Fixed position and size
        ImGui::SetNextWindowPos(ImVec2(static_cast<float>(GameView::X), 
                                       static_cast<float>(GameView::Y)), 
                                ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(GameView::WIDTH), 
                                        static_cast<float>(GameView::HEIGHT)), 
                                 ImGuiCond_Always);
        
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | 
                                 ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_NoCollapse;
        
        if (ImGui::Begin("Game View", nullptr, flags))
        {
            // Runtime control buttons
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, 6));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));
            
            // Play button
            if (m_IsPlaying && !m_IsPaused)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, EditorColors::Success);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorColors::Success);
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Button, EditorColors::SurfaceLight);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorColors::SurfaceHover);
            }
            
            if (ImGui::Button("Play", ImVec2(70, 28)))
            {
                m_IsPlaying = true;
                m_IsPaused = false;
                m_GameState = "Running";
            }
            ImGui::PopStyleColor(2);
            
            ImGui::SameLine();
            
            // Pause button
            if (m_IsPaused)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, EditorColors::Warning);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorColors::Warning);
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Button, EditorColors::SurfaceLight);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorColors::SurfaceHover);
            }
            
            if (ImGui::Button("Pause", ImVec2(70, 28)))
            {
                if (m_IsPlaying)
                {
                    m_IsPaused = !m_IsPaused;
                    m_GameState = m_IsPaused ? "Paused" : "Running";
                }
            }
            ImGui::PopStyleColor(2);
            
            ImGui::SameLine();
            
            // Step button
            ImGui::PushStyleColor(ImGuiCol_Button, EditorColors::SurfaceLight);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorColors::SurfaceHover);
            
            if (ImGui::Button("Step", ImVec2(70, 28)))
            {
                if (m_IsPlaying && m_IsPaused)
                {
                    // Step one frame
                    m_GameState = "Stepping";
                }
            }
            ImGui::PopStyleColor(2);
            
            ImGui::SameLine();
            
            // Restart button
            ImGui::PushStyleColor(ImGuiCol_Button, EditorColors::SurfaceLight);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, EditorColors::Error);
            
            if (ImGui::Button("Restart", ImVec2(80, 28)))
            {
                m_IsPlaying = false;
                m_IsPaused = false;
                m_GameState = "Idle";
            }
            ImGui::PopStyleColor(2);
            
            ImGui::PopStyleVar(2);
            
            ImGui::Separator();
            
            // Debug info
            ImGui::TextColored(EditorColors::TextDim, "Status:");
            ImGui::SameLine();
            
            // State color
            ImVec4 stateColor = EditorColors::TextDim;
            if (m_GameState == "Running") stateColor = EditorColors::Success;
            else if (m_GameState == "Paused") stateColor = EditorColors::Warning;
            else if (m_GameState == "Stepping") stateColor = EditorColors::Info;
            
            ImGui::TextColored(stateColor, "%s", m_GameState.c_str());
            
            ImGui::SameLine(0, 30);
            ImGui::TextColored(EditorColors::TextDim, "FPS:");
            ImGui::SameLine();
            
            // FPS color based on performance
            ImVec4 fpsColor = EditorColors::Success;
            if (m_FPS < 30) fpsColor = EditorColors::Error;
            else if (m_FPS < 60) fpsColor = EditorColors::Warning;
            
            ImGui::TextColored(fpsColor, "%.1f", m_FPS);
            
            ImGui::Separator();
            
            // Game view area (placeholder)
            ImVec2 availableSize = ImGui::GetContentRegionAvail();
            
            // Dark background for game view
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            
            drawList->AddRectFilled(
                cursorPos,
                ImVec2(cursorPos.x + availableSize.x, cursorPos.y + availableSize.y),
                IM_COL32(15, 15, 25, 255),
                4.0f
            );
            
            // Center text when not playing
            if (!m_IsPlaying)
            {
                const char* msg = "Press Play to start simulation";
                ImVec2 textSize = ImGui::CalcTextSize(msg);
                ImVec2 textPos = ImVec2(
                    cursorPos.x + (availableSize.x - textSize.x) * 0.5f,
                    cursorPos.y + (availableSize.y - textSize.y) * 0.5f
                );
                drawList->AddText(textPos, IM_COL32(100, 100, 120, 255), msg);
            }
            else
            {
                // Future: Render game framebuffer here
                const char* msg = "Game Running...";
                ImVec2 textSize = ImGui::CalcTextSize(msg);
                ImVec2 textPos = ImVec2(
                    cursorPos.x + (availableSize.x - textSize.x) * 0.5f,
                    cursorPos.y + (availableSize.y - textSize.y) * 0.5f
                );
                drawList->AddText(textPos, IM_COL32(52, 220, 128, 255), msg);
            }
        }
        ImGui::End();
    }
}
