#include "AbilityBlueprint.h"
#include "../Core/BlockScript.h"
#include "../Core/Block.h"
#include <Core/Logger.h>
#include <algorithm>
#include <cstring>
#include <sstream>

namespace RiftSpire
{
    //=========================================================================
    // Binary Serialization Helpers
    //=========================================================================
    
    void BlueprintSerializer::WriteUUID(std::vector<uint8_t>& buffer, const UUID& id)
    {
        uint64_t value = id.IsValid() ? std::stoull(id.ToString()) : 0;
        for (int i = 0; i < 8; ++i)
        {
            buffer.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
        }
    }
    
    void BlueprintSerializer::WriteString(std::vector<uint8_t>& buffer, const std::string& str)
    {
        WriteUInt32(buffer, static_cast<uint32_t>(str.size()));
        for (char c : str)
        {
            buffer.push_back(static_cast<uint8_t>(c));
        }
    }
    
    void BlueprintSerializer::WriteFloat(std::vector<uint8_t>& buffer, float value)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
        for (int i = 0; i < 4; ++i)
        {
            buffer.push_back(bytes[i]);
        }
    }
    
    void BlueprintSerializer::WriteInt32(std::vector<uint8_t>& buffer, int32_t value)
    {
        for (int i = 0; i < 4; ++i)
        {
            buffer.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
        }
    }
    
    void BlueprintSerializer::WriteUInt32(std::vector<uint8_t>& buffer, uint32_t value)
    {
        for (int i = 0; i < 4; ++i)
        {
            buffer.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
        }
    }
    
    void BlueprintSerializer::WriteValue(std::vector<uint8_t>& buffer, const Value& value)
    {
        // Type
        buffer.push_back(static_cast<uint8_t>(value.GetType()));
        
        // Data based on type
        switch (value.GetType())
        {
            case ValueType::Bool:
                buffer.push_back(value.AsBool() ? 1 : 0);
                break;
            case ValueType::Int:
                WriteInt32(buffer, value.AsInt());
                break;
            case ValueType::Float:
                WriteFloat(buffer, value.AsFloat());
                break;
            case ValueType::String:
                WriteString(buffer, value.AsString());
                break;
            default:
                // Complex types - serialize as string
                WriteString(buffer, value.AsString());
                break;
        }
    }
    
    UUID BlueprintSerializer::ReadUUID(const uint8_t*& ptr)
    {
        uint64_t value = 0;
        for (int i = 0; i < 8; ++i)
        {
            value |= static_cast<uint64_t>(*ptr++) << (i * 8);
        }
        return value == 0 ? UUID() : UUID::Generate(); // Simplified
    }
    
    std::string BlueprintSerializer::ReadString(const uint8_t*& ptr)
    {
        uint32_t length = ReadUInt32(ptr);
        std::string result(reinterpret_cast<const char*>(ptr), length);
        ptr += length;
        return result;
    }
    
    float BlueprintSerializer::ReadFloat(const uint8_t*& ptr)
    {
        float value;
        std::memcpy(&value, ptr, 4);
        ptr += 4;
        return value;
    }
    
    int32_t BlueprintSerializer::ReadInt32(const uint8_t*& ptr)
    {
        int32_t value = 0;
        for (int i = 0; i < 4; ++i)
        {
            value |= static_cast<int32_t>(*ptr++) << (i * 8);
        }
        return value;
    }
    
    uint32_t BlueprintSerializer::ReadUInt32(const uint8_t*& ptr)
    {
        uint32_t value = 0;
        for (int i = 0; i < 4; ++i)
        {
            value |= static_cast<uint32_t>(*ptr++) << (i * 8);
        }
        return value;
    }
    
    Value BlueprintSerializer::ReadValue(const uint8_t*& ptr)
    {
        ValueType type = static_cast<ValueType>(*ptr++);
        
        switch (type)
        {
            case ValueType::Bool:
                return Value(*ptr++ != 0);
            case ValueType::Int:
                return Value(ReadInt32(ptr));
            case ValueType::Float:
                return Value(ReadFloat(ptr));
            case ValueType::String:
                return Value(ReadString(ptr));
            default:
                return Value(ReadString(ptr));
        }
    }
    
    //=========================================================================
    // Serialize to Bytes
    //=========================================================================
    
    std::vector<uint8_t> BlueprintSerializer::SerializeToBytes(const AbilityBlueprint& blueprint)
    {
        std::vector<uint8_t> buffer;
        
        // Magic number
        buffer.push_back('R');
        buffer.push_back('S');
        buffer.push_back('A');
        buffer.push_back('B');
        
        // Version
        WriteUInt32(buffer, blueprint.Version);
        
        // Basic info
        WriteUUID(buffer, blueprint.Id);
        WriteString(buffer, blueprint.Name);
        WriteString(buffer, blueprint.Description);
        WriteString(buffer, blueprint.IconPath);
        
        // Properties
        WriteFloat(buffer, blueprint.BaseCooldown);
        WriteFloat(buffer, blueprint.ManaCost);
        WriteFloat(buffer, blueprint.CastTime);
        WriteFloat(buffer, blueprint.Range);
        WriteInt32(buffer, blueprint.MaxLevel);
        
        // Per-level arrays
        WriteUInt32(buffer, static_cast<uint32_t>(blueprint.CooldownPerLevel.size()));
        for (float v : blueprint.CooldownPerLevel) WriteFloat(buffer, v);
        
        WriteUInt32(buffer, static_cast<uint32_t>(blueprint.ManaCostPerLevel.size()));
        for (float v : blueprint.ManaCostPerLevel) WriteFloat(buffer, v);
        
        WriteUInt32(buffer, static_cast<uint32_t>(blueprint.DamagePerLevel.size()));
        for (float v : blueprint.DamagePerLevel) WriteFloat(buffer, v);
        
        // AST - Nodes
        WriteUUID(buffer, blueprint.ScriptAST.RootId);
        WriteUInt32(buffer, static_cast<uint32_t>(blueprint.ScriptAST.Nodes.size()));
        
        // Sort nodes by ID for deterministic order
        std::vector<UUID> sortedIds;
        for (const auto& [id, _] : blueprint.ScriptAST.Nodes)
        {
            sortedIds.push_back(id);
        }
        std::sort(sortedIds.begin(), sortedIds.end(), [](const UUID& a, const UUID& b) {
            return a.ToString() < b.ToString();
        });
        
        for (const UUID& id : sortedIds)
        {
            const auto& node = blueprint.ScriptAST.Nodes.at(id);
            WriteUUID(buffer, node.Id);
            WriteString(buffer, node.TypeId);
            WriteUUID(buffer, node.NextBlockId);
            WriteUUID(buffer, node.ParentId);
            
            // Properties
            WriteUInt32(buffer, static_cast<uint32_t>(node.Properties.size()));
            for (const auto& [key, value] : node.Properties)
            {
                WriteString(buffer, key);
                WriteValue(buffer, value);
            }
            
            // Children
            WriteUInt32(buffer, static_cast<uint32_t>(node.Children.size()));
            for (const UUID& childId : node.Children)
            {
                WriteUUID(buffer, childId);
            }
        }
        
        // AST - Connections
        WriteUInt32(buffer, static_cast<uint32_t>(blueprint.ScriptAST.Connections.size()));
        for (const auto& conn : blueprint.ScriptAST.Connections)
        {
            WriteUUID(buffer, conn.SourceBlockId);
            WriteUUID(buffer, conn.TargetBlockId);
            WriteString(buffer, conn.SourcePortName);
            WriteString(buffer, conn.TargetPortName);
        }
        
        return buffer;
    }
    
    //=========================================================================
    // Deserialize from Bytes
    //=========================================================================
    
    AbilityBlueprint BlueprintSerializer::DeserializeFromBytes(const std::vector<uint8_t>& data)
    {
        AbilityBlueprint blueprint;
        const uint8_t* ptr = data.data();
        
        // Check magic
        if (data.size() < 4 || ptr[0] != 'R' || ptr[1] != 'S' || ptr[2] != 'A' || ptr[3] != 'B')
        {
            RS_ERROR("BlueprintSerializer: Invalid file format");
            return blueprint;
        }
        ptr += 4;
        
        // Version
        blueprint.Version = ReadUInt32(ptr);
        
        // Basic info
        blueprint.Id = ReadUUID(ptr);
        blueprint.Name = ReadString(ptr);
        blueprint.Description = ReadString(ptr);
        blueprint.IconPath = ReadString(ptr);
        
        // Properties
        blueprint.BaseCooldown = ReadFloat(ptr);
        blueprint.ManaCost = ReadFloat(ptr);
        blueprint.CastTime = ReadFloat(ptr);
        blueprint.Range = ReadFloat(ptr);
        blueprint.MaxLevel = ReadInt32(ptr);
        
        // Per-level arrays
        uint32_t count = ReadUInt32(ptr);
        blueprint.CooldownPerLevel.resize(count);
        for (uint32_t i = 0; i < count; ++i) blueprint.CooldownPerLevel[i] = ReadFloat(ptr);
        
        count = ReadUInt32(ptr);
        blueprint.ManaCostPerLevel.resize(count);
        for (uint32_t i = 0; i < count; ++i) blueprint.ManaCostPerLevel[i] = ReadFloat(ptr);
        
        count = ReadUInt32(ptr);
        blueprint.DamagePerLevel.resize(count);
        for (uint32_t i = 0; i < count; ++i) blueprint.DamagePerLevel[i] = ReadFloat(ptr);
        
        // AST - Nodes
        blueprint.ScriptAST.RootId = ReadUUID(ptr);
        count = ReadUInt32(ptr);
        
        for (uint32_t i = 0; i < count; ++i)
        {
            ASTNode node;
            node.Id = ReadUUID(ptr);
            node.TypeId = ReadString(ptr);
            node.NextBlockId = ReadUUID(ptr);
            node.ParentId = ReadUUID(ptr);
            
            // Properties
            uint32_t propCount = ReadUInt32(ptr);
            for (uint32_t j = 0; j < propCount; ++j)
            {
                std::string key = ReadString(ptr);
                Value value = ReadValue(ptr);
                node.Properties[key] = value;
            }
            
            // Children
            uint32_t childCount = ReadUInt32(ptr);
            node.Children.resize(childCount);
            for (uint32_t j = 0; j < childCount; ++j)
            {
                node.Children[j] = ReadUUID(ptr);
            }
            
            blueprint.ScriptAST.Nodes[node.Id] = node;
        }
        
        // AST - Connections
        count = ReadUInt32(ptr);
        blueprint.ScriptAST.Connections.resize(count);
        for (uint32_t i = 0; i < count; ++i)
        {
            blueprint.ScriptAST.Connections[i].SourceBlockId = ReadUUID(ptr);
            blueprint.ScriptAST.Connections[i].TargetBlockId = ReadUUID(ptr);
            blueprint.ScriptAST.Connections[i].SourcePortName = ReadString(ptr);
            blueprint.ScriptAST.Connections[i].TargetPortName = ReadString(ptr);
        }
        
        return blueprint;
    }
    
    //=========================================================================
    // Checksum
    //=========================================================================
    
    uint64_t BlueprintSerializer::ComputeChecksum(const AbilityBlueprint& blueprint)
    {
        auto bytes = SerializeToBytes(blueprint);
        
        // Simple FNV-1a hash
        uint64_t hash = 14695981039346656037ULL;
        for (uint8_t byte : bytes)
        {
            hash ^= byte;
            hash *= 1099511628211ULL;
        }
        return hash;
    }
    
    //=========================================================================
    // JSON Serialization (Simplified)
    //=========================================================================
    
    std::string BlueprintSerializer::SerializeToJson(const AbilityBlueprint& blueprint)
    {
        std::ostringstream ss;
        ss << "{\n";
        ss << "  \"name\": \"" << blueprint.Name << "\",\n";
        ss << "  \"description\": \"" << blueprint.Description << "\",\n";
        ss << "  \"cooldown\": " << blueprint.BaseCooldown << ",\n";
        ss << "  \"manaCost\": " << blueprint.ManaCost << ",\n";
        ss << "  \"castTime\": " << blueprint.CastTime << ",\n";
        ss << "  \"range\": " << blueprint.Range << ",\n";
        ss << "  \"maxLevel\": " << blueprint.MaxLevel << ",\n";
        ss << "  \"nodeCount\": " << blueprint.ScriptAST.Nodes.size() << "\n";
        ss << "}";
        return ss.str();
    }
    
    AbilityBlueprint BlueprintSerializer::DeserializeFromJson(const std::string& json)
    {
        // TODO: Implement proper JSON parsing
        return AbilityBlueprint{};
    }
    
    //=========================================================================
    // AbilityBlueprintLibrary
    //=========================================================================
    
    AbilityBlueprintLibrary& AbilityBlueprintLibrary::Get()
    {
        static AbilityBlueprintLibrary s_Instance;
        return s_Instance;
    }
    
    void AbilityBlueprintLibrary::RegisterBlueprint(const AbilityBlueprint& blueprint)
    {
        m_Blueprints[blueprint.Id] = blueprint;
        m_NameToId[blueprint.Name] = blueprint.Id;
        RS_INFO("AbilityBlueprintLibrary: Registered '{}'", blueprint.Name);
    }
    
    void AbilityBlueprintLibrary::UnregisterBlueprint(const UUID& id)
    {
        auto it = m_Blueprints.find(id);
        if (it != m_Blueprints.end())
        {
            m_NameToId.erase(it->second.Name);
            m_Blueprints.erase(it);
        }
    }
    
    const AbilityBlueprint* AbilityBlueprintLibrary::GetBlueprint(const UUID& id) const
    {
        auto it = m_Blueprints.find(id);
        return it != m_Blueprints.end() ? &it->second : nullptr;
    }
    
    const AbilityBlueprint* AbilityBlueprintLibrary::GetBlueprintByName(const std::string& name) const
    {
        auto it = m_NameToId.find(name);
        if (it != m_NameToId.end())
        {
            return GetBlueprint(it->second);
        }
        return nullptr;
    }
    
    std::vector<const AbilityBlueprint*> AbilityBlueprintLibrary::GetAllBlueprints() const
    {
        std::vector<const AbilityBlueprint*> result;
        for (const auto& [id, bp] : m_Blueprints)
        {
            result.push_back(&bp);
        }
        return result;
    }
    
    void AbilityBlueprintLibrary::MarkDirty(const UUID& id)
    {
        m_DirtyBlueprints.push_back(id);
    }
    
    void AbilityBlueprintLibrary::ReloadDirty()
    {
        // TODO: Implement hot reload
        m_DirtyBlueprints.clear();
    }
}
