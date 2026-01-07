#include "BlockCanvasPanel.h"
#include "../EditorTheme.h"
#include "../Rendering/BlockGeometry.h"
#include <Scripting/Core/BlockRegistry.h>
#include <Scripting/Core/BlockTypes.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <cmath>
#include <algorithm>

namespace RiftSpire
{
    void BlockCanvasPanel::OnImGuiRender()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Block Canvas", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::PopStyleVar();
        
        if (!m_ActiveScript)
        {
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImVec2 center(windowSize.x / 2.0f, windowSize.y / 2.0f);
            
            ImGui::SetCursorPos(ImVec2(center.x - 140, center.y - 30));
            ImGui::PushStyleColor(ImGuiCol_Text, EditorColors::TextDim);
            ImGui::TextWrapped("No script loaded.\nDrag blocks from the palette to begin.");
            ImGui::PopStyleColor();
            ImGui::End();
            return;
        }
        
        RenderCanvas();
        
        ImGui::End();
    }
    
    void BlockCanvasPanel::RenderCanvas()
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
        ImVec2 canvas_size = ImGui::GetContentRegionAvail();
        
        // Draw gradient background
        ImU32 bgColorTop = IM_COL32(20, 22, 35, 255);
        ImU32 bgColorBottom = IM_COL32(30, 32, 50, 255);
        draw_list->AddRectFilledMultiColor(
            canvas_pos,
            ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
            bgColorTop, bgColorTop,
            bgColorBottom, bgColorBottom
        );
        
        // Draw grid
        const float GRID_STEP = GRID_SIZE * m_CanvasZoom;
        ImU32 gridColor = IM_COL32(60, 65, 90, 40);
        ImU32 gridColorMajor = IM_COL32(80, 85, 110, 60);
        
        int gridLineX = 0;
        for (float x = fmodf(m_CanvasOffset.x, GRID_STEP); x < canvas_size.x; x += GRID_STEP, gridLineX++)
        {
            ImU32 color = (gridLineX % 4 == 0) ? gridColorMajor : gridColor;
            draw_list->AddLine(
                ImVec2(canvas_pos.x + x, canvas_pos.y),
                ImVec2(canvas_pos.x + x, canvas_pos.y + canvas_size.y),
                color
            );
        }
        
        int gridLineY = 0;
        for (float y = fmodf(m_CanvasOffset.y, GRID_STEP); y < canvas_size.y; y += GRID_STEP, gridLineY++)
        {
            ImU32 color = (gridLineY % 4 == 0) ? gridColorMajor : gridColor;
            draw_list->AddLine(
                ImVec2(canvas_pos.x, canvas_pos.y + y),
                ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + y),
                color
            );
        }
        
        // Interactive canvas area
        ImGui::InvisibleButton("canvas", canvas_size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonMiddle);
        
        // Handle drag-drop from palette
        HandleBlockDrop();
        
        // Handle panning
        bool panning = ImGui::IsItemActive() && (ImGui::IsMouseDragging(ImGuiMouseButton_Middle) ||
            (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && ImGui::GetIO().KeyAlt));
        
        if (panning)
        {
            ImGuiMouseButton button = ImGui::IsMouseDragging(ImGuiMouseButton_Middle) ? 
                ImGuiMouseButton_Middle : ImGuiMouseButton_Left;
            ImVec2 delta = ImGui::GetMouseDragDelta(button);
            m_CanvasOffset.x += delta.x;
            m_CanvasOffset.y += delta.y;
            ImGui::ResetMouseDragDelta(button);
        }
        
        // Handle zoom
        if (ImGui::IsWindowHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                float prevZoom = m_CanvasZoom;
                m_CanvasZoom += wheel * 0.1f;
                m_CanvasZoom = std::clamp(m_CanvasZoom, 0.3f, 2.0f);
                
                // Zoom toward mouse position
                if (m_CanvasZoom != prevZoom) {
                    ImVec2 mousePos = ImGui::GetMousePos();
                    ImVec2 relMouse(mousePos.x - canvas_pos.x, mousePos.y - canvas_pos.y);
                    float zoomFactor = m_CanvasZoom / prevZoom;
                    m_CanvasOffset.x = relMouse.x - (relMouse.x - m_CanvasOffset.x) * zoomFactor;
                    m_CanvasOffset.y = relMouse.y - (relMouse.y - m_CanvasOffset.y) * zoomFactor;
                }
            }
        }
        
        // Render connections first (behind blocks)
        RenderConnections();
        
        // Render all blocks using new renderer
        RenderAllBlocks();
        
        // Handle keyboard input
        HandleInput();
        
        // Draw zoom indicator
        char zoomText[32];
        snprintf(zoomText, sizeof(zoomText), "%.0f%%", m_CanvasZoom * 100.0f);
        ImVec2 zoomPos(canvas_pos.x + canvas_size.x - 60, canvas_pos.y + canvas_size.y - 25);
        draw_list->AddText(zoomPos, IM_COL32(150, 150, 160, 180), zoomText);
    }
    
    void BlockCanvasPanel::RenderAllBlocks()
    {
        if (!m_ActiveScript) return;
        
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
        
        // Setup render context
        BlockRenderContext ctx;
        ctx.DrawList = draw_list;
        ctx.CanvasPos = canvas_pos;
        ctx.CanvasOffset = ImVec2(m_CanvasOffset.x, m_CanvasOffset.y);
        ctx.Zoom = m_CanvasZoom;
        ctx.SelectedBlocks = &m_SelectedBlocks;
        ctx.HoveredBlock = &m_HoveredBlockId;
        ctx.DraggingBlock = m_IsDraggingBlock ? &m_DraggedBlockId : nullptr;
        ctx.ActiveBlocks = &m_ActiveBlocks;
        ctx.RuntimeValues = &m_RuntimeValues;
        ctx.Errors = &m_Errors;
        ctx.ShowRuntimeValues = m_ShowRuntimeValues;
        ctx.DebugMode = m_DebugMode;
        
        // Clear hovered state
        m_HoveredBlockId = UUID();
        
        // Render all blocks
        for (const auto& block : m_ActiveScript->GetBlocks())
        {
            if (!block) continue;
            
            // Render block
            m_Renderer.RenderBlock(block.get(), ctx);
            
            // Handle interaction
            glm::vec2 blockPos = block->GetPosition();
            ImVec2 screenPos(
                ctx.CanvasPos.x + ctx.CanvasOffset.x + blockPos.x * ctx.Zoom,
                ctx.CanvasPos.y + ctx.CanvasOffset.y + blockPos.y * ctx.Zoom
            );
            BlockLayout layout = BlockGeometry::CalculateLayout(block.get(), ctx.Zoom);
            
            HandleBlockInteraction(block.get(), screenPos, layout);
        }
    }
    
    void BlockCanvasPanel::HandleBlockInteraction(Block* block, const ImVec2& screenPos, const BlockLayout& layout)
    {
        if (!block) return;
        
        ImVec2 mousePos = ImGui::GetMousePos();
        bool isHovered = (mousePos.x >= screenPos.x && mousePos.x <= screenPos.x + layout.TotalWidth &&
                          mousePos.y >= screenPos.y && mousePos.y <= screenPos.y + layout.TotalHeight);
        
        if (isHovered) {
            m_HoveredBlockId = block->GetId();
        }
        
        // Handle selection
        if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::GetIO().KeyAlt)
        {
            if (ImGui::GetIO().KeyCtrl) {
                // Toggle selection
                if (m_SelectedBlocks.contains(block->GetId())) {
                    m_SelectedBlocks.erase(block->GetId());
                } else {
                    m_SelectedBlocks.insert(block->GetId());
                }
            } else {
                // Single selection
                m_SelectedBlocks.clear();
                m_SelectedBlocks.insert(block->GetId());
            }
            m_SelectedBlockId = block->GetId();
        }
        
        // Handle dragging
        if (isHovered && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !ImGui::GetIO().KeyAlt)
        {
            if (!m_IsDraggingBlock)
            {
                m_IsDraggingBlock = true;
                m_DraggedBlockId = block->GetId();
                m_DragStartPos = block->GetPosition();
            }
        }
        
        // Update position while dragging
        if (m_IsDraggingBlock && m_DraggedBlockId == block->GetId())
        {
            ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            glm::vec2 newPos = m_DragStartPos + glm::vec2(drag_delta.x, drag_delta.y) / m_CanvasZoom;
            block->SetPosition(newPos);
            
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            {
                m_IsDraggingBlock = false;
            }
        }
        
        // Handle collapse toggle for C-blocks
        if (layout.IsCBlock && isHovered)
        {
            float collapseX = screenPos.x + layout.TotalWidth - 24.0f * m_CanvasZoom;
            float collapseY = screenPos.y + 4.0f * m_CanvasZoom;
            float collapseSize = 20.0f * m_CanvasZoom;
            
            if (mousePos.x >= collapseX && mousePos.x <= collapseX + collapseSize &&
                mousePos.y >= collapseY && mousePos.y <= collapseY + collapseSize)
            {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    block->SetCollapsed(!block->IsCollapsed());
                }
            }
        }
    }
    
    void BlockCanvasPanel::RenderConnections()
    {
        if (!m_ActiveScript) return;
        
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
        
        for (const auto& block : m_ActiveScript->GetBlocks())
        {
            if (!block) continue;
            
            // Draw connection to next block
            if (auto nextBlock = block->GetNextBlock())
            {
                auto* def = block->GetDefinition();
                ImVec4 blockColor = def ? m_Renderer.GetBlockColor(def->Category) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
                
                BlockLayout layout = BlockGeometry::CalculateLayout(block.get(), m_CanvasZoom);
                
                // From bottom notch center of current block
                glm::vec2 fromPos = block->GetPosition();
                fromPos.x += BlockGeometry::NOTCH_OFFSET + BlockGeometry::NOTCH_WIDTH / 2.0f;
                fromPos.y += layout.TotalHeight / m_CanvasZoom + BlockGeometry::NOTCH_HEIGHT / 2.0f;
                
                // To top notch center of next block  
                glm::vec2 toPos = nextBlock->GetPosition();
                toPos.x += BlockGeometry::NOTCH_OFFSET + BlockGeometry::NOTCH_WIDTH / 2.0f;
                
                glm::vec2 fromScreen = glm::vec2(canvas_pos.x, canvas_pos.y) + m_CanvasOffset + fromPos * m_CanvasZoom;
                glm::vec2 toScreen = glm::vec2(canvas_pos.x, canvas_pos.y) + m_CanvasOffset + toPos * m_CanvasZoom;
                
                DrawBezierConnection(
                    draw_list,
                    ImVec2(fromScreen.x, fromScreen.y),
                    ImVec2(toScreen.x, toScreen.y),
                    DarkenColor(blockColor, 0.2f),
                    3.0f * m_CanvasZoom
                );
            }
        }
    }
    
    void BlockCanvasPanel::DrawBezierConnection(
        ImDrawList* drawList,
        const ImVec2& from,
        const ImVec2& to,
        const ImVec4& color,
        float thickness)
    {
        float distance = std::abs(to.y - from.y);
        float cpOffset = std::min(distance * 0.5f, 50.0f);
        
        ImVec2 cp1(from.x, from.y + cpOffset);
        ImVec2 cp2(to.x, to.y - cpOffset);
        
        // Shadow
        drawList->AddBezierCubic(
            ImVec2(from.x + 2, from.y + 2),
            ImVec2(cp1.x + 2, cp1.y + 2),
            ImVec2(cp2.x + 2, cp2.y + 2),
            ImVec2(to.x + 2, to.y + 2),
            IM_COL32(0, 0, 0, 40),
            thickness + 2.0f
        );
        
        // Main curve
        drawList->AddBezierCubic(from, cp1, cp2, to, ColorToU32(color), thickness);
    }
    
    void BlockCanvasPanel::HandleBlockDrop()
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("BLOCK_TYPE"))
            {
                const char* typeId = static_cast<const char*>(payload->Data);
                
                ImVec2 mouse_pos = ImGui::GetMousePos();
                ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
                
                glm::vec2 spawnPos = (glm::vec2(mouse_pos.x - canvas_pos.x, mouse_pos.y - canvas_pos.y) - m_CanvasOffset) / m_CanvasZoom;
                
                SpawnBlock(typeId, spawnPos);
            }
            ImGui::EndDragDropTarget();
        }
    }
    
    void BlockCanvasPanel::SpawnBlock(const std::string& typeId, const glm::vec2& position)
    {
        if (!m_ActiveScript) return;
        
        auto& registry = BlockRegistry::Get();
        auto newBlock = registry.CreateBlock(typeId);
        
        if (newBlock)
        {
            newBlock->SetPosition(position);
            m_ActiveScript->AddBlock(newBlock);
            
            // Select the new block
            m_SelectedBlocks.clear();
            m_SelectedBlocks.insert(newBlock->GetId());
            m_SelectedBlockId = newBlock->GetId();
        }
    }
    
    void BlockCanvasPanel::HandleInput()
    {
        if (!ImGui::IsWindowFocused()) return;
        
        // Delete selected blocks
        if (ImGui::IsKeyPressed(ImGuiKey_Delete) && !m_SelectedBlocks.empty())
        {
            for (const auto& blockId : m_SelectedBlocks) {
                m_ActiveScript->RemoveBlock(blockId);
            }
            m_SelectedBlocks.clear();
            m_SelectedBlockId = UUID();
        }
        
        // Deselect with Escape
        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            ClearSelection();
        }
        
        // Reset zoom with Ctrl+0
        if (ImGui::IsKeyPressed(ImGuiKey_0) && ImGui::GetIO().KeyCtrl)
        {
            m_CanvasZoom = 1.0f;
        }
        
        // Select all with Ctrl+A
        if (ImGui::IsKeyPressed(ImGuiKey_A) && ImGui::GetIO().KeyCtrl)
        {
            m_SelectedBlocks.clear();
            for (const auto& block : m_ActiveScript->GetBlocks()) {
                m_SelectedBlocks.insert(block->GetId());
            }
        }
    }
    
    //=========================================================================
    // Runtime Integration
    //=========================================================================
    
    void BlockCanvasPanel::OnBlockExecuting(Block* block)
    {
        if (block) {
            m_ActiveBlocks.insert(block->GetId());
        }
    }
    
    void BlockCanvasPanel::OnBlockExecuted(Block* block)
    {
        if (block) {
            m_ActiveBlocks.erase(block->GetId());
        }
    }
    
    void BlockCanvasPanel::OnValueEvaluated(Block* block, const Value& value)
    {
        if (block) {
            m_RuntimeValues[block->GetId()] = value;
        }
    }
    
    void BlockCanvasPanel::OnExecutionError(Block* block, const std::string& error)
    {
        if (block) {
            m_Errors[block->GetId()] = error;
        }
    }
    
    void BlockCanvasPanel::ClearRuntimeState()
    {
        m_ActiveBlocks.clear();
        m_RuntimeValues.clear();
        m_Errors.clear();
    }
}
