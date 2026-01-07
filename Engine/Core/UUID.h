#pragma once

#include <Core/Types.h>
#include <random>
#include <string>
#include <sstream>
#include <iomanip>

namespace RiftSpire
{
    //=========================================================================
    // UUID - Universally Unique Identifier for blocks
    //=========================================================================
    
    class UUID
    {
    public:
        UUID() : m_High(0), m_Low(0) {}
        UUID(u64 high, u64 low) : m_High(high), m_Low(low) {}
        
        // Generate a new random UUID
        static UUID Generate()
        {
            static std::random_device rd;
            static std::mt19937_64 gen(rd());
            static std::uniform_int_distribution<u64> dis;
            
            u64 high = dis(gen);
            u64 low = dis(gen);
            
            // Set version to 4 (random) and variant to RFC 4122
            high = (high & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;  // Version 4
            low = (low & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;   // Variant RFC 4122
            
            return UUID(high, low);
        }
        
        // Parse from string (xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx)
        static UUID FromString(const std::string& str)
        {
            if (str.length() != 36) return UUID();
            
            std::string cleaned;
            for (char c : str)
            {
                if (c != '-') cleaned += c;
            }
            
            if (cleaned.length() != 32) return UUID();
            
            u64 high = std::stoull(cleaned.substr(0, 16), nullptr, 16);
            u64 low = std::stoull(cleaned.substr(16, 16), nullptr, 16);
            
            return UUID(high, low);
        }
        
        // Convert to string
        std::string ToString() const
        {
            std::ostringstream ss;
            ss << std::hex << std::setfill('0');
            
            // xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
            ss << std::setw(8) << ((m_High >> 32) & 0xFFFFFFFF) << "-";
            ss << std::setw(4) << ((m_High >> 16) & 0xFFFF) << "-";
            ss << std::setw(4) << (m_High & 0xFFFF) << "-";
            ss << std::setw(4) << ((m_Low >> 48) & 0xFFFF) << "-";
            ss << std::setw(12) << (m_Low & 0xFFFFFFFFFFFFULL);
            
            return ss.str();
        }
        
        // Operators
        bool operator==(const UUID& other) const { return m_High == other.m_High && m_Low == other.m_Low; }
        bool operator!=(const UUID& other) const { return !(*this == other); }
        bool operator<(const UUID& other) const
        {
            if (m_High != other.m_High) return m_High < other.m_High;
            return m_Low < other.m_Low;
        }
        
        // Validity check
        bool IsValid() const { return m_High != 0 || m_Low != 0; }
        operator bool() const { return IsValid(); }
        
        // Raw access (for serialization)
        u64 GetHigh() const { return m_High; }
        u64 GetLow() const { return m_Low; }
        
        // Hash support
        size_t Hash() const
        {
            return std::hash<u64>()(m_High) ^ (std::hash<u64>()(m_Low) << 1);
        }
        
    private:
        u64 m_High;
        u64 m_Low;
    };
}

// Hash specialization for std::unordered_map
namespace std
{
    template<>
    struct hash<RiftSpire::UUID>
    {
        size_t operator()(const RiftSpire::UUID& uuid) const
        {
            return uuid.Hash();
        }
    };
}

