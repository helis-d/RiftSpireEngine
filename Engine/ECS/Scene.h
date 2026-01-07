#pragma once

#include "../Core/Types.h"
#include <entt/entt.hpp>

namespace RiftSpire
{
    class Entity;

    class Scene
    {
    public:
        Scene() = default;
        ~Scene() = default;

        Entity CreateEntity(const std::string& name = "Entity");
        void DestroyEntity(Entity entity);

        void OnUpdate(float deltaTime);
        void OnRender();

        entt::registry& GetRegistry() { return m_Registry; }

    private:
        entt::registry m_Registry;
        friend class Entity;
    };
}
