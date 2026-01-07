#pragma once

#include "BlockTypes.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <cstdint>

using i32 = std::int32_t;
using i64 = std::int64_t;
using f32 = float;
using f64 = double;
using u64 = std::uint64_t;

namespace RiftSpire
{
    // Forward declarations
    class Entity;
    
    //=========================================================================
    // Script Value - Dynamic typed value for visual scripting
    //=========================================================================
    
    class Value
    {
    public:
        using ListType = std::vector<Value>;
        using EntityHandle = u64;  // Entity ID for network serialization
        
        /// Variant holding all possible value types
        using ValueVariant = std::variant<
            std::monostate,     // Void
            bool,               // Bool
            i64,                // Int
            f64,                // Float
            std::string,        // String
            glm::vec2,          // Vector2
            glm::vec3,          // Vector3
            glm::vec4,          // Color (RGBA)
            EntityHandle,       // Entity reference
            std::shared_ptr<ListType>  // List of values
        >;
        
    public:
        Value() : m_Data(std::monostate{}), m_Type(ValueType::Void) {}
        
        // Constructors for each type
        explicit Value(bool v) : m_Data(v), m_Type(ValueType::Bool) {}
        explicit Value(i32 v) : m_Data(static_cast<i64>(v)), m_Type(ValueType::Int) {}
        explicit Value(i64 v) : m_Data(v), m_Type(ValueType::Int) {}
        explicit Value(f32 v) : m_Data(static_cast<f64>(v)), m_Type(ValueType::Float) {}
        explicit Value(f64 v) : m_Data(v), m_Type(ValueType::Float) {}
        explicit Value(const char* v) : m_Data(std::string(v)), m_Type(ValueType::String) {}
        explicit Value(const std::string& v) : m_Data(v), m_Type(ValueType::String) {}
        explicit Value(std::string&& v) : m_Data(std::move(v)), m_Type(ValueType::String) {}
        explicit Value(const glm::vec2& v) : m_Data(v), m_Type(ValueType::Vector2) {}
        explicit Value(const glm::vec3& v) : m_Data(v), m_Type(ValueType::Vector3) {}
        explicit Value(const glm::vec4& v) : m_Data(v), m_Type(ValueType::Color) {}
        
        // Entity value (stores handle for network safety)
        static Value FromEntityHandle(EntityHandle handle)
        {
            Value v;
            v.m_Data = handle;
            v.m_Type = ValueType::Entity;
            return v;
        }
        
        // List value
        static Value CreateList()
        {
            Value v;
            v.m_Data = std::make_shared<ListType>();
            v.m_Type = ValueType::List;
            return v;
        }
        
        static Value CreateList(std::initializer_list<Value> items)
        {
            Value v;
            v.m_Data = std::make_shared<ListType>(items);
            v.m_Type = ValueType::List;
            return v;
        }
        
        //---------------------------------------------------------------------
        // Type checking
        //---------------------------------------------------------------------
        
        ValueType GetType() const { return m_Type; }
        
        bool IsVoid() const { return m_Type == ValueType::Void; }
        bool IsBool() const { return m_Type == ValueType::Bool; }
        bool IsInt() const { return m_Type == ValueType::Int; }
        bool IsFloat() const { return m_Type == ValueType::Float; }
        bool IsNumber() const { return m_Type == ValueType::Int || m_Type == ValueType::Float; }
        bool IsString() const { return m_Type == ValueType::String; }
        bool IsVector2() const { return m_Type == ValueType::Vector2; }
        bool IsVector3() const { return m_Type == ValueType::Vector3; }
        bool IsColor() const { return m_Type == ValueType::Color; }
        bool IsEntity() const { return m_Type == ValueType::Entity; }
        bool IsList() const { return m_Type == ValueType::List; }
        
        //---------------------------------------------------------------------
        // Value getters (with type checking)
        //---------------------------------------------------------------------
        
        bool AsBool() const
        {
            if (IsBool()) return std::get<bool>(m_Data);
            if (IsInt()) return std::get<i64>(m_Data) != 0;
            if (IsFloat()) return std::get<f64>(m_Data) != 0.0;
            if (IsString()) return !std::get<std::string>(m_Data).empty();
            return false;
        }
        
        i64 AsInt() const
        {
            if (IsInt()) return std::get<i64>(m_Data);
            if (IsFloat()) return static_cast<i64>(std::get<f64>(m_Data));
            if (IsBool()) return std::get<bool>(m_Data) ? 1 : 0;
            return 0;
        }
        
        f64 AsFloat() const
        {
            if (IsFloat()) return std::get<f64>(m_Data);
            if (IsInt()) return static_cast<f64>(std::get<i64>(m_Data));
            if (IsBool()) return std::get<bool>(m_Data) ? 1.0 : 0.0;
            return 0.0;
        }
        
        std::string AsString() const
        {
            if (IsString()) return std::get<std::string>(m_Data);
            if (IsBool()) return std::get<bool>(m_Data) ? "true" : "false";
            if (IsInt()) return std::to_string(std::get<i64>(m_Data));
            if (IsFloat()) return std::to_string(std::get<f64>(m_Data));
            if (IsVector2())
            {
                const auto& v = std::get<glm::vec2>(m_Data);
                return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
            }
            if (IsVector3())
            {
                const auto& v = std::get<glm::vec3>(m_Data);
                return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
            }
            return "";
        }
        
        glm::vec2 AsVector2() const
        {
            if (IsVector2()) return std::get<glm::vec2>(m_Data);
            if (IsVector3())
            {
                const auto& v = std::get<glm::vec3>(m_Data);
                return glm::vec2(v.x, v.y);
            }
            return glm::vec2(0.0f);
        }
        
        glm::vec3 AsVector3() const
        {
            if (IsVector3()) return std::get<glm::vec3>(m_Data);
            if (IsVector2())
            {
                const auto& v = std::get<glm::vec2>(m_Data);
                return glm::vec3(v.x, v.y, 0.0f);
            }
            return glm::vec3(0.0f);
        }
        
        glm::vec4 AsColor() const
        {
            if (IsColor()) return std::get<glm::vec4>(m_Data);
            return glm::vec4(1.0f);
        }
        
        EntityHandle AsEntityHandle() const
        {
            if (IsEntity()) return std::get<EntityHandle>(m_Data);
            return 0;
        }
        
        ListType& AsList()
        {
            if (IsList()) return *std::get<std::shared_ptr<ListType>>(m_Data);
            static ListType empty;
            return empty;
        }
        
        const ListType& AsList() const
        {
            if (IsList()) return *std::get<std::shared_ptr<ListType>>(m_Data);
            static ListType empty;
            return empty;
        }
        
        //---------------------------------------------------------------------
        // Operators
        //---------------------------------------------------------------------
        
        // Arithmetic
        Value operator+(const Value& other) const;
        Value operator-(const Value& other) const;
        Value operator*(const Value& other) const;
        Value operator/(const Value& other) const;
        Value operator%(const Value& other) const;
        Value operator-() const;
        
        // Comparison
        bool operator==(const Value& other) const;
        bool operator!=(const Value& other) const;
        bool operator<(const Value& other) const;
        bool operator<=(const Value& other) const;
        bool operator>(const Value& other) const;
        bool operator>=(const Value& other) const;
        
        // Logical
        Value operator&&(const Value& other) const;
        Value operator||(const Value& other) const;
        Value operator!() const;
        
        //---------------------------------------------------------------------
        // Type name for debugging
        //---------------------------------------------------------------------
        
        const char* GetTypeName() const
        {
            switch (m_Type)
            {
                case ValueType::Void:    return "Void";
                case ValueType::Bool:    return "Bool";
                case ValueType::Int:     return "Int";
                case ValueType::Float:   return "Float";
                case ValueType::String:  return "String";
                case ValueType::Vector2: return "Vector2";
                case ValueType::Vector3: return "Vector3";
                case ValueType::Color:   return "Color";
                case ValueType::Entity:  return "Entity";
                case ValueType::List:    return "List";
                case ValueType::Any:     return "Any";
                default:                 return "Unknown";
            }
        }
        
    private:
        ValueVariant m_Data;
        ValueType m_Type;
    };
}
