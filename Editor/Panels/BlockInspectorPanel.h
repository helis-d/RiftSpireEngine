#pragma once

#include <Scripting/Core/Block.h>
#include <Scripting/Core/BlockScript.h>

namespace RiftSpire
{
    //=========================================================================
    // BlockInspectorPanel - Selected block property editor
    //=========================================================================
    
    class BlockInspectorPanel
    {
    public:
        BlockInspectorPanel() = default;
        ~BlockInspectorPanel() = default;
        
        void OnImGuiRender();
        
        void SetSelectedBlock(Block* block, BlockScriptPtr script)
        {
            m_SelectedBlock = block;
            m_ActiveScript = script;
        }
        
    private:
        void RenderBlockProperties();
        void RenderInputSlots();
        void RenderBlockInfo();
        
    private:
        Block* m_SelectedBlock = nullptr;
        BlockScriptPtr m_ActiveScript;
    };
}
