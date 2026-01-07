#pragma once

#include "../Core/BlockScript.h"
#include <string>
#include <vector>

namespace RiftSpire
{
    //=========================================================================
    // ScriptSerializer - Serializes/deserializes BlockScripts
    //=========================================================================
    
    class ScriptSerializer
    {
    public:
        //---------------------------------------------------------------------
        // JSON Serialization (Human readable, for development)
        //---------------------------------------------------------------------
        
        /// Serialize script to JSON string
        static std::string ToJson(const BlockScript* script, bool pretty = true);
        
        /// Deserialize script from JSON string
        static BlockScriptPtr FromJson(const std::string& json);
        
        /// Save script to JSON file (.rbs)
        static bool SaveToJsonFile(const BlockScript* script, const std::string& path);
        
        /// Load script from JSON file
        static BlockScriptPtr LoadFromJsonFile(const std::string& path);
        
        //---------------------------------------------------------------------
        // Binary Serialization (Compact, for production)
        //---------------------------------------------------------------------
        
        /// Serialize script to binary buffer
        static std::vector<u8> ToBinary(const BlockScript* script);
        
        /// Deserialize script from binary buffer
        static BlockScriptPtr FromBinary(const std::vector<u8>& data);
        
        /// Save script to binary file (.rbsbin)
        static bool SaveToBinaryFile(const BlockScript* script, const std::string& path);
        
        /// Load script from binary file
        static BlockScriptPtr LoadFromBinaryFile(const std::string& path);
        
        //---------------------------------------------------------------------
        // Auto-detect format
        //---------------------------------------------------------------------
        
        /// Load script, auto-detecting format from extension
        static BlockScriptPtr LoadFromFile(const std::string& path);
        
        /// Save script with format based on extension
        static bool SaveToFile(const BlockScript* script, const std::string& path);
        
        //---------------------------------------------------------------------
        // Clipboard (simplified format for copy/paste)
        //---------------------------------------------------------------------
        
        /// Serialize blocks to clipboard format
        static std::string BlocksToClipboard(const std::vector<BlockPtr>& blocks);
        
        /// Deserialize blocks from clipboard format
        static std::vector<BlockPtr> BlocksFromClipboard(const std::string& data);
        
    private:
        // Version for compatibility
        static constexpr u32 JSON_VERSION = 1;
        static constexpr u32 BINARY_VERSION = 1;
        static constexpr u32 BINARY_MAGIC = 0x52425343;  // "RBSC" = RiftBlocks Script
    };
}
