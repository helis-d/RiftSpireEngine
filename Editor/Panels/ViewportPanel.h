#pragma once

#include <imgui.h>
#include <Renderer/Camera.h>
#include <Core/Types.h>

namespace RiftSpire
{
    class ViewportPanel
    {
    public:
        ViewportPanel() = default;

        void OnImGuiRender(u32 textureID);
        
        glm::vec2 GetViewportSize() const { return m_ViewportSize; }
        bool IsViewportFocused() const { return m_ViewportFocused; }
        bool IsViewportHovered() const { return m_ViewportHovered; }

    private:
        glm::vec2 m_ViewportSize = { 1280.0f, 720.0f };
        glm::vec2 m_ViewportBounds[2];
        bool m_ViewportFocused = false;
        bool m_ViewportHovered = false;
    };
}
