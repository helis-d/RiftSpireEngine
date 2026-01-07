#include "ScriptSerializer.h"
#include "../Core/BlockRegistry.h"
// #include <Core/Logger.h>  // TODO: Integrate logger
#include <fstream>
#include <sstream>
#include <iomanip>

namespace RiftSpire
{
    //=========================================================================
    // Simple JSON Writer (no external dependency)
    //=========================================================================
    
    class JsonWriter
    {
    public:
        JsonWriter(bool pretty = true) : m_Pretty(pretty) {}
        
        void BeginObject()
        {
            Write("{");
            m_Indent++;
            m_Empty = true;
        }
        
        void EndObject()
        {
            m_Indent--;
            if (!m_Empty) NewLine();
            Write("}");
            m_Empty = false;
        }
        
        void BeginArray()
        {
            Write("[");
            m_Indent++;
            m_Empty = true;
        }
        
        void EndArray()
        {
            m_Indent--;
            if (!m_Empty) NewLine();
            Write("]");
            m_Empty = false;
        }
        
        void Key(const std::string& key)
        {
            if (!m_Empty) Write(",");
            NewLine();
            Write("\"" + EscapeString(key) + "\":");
            if (m_Pretty) Write(" ");
            m_Empty = false;
        }
        
        void ArrayItem()
        {
            if (!m_Empty) Write(",");
            NewLine();
            m_Empty = false;
        }
        
        void WriteString(const std::string& value)
        {
            Write("\"" + EscapeString(value) + "\"");
        }
        
        void WriteInt(i64 value)
        {
            Write(std::to_string(value));
        }
        
        void WriteFloat(f64 value)
        {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(6) << value;
            Write(ss.str());
        }
        
        void WriteBool(bool value)
        {
            Write(value ? "true" : "false");
        }
        
        void WriteNull()
        {
            Write("null");
        }
        
        std::string GetResult() const { return m_Buffer.str(); }
        
    private:
        void Write(const std::string& s) { m_Buffer << s; }
        
        void NewLine()
        {
            if (m_Pretty)
            {
                m_Buffer << "\n";
                for (int i = 0; i < m_Indent; i++)
                {
                    m_Buffer << "  ";
                }
            }
        }
        
        std::string EscapeString(const std::string& s)
        {
            std::string result;
            for (char c : s)
            {
                switch (c)
                {
                    case '"': result += "\\\""; break;
                    case '\\': result += "\\\\"; break;
                    case '\n': result += "\\n"; break;
                    case '\r': result += "\\r"; break;
                    case '\t': result += "\\t"; break;
                    default: result += c; break;
                }
            }
            return result;
        }
        
        std::ostringstream m_Buffer;
        int m_Indent = 0;
        bool m_Pretty = true;
        bool m_Empty = true;
    };
    
    //=========================================================================
    // Helper: Serialize Value
    //=========================================================================
    
    static void SerializeValue(JsonWriter& writer, const Value& value)
    {
        writer.BeginObject();
        
        writer.Key("type");
        writer.WriteInt(static_cast<i64>(value.GetType()));
        
        writer.Key("value");
        switch (value.GetType())
        {
            case ValueType::Void:
                writer.WriteNull();
                break;
            case ValueType::Bool:
                writer.WriteBool(value.AsBool());
                break;
            case ValueType::Int:
                writer.WriteInt(value.AsInt());
                break;
            case ValueType::Float:
                writer.WriteFloat(value.AsFloat());
                break;
            case ValueType::String:
                writer.WriteString(value.AsString());
                break;
            case ValueType::Vector2:
            {
                auto v = value.AsVector2();
                writer.BeginObject();
                writer.Key("x"); writer.WriteFloat(v.x);
                writer.Key("y"); writer.WriteFloat(v.y);
                writer.EndObject();
                break;
            }
            case ValueType::Vector3:
            {
                auto v = value.AsVector3();
                writer.BeginObject();
                writer.Key("x"); writer.WriteFloat(v.x);
                writer.Key("y"); writer.WriteFloat(v.y);
                writer.Key("z"); writer.WriteFloat(v.z);
                writer.EndObject();
                break;
            }
            case ValueType::Color:
            {
                auto v = value.AsColor();
                writer.BeginObject();
                writer.Key("r"); writer.WriteFloat(v.r);
                writer.Key("g"); writer.WriteFloat(v.g);
                writer.Key("b"); writer.WriteFloat(v.b);
                writer.Key("a"); writer.WriteFloat(v.a);
                writer.EndObject();
                break;
            }
            case ValueType::Entity:
                writer.WriteInt(static_cast<i64>(value.AsEntityHandle()));
                break;
            default:
                writer.WriteNull();
                break;
        }
        
        writer.EndObject();
    }
    
    //=========================================================================
    // Helper: Serialize Block
    //=========================================================================
    
    static void SerializeBlock(JsonWriter& writer, const Block* block)
    {
        if (!block) return;
        
        writer.BeginObject();
        
        writer.Key("id");
        writer.WriteString(block->GetId().ToString());
        
        writer.Key("type");
        writer.WriteString(block->GetTypeId());
        
        writer.Key("position");
        writer.BeginObject();
        writer.Key("x"); writer.WriteFloat(block->GetPosition().x);
        writer.Key("y"); writer.WriteFloat(block->GetPosition().y);
        writer.EndObject();
        
        if (block->IsDisabled())
        {
            writer.Key("disabled");
            writer.WriteBool(true);
        }
        
        if (block->IsCollapsed())
        {
            writer.Key("collapsed");
            writer.WriteBool(true);
        }
        
        if (!block->GetComment().empty())
        {
            writer.Key("comment");
            writer.WriteString(block->GetComment());
        }
        
        // Input slots
        if (block->GetInputSlotCount() > 0)
        {
            writer.Key("inputs");
            writer.BeginArray();
            
            for (size_t i = 0; i < block->GetInputSlotCount(); i++)
            {
                const BlockSlot* slot = block->GetInputSlot(i);
                if (!slot) continue;
                
                writer.ArrayItem();
                writer.BeginObject();
                
                writer.Key("name");
                writer.WriteString(slot->GetName());
                
                if (slot->GetConnectedBlock())
                {
                    writer.Key("connected");
                    writer.WriteString(slot->GetConnectedBlock()->GetId().ToString());
                }
                else
                {
                    writer.Key("default");
                    SerializeValue(writer, slot->GetDefaultValue());
                }
                
                writer.EndObject();
            }
            
            writer.EndArray();
        }
        
        // Nested slots
        if (block->GetNestedSlotCount() > 0)
        {
            writer.Key("nested");
            writer.BeginArray();
            
            for (size_t i = 0; i < block->GetNestedSlotCount(); i++)
            {
                const BlockSlot* slot = block->GetNestedSlot(i);
                if (!slot) continue;
                
                writer.ArrayItem();
                writer.BeginObject();
                
                writer.Key("name");
                writer.WriteString(slot->GetName());
                
                writer.Key("blocks");
                writer.BeginArray();
                
                for (const auto& nested : slot->GetNestedBlocks())
                {
                    writer.ArrayItem();
                    SerializeBlock(writer, nested.get());
                }
                
                writer.EndArray();
                writer.EndObject();
            }
            
            writer.EndArray();
        }
        
        // Next block reference
        if (block->GetNextBlock())
        {
            writer.Key("next");
            writer.WriteString(block->GetNextBlock()->GetId().ToString());
        }
        
        writer.EndObject();
    }
    
    //=========================================================================
    // JSON Serialization
    //=========================================================================
    
    std::string ScriptSerializer::ToJson(const BlockScript* script, bool pretty)
    {
        if (!script) return "{}";
        
        JsonWriter writer(pretty);
        
        writer.BeginObject();
        
        writer.Key("version");
        writer.WriteInt(JSON_VERSION);
        
        writer.Key("id");
        writer.WriteString(script->GetId().ToString());
        
        writer.Key("name");
        writer.WriteString(script->GetName());
        
        if (!script->GetDescription().empty())
        {
            writer.Key("description");
            writer.WriteString(script->GetDescription());
        }
        
        writer.Key("blocks");
        writer.BeginArray();
        
        // Only serialize root blocks (others are nested or chained)
        auto rootBlocks = script->GetRootBlocks();
        for (const auto& block : rootBlocks)
        {
            writer.ArrayItem();
            SerializeBlock(writer, block.get());
        }
        
        writer.EndArray();
        
        writer.EndObject();
        
        return writer.GetResult();
    }
    
    BlockScriptPtr ScriptSerializer::FromJson(const std::string& json)
    {
        // TODO: Implement JSON parser
        // For now, return empty script
        // RS_WARN("JSON deserialization not yet implemented");
        return std::make_shared<BlockScript>("Imported Script");
    }
    
    bool ScriptSerializer::SaveToJsonFile(const BlockScript* script, const std::string& path)
    {
        std::string json = ToJson(script, true);
        
        std::ofstream file(path);
        if (!file.is_open())
        {
            // RS_ERROR("Failed to open file for writing: {}", path);
            return false;
        }
        
        file << json;
        file.close();
        
        // RS_INFO("Saved script to: {}", path);
        return true;
    }
    
    BlockScriptPtr ScriptSerializer::LoadFromJsonFile(const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            // RS_ERROR("Failed to open file for reading: {}", path);
            return nullptr;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        return FromJson(buffer.str());
    }
    
    //=========================================================================
    // Binary Serialization
    //=========================================================================
    
    std::vector<u8> ScriptSerializer::ToBinary(const BlockScript* script)
    {
        std::vector<u8> buffer;
        
        // Write magic number
        auto writeu32 = [&](u32 v) {
            buffer.push_back(v & 0xFF);
            buffer.push_back((v >> 8) & 0xFF);
            buffer.push_back((v >> 16) & 0xFF);
            buffer.push_back((v >> 24) & 0xFF);
        };
        
        writeu32(BINARY_MAGIC);
        writeu32(BINARY_VERSION);
        
        // TODO: Implement binary serialization
        // RS_WARN("Binary serialization not yet implemented");
        
        return buffer;
    }
    
    BlockScriptPtr ScriptSerializer::FromBinary(const std::vector<u8>& data)
    {
        if (data.size() < 8)
        {
            // RS_ERROR("Invalid binary script data");
            return nullptr;
        }
        
        // Read magic number
        u32 magic = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
        if (magic != BINARY_MAGIC)
        {
            // RS_ERROR("Invalid script file magic number");
            return nullptr;
        }
        
        // TODO: Implement binary deserialization
        // RS_WARN("Binary deserialization not yet implemented");
        return std::make_shared<BlockScript>("Imported Script");
    }
    
    bool ScriptSerializer::SaveToBinaryFile(const BlockScript* script, const std::string& path)
    {
        auto data = ToBinary(script);
        
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            // RS_ERROR("Failed to open file for writing: {}", path);
            return false;
        }
        
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        file.close();
        
        return true;
    }
    
    BlockScriptPtr ScriptSerializer::LoadFromBinaryFile(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            // RS_ERROR("Failed to open file for reading: {}", path);
            return nullptr;
        }
        
        std::vector<u8> data((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
        
        return FromBinary(data);
    }
    
    //=========================================================================
    // Auto-detect Format
    //=========================================================================
    
    BlockScriptPtr ScriptSerializer::LoadFromFile(const std::string& path)
    {
        // Check extension
        size_t dotPos = path.rfind('.');
        if (dotPos != std::string::npos)
        {
            std::string ext = path.substr(dotPos);
            if (ext == ".rbsbin")
            {
                return LoadFromBinaryFile(path);
            }
        }
        
        // Default to JSON
        return LoadFromJsonFile(path);
    }
    
    bool ScriptSerializer::SaveToFile(const BlockScript* script, const std::string& path)
    {
        size_t dotPos = path.rfind('.');
        if (dotPos != std::string::npos)
        {
            std::string ext = path.substr(dotPos);
            if (ext == ".rbsbin")
            {
                return SaveToBinaryFile(script, path);
            }
        }
        
        return SaveToJsonFile(script, path);
    }
    
    //=========================================================================
    // Clipboard
    //=========================================================================
    
    std::string ScriptSerializer::BlocksToClipboard(const std::vector<BlockPtr>& blocks)
    {
        JsonWriter writer(false);  // Compact for clipboard
        
        writer.BeginObject();
        writer.Key("type");
        writer.WriteString("riftblocks_clipboard");
        
        writer.Key("blocks");
        writer.BeginArray();
        
        for (const auto& block : blocks)
        {
            writer.ArrayItem();
            SerializeBlock(writer, block.get());
        }
        
        writer.EndArray();
        writer.EndObject();
        
        return writer.GetResult();
    }
    
    std::vector<BlockPtr> ScriptSerializer::BlocksFromClipboard(const std::string& data)
    {
        // TODO: Implement clipboard deserialization
        // RS_WARN("Clipboard deserialization not yet implemented");
        return {};
    }
}
