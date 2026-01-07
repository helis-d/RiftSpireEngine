#pragma once

#include "../Core/Types.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

namespace RiftSpire
{
    struct TagComponent
    {
        std::string Tag;
        TagComponent() = default;
        TagComponent(const std::string& tag) : Tag(tag) {}
    };

    struct TransformComponent
    {
        glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

        TransformComponent() = default;

        glm::mat4 GetTransform() const
        {
            glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, { 1, 0, 0 })
                               * glm::rotate(glm::mat4(1.0f), Rotation.y, { 0, 1, 0 })
                               * glm::rotate(glm::mat4(1.0f), Rotation.z, { 0, 0, 1 });

            return glm::translate(glm::mat4(1.0f), Position)
                 * rotation
                 * glm::scale(glm::mat4(1.0f), Scale);
        }
    };

    struct SpriteComponent
    {
        glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        // Ref<Texture2D> Texture; // Add later
    };

    // MOBA specific components
    struct HealthComponent
    {
        float MaxHealth = 100.0f;
        float CurrentHealth = 100.0f;
        float HealthRegen = 0.0f;
    };

    struct ManaComponent
    {
        float MaxMana = 100.0f;
        float CurrentMana = 100.0f;
        float ManaRegen = 0.0f;
    };

    struct MovementComponent
    {
        float MoveSpeed = 300.0f;
        glm::vec3 Velocity = { 0.0f, 0.0f, 0.0f };
        glm::vec3 TargetPosition = { 0.0f, 0.0f, 0.0f };
        bool IsMoving = false;
    };

    struct ChampionComponent
    {
        std::string ChampionName;
        u32 Level = 1;
        u32 Experience = 0;
        float AttackDamage = 50.0f;
        float AbilityPower = 0.0f;
        float Armor = 20.0f;
        float MagicResist = 20.0f;
        float AttackSpeed = 1.0f;
        float CritChance = 0.0f;
    };

    struct TeamComponent
    {
        u8 TeamId = 0; // 0 = Blue, 1 = Red
    };

    // Forward declaration for Mesh component
    class Model;

    struct MeshComponent
    {
        Ref<Model> ModelAsset;
        bool CastShadows = true;
        bool ReceiveShadows = true;
        bool Visible = true;
    };
}
