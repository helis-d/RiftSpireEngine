#include "BlockInspectorPanel.h"
#include <Scripting/Core/BlockTypes.h>
#include <imgui.h>

using RiftSpire::GetCategoryColor;
using RiftSpire::GetCategoryName;

namespace RiftSpire
{
    void BlockInspectorPanel::OnImGuiRender()
    {
        ImGui::Begin("Block Inspector");
        
        if (!m_SelectedBlock)
        {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No block selected");
            ImGui::End();
            return;
        }
        
        RenderBlockInfo();
        ImGui::Separator();
        RenderBlockProperties();
        ImGui::Separator();
        RenderInputSlots();
        
        ImGui::End();
    }
    
    void BlockInspectorPanel::RenderBlockInfo()
    {
        if (!m_SelectedBlock) return;
        
        auto* def = m_SelectedBlock->GetDefinition();
        if (!def) return;
        
        ImGui::Text("Block Type: %s", def->DisplayName.c_str());
        ImGui::Text("Category: %s", GetCategoryName(def->Category).c_str());
        ImGui::Text("ID: %s", m_SelectedBlock->GetId().ToString().c_str());
        
        if (!def->Description.empty())
        {
            ImGui::Separator();
            ImGui::TextWrapped("%s", def->Description.c_str());
        }
    }
    
    void BlockInspectorPanel::RenderBlockProperties()
    {
        if (!m_SelectedBlock) return;
        
        ImGui::Text("Properties");
        ImGui::Spacing();
        
        // Position
        glm::vec2 pos = m_SelectedBlock->GetPosition();
        float posArray[2] = {pos.x, pos.y};
        if (ImGui::DragFloat2("Position", posArray, 1.0f))
        {
            m_SelectedBlock->SetPosition(glm::vec2(posArray[0], posArray[1]));
        }
        
        // Disabled state
        bool disabled = m_SelectedBlock->IsDisabled();
        if (ImGui::Checkbox("Disabled", &disabled))
        {
            m_SelectedBlock->SetDisabled(disabled);
        }
        
        // Collapsed state
        bool collapsed = m_SelectedBlock->IsCollapsed();
        if (ImGui::Checkbox("Collapsed", &collapsed))
        {
            m_SelectedBlock->SetCollapsed(collapsed);
        }
        
        // Comment
        char commentBuffer[512] = "";
        std::string comment = m_SelectedBlock->GetComment();
        if (!comment.empty())
        {
            strcpy_s(commentBuffer, sizeof(commentBuffer), comment.c_str());
        }
        
        if (ImGui::InputTextMultiline("Comment", commentBuffer, sizeof(commentBuffer)))
        {
            m_SelectedBlock->SetComment(commentBuffer);
        }
    }
    
    void BlockInspectorPanel::RenderInputSlots()
    {
        if (!m_SelectedBlock) return;
        
        size_t inputCount = m_SelectedBlock->GetInputSlotCount();
        if (inputCount == 0) return;
        
        ImGui::Text("Input Slots");
        ImGui::Spacing();
        
        for (size_t i = 0; i < inputCount; ++i)
        {
            auto* slot = m_SelectedBlock->GetInputSlot(i);
            if (!slot) continue;
            
            ImGui::PushID(static_cast<int>(i));
            
            ImGui::Text("%s:", slot->GetName().c_str());
            ImGui::SameLine();
            
            // Show connected block or default value
            if (slot->IsConnected())
            {
                auto connected = slot->GetConnectedBlock();
                if (connected)
                {
                    ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "Connected");
                }
            }
            else
            {
                // Edit default value based on type
                Value defaultValue = slot->GetDefaultValue();
                
                switch (slot->GetValueType())
                {
                    case ValueType::Bool:
                    {
                        bool val = defaultValue.AsBool();
                        if (ImGui::Checkbox("##value", &val))
                        {
                            slot->SetDefaultValue(Value(val));
                        }
                        break;
                    }
                    case ValueType::Int:
                    {
                        int val = static_cast<int>(defaultValue.AsInt());
                        if (ImGui::DragInt("##value", &val))
                        {
                            slot->SetDefaultValue(Value(static_cast<i64>(val)));
                        }
                        break;
                    }
                    case ValueType::Float:
                    {
                        float val = static_cast<float>(defaultValue.AsFloat());
                        if (ImGui::DragFloat("##value", &val, 0.1f))
                        {
                            slot->SetDefaultValue(Value(static_cast<f64>(val)));
                        }
                        break;
                    }
                    case ValueType::String:
                    {
                        char buffer[256] = "";
                        std::string str = defaultValue.AsString();
                        if (!str.empty())
                        {
                            strcpy_s(buffer, sizeof(buffer), str.c_str());
                        }
                        if (ImGui::InputText("##value", buffer, sizeof(buffer)))
                        {
                            slot->SetDefaultValue(Value(std::string(buffer)));
                        }
                        break;
                    }
                    case ValueType::Vector2:
                    {
                        auto vec = defaultValue.AsVector2();
                        float vecArray[2] = {vec.x, vec.y};
                        if (ImGui::DragFloat2("##value", vecArray, 0.1f))
                        {
                            slot->SetDefaultValue(Value(glm::vec2(vecArray[0], vecArray[1])));
                        }
                        break;
                    }
                    case ValueType::Vector3:
                    {
                        auto vec = defaultValue.AsVector3();
                        float vecArray[3] = {vec.x, vec.y, vec.z};
                        if (ImGui::DragFloat3("##value", vecArray, 0.1f))
                        {
                            slot->SetDefaultValue(Value(glm::vec3(vecArray[0], vecArray[1], vecArray[2])));
                        }
                        break;
                    }
                    default:
                        ImGui::TextDisabled("(no editor)");
                        break;
                }
            }
            
            ImGui::PopID();
        }
    }
}
