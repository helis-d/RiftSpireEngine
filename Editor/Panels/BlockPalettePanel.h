#pragma once

#include <string>
#include <vector>

namespace RiftSpire
{
    // Forward declarations
    class BlockScript;
    struct BlockDefinition;
    
    //=========================================================================
    // BlockPalettePanel - Category-based block browser
    //=========================================================================
    
    class BlockPalettePanel
    {
    public:
        BlockPalettePanel() = default;
        ~BlockPalettePanel() = default;
        
        void OnImGuiRender();
        
        // Get the block type that user wants to spawn (via drag-drop)
        const std::string& GetDraggedBlockType() const { return m_DraggedBlockType; }
        bool IsDragging() const { return m_IsDragging; }
        void ClearDrag() { m_IsDragging = false; m_DraggedBlockType.clear(); }
        
    private:
        void RenderCategoryTree();
        void RenderBlockList(int category, int categoryIndex);
        void RenderBlockItem(const BlockDefinition* def, int uniqueId);
        
    private:
        std::string m_SearchFilter;
        int m_SelectedCategory = -1; // -1 = All
        
        // Drag state
        bool m_IsDragging = false;
        std::string m_DraggedBlockType;
    };
}
