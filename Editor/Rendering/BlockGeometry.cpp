#include "BlockGeometry.h"
#include <algorithm>
#include <cmath>

namespace RiftSpire
{
    //=========================================================================
    // Layout Calculation
    //=========================================================================
    
    BlockLayout BlockGeometry::CalculateLayout(const Block* block, float zoom)
    {
        BlockLayout layout;
        
        if (!block || !block->GetDefinition()) {
            return layout;
        }
        
        auto* def = block->GetDefinition();
        
        // Determine block characteristics based on shape
        switch (def->Shape)
        {
            case BlockShape::EventNested:
                layout.IsHatBlock = true;
                layout.HasTopNotch = false;
                break;
                
            case BlockShape::SingleNested:
            case BlockShape::LoopNested:
            case BlockShape::MultiNested:
            case BlockShape::ConditionalNested:
            case BlockShape::ScopedNested:
                layout.IsCBlock = true;
                break;
                
            case BlockShape::Flat:
                if (def->TypeId == "control.stop" || def->TypeId == "control.return") {
                    layout.IsCapBlock = true;
                    layout.HasBottomNotch = false;
                }
                break;
                
            default:
                break;
        }
        
        // Reporter blocks (value blocks)
        if (def->IsValueBlock) {
            layout.IsReporter = true;
            layout.HasTopNotch = false;
            layout.HasBottomNotch = false;
        }
        
        // Calculate base width from display name
        float textWidth = CalculateTextWidth(def->DisplayName);
        float iconWidth = def->Icon.empty() ? 0.0f : 24.0f;
        float baseWidth = iconWidth + textWidth + BLOCK_PADDING * 4;
        
        // Calculate input slots
        float slotsWidth = 0.0f;
        float currentX = baseWidth;
        
        for (size_t i = 0; i < block->GetInputSlotCount(); ++i)
        {
            const auto* slot = block->GetInputSlot(i);
            if (!slot) continue;
            
            SlotLayout slotLayout;
            slotLayout.Name = slot->GetName();
            slotLayout.Type = slot->GetValueType();
            slotLayout.IsConnected = slot->IsConnected();
            slotLayout.IsEmbedded = slot->IsConnected();
            
            float slotWidth = CalculateSlotWidth(slot);
            
            slotLayout.Position = ImVec2(currentX * zoom, BLOCK_PADDING * zoom);
            slotLayout.Size = ImVec2(slotWidth * zoom, SLOT_HEIGHT * zoom);
            
            layout.InputSlots.push_back(slotLayout);
            
            currentX += slotWidth + SLOT_SPACING;
            slotsWidth += slotWidth + SLOT_SPACING;
        }
        
        // Total width
        layout.TotalWidth = std::max(MIN_BLOCK_WIDTH, baseWidth + slotsWidth) * zoom;
        
        // Calculate header height
        layout.HeaderHeight = BLOCK_HEADER_HEIGHT * zoom;
        
        // Calculate nested areas for C-blocks
        if (layout.IsCBlock)
        {
            for (size_t i = 0; i < block->GetNestedSlotCount(); ++i)
            {
                const auto* slot = block->GetNestedSlot(i);
                if (!slot) continue;
                
                NestedAreaLayout nested;
                nested.Name = slot->GetName();
                nested.MinHeight = NESTED_MIN_HEIGHT * zoom;
                
                // Calculate height based on nested blocks
                float nestedHeight = CalculateNestedAreaHeight(slot, zoom);
                nestedHeight = std::max(nestedHeight, nested.MinHeight);
                
                nested.Position = ImVec2(C_INDENT * zoom, layout.HeaderHeight + layout.BodyHeight);
                nested.Size = ImVec2((layout.TotalWidth / zoom - C_INDENT - BLOCK_PADDING) * zoom, nestedHeight);
                
                // Copy nested block pointers for rendering
                for (const auto& nb : slot->GetNestedBlocks()) {
                    nested.NestedBlocks.push_back(nb.get());
                }
                
                layout.NestedAreas.push_back(nested);
                layout.BodyHeight += nestedHeight + BLOCK_PADDING * zoom;
            }
            
            // Footer for C-block
            layout.FooterHeight = 12.0f * zoom;
        }
        
        // Total height
        if (layout.IsHatBlock) {
            layout.TotalHeight = HAT_CURVE_HEIGHT * zoom + layout.HeaderHeight + layout.BodyHeight + layout.FooterHeight;
        } else {
            layout.TotalHeight = layout.HeaderHeight + layout.BodyHeight + layout.FooterHeight;
        }
        
        // Notch positions
        layout.TopNotchPos = ImVec2(NOTCH_OFFSET * zoom, 0.0f);
        layout.BottomNotchPos = ImVec2(NOTCH_OFFSET * zoom, layout.TotalHeight);
        
        return layout;
    }
    
    //=========================================================================
    // Path Generation - Flat Block
    //=========================================================================
    
    std::vector<ImVec2> BlockGeometry::GenerateFlatBlockPath(
        const ImVec2& pos,
        const BlockLayout& layout,
        float zoom)
    {
        std::vector<ImVec2> path;
        
        float r = CORNER_RADIUS * zoom;
        float nw = NOTCH_WIDTH * zoom;
        float nh = NOTCH_HEIGHT * zoom;
        float no = NOTCH_OFFSET * zoom;
        
        // Top edge with notch indent (if has top notch)
        path.push_back(ImVec2(pos.x, pos.y + r));
        path.push_back(ImVec2(pos.x + r, pos.y));
        
        if (layout.HasTopNotch) {
            path.push_back(ImVec2(pos.x + no, pos.y));
            path.push_back(ImVec2(pos.x + no + 2, pos.y + nh));
            path.push_back(ImVec2(pos.x + no + nw - 2, pos.y + nh));
            path.push_back(ImVec2(pos.x + no + nw, pos.y));
        }
        
        path.push_back(ImVec2(pos.x + layout.TotalWidth - r, pos.y));
        path.push_back(ImVec2(pos.x + layout.TotalWidth, pos.y + r));
        
        // Right edge
        path.push_back(ImVec2(pos.x + layout.TotalWidth, pos.y + layout.TotalHeight - r));
        path.push_back(ImVec2(pos.x + layout.TotalWidth - r, pos.y + layout.TotalHeight));
        
        // Bottom edge with notch bump (if has bottom notch)
        if (layout.HasBottomNotch) {
            path.push_back(ImVec2(pos.x + no + nw, pos.y + layout.TotalHeight));
            path.push_back(ImVec2(pos.x + no + nw - 2, pos.y + layout.TotalHeight + nh));
            path.push_back(ImVec2(pos.x + no + 2, pos.y + layout.TotalHeight + nh));
            path.push_back(ImVec2(pos.x + no, pos.y + layout.TotalHeight));
        }
        
        path.push_back(ImVec2(pos.x + r, pos.y + layout.TotalHeight));
        path.push_back(ImVec2(pos.x, pos.y + layout.TotalHeight - r));
        
        return path;
    }
    
    //=========================================================================
    // Path Generation - C Block
    //=========================================================================
    
    std::vector<ImVec2> BlockGeometry::GenerateCBlockPath(
        const ImVec2& pos,
        const BlockLayout& layout,
        float zoom)
    {
        std::vector<ImVec2> path;
        
        float r = CORNER_RADIUS * zoom;
        float nw = NOTCH_WIDTH * zoom;
        float nh = NOTCH_HEIGHT * zoom;
        float no = NOTCH_OFFSET * zoom;
        float ci = C_INDENT * zoom;
        
        float headerBottom = pos.y + layout.HeaderHeight;
        float bodyBottom = headerBottom + layout.BodyHeight;
        
        // Top left corner
        path.push_back(ImVec2(pos.x, pos.y + r));
        path.push_back(ImVec2(pos.x + r, pos.y));
        
        // Top notch
        if (layout.HasTopNotch) {
            path.push_back(ImVec2(pos.x + no, pos.y));
            path.push_back(ImVec2(pos.x + no + 2, pos.y + nh));
            path.push_back(ImVec2(pos.x + no + nw - 2, pos.y + nh));
            path.push_back(ImVec2(pos.x + no + nw, pos.y));
        }
        
        // Top right
        path.push_back(ImVec2(pos.x + layout.TotalWidth - r, pos.y));
        path.push_back(ImVec2(pos.x + layout.TotalWidth, pos.y + r));
        
        // Right edge to header bottom
        path.push_back(ImVec2(pos.x + layout.TotalWidth, headerBottom));
        
        // C-indent right side (inner right corner)
        path.push_back(ImVec2(pos.x + layout.TotalWidth - r, headerBottom));
        path.push_back(ImVec2(pos.x + ci + r, headerBottom));
        path.push_back(ImVec2(pos.x + ci, headerBottom + r));
        
        // Inner left edge
        path.push_back(ImVec2(pos.x + ci, bodyBottom - r));
        path.push_back(ImVec2(pos.x + ci + r, bodyBottom));
        
        // Inner notch (for nesting)
        path.push_back(ImVec2(pos.x + ci + no, bodyBottom));
        path.push_back(ImVec2(pos.x + ci + no + 2, bodyBottom + nh));
        path.push_back(ImVec2(pos.x + ci + no + nw - 2, bodyBottom + nh));
        path.push_back(ImVec2(pos.x + ci + no + nw, bodyBottom));
        
        // Resume outer right
        path.push_back(ImVec2(pos.x + layout.TotalWidth - r, bodyBottom));
        path.push_back(ImVec2(pos.x + layout.TotalWidth, bodyBottom + r));
        
        // Footer right edge
        path.push_back(ImVec2(pos.x + layout.TotalWidth, pos.y + layout.TotalHeight - r));
        path.push_back(ImVec2(pos.x + layout.TotalWidth - r, pos.y + layout.TotalHeight));
        
        // Bottom notch
        if (layout.HasBottomNotch) {
            path.push_back(ImVec2(pos.x + no + nw, pos.y + layout.TotalHeight));
            path.push_back(ImVec2(pos.x + no + nw - 2, pos.y + layout.TotalHeight + nh));
            path.push_back(ImVec2(pos.x + no + 2, pos.y + layout.TotalHeight + nh));
            path.push_back(ImVec2(pos.x + no, pos.y + layout.TotalHeight));
        }
        
        // Bottom left
        path.push_back(ImVec2(pos.x + r, pos.y + layout.TotalHeight));
        path.push_back(ImVec2(pos.x, pos.y + layout.TotalHeight - r));
        
        return path;
    }
    
    //=========================================================================
    // Path Generation - Hat Block (Events)
    //=========================================================================
    
    std::vector<ImVec2> BlockGeometry::GenerateHatBlockPath(
        const ImVec2& pos,
        const BlockLayout& layout,
        float zoom)
    {
        std::vector<ImVec2> path;
        
        float r = CORNER_RADIUS * zoom;
        float nw = NOTCH_WIDTH * zoom;
        float nh = NOTCH_HEIGHT * zoom;
        float no = NOTCH_OFFSET * zoom;
        float hc = HAT_CURVE_HEIGHT * zoom;
        
        // Hat curve (rounded top)
        float midX = pos.x + layout.TotalWidth / 2;
        path.push_back(ImVec2(pos.x, pos.y + hc));
        
        // Bezier-like curve for hat (approximated with points)
        for (float t = 0.0f; t <= 1.0f; t += 0.1f) {
            float x = pos.x + t * layout.TotalWidth;
            float y = pos.y + hc * (1.0f - 4.0f * (t - 0.5f) * (t - 0.5f));
            path.push_back(ImVec2(x, y));
        }
        
        path.push_back(ImVec2(pos.x + layout.TotalWidth, pos.y + hc));
        
        // Right edge
        path.push_back(ImVec2(pos.x + layout.TotalWidth, pos.y + layout.TotalHeight - r));
        path.push_back(ImVec2(pos.x + layout.TotalWidth - r, pos.y + layout.TotalHeight));
        
        // Bottom notch
        if (layout.HasBottomNotch) {
            path.push_back(ImVec2(pos.x + no + nw, pos.y + layout.TotalHeight));
            path.push_back(ImVec2(pos.x + no + nw - 2, pos.y + layout.TotalHeight + nh));
            path.push_back(ImVec2(pos.x + no + 2, pos.y + layout.TotalHeight + nh));
            path.push_back(ImVec2(pos.x + no, pos.y + layout.TotalHeight));
        }
        
        // Bottom left
        path.push_back(ImVec2(pos.x + r, pos.y + layout.TotalHeight));
        path.push_back(ImVec2(pos.x, pos.y + layout.TotalHeight - r));
        
        return path;
    }
    
    //=========================================================================
    // Path Generation - Reporter Block (Value blocks)
    //=========================================================================
    
    std::vector<ImVec2> BlockGeometry::GenerateReporterBlockPath(
        const ImVec2& pos,
        const BlockLayout& layout,
        float zoom)
    {
        std::vector<ImVec2> path;
        
        float r = REPORTER_ROUNDING * zoom;
        float h = layout.TotalHeight;
        float w = layout.TotalWidth;
        
        // Left rounded end
        path.push_back(ImVec2(pos.x + r, pos.y));
        path.push_back(ImVec2(pos.x, pos.y + h / 2));
        path.push_back(ImVec2(pos.x + r, pos.y + h));
        
        // Bottom edge
        path.push_back(ImVec2(pos.x + w - r, pos.y + h));
        
        // Right rounded end
        path.push_back(ImVec2(pos.x + w, pos.y + h / 2));
        path.push_back(ImVec2(pos.x + w - r, pos.y));
        
        return path;
    }
    
    //=========================================================================
    // Path Generation - Cap Block (Stop, Return)
    //=========================================================================
    
    std::vector<ImVec2> BlockGeometry::GenerateCapBlockPath(
        const ImVec2& pos,
        const BlockLayout& layout,
        float zoom)
    {
        std::vector<ImVec2> path;
        
        float r = CORNER_RADIUS * zoom;
        float nw = NOTCH_WIDTH * zoom;
        float nh = NOTCH_HEIGHT * zoom;
        float no = NOTCH_OFFSET * zoom;
        
        // Top left
        path.push_back(ImVec2(pos.x, pos.y + r));
        path.push_back(ImVec2(pos.x + r, pos.y));
        
        // Top notch
        path.push_back(ImVec2(pos.x + no, pos.y));
        path.push_back(ImVec2(pos.x + no + 2, pos.y + nh));
        path.push_back(ImVec2(pos.x + no + nw - 2, pos.y + nh));
        path.push_back(ImVec2(pos.x + no + nw, pos.y));
        
        // Top right
        path.push_back(ImVec2(pos.x + layout.TotalWidth - r, pos.y));
        path.push_back(ImVec2(pos.x + layout.TotalWidth, pos.y + r));
        
        // Right edge
        path.push_back(ImVec2(pos.x + layout.TotalWidth, pos.y + layout.TotalHeight - r));
        
        // Flat bottom (no notch) with slight taper for "cap" look
        float taperX = 5.0f * zoom;
        path.push_back(ImVec2(pos.x + layout.TotalWidth - taperX, pos.y + layout.TotalHeight));
        path.push_back(ImVec2(pos.x + taperX, pos.y + layout.TotalHeight));
        
        // Left edge
        path.push_back(ImVec2(pos.x, pos.y + layout.TotalHeight - r));
        
        return path;
    }
    
    //=========================================================================
    // Helper Functions
    //=========================================================================
    
    float BlockGeometry::CalculateTextWidth(const std::string& text)
    {
        // Approximate: 7 pixels per character (monospace)
        return static_cast<float>(text.length()) * 8.0f + 16.0f;
    }
    
    float BlockGeometry::CalculateSlotWidth(const BlockSlot* slot)
    {
        if (!slot) return 60.0f;
        
        // Base width for slot name
        float width = CalculateTextWidth(slot->GetName()) + 20.0f;
        
        // Adjust based on value type
        switch (slot->GetValueType())
        {
            case ValueType::Bool:
                width = std::max(width, 40.0f);
                break;
            case ValueType::Int:
            case ValueType::Float:
                width = std::max(width, 60.0f);
                break;
            case ValueType::String:
                width = std::max(width, 80.0f);
                break;
            case ValueType::Vector2:
                width = std::max(width, 100.0f);
                break;
            case ValueType::Vector3:
                width = std::max(width, 140.0f);
                break;
            case ValueType::Color:
                width = std::max(width, 80.0f);
                break;
            default:
                break;
        }
        
        return width;
    }
    
    float BlockGeometry::CalculateNestedAreaHeight(const BlockSlot* nestedSlot, float zoom)
    {
        if (!nestedSlot) return NESTED_MIN_HEIGHT * zoom;
        
        const auto& nestedBlocks = nestedSlot->GetNestedBlocks();
        if (nestedBlocks.empty()) {
            return NESTED_MIN_HEIGHT * zoom;
        }
        
        float totalHeight = 0.0f;
        for (const auto& block : nestedBlocks)
        {
            BlockLayout childLayout = CalculateLayout(block.get(), zoom);
            totalHeight += childLayout.TotalHeight + 4.0f * zoom;
        }
        
        return std::max(totalHeight, NESTED_MIN_HEIGHT * zoom);
    }
}
