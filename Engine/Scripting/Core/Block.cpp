#include "Block.h"

namespace RiftSpire
{
    //=========================================================================
    // BlockSlot Implementation
    //=========================================================================
    
    BlockSlot::BlockSlot(const std::string& name, SlotType type, ValueType valueType)
        : m_Name(name)
        , m_SlotType(type)
        , m_ValueType(valueType)
    {
    }
    
    bool BlockSlot::IsConnected() const
    {
        if (m_SlotType == SlotType::NestedBody)
        {
            return !m_NestedBlocks.empty();
        }
        return !m_ConnectedBlock.expired();
    }
    
    void BlockSlot::Connect(BlockPtr block)
    {
        if (m_SlotType == SlotType::ValueInput)
        {
            m_ConnectedBlock = block;
        }
    }
    
    void BlockSlot::Disconnect()
    {
        m_ConnectedBlock.reset();
    }
    
    BlockPtr BlockSlot::GetConnectedBlock() const
    {
        return m_ConnectedBlock.lock();
    }
    
    void BlockSlot::AddNestedBlock(BlockPtr block)
    {
        if (m_SlotType == SlotType::NestedBody && block)
        {
            m_NestedBlocks.push_back(block);
        }
    }
    
    void BlockSlot::RemoveNestedBlock(BlockPtr block)
    {
        if (!block) return;
        
        auto it = std::find(m_NestedBlocks.begin(), m_NestedBlocks.end(), block);
        if (it != m_NestedBlocks.end())
        {
            m_NestedBlocks.erase(it);
        }
    }
    
    void BlockSlot::ClearNestedBlocks()
    {
        m_NestedBlocks.clear();
    }
    
    bool BlockSlot::CanAccept(const Block* block) const
    {
        if (!block) return false;
        
        // Value slots require value blocks
        if (m_SlotType == SlotType::ValueInput)
        {
            if (!block->IsValueBlock()) return false;
            return CanAcceptType(block->GetReturnType());
        }
        
        // Nested slots accept statement blocks
        if (m_SlotType == SlotType::NestedBody)
        {
            return !block->IsValueBlock();
        }
        
        return false;
    }
    
    bool BlockSlot::CanAcceptType(ValueType type) const
    {
        // Any type accepts all
        if (m_ValueType == ValueType::Any) return true;
        if (type == ValueType::Any) return true;
        
        // Exact match
        if (m_ValueType == type) return true;
        
        // Numeric coercion
        if ((m_ValueType == ValueType::Float || m_ValueType == ValueType::Int) &&
            (type == ValueType::Float || type == ValueType::Int))
        {
            return true;
        }
        
        // Vector coercion
        if ((m_ValueType == ValueType::Vector2 || m_ValueType == ValueType::Vector3) &&
            (type == ValueType::Vector2 || type == ValueType::Vector3))
        {
            return true;
        }
        
        return false;
    }
    
    //=========================================================================
    // Block Implementation
    //=========================================================================
    
    Block::Block(const BlockDefinition* definition)
        : m_Id(UUID::Generate())
        , m_Definition(definition)
    {
        // Copy slots from definition
        if (m_Definition)
        {
            m_InputSlots = m_Definition->InputSlots;
            m_NestedSlots = m_Definition->NestedSlots;
        }
    }
    
    NetworkAuthority Block::GetAuthority() const
    {
        // State-changing blocks are always Server authoritative
        if (m_Definition->ChangesState)
        {
            return NetworkAuthority::Server;
        }
        return m_Definition->Authority;
    }
    
    //---------------------------------------------------------------------
    // Input Slots
    //---------------------------------------------------------------------
    
    BlockSlot* Block::GetInputSlot(size_t index)
    {
        if (index < m_InputSlots.size())
        {
            return &m_InputSlots[index];
        }
        return nullptr;
    }
    
    BlockSlot* Block::GetInputSlot(const std::string& name)
    {
        for (auto& slot : m_InputSlots)
        {
            if (slot.GetName() == name)
            {
                return &slot;
            }
        }
        return nullptr;
    }
    
    const BlockSlot* Block::GetInputSlot(size_t index) const
    {
        if (index < m_InputSlots.size())
        {
            return &m_InputSlots[index];
        }
        return nullptr;
    }
    
    const BlockSlot* Block::GetInputSlot(const std::string& name) const
    {
        for (const auto& slot : m_InputSlots)
        {
            if (slot.GetName() == name)
            {
                return &slot;
            }
        }
        return nullptr;
    }
    
    //---------------------------------------------------------------------
    // Nested Slots
    //---------------------------------------------------------------------
    
    BlockSlot* Block::GetNestedSlot(size_t index)
    {
        if (index < m_NestedSlots.size())
        {
            return &m_NestedSlots[index];
        }
        return nullptr;
    }
    
    BlockSlot* Block::GetNestedSlot(const std::string& name)
    {
        for (auto& slot : m_NestedSlots)
        {
            if (slot.GetName() == name)
            {
                return &slot;
            }
        }
        return nullptr;
    }
    
    const BlockSlot* Block::GetNestedSlot(size_t index) const
    {
        if (index < m_NestedSlots.size())
        {
            return &m_NestedSlots[index];
        }
        return nullptr;
    }
    
    const BlockSlot* Block::GetNestedSlot(const std::string& name) const
    {
        for (const auto& slot : m_NestedSlots)
        {
            if (slot.GetName() == name)
            {
                return &slot;
            }
        }
        return nullptr;
    }
    
    //---------------------------------------------------------------------
    // Statement Connections
    //---------------------------------------------------------------------
    
    void Block::SetNextBlock(BlockPtr block)
    {
        // Disconnect old next block
        if (m_NextBlock)
        {
            m_NextBlock->m_PreviousBlock.reset();
        }
        
        m_NextBlock = block;
        
        // Set back reference
        if (block)
        {
            block->m_PreviousBlock = weak_from_this();
        }
    }
    
    void Block::SetPreviousBlock(BlockWeakPtr block)
    {
        m_PreviousBlock = block;
    }
    
    //---------------------------------------------------------------------
    // Execution
    //---------------------------------------------------------------------
    
    Value Block::Execute(ExecutionContext& context)
    {
        // Skip disabled blocks
        if (m_Disabled)
        {
            return Value();
        }
        
        // Execute using definition's function
        if (m_Definition && m_Definition->Execute)
        {
            return m_Definition->Execute(this, context);
        }
        
        return Value();
    }
    
    //---------------------------------------------------------------------
    // Cloning
    //---------------------------------------------------------------------
    
    BlockPtr Block::Clone() const
    {
        auto clone = std::make_shared<Block>(m_Definition);
        
        // Copy slot values (but not connections)
        for (size_t i = 0; i < m_InputSlots.size() && i < clone->m_InputSlots.size(); ++i)
        {
            clone->m_InputSlots[i].SetDefaultValue(m_InputSlots[i].GetDefaultValue());
        }
        
        // Clone nested blocks
        for (size_t i = 0; i < m_NestedSlots.size() && i < clone->m_NestedSlots.size(); ++i)
        {
            for (const auto& nested : m_NestedSlots[i].GetNestedBlocks())
            {
                clone->m_NestedSlots[i].AddNestedBlock(nested->Clone());
            }
        }
        
        // Copy editor state
        clone->m_Position = m_Position;
        clone->m_Collapsed = m_Collapsed;
        clone->m_Disabled = m_Disabled;
        clone->m_Comment = m_Comment;
        
        return clone;
    }
}
