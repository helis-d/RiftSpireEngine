#include "BlockRegistry.h"
#include <algorithm>

namespace RiftSpire
{
    //=========================================================================
    // BlockBuilder Implementation
    //=========================================================================
    
    BlockBuilder::BlockBuilder(BlockRegistry& registry, const std::string& typeId)
        : m_Registry(registry)
    {
        m_Definition.TypeId = typeId;
        m_Definition.DisplayName = typeId;  // Default display name
    }
    
    BlockBuilder& BlockBuilder::DisplayName(const std::string& name)
    {
        m_Definition.DisplayName = name;
        return *this;
    }
    
    BlockBuilder& BlockBuilder::Description(const std::string& desc)
    {
        m_Definition.Description = desc;
        return *this;
    }
    
    BlockBuilder& BlockBuilder::Icon(const std::string& icon)
    {
        m_Definition.Icon = icon;
        return *this;
    }
    
    BlockBuilder& BlockBuilder::Shape(BlockShape shape)
    {
        m_Definition.Shape = shape;
        return *this;
    }
    
    BlockBuilder& BlockBuilder::Category(BlockCategory category)
    {
        m_Definition.Category = category;
        return *this;
    }
    
    BlockBuilder& BlockBuilder::Authority(NetworkAuthority authority)
    {
        m_Definition.Authority = authority;
        return *this;
    }
    
    BlockBuilder& BlockBuilder::ChangesState(bool changes)
    {
        m_Definition.ChangesState = changes;
        return *this;
    }
    
    BlockBuilder& BlockBuilder::ReturnsValue(ValueType type)
    {
        m_Definition.IsValueBlock = true;
        m_Definition.ReturnType = type;
        return *this;
    }
    
    BlockBuilder& BlockBuilder::Input(const std::string& name, ValueType type)
    {
        m_Definition.InputSlots.emplace_back(name, SlotType::ValueInput, type);
        return *this;
    }
    
    BlockBuilder& BlockBuilder::Input(const std::string& name, ValueType type, const Value& defaultValue)
    {
        BlockSlot slot(name, SlotType::ValueInput, type);
        slot.SetDefaultValue(defaultValue);
        m_Definition.InputSlots.push_back(std::move(slot));
        return *this;
    }
    
    BlockBuilder& BlockBuilder::NestedBody(const std::string& name)
    {
        m_Definition.NestedSlots.emplace_back(name, SlotType::NestedBody);
        return *this;
    }
    
    BlockBuilder& BlockBuilder::OnExecute(BlockDefinition::ExecuteFunc func)
    {
        m_Definition.Execute = std::move(func);
        return *this;
    }
    
    void BlockBuilder::Register()
    {
        m_Registry.RegisterDefinition(std::move(m_Definition));
    }
    
    //=========================================================================
    // BlockRegistry Implementation
    //=========================================================================
    
    BlockRegistry& BlockRegistry::Get()
    {
        static BlockRegistry instance;
        return instance;
    }
    
    BlockBuilder BlockRegistry::DefineBlock(const std::string& typeId)
    {
        return BlockBuilder(*this, typeId);
    }
    
    void BlockRegistry::RegisterDefinition(BlockDefinition definition)
    {
        m_Definitions[definition.TypeId] = std::move(definition);
    }
    
    const BlockDefinition* BlockRegistry::GetDefinition(const std::string& typeId) const
    {
        auto it = m_Definitions.find(typeId);
        if (it != m_Definitions.end())
        {
            return &it->second;
        }
        return nullptr;
    }
    
    bool BlockRegistry::HasDefinition(const std::string& typeId) const
    {
        return m_Definitions.find(typeId) != m_Definitions.end();
    }
    
    BlockPtr BlockRegistry::CreateBlock(const std::string& typeId) const
    {
        const BlockDefinition* def = GetDefinition(typeId);
        if (def)
        {
            return std::make_shared<Block>(def);
        }
        return nullptr;
    }
    
    std::vector<std::string> BlockRegistry::GetAllTypeIds() const
    {
        std::vector<std::string> ids;
        ids.reserve(m_Definitions.size());
        
        for (const auto& [id, def] : m_Definitions)
        {
            ids.push_back(id);
        }
        
        std::sort(ids.begin(), ids.end());
        return ids;
    }
    
    std::vector<const BlockDefinition*> BlockRegistry::GetBlocksByCategory(BlockCategory category) const
    {
        std::vector<const BlockDefinition*> blocks;
        
        for (const auto& [id, def] : m_Definitions)
        {
            if (def.Category == category)
            {
                blocks.push_back(&def);
            }
        }
        
        // Sort by display name
        std::sort(blocks.begin(), blocks.end(), 
            [](const BlockDefinition* a, const BlockDefinition* b) {
                return a->DisplayName < b->DisplayName;
            });
        
        return blocks;
    }
    
    std::vector<BlockCategory> BlockRegistry::GetAllCategories() const
    {
        std::vector<BlockCategory> categories;
        
        for (const auto& [id, def] : m_Definitions)
        {
            auto it = std::find(categories.begin(), categories.end(), def.Category);
            if (it == categories.end())
            {
                categories.push_back(def.Category);
            }
        }
        
        // Sort by category enum value
        std::sort(categories.begin(), categories.end());
        return categories;
    }
    
    //=========================================================================
    // Block Registration
    //=========================================================================
    
    void RegisterAllBlocks()
    {
        // This will be called to ensure all block registrations happen
        // Each block category file uses static initialization to register
        // But we call this to ensure linking includes all categories
    }
}
