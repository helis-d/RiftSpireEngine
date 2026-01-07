#pragma once

#include "BlockGeometry.h"
#include "../EditorTheme.h"
#include <Scripting/Core/Block.h>
#include <Scripting/Core/BlockTypes.h>
#include <imgui.h>
#include <unordered_set>
#include <unordered_map>

namespace RiftSpire
{
    //=========================================================================
    // RenderContext - Passed to all rendering functions
    //=========================================================================
    
    struct BlockRenderContext
    {
        ImDrawList* DrawList = nullptr;
        ImVec2 CanvasPos;
        ImVec2 CanvasOffset;
        float Zoom = 1.0f;
        
        // Selection/Interaction state
        std::unordered_set<UUID>* SelectedBlocks = nullptr;
        UUID* HoveredBlock = nullptr;
        UUID* DraggingBlock = nullptr;
        
        // Runtime state
        std::unordered_set<UUID>* ActiveBlocks = nullptr;
        std::unordered_map<UUID, Value>* RuntimeValues = nullptr;
        std::unordered_map<UUID, std::string>* Errors = nullptr;
        
        // Options
        bool ShowRuntimeValues = false;
        bool DebugMode = false;
    };
    
    //=========================================================================
    // ScratchBlockRenderer - Scratch-style block rendering engine
    //=========================================================================
    
    class ScratchBlockRenderer
    {
    public:
        ScratchBlockRenderer() = default;
        ~ScratchBlockRenderer() = default;
        
        //---------------------------------------------------------------------
        // Main rendering entry point
        //---------------------------------------------------------------------
        
        void RenderBlock(Block* block, const BlockRenderContext& ctx);
        
        //---------------------------------------------------------------------
        // Block type-specific renderers
        //---------------------------------------------------------------------
        
        void RenderFlatBlock(Block* block, const ImVec2& pos, const BlockLayout& layout, const BlockRenderContext& ctx);
        void RenderCBlock(Block* block, const ImVec2& pos, const BlockLayout& layout, const BlockRenderContext& ctx);
        void RenderHatBlock(Block* block, const ImVec2& pos, const BlockLayout& layout, const BlockRenderContext& ctx);
        void RenderReporterBlock(Block* block, const ImVec2& pos, const BlockLayout& layout, const BlockRenderContext& ctx);
        void RenderCapBlock(Block* block, const ImVec2& pos, const BlockLayout& layout, const BlockRenderContext& ctx);
        
        //---------------------------------------------------------------------
        // Inline field rendering
        //---------------------------------------------------------------------
        
        bool RenderInlineField(BlockSlot* slot, const ImVec2& pos, float width, const BlockRenderContext& ctx);
        bool RenderNumberInput(BlockSlot* slot, const ImVec2& pos, float width, const BlockRenderContext& ctx);
        bool RenderStringInput(BlockSlot* slot, const ImVec2& pos, float width, const BlockRenderContext& ctx);
        bool RenderBooleanToggle(BlockSlot* slot, const ImVec2& pos, const BlockRenderContext& ctx);
        bool RenderDropdown(BlockSlot* slot, const ImVec2& pos, float width, const BlockRenderContext& ctx);
        bool RenderColorPicker(BlockSlot* slot, const ImVec2& pos, const BlockRenderContext& ctx);
        bool RenderVectorInput(BlockSlot* slot, const ImVec2& pos, float width, const BlockRenderContext& ctx);
        
        //---------------------------------------------------------------------
        // Socket rendering  
        //---------------------------------------------------------------------
        
        void RenderExecutionSocket(const ImVec2& pos, bool isTop, bool isConnected, const BlockRenderContext& ctx);
        void RenderValueSocket(const ImVec2& pos, ValueType type, bool isInput, bool isConnected, const BlockRenderContext& ctx);
        
        //---------------------------------------------------------------------
        // Nested area rendering
        //---------------------------------------------------------------------
        
        void RenderNestedArea(const NestedAreaLayout& area, const ImVec4& color, const BlockRenderContext& ctx);
        void RenderNestedDropZone(const ImVec2& pos, const ImVec2& size, bool isActive, const BlockRenderContext& ctx);
        
        //---------------------------------------------------------------------
        // Visual effects
        //---------------------------------------------------------------------
        
        void RenderBlockShadow(const ImVec2& pos, const BlockLayout& layout, const BlockRenderContext& ctx);
        void RenderSelectionGlow(const ImVec2& pos, const BlockLayout& layout, const BlockRenderContext& ctx);
        void RenderExecutionHighlight(const ImVec2& pos, const BlockLayout& layout, const BlockRenderContext& ctx);
        void RenderErrorOverlay(const ImVec2& pos, const BlockLayout& layout, const std::string& error, const BlockRenderContext& ctx);
        void RenderRuntimeValue(const ImVec2& pos, const Value& value, const BlockRenderContext& ctx);
        
        //---------------------------------------------------------------------
        // Block path drawing
        //---------------------------------------------------------------------
        
        void DrawBlockPath(const std::vector<ImVec2>& path, const ImVec4& fillColor, const ImVec4& borderColor, float borderWidth, const BlockRenderContext& ctx);
        void DrawGradientBlock(const std::vector<ImVec2>& path, const ImVec4& topColor, const ImVec4& bottomColor, const BlockRenderContext& ctx);
        
        //---------------------------------------------------------------------
        // Color utilities
        //---------------------------------------------------------------------
        
        ImVec4 GetBlockColor(BlockCategory category);
        
    private:
        // Temporary state for inline editing
        struct InlineEditState
        {
            UUID BlockId;
            std::string SlotName;
            bool IsEditing = false;
            char Buffer[256] = {0};
        };
        
        InlineEditState m_EditState;
        
        // Unique ID generation for ImGui
        int m_UniqueIdCounter = 0;
        void PushUniqueId(Block* block, const std::string& slotName);
        void PopUniqueId();
    };
}
