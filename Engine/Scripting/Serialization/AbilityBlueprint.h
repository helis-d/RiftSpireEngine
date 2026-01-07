#pragma once

#include "../Core/Block.h"
#include "../Core/Value.h"
#include <Core/UUID.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>

namespace RiftSpire
{
    //=========================================================================
    // BlockAST - Blok soyut syntax agaci
    //=========================================================================
    
    struct ASTNode
    {
        UUID Id;
        std::string TypeId;            // Blok tipi (orn: "control.if", "action.damage")
        std::map<std::string, Value> Properties;  // Blok ozellikleri
        std::vector<UUID> Children;    // Ic blok ID'leri
        UUID NextBlockId;              // Sonraki blok ID'si
        UUID ParentId;                 // Ust blok ID'si
    };
    
    struct ASTConnection
    {
        UUID SourceBlockId;
        UUID TargetBlockId;
        std::string SourcePortName;
        std::string TargetPortName;
    };
    
    struct BlockAST
    {
        UUID RootId;                   // Kok blok (Event)
        std::unordered_map<UUID, ASTNode> Nodes;
        std::vector<ASTConnection> Connections;
    };
    
    //=========================================================================
    // AbilityBlueprint - Yetkinlik tasarimi
    //=========================================================================
    
    struct AbilityBlueprint
    {
        UUID Id;
        std::string Name;              // Yetkinlik adi
        std::string Description;       // Aciklama
        std::string IconPath;          // Ikon yolu
        
        // Temel ozellikler
        float BaseCooldown = 10.0f;
        float ManaCost = 50.0f;
        float CastTime = 0.0f;
        float Range = 500.0f;
        
        // Olcekleme
        int MaxLevel = 5;
        std::vector<float> CooldownPerLevel;
        std::vector<float> ManaCostPerLevel;
        std::vector<float> DamagePerLevel;
        
        // Script
        BlockAST ScriptAST;            // Blok grafigi AST
        
        // Surum kontrolu
        uint32_t Version = 1;
        uint64_t LastModified = 0;
    };
    
    //=========================================================================
    // BlueprintSerializer - Deterministik serializasyon
    //=========================================================================
    
    class BlueprintSerializer
    {
    public:
        // Binary serializasyon (network ve dosya)
        static std::vector<uint8_t> SerializeToBytes(const AbilityBlueprint& blueprint);
        static AbilityBlueprint DeserializeFromBytes(const std::vector<uint8_t>& data);
        
        // JSON serializasyon (debug ve editor)
        static std::string SerializeToJson(const AbilityBlueprint& blueprint);
        static AbilityBlueprint DeserializeFromJson(const std::string& json);
        
        // Block Graph <-> AST donusumleri
        static BlockAST GraphToAST(const class BlockScript& script);
        static void ASTToGraph(const BlockAST& ast, class BlockScript& outScript);
        
        // Checksum (deterministik dogrulama)
        static uint64_t ComputeChecksum(const AbilityBlueprint& blueprint);
        
    private:
        // Binary helpers
        static void WriteUUID(std::vector<uint8_t>& buffer, const UUID& id);
        static void WriteString(std::vector<uint8_t>& buffer, const std::string& str);
        static void WriteFloat(std::vector<uint8_t>& buffer, float value);
        static void WriteInt32(std::vector<uint8_t>& buffer, int32_t value);
        static void WriteUInt32(std::vector<uint8_t>& buffer, uint32_t value);
        static void WriteValue(std::vector<uint8_t>& buffer, const Value& value);
        
        static UUID ReadUUID(const uint8_t*& ptr);
        static std::string ReadString(const uint8_t*& ptr);
        static float ReadFloat(const uint8_t*& ptr);
        static int32_t ReadInt32(const uint8_t*& ptr);
        static uint32_t ReadUInt32(const uint8_t*& ptr);
        static Value ReadValue(const uint8_t*& ptr);
    };
    
    //=========================================================================
    // AbilityBlueprintLibrary - Blueprint kutuphanesi
    //=========================================================================
    
    class AbilityBlueprintLibrary
    {
    public:
        static AbilityBlueprintLibrary& Get();
        
        // Blueprint yonetimi
        void RegisterBlueprint(const AbilityBlueprint& blueprint);
        void UnregisterBlueprint(const UUID& id);
        
        const AbilityBlueprint* GetBlueprint(const UUID& id) const;
        const AbilityBlueprint* GetBlueprintByName(const std::string& name) const;
        
        std::vector<const AbilityBlueprint*> GetAllBlueprints() const;
        
        // Dosya islemleri
        bool LoadFromFile(const std::string& path);
        bool SaveToFile(const std::string& path, const UUID& blueprintId);
        bool LoadAllFromDirectory(const std::string& directory);
        
        // Hot reload
        void MarkDirty(const UUID& id);
        void ReloadDirty();
        
    private:
        AbilityBlueprintLibrary() = default;
        
        std::unordered_map<UUID, AbilityBlueprint> m_Blueprints;
        std::unordered_map<std::string, UUID> m_NameToId;
        std::vector<UUID> m_DirtyBlueprints;
    };
}
