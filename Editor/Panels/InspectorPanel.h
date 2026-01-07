#pragma once

#include <imgui.h>
#include <ECS/Entity.h>
#include <ECS/Components.h>
#include <glm/gtc/type_ptr.hpp>

namespace RiftSpire
{
    class InspectorPanel
    {
    public:
        InspectorPanel() = default;

        void SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }
        void OnImGuiRender();

    private:
        void DrawComponents(Entity entity);

        template<typename T>
        void DrawComponentUI(const char* name, Entity entity, std::function<void(T&)> uiFunction);

    private:
        Entity m_SelectionContext;
    };
}
