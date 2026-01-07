#include "ScratchBlockRenderer.h"
#include <cmath>
#include <algorithm>

namespace RiftSpire
{
    //=========================================================================
    // Main Rendering Entry Point
    //=========================================================================
    
    void ScratchBlockRenderer::RenderBlock(Block* block, const BlockRenderContext& ctx)
    {
        if (!block || !block->GetDefinition()) return;
        
        // Calculate screen position
        glm::vec2 blockPos = block->GetPosition();
        ImVec2 screenPos(
            ctx.CanvasPos.x + ctx.CanvasOffset.x + blockPos.x * ctx.Zoom,
            ctx.CanvasPos.y + ctx.CanvasOffset.y + blockPos.y * ctx.Zoom
        );
        
        // Calculate layout
        BlockLayout layout = BlockGeometry::CalculateLayout(block, ctx.Zoom);
        
        // Determine block state
        bool isSelected = ctx.SelectedBlocks && ctx.SelectedBlocks->contains(block->GetId());
        bool isHovered = ctx.HoveredBlock && *ctx.HoveredBlock == block->GetId();
        bool isActive = ctx.ActiveBlocks && ctx.ActiveBlocks->contains(block->GetId());
        bool hasError = ctx.Errors && ctx.Errors->contains(block->GetId());
        bool isDisabled = block->IsDisabled();
        
        // Draw shadow
        if (!isDisabled) {
            RenderBlockShadow(screenPos, layout, ctx);
        }
        
        // Draw based on shape
        if (layout.IsReporter) {
            RenderReporterBlock(block, screenPos, layout, ctx);
        }
        else if (layout.IsHatBlock) {
            RenderHatBlock(block, screenPos, layout, ctx);
        }
        else if (layout.IsCBlock) {
            RenderCBlock(block, screenPos, layout, ctx);
        }
        else if (layout.IsCapBlock) {
            RenderCapBlock(block, screenPos, layout, ctx);
        }
        else {
            RenderFlatBlock(block, screenPos, layout, ctx);
        }
        
        // Draw overlays
        if (isSelected) {
            RenderSelectionGlow(screenPos, layout, ctx);
        }
        
        if (isActive && ctx.ShowRuntimeValues) {
            RenderExecutionHighlight(screenPos, layout, ctx);
        }
        
        if (hasError) {
            RenderErrorOverlay(screenPos, layout, ctx.Errors->at(block->GetId()), ctx);
        }
        
        // Draw runtime value if available
        if (ctx.ShowRuntimeValues && ctx.RuntimeValues && ctx.RuntimeValues->contains(block->GetId())) {
            RenderRuntimeValue(
                ImVec2(screenPos.x + layout.TotalWidth + 10, screenPos.y),
                ctx.RuntimeValues->at(block->GetId()),
                ctx
            );
        }
    }
    
    //=========================================================================
    // Block Type Renderers
    //=========================================================================
    
    void ScratchBlockRenderer::RenderFlatBlock(Block* block, const ImVec2& pos, const BlockLayout& layout, const BlockRenderContext& ctx)
    {
        auto* def = block->GetDefinition();
        ImVec4 color = GetBlockColor(def->Category);
        
        // Generate path
        auto path = BlockGeometry::GenerateFlatBlockPath(pos, layout, ctx.Zoom);
        
        // Draw with gradient
        ImVec4 topColor = LightenColor(color, 0.15f);
        ImVec4 bottomColor = DarkenColor(color, 0.2f);
        DrawGradientBlock(path, topColor, bottomColor, ctx);
        
        // Draw border
        DrawBlockPath(path, ImVec4(0,0,0,0), DarkenColor(color, 0.4f), 1.5f, ctx);
        
        // Draw highlight stripe
        float stripeHeight = 4.0f * ctx.Zoom;
        ctx.DrawList->AddRectFilled(
            ImVec2(pos.x + BlockGeometry::CORNER_RADIUS * ctx.Zoom, pos.y),
            ImVec2(pos.x + layout.TotalWidth - BlockGeometry::CORNER_RADIUS * ctx.Zoom, pos.y + stripeHeight),
            ColorToU32(LightenColor(color, 0.35f))
        );
        
        // Draw icon
        float iconX = pos.x + 12.0f * ctx.Zoom;
        float textY = pos.y + (layout.HeaderHeight - ImGui::GetFontSize()) / 2.0f;
        
        if (!def->Icon.empty()) {
            ctx.DrawList->AddText(
                ImVec2(iconX, textY),
                IM_COL32(255, 255, 255, 220),
                def->Icon.c_str()
            );
            iconX += 24.0f * ctx.Zoom;
        }
        
        // Draw text with shadow
        ctx.DrawList->AddText(
            ImVec2(iconX + 1, textY + 1),
            IM_COL32(0, 0, 0, 80),
            def->DisplayName.c_str()
        );
        ctx.DrawList->AddText(
            ImVec2(iconX, textY),
            IM_COL32(255, 255, 255, 255),
            def->DisplayName.c_str()
        );
        
        // Render inline input fields
        for (size_t i = 0; i < layout.InputSlots.size(); ++i) {
            auto& slotLayout = layout.InputSlots[i];
            ImVec2 slotPos(pos.x + slotLayout.Position.x, pos.y + slotLayout.Position.y);
            
            BlockSlot* slot = block->GetInputSlot(i);
            if (slot && !slot->IsConnected()) {
                RenderInlineField(slot, slotPos, slotLayout.Size.x, ctx);
            }
        }
    }
    
    void ScratchBlockRenderer::RenderCBlock(Block* block, const ImVec2& pos, const BlockLayout& layout, const BlockRenderContext& ctx)
    {
        auto* def = block->GetDefinition();
        ImVec4 color = GetBlockColor(def->Category);
        
        // Generate C-block path
        auto path = BlockGeometry::GenerateCBlockPath(pos, layout, ctx.Zoom);
        
        // Draw with gradient
        ImVec4 topColor = LightenColor(color, 0.15f);
        ImVec4 bottomColor = DarkenColor(color, 0.2f);
        DrawGradientBlock(path, topColor, bottomColor, ctx);
        
        // Draw border
        DrawBlockPath(path, ImVec4(0,0,0,0), DarkenColor(color, 0.4f), 1.5f, ctx);
        
        // Draw header text
        float iconX = pos.x + 12.0f * ctx.Zoom;
        float textY = pos.y + (layout.HeaderHeight - ImGui::GetFontSize()) / 2.0f;
        
        if (!def->Icon.empty()) {
            ctx.DrawList->AddText(ImVec2(iconX, textY), IM_COL32(255, 255, 255, 220), def->Icon.c_str());
            iconX += 24.0f * ctx.Zoom;
        }
        
        ctx.DrawList->AddText(ImVec2(iconX + 1, textY + 1), IM_COL32(0, 0, 0, 80), def->DisplayName.c_str());
        ctx.DrawList->AddText(ImVec2(iconX, textY), IM_COL32(255, 255, 255, 255), def->DisplayName.c_str());
        
        // Draw inline fields
        for (size_t i = 0; i < layout.InputSlots.size(); ++i) {
            auto& slotLayout = layout.InputSlots[i];
            ImVec2 slotPos(pos.x + slotLayout.Position.x, pos.y + slotLayout.Position.y);
            
            BlockSlot* slot = block->GetInputSlot(i);
            if (slot && !slot->IsConnected()) {
                RenderInlineField(slot, slotPos, slotLayout.Size.x, ctx);
            }
        }
        
        // Draw nested areas
        for (size_t i = 0; i < layout.NestedAreas.size(); ++i) {
            auto& nested = layout.NestedAreas[i];
            ImVec2 areaPos(pos.x + nested.Position.x, pos.y + nested.Position.y);
            
            RenderNestedArea(nested, color, ctx);
            
            // If collapsed, show indicator
            if (block->IsCollapsed()) {
                ctx.DrawList->AddText(
                    ImVec2(areaPos.x + 5, areaPos.y + 5),
                    IM_COL32(200, 200, 200, 180),
                    "..."
                );
            }
        }
        
        // Collapse button
        float collapseX = pos.x + layout.TotalWidth - 24.0f * ctx.Zoom;
        float collapseY = pos.y + 4.0f * ctx.Zoom;
        const char* collapseIcon = block->IsCollapsed() ? "▶" : "▼";
        ctx.DrawList->AddText(ImVec2(collapseX, collapseY), IM_COL32(255, 255, 255, 180), collapseIcon);
    }
    
    void ScratchBlockRenderer::RenderHatBlock(Block* block, const ImVec2& pos, const BlockLayout& layout, const BlockRenderContext& ctx)
    {
        auto* def = block->GetDefinition();
        ImVec4 color = GetBlockColor(def->Category);
        
        // Generate hat path
        auto path = BlockGeometry::GenerateHatBlockPath(pos, layout, ctx.Zoom);
        
        // Draw with gradient
        ImVec4 topColor = LightenColor(color, 0.2f);
        ImVec4 bottomColor = DarkenColor(color, 0.15f);
        DrawGradientBlock(path, topColor, bottomColor, ctx);
        
        // Draw border
        DrawBlockPath(path, ImVec4(0,0,0,0), DarkenColor(color, 0.4f), 1.5f, ctx);
        
        // Draw text (offset for hat curve)
        float iconX = pos.x + 12.0f * ctx.Zoom;
        float textY = pos.y + BlockGeometry::HAT_CURVE_HEIGHT * ctx.Zoom + 
                     (layout.HeaderHeight - ImGui::GetFontSize()) / 2.0f;
        
        if (!def->Icon.empty()) {
            ctx.DrawList->AddText(ImVec2(iconX, textY), IM_COL32(255, 255, 255, 220), def->Icon.c_str());
            iconX += 24.0f * ctx.Zoom;
        }
        
        ctx.DrawList->AddText(ImVec2(iconX + 1, textY + 1), IM_COL32(0, 0, 0, 80), def->DisplayName.c_str());
        ctx.DrawList->AddText(ImVec2(iconX, textY), IM_COL32(255, 255, 255, 255), def->DisplayName.c_str());
    }
    
    void ScratchBlockRenderer::RenderReporterBlock(Block* block, const ImVec2& pos, const BlockLayout& layout, const BlockRenderContext& ctx)
    {
        auto* def = block->GetDefinition();
        ImVec4 color = GetBlockColor(def->Category);
        
        // Reporter blocks are oval/rounded
        float r = BlockGeometry::REPORTER_ROUNDING * ctx.Zoom;
        
        // Draw oval background
        ctx.DrawList->AddRectFilled(
            pos,
            ImVec2(pos.x + layout.TotalWidth, pos.y + layout.TotalHeight),
            ColorToU32(color),
            r
        );
        
        // Border
        ctx.DrawList->AddRect(
            pos,
            ImVec2(pos.x + layout.TotalWidth, pos.y + layout.TotalHeight),
            ColorToU32(DarkenColor(color, 0.4f)),
            r,
            0,
            1.5f
        );
        
        // Text centered
        float textWidth = ImGui::CalcTextSize(def->DisplayName.c_str()).x;
        float textX = pos.x + (layout.TotalWidth - textWidth) / 2.0f;
        float textY = pos.y + (layout.TotalHeight - ImGui::GetFontSize()) / 2.0f;
        
        ctx.DrawList->AddText(ImVec2(textX, textY), IM_COL32(255, 255, 255, 255), def->DisplayName.c_str());
    }
    
    void ScratchBlockRenderer::RenderCapBlock(Block* block, const ImVec2& pos, const BlockLayout& layout, const BlockRenderContext& ctx)
    {
        auto* def = block->GetDefinition();
        ImVec4 color = GetBlockColor(def->Category);
        
        // Generate cap path
        auto path = BlockGeometry::GenerateCapBlockPath(pos, layout, ctx.Zoom);
        
        // Draw with gradient
        DrawGradientBlock(path, LightenColor(color, 0.15f), DarkenColor(color, 0.2f), ctx);
        DrawBlockPath(path, ImVec4(0,0,0,0), DarkenColor(color, 0.4f), 1.5f, ctx);
        
        // Draw text
        float iconX = pos.x + 12.0f * ctx.Zoom;
        float textY = pos.y + (layout.HeaderHeight - ImGui::GetFontSize()) / 2.0f;
        
        if (!def->Icon.empty()) {
            ctx.DrawList->AddText(ImVec2(iconX, textY), IM_COL32(255, 255, 255, 220), def->Icon.c_str());
            iconX += 24.0f * ctx.Zoom;
        }
        
        ctx.DrawList->AddText(ImVec2(iconX, textY), IM_COL32(255, 255, 255, 255), def->DisplayName.c_str());
    }
    
    //=========================================================================
    // Inline Field Rendering
    //=========================================================================
    
    bool ScratchBlockRenderer::RenderInlineField(BlockSlot* slot, const ImVec2& pos, float width, const BlockRenderContext& ctx)
    {
        if (!slot) return false;
        
        switch (slot->GetValueType())
        {
            case ValueType::Bool:
                return RenderBooleanToggle(slot, pos, ctx);
            case ValueType::Int:
            case ValueType::Float:
                return RenderNumberInput(slot, pos, width, ctx);
            case ValueType::String:
                return RenderStringInput(slot, pos, width, ctx);
            case ValueType::Vector2:
            case ValueType::Vector3:
                return RenderVectorInput(slot, pos, width, ctx);
            case ValueType::Color:
                return RenderColorPicker(slot, pos, ctx);
            default:
                return false;
        }
    }
    
    bool ScratchBlockRenderer::RenderNumberInput(BlockSlot* slot, const ImVec2& pos, float width, const BlockRenderContext& ctx)
    {
        // Draw rounded input field background
        float height = 20.0f * ctx.Zoom;
        float rounding = 4.0f * ctx.Zoom;
        
        ctx.DrawList->AddRectFilled(
            pos,
            ImVec2(pos.x + width, pos.y + height),
            IM_COL32(255, 255, 255, 230),
            rounding
        );
        
        // Draw current value
        Value val = slot->GetDefaultValue();
        std::string text;
        if (slot->GetValueType() == ValueType::Int) {
            text = std::to_string(val.AsInt());
        } else {
            char buf[32];
            snprintf(buf, sizeof(buf), "%.2f", val.AsFloat());
            text = buf;
        }
        
        float textY = pos.y + (height - ImGui::GetFontSize()) / 2.0f;
        ctx.DrawList->AddText(
            ImVec2(pos.x + 4, textY),
            IM_COL32(40, 40, 50, 255),
            text.c_str()
        );
        
        return false;
    }
    
    bool ScratchBlockRenderer::RenderStringInput(BlockSlot* slot, const ImVec2& pos, float width, const BlockRenderContext& ctx)
    {
        float height = 20.0f * ctx.Zoom;
        float rounding = 4.0f * ctx.Zoom;
        
        ctx.DrawList->AddRectFilled(
            pos,
            ImVec2(pos.x + width, pos.y + height),
            IM_COL32(255, 255, 255, 230),
            rounding
        );
        
        std::string text = slot->GetDefaultValue().AsString();
        if (text.empty()) text = "(empty)";
        
        float textY = pos.y + (height - ImGui::GetFontSize()) / 2.0f;
        ctx.DrawList->AddText(
            ImVec2(pos.x + 4, textY),
            IM_COL32(40, 40, 50, 255),
            text.c_str()
        );
        
        return false;
    }
    
    bool ScratchBlockRenderer::RenderBooleanToggle(BlockSlot* slot, const ImVec2& pos, const BlockRenderContext& ctx)
    {
        float size = 20.0f * ctx.Zoom;
        float rounding = 4.0f * ctx.Zoom;
        
        bool value = slot->GetDefaultValue().AsBool();
        ImU32 bgColor = value ? IM_COL32(80, 200, 120, 255) : IM_COL32(200, 80, 80, 255);
        
        ctx.DrawList->AddRectFilled(pos, ImVec2(pos.x + size, pos.y + size), bgColor, rounding);
        
        // Checkmark or X
        const char* icon = value ? "✓" : "✗";
        float textY = pos.y + (size - ImGui::GetFontSize()) / 2.0f;
        ctx.DrawList->AddText(ImVec2(pos.x + 4, textY), IM_COL32(255, 255, 255, 255), icon);
        
        return false;
    }
    
    bool ScratchBlockRenderer::RenderDropdown(BlockSlot* slot, const ImVec2& pos, float width, const BlockRenderContext& ctx)
    {
        float height = 20.0f * ctx.Zoom;
        float rounding = 4.0f * ctx.Zoom;
        
        ctx.DrawList->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), IM_COL32(240, 240, 250, 255), rounding);
        
        // Arrow indicator
        ctx.DrawList->AddText(ImVec2(pos.x + width - 16, pos.y + 2), IM_COL32(100, 100, 100, 255), "▼");
        
        return false;
    }
    
    bool ScratchBlockRenderer::RenderColorPicker(BlockSlot* slot, const ImVec2& pos, const BlockRenderContext& ctx)
    {
        float size = 20.0f * ctx.Zoom;
        
        // TODO: Get color from value
        ImU32 color = IM_COL32(100, 150, 200, 255);
        
        ctx.DrawList->AddRectFilled(pos, ImVec2(pos.x + size, pos.y + size), color, 4.0f * ctx.Zoom);
        ctx.DrawList->AddRect(pos, ImVec2(pos.x + size, pos.y + size), IM_COL32(0, 0, 0, 100), 4.0f * ctx.Zoom);
        
        return false;
    }
    
    bool ScratchBlockRenderer::RenderVectorInput(BlockSlot* slot, const ImVec2& pos, float width, const BlockRenderContext& ctx)
    {
        float height = 20.0f * ctx.Zoom;
        float rounding = 4.0f * ctx.Zoom;
        
        ctx.DrawList->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), IM_COL32(255, 255, 255, 230), rounding);
        
        // Show compact vector representation
        std::string text = "(...)";
        float textY = pos.y + (height - ImGui::GetFontSize()) / 2.0f;
        ctx.DrawList->AddText(ImVec2(pos.x + 4, textY), IM_COL32(40, 40, 50, 255), text.c_str());
        
        return false;
    }
    
    //=========================================================================
    // Nested Area Rendering
    //=========================================================================
    
    void ScratchBlockRenderer::RenderNestedArea(const NestedAreaLayout& area, const ImVec4& color, const BlockRenderContext& ctx)
    {
        // Draw slightly darker background for nested area
        ctx.DrawList->AddRectFilled(
            ImVec2(ctx.CanvasPos.x + area.Position.x, ctx.CanvasPos.y + area.Position.y),
            ImVec2(ctx.CanvasPos.x + area.Position.x + area.Size.x, ctx.CanvasPos.y + area.Position.y + area.Size.y),
            IM_COL32(20, 22, 35, 180),
            4.0f * ctx.Zoom
        );
    }
    
    void ScratchBlockRenderer::RenderNestedDropZone(const ImVec2& pos, const ImVec2& size, bool isActive, const BlockRenderContext& ctx)
    {
        ImU32 color = isActive ? IM_COL32(100, 200, 100, 100) : IM_COL32(100, 100, 100, 50);
        ctx.DrawList->AddRectFilled(pos, ImVec2(pos.x + size.x, pos.y + size.y), color, 4.0f);
        
        if (isActive) {
            ctx.DrawList->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(100, 255, 100, 200), 4.0f, 0, 2.0f);
        }
    }
    
    //=========================================================================
    // Visual Effects
    //=========================================================================
    
    void ScratchBlockRenderer::RenderBlockShadow(const ImVec2& pos, const BlockLayout& layout, const BlockRenderContext& ctx)
    {
        float offset = 3.0f * ctx.Zoom;
        ctx.DrawList->AddRectFilled(
            ImVec2(pos.x + offset, pos.y + offset),
            ImVec2(pos.x + layout.TotalWidth + offset, pos.y + layout.TotalHeight + offset),
            IM_COL32(0, 0, 0, 40),
            BlockGeometry::CORNER_RADIUS * ctx.Zoom
        );
    }
    
    void ScratchBlockRenderer::RenderSelectionGlow(const ImVec2& pos, const BlockLayout& layout, const BlockRenderContext& ctx)
    {
        float padding = 3.0f * ctx.Zoom;
        ctx.DrawList->AddRect(
            ImVec2(pos.x - padding, pos.y - padding),
            ImVec2(pos.x + layout.TotalWidth + padding, pos.y + layout.TotalHeight + padding),
            IM_COL32(255, 255, 100, 255),
            BlockGeometry::CORNER_RADIUS * ctx.Zoom + padding,
            0,
            3.0f
        );
    }
    
    void ScratchBlockRenderer::RenderExecutionHighlight(const ImVec2& pos, const BlockLayout& layout, const BlockRenderContext& ctx)
    {
        float padding = 2.0f * ctx.Zoom;
        ctx.DrawList->AddRect(
            ImVec2(pos.x - padding, pos.y - padding),
            ImVec2(pos.x + layout.TotalWidth + padding, pos.y + layout.TotalHeight + padding),
            IM_COL32(100, 255, 100, 200),
            BlockGeometry::CORNER_RADIUS * ctx.Zoom + padding,
            0,
            2.0f
        );
    }
    
    void ScratchBlockRenderer::RenderErrorOverlay(const ImVec2& pos, const BlockLayout& layout, const std::string& error, const BlockRenderContext& ctx)
    {
        // Red tint overlay
        ctx.DrawList->AddRectFilled(
            pos,
            ImVec2(pos.x + layout.TotalWidth, pos.y + layout.TotalHeight),
            IM_COL32(255, 0, 0, 50),
            BlockGeometry::CORNER_RADIUS * ctx.Zoom
        );
        
        // Error icon
        ctx.DrawList->AddText(
            ImVec2(pos.x + layout.TotalWidth - 20, pos.y + 2),
            IM_COL32(255, 100, 100, 255),
            "⚠"
        );
    }
    
    void ScratchBlockRenderer::RenderRuntimeValue(const ImVec2& pos, const Value& value, const BlockRenderContext& ctx)
    {
        std::string text = value.AsString();
        if (text.length() > 20) {
            text = text.substr(0, 17) + "...";
        }
        
        float padding = 4.0f;
        float textWidth = ImGui::CalcTextSize(text.c_str()).x;
        
        // Background bubble
        ctx.DrawList->AddRectFilled(
            ImVec2(pos.x, pos.y),
            ImVec2(pos.x + textWidth + padding * 2, pos.y + 20),
            IM_COL32(60, 60, 80, 230),
            4.0f
        );
        
        ctx.DrawList->AddText(
            ImVec2(pos.x + padding, pos.y + 2),
            IM_COL32(200, 255, 200, 255),
            text.c_str()
        );
    }
    
    //=========================================================================
    // Path Drawing
    //=========================================================================
    
    void ScratchBlockRenderer::DrawBlockPath(const std::vector<ImVec2>& path, const ImVec4& fillColor, const ImVec4& borderColor, float borderWidth, const BlockRenderContext& ctx)
    {
        if (path.empty()) return;
        
        // Draw filled polygon
        if (fillColor.w > 0.0f) {
            ctx.DrawList->AddConvexPolyFilled(path.data(), static_cast<int>(path.size()), ColorToU32(fillColor));
        }
        
        // Draw border
        if (borderColor.w > 0.0f && borderWidth > 0.0f) {
            ctx.DrawList->AddPolyline(path.data(), static_cast<int>(path.size()), ColorToU32(borderColor), ImDrawFlags_Closed, borderWidth);
        }
    }
    
    void ScratchBlockRenderer::DrawGradientBlock(const std::vector<ImVec2>& path, const ImVec4& topColor, const ImVec4& bottomColor, const BlockRenderContext& ctx)
    {
        // For simplicity, draw solid color (gradient would require custom shader or multi-pass)
        ImVec4 avgColor(
            (topColor.x + bottomColor.x) / 2.0f,
            (topColor.y + bottomColor.y) / 2.0f,
            (topColor.z + bottomColor.z) / 2.0f,
            (topColor.w + bottomColor.w) / 2.0f
        );
        
        ctx.DrawList->AddConvexPolyFilled(path.data(), static_cast<int>(path.size()), ColorToU32(avgColor));
    }
    
    //=========================================================================
    // Color Utilities
    //=========================================================================
    
    ImVec4 ScratchBlockRenderer::GetBlockColor(BlockCategory category)
    {
        switch (category)
        {
            case BlockCategory::Operators:          return ScratchBlockColors::Operators;
            case BlockCategory::ControlFlow:        return ScratchBlockColors::Control;
            case BlockCategory::Events:             return ScratchBlockColors::Events;
            case BlockCategory::Movement:           return ScratchBlockColors::Motion;
            case BlockCategory::PositionTransform:  return ScratchBlockColors::Motion;
            case BlockCategory::Targeting:          return ScratchBlockColors::Targeting;
            case BlockCategory::Attack:             return ScratchBlockColors::Attack;
            case BlockCategory::Damage:             return ScratchBlockColors::Attack;
            case BlockCategory::Defense:            return ScratchBlockColors::Defense;
            case BlockCategory::Health:             return ScratchBlockColors::Health;
            case BlockCategory::MannaEnergy:        return ScratchBlockColors::Mana;
            case BlockCategory::Abilities:          return ScratchBlockColors::Looks;
            case BlockCategory::Ultimate:           return ScratchBlockColors::Ultimate;
            case BlockCategory::Cooldown:           return ScratchBlockColors::Sensing;
            case BlockCategory::BuffsDebuffs:       return ScratchBlockColors::Variables;
            case BlockCategory::StatusEffects:      return ScratchBlockColors::Variables;
            case BlockCategory::AreaEffects:        return ScratchBlockColors::MyBlocks;
            case BlockCategory::TeamGroup:          return ScratchBlockColors::Operators;
            case BlockCategory::Vision:             return ScratchBlockColors::Sensing;
            case BlockCategory::AI:                 return ScratchBlockColors::Network;
            case BlockCategory::CollisionPhysics:   return ScratchBlockColors::Sensing;
            case BlockCategory::Time:               return ScratchBlockColors::Control;
            case BlockCategory::Animation:          return ScratchBlockColors::Looks;
            case BlockCategory::Effects:            return ScratchBlockColors::Looks;
            case BlockCategory::Sound:              return ScratchBlockColors::Sound;
            case BlockCategory::UIHud:              return ScratchBlockColors::Sensing;
            case BlockCategory::Camera:             return ScratchBlockColors::Motion;
            case BlockCategory::Input:              return ScratchBlockColors::Sensing;
            case BlockCategory::NetworkMultiplayer: return ScratchBlockColors::Network;
            case BlockCategory::Synchronization:    return ScratchBlockColors::Network;
            case BlockCategory::DataVariables:      return ScratchBlockColors::Variables;
            case BlockCategory::SaveState:          return ScratchBlockColors::Variables;
            case BlockCategory::DebugLogging:       return ScratchBlockColors::Debug;
            default:                                return ScratchBlockColors::Debug;
        }
    }
    
    //=========================================================================
    // ImGui ID Management
    //=========================================================================
    
    void ScratchBlockRenderer::PushUniqueId(Block* block, const std::string& slotName)
    {
        std::string id = block->GetId().ToString() + "_" + slotName + "_" + std::to_string(m_UniqueIdCounter++);
        ImGui::PushID(id.c_str());
    }
    
    void ScratchBlockRenderer::PopUniqueId()
    {
        ImGui::PopID();
    }
}
