#include "BlockPalettePanel.h"
#include "../EditorTheme.h"
#include <Scripting/Scripting.h>
#include <Scripting/Core/BlockRegistry.h>
#include <Scripting/Core/BlockTypes.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <algorithm>

using RiftSpire::GetCategoryColor;
using RiftSpire::GetCategoryName;

namespace RiftSpire
{
    void BlockPalettePanel::OnImGuiRender()
    {
        ImGui::Begin("Block Palette");
        
        // Search bar with modern styling
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, EditorColors::SurfaceLight);
        
        char searchBuffer[256] = "";
        if (!m_SearchFilter.empty())
        {
            strcpy_s(searchBuffer, sizeof(searchBuffer), m_SearchFilter.c_str());
        }
        
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputTextWithHint("##Search", "🔍 Search blocks...", searchBuffer, sizeof(searchBuffer)))
        {
            m_SearchFilter = searchBuffer;
        }
        
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // Category tree with colored headers
        RenderCategoryTree();
        
        ImGui::End();
    }
    
    void BlockPalettePanel::RenderCategoryTree()
    {
        auto& registry = BlockRegistry::Get();
        auto categories = registry.GetAllCategories();
        
        // "All Blocks" option with icon
        bool allSelected = (m_SelectedCategory == -1);
        ImGui::PushStyleColor(ImGuiCol_Header, EditorColors::SurfaceLight);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, EditorColors::SurfaceHover);
        
        if (ImGui::Selectable("📦 All Blocks", allSelected))
        {
            m_SelectedCategory = -1;
        }
        
        ImGui::PopStyleColor(2);
        
        if (allSelected)
        {
            ImGui::Indent(12.0f);
            
            // Show all blocks across all categories
            auto allTypeIds = registry.GetAllTypeIds();
            int blockIndex = 0;
            for (const auto& typeId : allTypeIds)
            {
                auto* def = registry.GetDefinition(typeId);
                if (!def) continue;
                
                // Filter by search
                if (!m_SearchFilter.empty())
                {
                    std::string lowerName = def->DisplayName;
                    std::string lowerFilter = m_SearchFilter;
                    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
                    std::transform(lowerFilter.begin(), lowerFilter.end(), lowerFilter.begin(), ::tolower);
                    
                    if (lowerName.find(lowerFilter) == std::string::npos)
                        continue;
                }
                
                RenderBlockItem(def, blockIndex++);
            }
            
            ImGui::Unindent(12.0f);
        }
        
        ImGui::Spacing();
        
        // Category-specific lists with colored headers
        for (size_t i = 0; i < categories.size(); ++i)
        {
            auto category = categories[i];
            std::string categoryName = GetCategoryName(category);
            
            // Get category color
            auto catColor = GetCategoryColor(category);
            ImVec4 headerColor(catColor.r / 255.0f, catColor.g / 255.0f, catColor.b / 255.0f, 0.4f);
            ImVec4 headerHoverColor(catColor.r / 255.0f, catColor.g / 255.0f, catColor.b / 255.0f, 0.6f);
            
            ImGui::PushStyleColor(ImGuiCol_Header, headerColor);
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, headerHoverColor);
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, headerHoverColor);
            
            // Use unique ID suffix to avoid ImGui ID conflicts
            std::string headerId = categoryName + "###cat_" + std::to_string(i);
            
            // Draw color indicator before header
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            float indicatorWidth = 4.0f;
            
            if (ImGui::CollapsingHeader(headerId.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                // Draw colored indicator bar
                drawList->AddRectFilled(
                    ImVec2(cursorPos.x, cursorPos.y),
                    ImVec2(cursorPos.x + indicatorWidth, cursorPos.y + ImGui::GetTextLineHeightWithSpacing()),
                    IM_COL32(catColor.r, catColor.g, catColor.b, 255),
                    2.0f
                );
                
                ImGui::Indent(12.0f);
                RenderBlockList(static_cast<int>(category), static_cast<int>(i));
                ImGui::Unindent(12.0f);
            }
            else
            {
                // Draw colored indicator bar even when collapsed
                drawList->AddRectFilled(
                    ImVec2(cursorPos.x, cursorPos.y),
                    ImVec2(cursorPos.x + indicatorWidth, cursorPos.y + ImGui::GetTextLineHeightWithSpacing()),
                    IM_COL32(catColor.r, catColor.g, catColor.b, 255),
                    2.0f
                );
            }
            
            ImGui::PopStyleColor(3);
        }
    }
    
    void BlockPalettePanel::RenderBlockList(int category, int categoryIndex)
    {
        auto& registry = BlockRegistry::Get();
        auto blocks = registry.GetBlocksByCategory(static_cast<BlockCategory>(category));
        
        auto catColor = GetCategoryColor(static_cast<BlockCategory>(category));
        ImVec4 blockTextColor(catColor.r / 255.0f, catColor.g / 255.0f, catColor.b / 255.0f, 1.0f);
        
        int blockIdx = 0;
        for (const auto* def : blocks)
        {
            if (!def) continue;
            
            // Filter by search
            if (!m_SearchFilter.empty())
            {
                std::string lowerName = def->DisplayName;
                std::string lowerFilter = m_SearchFilter;
                std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
                std::transform(lowerFilter.begin(), lowerFilter.end(), lowerFilter.begin(), ::tolower);
                
                if (lowerName.find(lowerFilter) == std::string::npos)
                    continue;
            }
            
            RenderBlockItem(def, categoryIndex * 1000 + blockIdx++);
        }
    }
    
    void BlockPalettePanel::RenderBlockItem(const BlockDefinition* def, int uniqueId)
    {
        ImGui::PushID(uniqueId);
        
        // Get category color
        auto catColor = GetCategoryColor(def->Category);
        ImVec4 blockColor(catColor.r / 255.0f, catColor.g / 255.0f, catColor.b / 255.0f, 1.0f);
        ImVec4 bgColor = ImVec4(blockColor.x * 0.3f, blockColor.y * 0.3f, blockColor.z * 0.3f, 0.5f);
        ImVec4 bgHoverColor = ImVec4(blockColor.x * 0.4f, blockColor.y * 0.4f, blockColor.z * 0.4f, 0.7f);
        
        // Styled selectable
        ImGui::PushStyleColor(ImGuiCol_Header, bgColor);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, bgHoverColor);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
        
        // Draw mini block preview indicator
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        
        // Block item with icon
        std::string displayText = def->Icon.empty() ? def->DisplayName : def->Icon + " " + def->DisplayName;
        
        if (ImGui::Selectable(displayText.c_str(), false, ImGuiSelectableFlags_None, ImVec2(0, 24)))
        {
            // Could open details or do something on click
        }
        
        // Draw left color bar
        drawList->AddRectFilled(
            ImVec2(cursorPos.x, cursorPos.y + 2),
            ImVec2(cursorPos.x + 3, cursorPos.y + 22),
            IM_COL32(catColor.r, catColor.g, catColor.b, 255),
            1.5f
        );
        
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);
        
        // Drag source
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            m_IsDragging = true;
            m_DraggedBlockType = def->TypeId;
            
            ImGui::SetDragDropPayload("BLOCK_TYPE", def->TypeId.c_str(), def->TypeId.size() + 1);
            
            // Draw preview with color
            auto color = GetCategoryColor(def->Category);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, 1.0f));
            ImGui::Text("%s %s", def->Icon.c_str(), def->DisplayName.c_str());
            ImGui::PopStyleColor();
            
            ImGui::EndDragDropSource();
        }
        
        // Tooltip on hover
        if (ImGui::IsItemHovered() && !def->Description.empty())
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
            ImGui::BeginTooltip();
            
            ImGui::PushStyleColor(ImGuiCol_Text, blockColor);
            ImGui::Text("%s %s", def->Icon.c_str(), def->DisplayName.c_str());
            ImGui::PopStyleColor();
            
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::TextDim);
            ImGui::TextWrapped("%s", def->Description.c_str());
            ImGui::PopStyleColor();
            
            ImGui::EndTooltip();
            ImGui::PopStyleVar(2);
        }
        
        ImGui::PopID();
    }
}
