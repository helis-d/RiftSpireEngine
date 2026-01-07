#pragma once

#include "Core/Block.h"
#include "Core/BlockTypes.h"
#include "Core/Value.h"
#include <Core/UUID.h> // For unordered_set hash support
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace RiftSpire
{
    //=========================================================================
    // BlockScript - Container for a complete visual script
    //=========================================================================
    
    class BlockScript
    {
    public:
        BlockScript();
        explicit BlockScript(const std::string& name);
        ~BlockScript() = default;
        
        //---------------------------------------------------------------------
        // Identity
        //---------------------------------------------------------------------
        
        UUID GetId() const { return m_Id; }
        
        const std::string& GetName() const { return m_Name; }
        void SetName(const std::string& name) { m_Name = name; }
        
        const std::string& GetDescription() const { return m_Description; }
        void SetDescription(const std::string& desc) { m_Description = desc; }
        
        //---------------------------------------------------------------------
        // Block management
        //---------------------------------------------------------------------
        
        /// Add a block to the script (takes ownership)
        void AddBlock(BlockPtr block);
        
        /// Remove a block from the script
        void RemoveBlock(BlockPtr block);
        void RemoveBlock(const UUID& blockId);
        
        /// Get block by ID
        BlockPtr GetBlock(const UUID& blockId) const;
        
        /// Get all blocks
        const std::vector<BlockPtr>& GetBlocks() const { return m_Blocks; }
        
        /// Get event handler blocks
        std::vector<BlockPtr> GetEventBlocks() const;
        std::vector<BlockPtr> GetEventBlocks(const std::string& eventType) const;
        
        /// Get root blocks (blocks without previous connection)
        std::vector<BlockPtr> GetRootBlocks() const;
        
        //---------------------------------------------------------------------
        // Block lookup
        //---------------------------------------------------------------------
        
        BlockPtr FindBlockAt(const glm::vec2& position, f32 tolerance = 10.0f) const;
        std::vector<BlockPtr> FindBlocksInArea(const glm::vec2& min, const glm::vec2& max) const;
        
        //---------------------------------------------------------------------
        // Statistics
        //---------------------------------------------------------------------
        
        size_t GetBlockCount() const { return m_Blocks.size(); }
        size_t GetMaxNestedDepth() const;
        bool HasPerformanceWarning() const { return m_Blocks.size() > 500; }
        
        //---------------------------------------------------------------------
        // Validation
        //---------------------------------------------------------------------
        
        struct ValidationError
        {
            UUID BlockId;
            std::string Message;
            enum class Severity { Warning, Error } Level;
        };
        
        std::vector<ValidationError> Validate() const;
        bool IsValid() const;
        
        //---------------------------------------------------------------------
        // Versioning
        //---------------------------------------------------------------------
        
        u32 GetVersion() const { return m_Version; }
        void IncrementVersion() { m_Version++; }
        
        //---------------------------------------------------------------------
        // Undo/Redo support
        //---------------------------------------------------------------------
        
        struct Snapshot
        {
            std::vector<BlockPtr> Blocks;
            u32 Version;
        };
        
        Snapshot CreateSnapshot() const;
        void RestoreSnapshot(const Snapshot& snapshot);
        
        //---------------------------------------------------------------------
        // Clipboard
        //---------------------------------------------------------------------
        
        std::vector<BlockPtr> CopyBlocks(const std::vector<UUID>& blockIds) const;
        void PasteBlocks(const std::vector<BlockPtr>& blocks, const glm::vec2& offset);
        
        //---------------------------------------------------------------------
        // Clear
        //---------------------------------------------------------------------
        
        void Clear();
        
    private:
        UUID m_Id;
        std::string m_Name = "Untitled Script";
        std::string m_Description;
        
        std::vector<BlockPtr> m_Blocks;
        std::unordered_map<UUID, BlockPtr> m_BlockMap;  // Fast lookup by ID
        
        u32 m_Version = 1;
        
        size_t CalculateNestedDepth(const Block* block, size_t currentDepth = 0) const;
    };
    
    using BlockScriptPtr = std::shared_ptr<BlockScript>;
}
