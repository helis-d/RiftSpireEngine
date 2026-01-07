#pragma once

#include "Value.h"
#include "BlockTypes.h"
#include <Core/UUID.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace RiftSpire
{
    // Forward declarations
    class Block;
    class BlockSlot;
    class ExecutionContext;
    
    using BlockPtr = std::shared_ptr<Block>;
    using BlockWeakPtr = std::weak_ptr<Block>;
    
    //=========================================================================
    // BlockSlot - Input/Output connection points on blocks
    //=========================================================================
    
    class BlockSlot
    {
    public:
        BlockSlot() = default;
        BlockSlot(const std::string& name, SlotType type, ValueType valueType = ValueType::Any);
        
        // Properties
        const std::string& GetName() const { return m_Name; }
        SlotType GetSlotType() const { return m_SlotType; }
        ValueType GetValueType() const { return m_ValueType; }
        
        // Connection management
        bool IsConnected() const;
        void Connect(BlockPtr block);
        void Disconnect();
        BlockPtr GetConnectedBlock() const;
        
        // For value slots - inline/default value
        void SetDefaultValue(const Value& value) { m_DefaultValue = value; }
        const Value& GetDefaultValue() const { return m_DefaultValue; }
        
        // For nested body slots - contained blocks
        void AddNestedBlock(BlockPtr block);
        void RemoveNestedBlock(BlockPtr block);
        void ClearNestedBlocks();
        const std::vector<BlockPtr>& GetNestedBlocks() const { return m_NestedBlocks; }
        
        // Validation
        bool CanAccept(const Block* block) const;
        bool CanAcceptType(ValueType type) const;
        
    private:
        std::string m_Name;
        SlotType m_SlotType = SlotType::None;
        ValueType m_ValueType = ValueType::Any;
        
        BlockWeakPtr m_ConnectedBlock;          // For value input connections
        std::vector<BlockPtr> m_NestedBlocks;   // For nested body slots
        Value m_DefaultValue;                   // Default/inline value
    };
    
    //=========================================================================
    // BlockDefinition - Static definition of a block type
    //=========================================================================
    
    struct BlockDefinition
    {
        std::string TypeId;                     // Unique identifier (e.g., "operators.add")
        std::string DisplayName;                // User-facing name (e.g., "Add")
        std::string Description;                // Tooltip description
        std::string Icon;                       // Icon/emoji for display
        
        BlockShape Shape = BlockShape::Flat;
        BlockCategory Category = BlockCategory::DebugLogging;
        NetworkAuthority Authority = NetworkAuthority::Local;
        
        bool ChangesState = false;              // If true, forces Server authority
        bool IsValueBlock = false;              // Returns a value (can be connected to value slots)
        ValueType ReturnType = ValueType::Void; // For value blocks
        
        std::vector<BlockSlot> InputSlots;      // Value input slots
        std::vector<BlockSlot> NestedSlots;     // Nested body slots (for control flow)
        
        // Execution function pointer
        using ExecuteFunc = std::function<Value(Block*, ExecutionContext&)>;
        ExecuteFunc Execute = nullptr;
    };
    
    //=========================================================================
    // Block - Instance of a block in a script
    //=========================================================================
    
    class Block : public std::enable_shared_from_this<Block>
    {
    public:
        Block() = default;
        explicit Block(const BlockDefinition* definition);
        ~Block() = default;
        
        // Prevent copying
        Block(const Block&) = delete;
        Block& operator=(const Block&) = delete;
        
        // Allow moving
        Block(Block&&) = default;
        Block& operator=(Block&&) = default;
        
        //---------------------------------------------------------------------
        // Identity
        //---------------------------------------------------------------------
        
        UUID GetId() const { return m_Id; }
        const BlockDefinition* GetDefinition() const { return m_Definition; }
        const std::string& GetTypeId() const { return m_Definition->TypeId; }
        
        //---------------------------------------------------------------------
        // Properties from definition
        //---------------------------------------------------------------------
        
        const std::string& GetDisplayName() const { return m_Definition->DisplayName; }
        const std::string& GetDescription() const { return m_Definition->Description; }
        const std::string& GetIcon() const { return m_Definition->Icon; }
        BlockShape GetShape() const { return m_Definition->Shape; }
        BlockCategory GetCategory() const { return m_Definition->Category; }
        NetworkAuthority GetAuthority() const;  // Considers ChangesState
        bool IsValueBlock() const { return m_Definition->IsValueBlock; }
        ValueType GetReturnType() const { return m_Definition->ReturnType; }
        
        //---------------------------------------------------------------------
        // Slots
        //---------------------------------------------------------------------
        
        // Input slots (value inputs)
        size_t GetInputSlotCount() const { return m_InputSlots.size(); }
        BlockSlot* GetInputSlot(size_t index);
        BlockSlot* GetInputSlot(const std::string& name);
        const BlockSlot* GetInputSlot(size_t index) const;
        const BlockSlot* GetInputSlot(const std::string& name) const;
        
        // Nested slots (body areas)
        size_t GetNestedSlotCount() const { return m_NestedSlots.size(); }
        BlockSlot* GetNestedSlot(size_t index);
        BlockSlot* GetNestedSlot(const std::string& name);
        const BlockSlot* GetNestedSlot(size_t index) const;
        const BlockSlot* GetNestedSlot(const std::string& name) const;
        
        //---------------------------------------------------------------------
        // Statement connections (block chain)
        //---------------------------------------------------------------------
        
        void SetNextBlock(BlockPtr block);
        void SetPreviousBlock(BlockWeakPtr block);
        BlockPtr GetNextBlock() const { return m_NextBlock; }
        BlockPtr GetPreviousBlock() const { return m_PreviousBlock.lock(); }
        
        //---------------------------------------------------------------------
        // Editor state (not serialized to binary)
        //---------------------------------------------------------------------
        
        void SetPosition(const glm::vec2& pos) { m_Position = pos; }
        glm::vec2 GetPosition() const { return m_Position; }
        
        void SetCollapsed(bool collapsed) { m_Collapsed = collapsed; }
        bool IsCollapsed() const { return m_Collapsed; }
        
        void SetComment(const std::string& comment) { m_Comment = comment; }
        const std::string& GetComment() const { return m_Comment; }
        
        void SetDisabled(bool disabled) { m_Disabled = disabled; }
        bool IsDisabled() const { return m_Disabled; }
        
        //---------------------------------------------------------------------
        // Execution
        //---------------------------------------------------------------------
        
        Value Execute(ExecutionContext& context);
        
        //---------------------------------------------------------------------
        // Cloning
        //---------------------------------------------------------------------
        
        BlockPtr Clone() const;
        
    private:
        UUID m_Id;
        const BlockDefinition* m_Definition = nullptr;
        
        // Slot instances (copied from definition, can be modified)
        std::vector<BlockSlot> m_InputSlots;
        std::vector<BlockSlot> m_NestedSlots;
        
        // Statement chain connections
        BlockPtr m_NextBlock;
        BlockWeakPtr m_PreviousBlock;
        
        // Editor state
        glm::vec2 m_Position = {0, 0};
        bool m_Collapsed = false;
        bool m_Disabled = false;
        std::string m_Comment;
    };
}
