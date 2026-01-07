#pragma once

#include <imgui.h>
#include <ECS/Scene.h>
#include <ECS/Entity.h>
#include <ECS/Components.h>

namespace RiftSpire
{
    class SceneHierarchyPanel
    {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(Scene* scene);

        void SetContext(Scene* scene);
        void OnImGuiRender();

        Entity GetSelectedEntity() const { return m_SelectionContext; }
        void SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }

    private:
        void DrawEntityNode(Entity entity);

    private:
        Scene* m_Context = nullptr;
        Entity m_SelectionContext;
    };
}
