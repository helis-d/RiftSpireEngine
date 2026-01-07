#include "ViewportPanel.h"

namespace RiftSpire
{
    void ViewportPanel::OnImGuiRender(u32 textureID)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport");

        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();

        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        if (m_ViewportSize.x != viewportPanelSize.x || m_ViewportSize.y != viewportPanelSize.y)
        {
            m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
            // TODO: Resize framebuffer
        }

        // Render the framebuffer texture
        ImGui::Image(
            (ImTextureID)(uint64_t)textureID,
            ImVec2(m_ViewportSize.x, m_ViewportSize.y),
            ImVec2(0, 1),  // UV flipped for OpenGL
            ImVec2(1, 0)
        );

        // Get viewport bounds for mouse picking
        auto windowPos = ImGui::GetWindowPos();
        auto contentMin = ImGui::GetWindowContentRegionMin();
        auto contentMax = ImGui::GetWindowContentRegionMax();
        
        m_ViewportBounds[0] = { windowPos.x + contentMin.x, windowPos.y + contentMin.y };
        m_ViewportBounds[1] = { windowPos.x + contentMax.x, windowPos.y + contentMax.y };

        ImGui::End();
        ImGui::PopStyleVar();
    }
}
