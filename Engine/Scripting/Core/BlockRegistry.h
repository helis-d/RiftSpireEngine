#pragma once

#include "Block.h"
#include "BlockTypes.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <memory>

namespace RiftSpire
{
    //=========================================================================
    // BlockBuilder - Fluent API for defining blocks
    //=========================================================================
    
    class BlockRegistry;
    
    class BlockBuilder
    {
    public:
        BlockBuilder(BlockRegistry& registry, const std::string& typeId);
        
        // Basic properties
        BlockBuilder& DisplayName(const std::string& name);
        BlockBuilder& Description(const std::string& desc);
        BlockBuilder& Icon(const std::string& icon);
        
        // Block type configuration
        BlockBuilder& Shape(BlockShape shape);
        BlockBuilder& Category(BlockCategory category);
        BlockBuilder& Authority(NetworkAuthority authority);
        BlockBuilder& ChangesState(bool changes = true);
        
        // Value block configuration
        BlockBuilder& ReturnsValue(ValueType type);
        
        // Add input slots
        BlockBuilder& Input(const std::string& name, ValueType type = ValueType::Any);
        BlockBuilder& Input(const std::string& name, ValueType type, const Value& defaultValue);
        
        // Add nested body slots
        BlockBuilder& NestedBody(const std::string& name = "body");
        
        // Execution function
        BlockBuilder& OnExecute(BlockDefinition::ExecuteFunc func);
        
        // Finalize and register
        void Register();
        
    private:
        BlockRegistry& m_Registry;
        BlockDefinition m_Definition;
    };
    
    //=========================================================================
    // BlockRegistry - Central registry for all block types
    //=========================================================================
    
    class BlockRegistry
    {
    public:
        static BlockRegistry& Get();
        
        // Registration
        BlockBuilder DefineBlock(const std::string& typeId);
        void RegisterDefinition(BlockDefinition definition);
        
        // Lookup
        const BlockDefinition* GetDefinition(const std::string& typeId) const;
        bool HasDefinition(const std::string& typeId) const;
        
        // Creation
        BlockPtr CreateBlock(const std::string& typeId) const;
        
        // Enumeration
        std::vector<std::string> GetAllTypeIds() const;
        std::vector<const BlockDefinition*> GetBlocksByCategory(BlockCategory category) const;
        std::vector<BlockCategory> GetAllCategories() const;
        
        // Statistics
        size_t GetBlockCount() const { return m_Definitions.size(); }
        
    private:
        BlockRegistry() = default;
        ~BlockRegistry() = default;
        BlockRegistry(const BlockRegistry&) = delete;
        BlockRegistry& operator=(const BlockRegistry&) = delete;
        
        std::unordered_map<std::string, BlockDefinition> m_Definitions;
    };
    
    //=========================================================================
    // Block Registration Macros
    //=========================================================================
    
    // Helper for static registration
    struct BlockRegistrar
    {
        BlockRegistrar(std::function<void()> registerFunc)
        {
            registerFunc();
        }
    };
    
    #define REGISTER_BLOCK(typeId) \
        BlockRegistry::Get().DefineBlock(typeId)
    
    #define REGISTER_BLOCKS_BEGIN(name) \
        static void Register##name##Blocks(); \
        static BlockRegistrar s_##name##Registrar(Register##name##Blocks); \
        static void Register##name##Blocks()
    
    // Auto-registration call (put in module init)
    void RegisterAllBlocks();
}
