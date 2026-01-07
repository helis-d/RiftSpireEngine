#include "EventBlocks.h"
#include "../Execution/ExecutionContext.h"
#include "../Execution/ScriptVM.h"

namespace RiftSpire
{
    static ScriptVM s_EventVM;
    
    void RegisterEventBlocks()
    {
        auto& registry = BlockRegistry::Get();
        
        //=====================================================================
        // Lifecycle Events
        //=====================================================================
        
        registry.DefineBlock("events.on_start")
            .DisplayName("When Game Starts")
            .Description("Triggered when the game/scene starts")
            .Icon("⚡")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("events.on_spawn")
            .DisplayName("When Spawned")
            .Description("Triggered when this entity is spawned")
            .Icon("⚡")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("events.on_destroy")
            .DisplayName("When Destroyed")
            .Description("Triggered when this entity is destroyed")
            .Icon("⚡")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("events.on_update")
            .DisplayName("On Update")
            .Description("Triggered every frame")
            .Icon("🔄")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        //=====================================================================
        // Combat Events
        //=====================================================================
        
        registry.DefineBlock("events.on_damage_received")
            .DisplayName("When Damage Received")
            .Description("Triggered when this entity receives damage")
            .Icon("💥")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                // The damage amount and source would be set in ctx before triggering
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("events.on_damage_dealt")
            .DisplayName("When Damage Dealt")
            .Description("Triggered when this entity deals damage")
            .Icon("⚔")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("events.on_health_changed")
            .DisplayName("When Health Changed")
            .Description("Triggered when this entity's health changes")
            .Icon("❤")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("events.on_death")
            .DisplayName("When Died")
            .Description("Triggered when this entity dies")
            .Icon("💀")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("events.on_respawn")
            .DisplayName("When Respawned")
            .Description("Triggered when this entity respawns")
            .Icon("✨")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("events.on_kill")
            .DisplayName("When Killed Enemy")
            .Description("Triggered when this entity kills an enemy")
            .Icon("🏆")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        //=====================================================================
        // Ability Events
        //=====================================================================
        
        registry.DefineBlock("events.on_ability_cast")
            .DisplayName("When Ability Casted")
            .Description("Triggered when an ability is used")
            .Icon("🔮")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .Input("slot", ValueType::String, Value("Q"))
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("events.on_ability_hit")
            .DisplayName("When Ability Hits")
            .Description("Triggered when an ability hits a target")
            .Icon("🎯")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .Input("slot", ValueType::String, Value("Q"))
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        //=====================================================================
        // Buff/Debuff Events
        //=====================================================================
        
        registry.DefineBlock("events.on_buff_applied")
            .DisplayName("When Buff Applied")
            .Description("Triggered when a buff is applied to this entity")
            .Icon("⬆")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .Input("buff_name", ValueType::String)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("events.on_buff_removed")
            .DisplayName("When Buff Removed")
            .Description("Triggered when a buff is removed from this entity")
            .Icon("⬇")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .Input("buff_name", ValueType::String)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        //=====================================================================
        // Area/Collision Events
        //=====================================================================
        
        registry.DefineBlock("events.on_enter_area")
            .DisplayName("When Entered Area")
            .Description("Triggered when entering a zone/area")
            .Icon("📍")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .Input("zone", ValueType::String)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("events.on_leave_area")
            .DisplayName("When Left Area")
            .Description("Triggered when leaving a zone/area")
            .Icon("🚪")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .Input("zone", ValueType::String)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("events.on_collision")
            .DisplayName("When Collision")
            .Description("Triggered when colliding with another object")
            .Icon("💫")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .Input("tag", ValueType::String)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        //=====================================================================
        // Input Events
        //=====================================================================
        
        registry.DefineBlock("events.on_key_pressed")
            .DisplayName("When Key Pressed")
            .Description("Triggered when a key is pressed")
            .Icon("⌨")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .Authority(NetworkAuthority::Local)
            .Input("key", ValueType::String, Value("Space"))
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("events.on_mouse_click")
            .DisplayName("When Mouse Clicked")
            .Description("Triggered when mouse is clicked")
            .Icon("🖱")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .Authority(NetworkAuthority::Local)
            .Input("button", ValueType::String, Value("Left"))
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        //=====================================================================
        // Custom Events
        //=====================================================================
        
        registry.DefineBlock("events.on_custom")
            .DisplayName("When Custom Event")
            .Description("Triggered when a custom event is broadcast")
            .Icon("📡")
            .Shape(BlockShape::EventNested)
            .Category(BlockCategory::Events)
            .Input("event_name", ValueType::String, Value("MyEvent"))
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (auto* slot = block->GetNestedSlot("body"))
                {
                    return s_EventVM.ExecuteNestedBlocks(slot, ctx);
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("events.broadcast")
            .DisplayName("Broadcast Event")
            .Description("Broadcast a custom event to all scripts")
            .Icon("📢")
            .Shape(BlockShape::Flat)
            .Category(BlockCategory::Events)
            .Input("event_name", ValueType::String, Value("MyEvent"))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                // TODO: Implement event broadcasting system
                return Value();
            })
            .Register();
        
        registry.DefineBlock("events.broadcast_with_data")
            .DisplayName("Broadcast Event with Data")
            .Description("Broadcast a custom event with data")
            .Icon("📢")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::Events)
            .Input("event_name", ValueType::String, Value("MyEvent"))
            .Input("data", ValueType::Any)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                // TODO: Implement event broadcasting with data
                return Value();
            })
            .Register();
    }
}
