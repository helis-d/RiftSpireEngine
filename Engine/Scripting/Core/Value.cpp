#include "Value.h"
#include <cmath>

namespace RiftSpire
{
    //=========================================================================
    // Arithmetic Operators
    //=========================================================================
    
    Value Value::operator+(const Value& other) const
    {
        // String concatenation
        if (IsString() || other.IsString())
        {
            return Value(AsString() + other.AsString());
        }
        
        // Vector addition
        if (IsVector3() || other.IsVector3())
        {
            return Value(AsVector3() + other.AsVector3());
        }
        if (IsVector2() || other.IsVector2())
        {
            return Value(AsVector2() + other.AsVector2());
        }
        
        // Numeric addition
        if (IsFloat() || other.IsFloat())
        {
            return Value(AsFloat() + other.AsFloat());
        }
        return Value(AsInt() + other.AsInt());
    }
    
    Value Value::operator-(const Value& other) const
    {
        // Vector subtraction
        if (IsVector3() || other.IsVector3())
        {
            return Value(AsVector3() - other.AsVector3());
        }
        if (IsVector2() || other.IsVector2())
        {
            return Value(AsVector2() - other.AsVector2());
        }
        
        // Numeric subtraction
        if (IsFloat() || other.IsFloat())
        {
            return Value(AsFloat() - other.AsFloat());
        }
        return Value(AsInt() - other.AsInt());
    }
    
    Value Value::operator*(const Value& other) const
    {
        // Vector-scalar multiplication
        if (IsVector3())
        {
            if (other.IsNumber())
            {
                return Value(AsVector3() * static_cast<f32>(other.AsFloat()));
            }
            return Value(AsVector3() * other.AsVector3());
        }
        if (IsVector2())
        {
            if (other.IsNumber())
            {
                return Value(AsVector2() * static_cast<f32>(other.AsFloat()));
            }
            return Value(AsVector2() * other.AsVector2());
        }
        
        // Numeric multiplication
        if (IsFloat() || other.IsFloat())
        {
            return Value(AsFloat() * other.AsFloat());
        }
        return Value(AsInt() * other.AsInt());
    }
    
    Value Value::operator/(const Value& other) const
    {
        // Prevent division by zero
        if (other.IsNumber() && other.AsFloat() == 0.0)
        {
            return Value(0.0);
        }
        
        // Vector-scalar division
        if (IsVector3())
        {
            if (other.IsNumber())
            {
                return Value(AsVector3() / static_cast<f32>(other.AsFloat()));
            }
            // Component-wise division
            glm::vec3 a = AsVector3();
            glm::vec3 b = other.AsVector3();
            return Value(glm::vec3(
                b.x != 0 ? a.x / b.x : 0,
                b.y != 0 ? a.y / b.y : 0,
                b.z != 0 ? a.z / b.z : 0
            ));
        }
        if (IsVector2())
        {
            if (other.IsNumber())
            {
                return Value(AsVector2() / static_cast<f32>(other.AsFloat()));
            }
            glm::vec2 a = AsVector2();
            glm::vec2 b = other.AsVector2();
            return Value(glm::vec2(
                b.x != 0 ? a.x / b.x : 0,
                b.y != 0 ? a.y / b.y : 0
            ));
        }
        
        // Numeric division
        if (IsFloat() || other.IsFloat())
        {
            return Value(AsFloat() / other.AsFloat());
        }
        i64 divisor = other.AsInt();
        return Value(divisor != 0 ? AsInt() / divisor : 0LL);
    }
    
    Value Value::operator%(const Value& other) const
    {
        if (IsFloat() || other.IsFloat())
        {
            f64 divisor = other.AsFloat();
            return Value(divisor != 0.0 ? std::fmod(AsFloat(), divisor) : 0.0);
        }
        i64 divisor = other.AsInt();
        return Value(divisor != 0 ? AsInt() % divisor : 0LL);
    }
    
    Value Value::operator-() const
    {
        if (IsVector3())
        {
            return Value(-AsVector3());
        }
        if (IsVector2())
        {
            return Value(-AsVector2());
        }
        if (IsFloat())
        {
            return Value(-AsFloat());
        }
        return Value(-AsInt());
    }
    
    //=========================================================================
    // Comparison Operators
    //=========================================================================
    
    bool Value::operator==(const Value& other) const
    {
        // Same type comparison
        if (m_Type == other.m_Type)
        {
            switch (m_Type)
            {
                case ValueType::Void:    return true;
                case ValueType::Bool:    return std::get<bool>(m_Data) == std::get<bool>(other.m_Data);
                case ValueType::Int:     return std::get<i64>(m_Data) == std::get<i64>(other.m_Data);
                case ValueType::Float:   return std::get<f64>(m_Data) == std::get<f64>(other.m_Data);
                case ValueType::String:  return std::get<std::string>(m_Data) == std::get<std::string>(other.m_Data);
                case ValueType::Vector2: return std::get<glm::vec2>(m_Data) == std::get<glm::vec2>(other.m_Data);
                case ValueType::Vector3: return std::get<glm::vec3>(m_Data) == std::get<glm::vec3>(other.m_Data);
                case ValueType::Color:   return std::get<glm::vec4>(m_Data) == std::get<glm::vec4>(other.m_Data);
                case ValueType::Entity:  return std::get<EntityHandle>(m_Data) == std::get<EntityHandle>(other.m_Data);
                case ValueType::List:    return false; // Lists compare by reference for now
                default:                 return false;
            }
        }
        
        // Cross-type numeric comparison
        if (IsNumber() && other.IsNumber())
        {
            return AsFloat() == other.AsFloat();
        }
        
        return false;
    }
    
    bool Value::operator!=(const Value& other) const
    {
        return !(*this == other);
    }
    
    bool Value::operator<(const Value& other) const
    {
        if (IsNumber() && other.IsNumber())
        {
            return AsFloat() < other.AsFloat();
        }
        if (IsString() && other.IsString())
        {
            return std::get<std::string>(m_Data) < std::get<std::string>(other.m_Data);
        }
        return false;
    }
    
    bool Value::operator<=(const Value& other) const
    {
        return *this < other || *this == other;
    }
    
    bool Value::operator>(const Value& other) const
    {
        return !(*this <= other);
    }
    
    bool Value::operator>=(const Value& other) const
    {
        return !(*this < other);
    }
    
    //=========================================================================
    // Logical Operators
    //=========================================================================
    
    Value Value::operator&&(const Value& other) const
    {
        return Value(AsBool() && other.AsBool());
    }
    
    Value Value::operator||(const Value& other) const
    {
        return Value(AsBool() || other.AsBool());
    }
    
    Value Value::operator!() const
    {
        return Value(!AsBool());
    }
}
