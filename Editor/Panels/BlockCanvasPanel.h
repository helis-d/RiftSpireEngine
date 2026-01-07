#pragma once

#include "../Rendering/ScratchBlockRenderer.h"
#include <Scripting/Core/BlockScript.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_set>
#include <unordered_map>

struct ImVec2;
struct ImDrawList;
struct ImVec4;

namespace RiftSpire
{
    //=========================================================================
    // BlockCanvasPanel - Scratch-style visual block editor
    //=========================================================================
    
    class BlockCanvasPanel
    {
    public:
        BlockCanvasPanel() = default;
        ~BlockCanvasPanel() = default;
        
        void OnImGuiRender();
        
        void SetActiveScript(BlockScriptPtr script) { m_ActiveScript = script; }
        BlockScriptPtr GetActiveScript() const { return m_ActiveScript; }
        
        // Get selected block ID
        const UUID& GetSelectedBlock() const { return m_SelectedBlockId; }
        void SetSelectedBlock(const UUID& id) { m_SelectedBlockId = id; }
        
        // Multi-selection
        const std::unordered_set<UUID>& GetSelectedBlocks() const { return m_SelectedBlocks; }
        void ClearSelection() { m_SelectedBlocks.clear(); m_SelectedBlockId = UUID(); }
        
        // Spawn a new block at mouse position
        void SpawnBlock(const std::string& typeId, const glm::vec2& position);
        
        // Runtime integration
        void OnBlockExecuting(Block* block);
        void OnBlockExecuted(Block* block);
        void OnValueEvaluated(Block* block, const Value& value);
        void OnExecutionError(Block* block, const std::string& error);
        void ClearRuntimeState();
        
        // Options
        void SetShowRuntimeValues(bool show) { m_ShowRuntimeValues = show; }
        bool GetShowRuntimeValues() const { return m_ShowRuntimeValues; }
        
    private:
        void RenderCanvas();
        void RenderAllBlocks();
        void RenderConnections();
        void HandleInput();
        void HandleBlockDrop();
        void HandleBlockInteraction(Block* block, const ImVec2& screenPos, const BlockLayout& layout);
        
        // Connection drawing
        void DrawBezierConnection(
            ImDrawList* drawList,
            const ImVec2& from,
            const ImVec2& to,
            const ImVec4& color,
            float thickness
        );
        
    private:
        BlockScriptPtr m_ActiveScript;
        
        // Selection state
        UUID m_SelectedBlockId;
        std::unordered_set<UUID> m_SelectedBlocks;
        
        // Canvas state
        glm::vec2 m_CanvasOffset = {50.0f, 50.0f};
        float m_CanvasZoom = 1.0f;
        
        // Interaction state
        bool m_IsDraggingBlock = false;
        UUID m_DraggedBlockId;
        glm::vec2 m_DragStartPos;
        
        bool m_IsPanning = false;
        glm::vec2 m_PanStartPos;
        
        bool m_IsBoxSelecting = false;
        ImVec2 m_BoxSelectStart;
        
        // Runtime state
        std::unordered_set<UUID> m_ActiveBlocks;
        std::unordered_map<UUID, Value> m_RuntimeValues;
        std::unordered_map<UUID, std::string> m_Errors;
        
        // Options
        bool m_ShowRuntimeValues = true;
        bool m_DebugMode = false;
        
        // Renderer
        ScratchBlockRenderer m_Renderer;
        
        // Hovered block
        UUID m_HoveredBlockId;
        
        // Drawing constants
        static constexpr float GRID_SIZE = 64.0f;
    };
}
