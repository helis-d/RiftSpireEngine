#include "InspectorPanel.h"

namespace RiftSpire
{
    template<typename T>
    void InspectorPanel::DrawComponentUI(const char* name, Entity entity, std::function<void(T&)> uiFunction)
    {
        if (entity.HasComponent<T>())
        {
            ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
            
            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", name);
            
            ImGui::SameLine(ImGui::GetWindowWidth() - 25.0f);
            if (ImGui::Button("...", ImVec2(20, 20)))
            {
                ImGui::OpenPopup("ComponentSettings");
            }

            bool removeComponent = false;
            if (ImGui::BeginPopup("ComponentSettings"))
            {
                if (ImGui::MenuItem("Remove Component"))
                {
                    removeComponent = true;
                }
                ImGui::EndPopup();
            }

            if (open)
            {
                auto& component = entity.GetComponent<T>();
                uiFunction(component);
                ImGui::TreePop();
            }

            if (removeComponent)
            {
                entity.RemoveComponent<T>();
            }
        }
    }

    void InspectorPanel::OnImGuiRender()
    {
        ImGui::Begin("Inspector");

        if (m_SelectionContext)
        {
            DrawComponents(m_SelectionContext);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Add Component button
            if (ImGui::Button("Add Component", ImVec2(-1, 30)))
            {
                ImGui::OpenPopup("AddComponent");
            }

            if (ImGui::BeginPopup("AddComponent"))
            {
                if (!m_SelectionContext.HasComponent<HealthComponent>())
                {
                    if (ImGui::MenuItem("Health"))
                    {
                        m_SelectionContext.AddComponent<HealthComponent>();
                        ImGui::CloseCurrentPopup();
                    }
                }
                if (!m_SelectionContext.HasComponent<ManaComponent>())
                {
                    if (ImGui::MenuItem("Mana"))
                    {
                        m_SelectionContext.AddComponent<ManaComponent>();
                        ImGui::CloseCurrentPopup();
                    }
                }
                if (!m_SelectionContext.HasComponent<MovementComponent>())
                {
                    if (ImGui::MenuItem("Movement"))
                    {
                        m_SelectionContext.AddComponent<MovementComponent>();
                        ImGui::CloseCurrentPopup();
                    }
                }
                if (!m_SelectionContext.HasComponent<ChampionComponent>())
                {
                    if (ImGui::MenuItem("Champion"))
                    {
                        m_SelectionContext.AddComponent<ChampionComponent>();
                        ImGui::CloseCurrentPopup();
                    }
                }
                if (!m_SelectionContext.HasComponent<TeamComponent>())
                {
                    if (ImGui::MenuItem("Team"))
                    {
                        m_SelectionContext.AddComponent<TeamComponent>();
                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::EndPopup();
            }
        }
        else
        {
            ImGui::TextDisabled("No entity selected");
        }

        ImGui::End();
    }

    void InspectorPanel::DrawComponents(Entity entity)
    {
        // Tag Component (always present)
        if (entity.HasComponent<TagComponent>())
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer) - 1);
            
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }
        }

        ImGui::Separator();

        // Transform Component
        DrawComponentUI<TransformComponent>("Transform", entity, [](TransformComponent& component)
        {
            ImGui::DragFloat3("Position", glm::value_ptr(component.Position), 0.1f);
            
            glm::vec3 rotationDegrees = glm::degrees(component.Rotation);
            if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotationDegrees), 1.0f))
            {
                component.Rotation = glm::radians(rotationDegrees);
            }
            
            ImGui::DragFloat3("Scale", glm::value_ptr(component.Scale), 0.1f, 0.0f, 100.0f);
        });

        // Health Component
        DrawComponentUI<HealthComponent>("Health", entity, [](HealthComponent& component)
        {
            ImGui::DragFloat("Max Health", &component.MaxHealth, 1.0f, 0.0f, 10000.0f);
            ImGui::DragFloat("Current Health", &component.CurrentHealth, 1.0f, 0.0f, component.MaxHealth);
            ImGui::DragFloat("Health Regen", &component.HealthRegen, 0.1f, 0.0f, 100.0f);
            
            // Health bar preview
            float healthPercent = component.CurrentHealth / component.MaxHealth;
            ImGui::ProgressBar(healthPercent, ImVec2(-1, 0), "");
        });

        // Mana Component
        DrawComponentUI<ManaComponent>("Mana", entity, [](ManaComponent& component)
        {
            ImGui::DragFloat("Max Mana", &component.MaxMana, 1.0f, 0.0f, 10000.0f);
            ImGui::DragFloat("Current Mana", &component.CurrentMana, 1.0f, 0.0f, component.MaxMana);
            ImGui::DragFloat("Mana Regen", &component.ManaRegen, 0.1f, 0.0f, 100.0f);
            
            float manaPercent = component.CurrentMana / component.MaxMana;
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
            ImGui::ProgressBar(manaPercent, ImVec2(-1, 0), "");
            ImGui::PopStyleColor();
        });

        // Movement Component
        DrawComponentUI<MovementComponent>("Movement", entity, [](MovementComponent& component)
        {
            ImGui::DragFloat("Move Speed", &component.MoveSpeed, 1.0f, 0.0f, 1000.0f);
            ImGui::DragFloat3("Velocity", glm::value_ptr(component.Velocity), 0.1f);
            ImGui::DragFloat3("Target Position", glm::value_ptr(component.TargetPosition), 0.1f);
            ImGui::Checkbox("Is Moving", &component.IsMoving);
        });

        // Champion Component
        DrawComponentUI<ChampionComponent>("Champion", entity, [](ChampionComponent& component)
        {
            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            strncpy_s(buffer, sizeof(buffer), component.ChampionName.c_str(), sizeof(buffer) - 1);
            if (ImGui::InputText("Name", buffer, sizeof(buffer)))
            {
                component.ChampionName = std::string(buffer);
            }

            int level = static_cast<int>(component.Level);
            if (ImGui::DragInt("Level", &level, 0.1f, 1, 18))
            {
                component.Level = static_cast<u32>(level);
            }

            ImGui::Separator();
            ImGui::Text("Stats");
            ImGui::DragFloat("Attack Damage", &component.AttackDamage, 0.5f, 0.0f, 1000.0f);
            ImGui::DragFloat("Ability Power", &component.AbilityPower, 0.5f, 0.0f, 1000.0f);
            ImGui::DragFloat("Armor", &component.Armor, 0.5f, 0.0f, 500.0f);
            ImGui::DragFloat("Magic Resist", &component.MagicResist, 0.5f, 0.0f, 500.0f);
            ImGui::DragFloat("Attack Speed", &component.AttackSpeed, 0.01f, 0.1f, 5.0f);
            ImGui::SliderFloat("Crit Chance", &component.CritChance, 0.0f, 1.0f, "%.0f%%");
        });

        // Team Component
        DrawComponentUI<TeamComponent>("Team", entity, [](TeamComponent& component)
        {
            const char* teams[] = { "Blue Team", "Red Team" };
            int currentTeam = component.TeamId;
            if (ImGui::Combo("Team", &currentTeam, teams, IM_ARRAYSIZE(teams)))
            {
                component.TeamId = static_cast<u8>(currentTeam);
            }
        });
    }
}
