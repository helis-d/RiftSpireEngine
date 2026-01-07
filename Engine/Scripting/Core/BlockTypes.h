#pragma once

#include <cstdint>
#include <string>

using u8 = std::uint8_t;
using u32 = std::uint32_t;

namespace RiftSpire
{
    //=========================================================================
    // Block Type Enumerations
    //=========================================================================
    
    /// Block shape/structure type
    enum class BlockShape : u8
    {
        Flat,               // Simple single-line block
        SingleNested,       // Contains one nested block area
        MultiNested,        // Contains multiple nested areas (if-else)
        NestedInNested,     // Deeply nested structure
        ValueNested,        // Has value input slots
        MultiValueNested,   // Multiple value input slots
        ConditionalNested,  // Switch/case style
        EventNested,        // Event handler (hat-shaped top)
        LoopNested,         // Loop blocks (forever, repeat, while)
        ScopedNested        // Scoped execution (server, client)
    };
    
    /// Block category for organization and coloring
    enum class BlockCategory : u8
    {
        Operators,          // Purple  #9B59B6
        ControlFlow,        // Orange  #E67E22
        Events,             // Yellow  #F1C40F
        Movement,           // Blue    #3498DB
        PositionTransform,  // LBlue   #5DADE2
        Targeting,          // Red     #E74C3C
        Attack,             // DRed    #922B21
        Damage,             // Crimson #DC143C
        Defense,            // Green   #27AE60
        Health,             // BGreen  #2ECC71
        MannaEnergy,        // Cyan    #00BCD4
        Abilities,          // Magenta #9B59B6
        Ultimate,           // Gold    #FFD700
        Cooldown,           // Gray    #95A5A6
        BuffsDebuffs,       // Teal    #008080
        StatusEffects,      // Brown   #8B4513
        AreaEffects,        // Violet  #8E44AD
        TeamGroup,          // Lime    #32CD32
        Vision,             // Olive   #808000
        AI,                 // DBlue   #1A5276
        CollisionPhysics,   // Steel   #4682B4
        Time,               // Silver  #C0C0C0
        Animation,          // Pink    #FF69B4
        Effects,            // NGreen  #39FF14
        Sound,              // SkyBlue #87CEEB
        UIHud,              // White   #FFFFFF
        Camera,             // Indigo  #4B0082
        Input,              // Black   #2C3E50
        NetworkMultiplayer, // Navy    #000080
        Synchronization,    // Turq    #40E0D0
        DataVariables,      // LGreen  #58D68D
        SaveState,          // Beige   #F5F5DC
        DebugLogging        // DGray   #7F8C8D
    };
    
    /// Network authority for block execution
    enum class NetworkAuthority : u8
    {
        Local,              // Client-side only, no sync
        Server,             // Server authoritative, results synced
        Predicted,          // Client prediction + server reconciliation
        Multicast           // Broadcast to all clients
    };
    
    /// Slot type for block connections
    enum class SlotType : u8
    {
        None,               // No slot
        ValueInput,         // Accepts a value block (expression)
        ValueOutput,        // Outputs a value
        StatementInput,     // Top connector (previous block)
        StatementOutput,    // Bottom connector (next block)
        NestedBody          // Contains nested blocks
    };
    
    /// Data types for values
    enum class ValueType : u8
    {
        Void,
        Bool,
        Int,
        Float,
        String,
        Vector2,
        Vector3,
        Color,
        Entity,
        List,
        Any                 // Dynamic type
    };
    
    //=========================================================================
    // Category Colors (ImGui compatible RGBA)
    //=========================================================================
    
    struct CategoryColor
    {
        u8 r, g, b, a;
        
        constexpr CategoryColor(u8 r_, u8 g_, u8 b_, u8 a_ = 255)
            : r(r_), g(g_), b(b_), a(a_) {}
        
        constexpr u32 ToU32() const
        {
            return (static_cast<u32>(a) << 24) |
                   (static_cast<u32>(b) << 16) |
                   (static_cast<u32>(g) << 8) |
                   static_cast<u32>(r);
        }
        
        static constexpr CategoryColor FromHex(u32 hex)
        {
            return CategoryColor(
                (hex >> 16) & 0xFF,
                (hex >> 8) & 0xFF,
                hex & 0xFF
            );
        }
    };
    
    namespace BlockColors
    {
        constexpr CategoryColor Operators       = CategoryColor::FromHex(0x9B59B6);
        constexpr CategoryColor ControlFlow     = CategoryColor::FromHex(0xE67E22);
        constexpr CategoryColor Events          = CategoryColor::FromHex(0xF1C40F);
        constexpr CategoryColor Movement        = CategoryColor::FromHex(0x3498DB);
        constexpr CategoryColor PositionTransform = CategoryColor::FromHex(0x5DADE2);
        constexpr CategoryColor Targeting       = CategoryColor::FromHex(0xE74C3C);
        constexpr CategoryColor Attack          = CategoryColor::FromHex(0x922B21);
        constexpr CategoryColor Damage          = CategoryColor::FromHex(0xDC143C);
        constexpr CategoryColor Defense         = CategoryColor::FromHex(0x27AE60);
        constexpr CategoryColor Health          = CategoryColor::FromHex(0x2ECC71);
        constexpr CategoryColor ManaEnergy      = CategoryColor::FromHex(0x00BCD4);
        constexpr CategoryColor Abilities       = CategoryColor::FromHex(0x9B59B6);
        constexpr CategoryColor Ultimate        = CategoryColor::FromHex(0xFFD700);
        constexpr CategoryColor Cooldown        = CategoryColor::FromHex(0x95A5A6);
        constexpr CategoryColor BuffsDebuffs    = CategoryColor::FromHex(0x008080);
        constexpr CategoryColor StatusEffects   = CategoryColor::FromHex(0x8B4513);
        constexpr CategoryColor AreaEffects     = CategoryColor::FromHex(0x8E44AD);
        constexpr CategoryColor TeamGroup       = CategoryColor::FromHex(0x32CD32);
        constexpr CategoryColor Vision          = CategoryColor::FromHex(0x808000);
        constexpr CategoryColor AI              = CategoryColor::FromHex(0x1A5276);
        constexpr CategoryColor CollisionPhysics = CategoryColor::FromHex(0x4682B4);
        constexpr CategoryColor Time            = CategoryColor::FromHex(0xC0C0C0);
        constexpr CategoryColor Animation       = CategoryColor::FromHex(0xFF69B4);
        constexpr CategoryColor Effects         = CategoryColor::FromHex(0x39FF14);
        constexpr CategoryColor Sound           = CategoryColor::FromHex(0x87CEEB);
        constexpr CategoryColor UIHud           = CategoryColor::FromHex(0xFFFFFF);
        constexpr CategoryColor Camera          = CategoryColor::FromHex(0x4B0082);
        constexpr CategoryColor Input           = CategoryColor::FromHex(0x2C3E50);
        constexpr CategoryColor NetworkMultiplayer = CategoryColor::FromHex(0x000080);
        constexpr CategoryColor Synchronization = CategoryColor::FromHex(0x40E0D0);
        constexpr CategoryColor DataVariables   = CategoryColor::FromHex(0x58D68D);
        constexpr CategoryColor SaveState       = CategoryColor::FromHex(0xF5F5DC);
        constexpr CategoryColor DebugLogging    = CategoryColor::FromHex(0x7F8C8D);
    }
    
    /// Get color for a category
    inline CategoryColor GetCategoryColor(BlockCategory category)
    {
        switch (category)
        {
            case BlockCategory::Operators:          return BlockColors::Operators;
            case BlockCategory::ControlFlow:        return BlockColors::ControlFlow;
            case BlockCategory::Events:             return BlockColors::Events;
            case BlockCategory::Movement:           return BlockColors::Movement;
            case BlockCategory::PositionTransform:  return BlockColors::PositionTransform;
            case BlockCategory::Targeting:          return BlockColors::Targeting;
            case BlockCategory::Attack:             return BlockColors::Attack;
            case BlockCategory::Damage:             return BlockColors::Damage;
            case BlockCategory::Defense:            return BlockColors::Defense;
            case BlockCategory::Health:             return BlockColors::Health;
            case BlockCategory::MannaEnergy:        return BlockColors::ManaEnergy;
            case BlockCategory::Abilities:          return BlockColors::Abilities;
            case BlockCategory::Ultimate:           return BlockColors::Ultimate;
            case BlockCategory::Cooldown:           return BlockColors::Cooldown;
            case BlockCategory::BuffsDebuffs:       return BlockColors::BuffsDebuffs;
            case BlockCategory::StatusEffects:      return BlockColors::StatusEffects;
            case BlockCategory::AreaEffects:        return BlockColors::AreaEffects;
            case BlockCategory::TeamGroup:          return BlockColors::TeamGroup;
            case BlockCategory::Vision:             return BlockColors::Vision;
            case BlockCategory::AI:                 return BlockColors::AI;
            case BlockCategory::CollisionPhysics:   return BlockColors::CollisionPhysics;
            case BlockCategory::Time:               return BlockColors::Time;
            case BlockCategory::Animation:          return BlockColors::Animation;
            case BlockCategory::Effects:            return BlockColors::Effects;
            case BlockCategory::Sound:              return BlockColors::Sound;
            case BlockCategory::UIHud:              return BlockColors::UIHud;
            case BlockCategory::Camera:             return BlockColors::Camera;
            case BlockCategory::Input:              return BlockColors::Input;
            case BlockCategory::NetworkMultiplayer: return BlockColors::NetworkMultiplayer;
            case BlockCategory::Synchronization:    return BlockColors::Synchronization;
            case BlockCategory::DataVariables:      return BlockColors::DataVariables;
            case BlockCategory::SaveState:          return BlockColors::SaveState;
            case BlockCategory::DebugLogging:       return BlockColors::DebugLogging;
            default:                                return BlockColors::DebugLogging;
        }
    }
    
    /// Get category name as string
    inline std::string GetCategoryName(BlockCategory category)
    {
        switch (category)
        {
            case BlockCategory::Operators:          return "Operators";
            case BlockCategory::ControlFlow:        return "Control Flow";
            case BlockCategory::Events:             return "Events";
            case BlockCategory::Movement:           return "Movement";
            case BlockCategory::PositionTransform:  return "Position/Transform";
            case BlockCategory::Targeting:          return "Targeting";
            case BlockCategory::Attack:             return "Attack";
            case BlockCategory::Damage:             return "Damage";
            case BlockCategory::Defense:            return "Defense";
            case BlockCategory::Health:             return "Health";
            case BlockCategory::MannaEnergy:        return "Mana/Energy";
            case BlockCategory::Abilities:          return "Abilities";
            case BlockCategory::Ultimate:           return "Ultimate";
            case BlockCategory::Cooldown:           return "Cooldown";
            case BlockCategory::BuffsDebuffs:       return "Buffs/Debuffs";
            case BlockCategory::StatusEffects:      return "Status Effects";
            case BlockCategory::AreaEffects:        return "Area Effects";
            case BlockCategory::TeamGroup:          return "Team/Group";
            case BlockCategory::Vision:             return "Vision";
            case BlockCategory::AI:                 return "AI";
            case BlockCategory::CollisionPhysics:   return "Collision/Physics";
            case BlockCategory::Time:               return "Time";
            case BlockCategory::Animation:          return "Animation";
            case BlockCategory::Effects:            return "Effects (VFX)";
            case BlockCategory::Sound:              return "Sound (SFX)";
            case BlockCategory::UIHud:              return "UI/HUD";
            case BlockCategory::Camera:             return "Camera";
            case BlockCategory::Input:              return "Input";
            case BlockCategory::NetworkMultiplayer: return "Network/Multiplayer";
            case BlockCategory::Synchronization:    return "Synchronization";
            case BlockCategory::DataVariables:      return "Data/Variables";
            case BlockCategory::SaveState:          return "Save/State";
            case BlockCategory::DebugLogging:       return "Debug/Logging";
            default:                                return "Unknown";
        }
    }
}
