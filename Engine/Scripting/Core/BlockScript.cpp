#include "BlockScript.h"
#include <algorithm>
#include <unordered_set>

namespace RiftSpire
{
    BlockScript::BlockScript()
        : m_Id(UUID::Generate())
    {
    }
    
    BlockScript::BlockScript(const std::string& name)
        : m_Id(UUID::Generate())
        , m_Name(name)
    {
    }
    
    //=========================================================================
    // Block Management
    //=========================================================================
    
    void BlockScript::AddBlock(BlockPtr block)
    {
        if (!block) return;
        
        // Check if already added
        if (m_BlockMap.find(block->GetId()) != m_BlockMap.end())
        {
            return;
        }
        
        m_Blocks.push_back(block);
        m_BlockMap[block->GetId()] = block;
        IncrementVersion();
    }
    
    void BlockScript::RemoveBlock(BlockPtr block)
    {
        if (!block) return;
        RemoveBlock(block->GetId());
    }
    
    void BlockScript::RemoveBlock(const UUID& blockId)
    {
        auto it = m_BlockMap.find(blockId);
        if (it == m_BlockMap.end()) return;
        
        BlockPtr block = it->second;
        
        // Disconnect from chain
        if (auto prev = block->GetPreviousBlock())
        {
            prev->SetNextBlock(block->GetNextBlock());
        }
        
        // Remove from collections
        m_BlockMap.erase(it);
        m_Blocks.erase(
            std::remove(m_Blocks.begin(), m_Blocks.end(), block),
            m_Blocks.end()
        );
        
        IncrementVersion();
    }
    
    BlockPtr BlockScript::GetBlock(const UUID& blockId) const
    {
        auto it = m_BlockMap.find(blockId);
        if (it != m_BlockMap.end())
        {
            return it->second;
        }
        return nullptr;
    }
    
    std::vector<BlockPtr> BlockScript::GetEventBlocks() const
    {
        std::vector<BlockPtr> events;
        
        for (const auto& block : m_Blocks)
        {
            if (block->GetShape() == BlockShape::EventNested)
            {
                events.push_back(block);
            }
        }
        
        return events;
    }
    
    std::vector<BlockPtr> BlockScript::GetEventBlocks(const std::string& eventType) const
    {
        std::vector<BlockPtr> events;
        
        for (const auto& block : m_Blocks)
        {
            if (block->GetShape() == BlockShape::EventNested &&
                block->GetTypeId().find(eventType) != std::string::npos)
            {
                events.push_back(block);
            }
        }
        
        return events;
    }
    
    std::vector<BlockPtr> BlockScript::GetRootBlocks() const
    {
        std::vector<BlockPtr> roots;
        
        for (const auto& block : m_Blocks)
        {
            if (!block->GetPreviousBlock())
            {
                roots.push_back(block);
            }
        }
        
        return roots;
    }
    
    //=========================================================================
    // Block Lookup
    //=========================================================================
    
    BlockPtr BlockScript::FindBlockAt(const glm::vec2& position, f32 tolerance) const
    {
        // TODO: Use spatial partitioning for large scripts
        for (const auto& block : m_Blocks)
        {
            glm::vec2 blockPos = block->GetPosition();
            // Approximate block size (should be calculated based on block shape)
            const f32 blockWidth = 200.0f;
            const f32 blockHeight = 40.0f;
            
            if (position.x >= blockPos.x - tolerance &&
                position.x <= blockPos.x + blockWidth + tolerance &&
                position.y >= blockPos.y - tolerance &&
                position.y <= blockPos.y + blockHeight + tolerance)
            {
                return block;
            }
        }
        
        return nullptr;
    }
    
    std::vector<BlockPtr> BlockScript::FindBlocksInArea(const glm::vec2& min, const glm::vec2& max) const
    {
        std::vector<BlockPtr> result;
        
        for (const auto& block : m_Blocks)
        {
            glm::vec2 pos = block->GetPosition();
            if (pos.x >= min.x && pos.x <= max.x &&
                pos.y >= min.y && pos.y <= max.y)
            {
                result.push_back(block);
            }
        }
        
        return result;
    }
    
    //=========================================================================
    // Statistics
    //=========================================================================
    
    size_t BlockScript::GetMaxNestedDepth() const
    {
        size_t maxDepth = 0;
        
        for (const auto& block : m_Blocks)
        {
            size_t depth = CalculateNestedDepth(block.get());
            maxDepth = std::max(maxDepth, depth);
        }
        
        return maxDepth;
    }
    
    size_t BlockScript::CalculateNestedDepth(const Block* block, size_t currentDepth) const
    {
        if (!block) return currentDepth;
        
        size_t maxChildDepth = currentDepth;
        
        for (size_t i = 0; i < block->GetNestedSlotCount(); ++i)
        {
            const BlockSlot* slot = block->GetNestedSlot(i);
            if (slot)
            {
                for (const auto& nested : slot->GetNestedBlocks())
                {
                    size_t childDepth = CalculateNestedDepth(nested.get(), currentDepth + 1);
                    maxChildDepth = std::max(maxChildDepth, childDepth);
                }
            }
        }
        
        return maxChildDepth;
    }
    
    //=========================================================================
    // Validation
    //=========================================================================
    
    std::vector<BlockScript::ValidationError> BlockScript::Validate() const
    {
        std::vector<ValidationError> errors;
        
        // Check for too many blocks
        if (m_Blocks.size() > 500)
        {
            errors.push_back({
                UUID(),
                "Script contains " + std::to_string(m_Blocks.size()) + " blocks. Consider optimizing for performance.",
                ValidationError::Severity::Warning
            });
        }
        
        // Check nested depth
        size_t maxDepth = GetMaxNestedDepth();
        if (maxDepth > 10)
        {
            errors.push_back({
                UUID(),
                "Maximum nested depth (" + std::to_string(maxDepth) + ") exceeds recommended limit of 10.",
                ValidationError::Severity::Warning
            });
        }
        
        // Check for disconnected blocks (not reachable from any event)
        auto eventBlocks = GetEventBlocks();
        std::unordered_set<UUID> reachable;
        
        std::function<void(BlockPtr)> markReachable = [&](BlockPtr block) {
            if (!block || reachable.count(block->GetId())) return;
            reachable.insert(block->GetId());
            
            // Next block in chain
            markReachable(block->GetNextBlock());
            
            // Nested blocks
            for (size_t i = 0; i < block->GetNestedSlotCount(); ++i)
            {
                if (auto* slot = block->GetNestedSlot(i))
                {
                    for (const auto& nested : slot->GetNestedBlocks())
                    {
                        markReachable(nested);
                    }
                }
            }
            
            // Connected value blocks
            for (size_t i = 0; i < block->GetInputSlotCount(); ++i)
            {
                if (auto* slot = block->GetInputSlot(i))
                {
                    markReachable(slot->GetConnectedBlock());
                }
            }
        };
        
        for (const auto& event : eventBlocks)
        {
            markReachable(event);
        }
        
        // Report unreachable blocks
        for (const auto& block : m_Blocks)
        {
            if (!reachable.count(block->GetId()) && block->GetShape() != BlockShape::EventNested)
            {
                // Only warn for non-event blocks that are unreachable
                if (!block->GetPreviousBlock())
                {
                    errors.push_back({
                        block->GetId(),
                        "Block '" + block->GetDisplayName() + "' is not connected to any event.",
                        ValidationError::Severity::Warning
                    });
                }
            }
        }
        
        return errors;
    }
    
    bool BlockScript::IsValid() const
    {
        auto errors = Validate();
        return std::none_of(errors.begin(), errors.end(), [](const ValidationError& e) {
            return e.Level == ValidationError::Severity::Error;
        });
    }
    
    //=========================================================================
    // Snapshot (Undo/Redo)
    //=========================================================================
    
    BlockScript::Snapshot BlockScript::CreateSnapshot() const
    {
        Snapshot snapshot;
        snapshot.Version = m_Version;
        
        for (const auto& block : m_Blocks)
        {
            snapshot.Blocks.push_back(block->Clone());
        }
        
        return snapshot;
    }
    
    void BlockScript::RestoreSnapshot(const Snapshot& snapshot)
    {
        Clear();
        
        for (const auto& block : snapshot.Blocks)
        {
            AddBlock(block->Clone());
        }
        
        m_Version = snapshot.Version;
    }
    
    //=========================================================================
    // Clipboard
    //=========================================================================
    
    std::vector<BlockPtr> BlockScript::CopyBlocks(const std::vector<UUID>& blockIds) const
    {
        std::vector<BlockPtr> copies;
        
        for (const auto& id : blockIds)
        {
            if (auto block = GetBlock(id))
            {
                copies.push_back(block->Clone());
            }
        }
        
        return copies;
    }
    
    void BlockScript::PasteBlocks(const std::vector<BlockPtr>& blocks, const glm::vec2& offset)
    {
        for (const auto& block : blocks)
        {
            auto clone = block->Clone();
            clone->SetPosition(block->GetPosition() + offset);
            AddBlock(clone);
        }
    }
    
    //=========================================================================
    // Clear
    //=========================================================================
    
    void BlockScript::Clear()
    {
        m_Blocks.clear();
        m_BlockMap.clear();
        IncrementVersion();
    }
}
