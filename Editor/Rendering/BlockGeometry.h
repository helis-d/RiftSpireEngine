#pragma once

#include <Scripting/Core/Block.h>
#include <Scripting/Core/BlockTypes.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace RiftSpire
{
    //=========================================================================
    // SlotLayout - Layout info for a single slot
    //=========================================================================
    
    struct SlotLayout
    {
        std::string Name;
        ImVec2 Position;        // Relative to block
        ImVec2 Size;
        ValueType Type;
        bool IsConnected;
        bool IsEmbedded;        // True if slot has embedded reporter block
    };
    
    //=========================================================================
    // NestedAreaLayout - Layout for C-block nested body
    //=========================================================================
    
    struct NestedAreaLayout
    {
        std::string Name;
        ImVec2 Position;        // Relative to block
        ImVec2 Size;
        float MinHeight;        // Even when empty
        std::vector<Block*> NestedBlocks;
    };
    
    //=========================================================================
    // BlockLayout - Complete layout for a block
    //=========================================================================
    
    struct BlockLayout
    {
        // Dimensions
        float TotalWidth = 0.0f;
        float TotalHeight = 0.0f;
        
        // Section heights (for C-blocks)
        float HeaderHeight = 0.0f;
        float BodyHeight = 0.0f;
        float FooterHeight = 0.0f;
        
        // Slot layouts
        std::vector<SlotLayout> InputSlots;
        std::vector<NestedAreaLayout> NestedAreas;
        
        // Notch positions (relative to block)
        ImVec2 TopNotchPos;
        ImVec2 BottomNotchPos;
        
        // Shape-specific
        bool HasTopNotch = true;
        bool HasBottomNotch = true;
        bool IsCBlock = false;
        bool IsHatBlock = false;
        bool IsCapBlock = false;
        bool IsReporter = false;
    };
    
    //=========================================================================
    // BlockGeometry - Calculates block layouts and generates paths
    //=========================================================================
    
    class BlockGeometry
    {
    public:
        //---------------------------------------------------------------------
        // Layout calculation
        //---------------------------------------------------------------------
        
        static BlockLayout CalculateLayout(const Block* block, float zoom = 1.0f);
        
        //---------------------------------------------------------------------
        // Path generation for different shapes
        //---------------------------------------------------------------------
        
        static std::vector<ImVec2> GenerateFlatBlockPath(
            const ImVec2& pos,
            const BlockLayout& layout,
            float zoom
        );
        
        static std::vector<ImVec2> GenerateCBlockPath(
            const ImVec2& pos,
            const BlockLayout& layout,
            float zoom
        );
        
        static std::vector<ImVec2> GenerateHatBlockPath(
            const ImVec2& pos,
            const BlockLayout& layout,
            float zoom
        );
        
        static std::vector<ImVec2> GenerateReporterBlockPath(
            const ImVec2& pos,
            const BlockLayout& layout,
            float zoom
        );
        
        static std::vector<ImVec2> GenerateCapBlockPath(
            const ImVec2& pos,
            const BlockLayout& layout,
            float zoom
        );
        
        //---------------------------------------------------------------------
        // Geometry constants
        //---------------------------------------------------------------------
        
        static constexpr float MIN_BLOCK_WIDTH = 100.0f;
        static constexpr float BLOCK_HEADER_HEIGHT = 40.0f;
        static constexpr float BLOCK_PADDING = 8.0f;
        static constexpr float SLOT_HEIGHT = 24.0f;
        static constexpr float SLOT_SPACING = 4.0f;
        static constexpr float CORNER_RADIUS = 8.0f;
        static constexpr float NOTCH_WIDTH = 16.0f;
        static constexpr float NOTCH_HEIGHT = 6.0f;
        static constexpr float NOTCH_OFFSET = 20.0f;
        static constexpr float C_INDENT = 20.0f;
        static constexpr float NESTED_MIN_HEIGHT = 30.0f;
        static constexpr float HAT_CURVE_HEIGHT = 20.0f;
        static constexpr float REPORTER_ROUNDING = 12.0f;
        
    private:
        static float CalculateTextWidth(const std::string& text);
        static float CalculateSlotWidth(const BlockSlot* slot);
        static float CalculateNestedAreaHeight(const BlockSlot* nestedSlot, float zoom);
    };
}
