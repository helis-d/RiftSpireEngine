#include "Scene.h"
#include "Entity.h"
#include "Components.h"

namespace RiftSpire
{
    Entity Scene::CreateEntity(const std::string& name)
    {
        Entity entity = { m_Registry.create(), this };
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "Entity" : name;
        return entity;
    }

    void Scene::DestroyEntity(Entity entity)
    {
        m_Registry.destroy(entity.GetHandle());
    }

    void Scene::OnUpdate(float deltaTime)
    {
        // Update systems here
    }

    void Scene::OnRender()
    {
        // Render entities with sprite components
    }
}
