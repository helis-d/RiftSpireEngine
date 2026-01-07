#include "TimeBlocks.h"
#include "../Execution/ExecutionContext.h"
#include "../Execution/ScriptVM.h"
#include <thread>
#include <chrono>

namespace RiftSpire
{
    static ScriptVM s_TimeVM;
    
    void RegisterTimeBlocks()
    {
        auto& registry = BlockRegistry::Get();
        
        //=====================================================================
        // Wait/Delay Blocks
        //=====================================================================
        
        registry.DefineBlock("time.wait")
            .DisplayName("Wait")
            .Description("Pause execution for specified seconds")
            .Icon("⏱")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Time)
            .Authority(NetworkAuthority::Local)
            .Input("seconds", ValueType::Float, Value(1.0))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                f64 seconds = s_TimeVM.GetSlotValue(block->GetInputSlot("seconds"), ctx).AsFloat();
                
                // In a real implementation, this would yield execution
                // For now, we simulate with the execution context
                // TODO: Implement coroutine-style yielding
                
                return Value();
            })
            .Register();
        
        registry.DefineBlock("time.delay")
            .DisplayName("Delay Then")
            .Description("Execute blocks after a delay")
            .Icon("⏲")
            .Shape(BlockShape::ScopedNested)
            .Category(BlockCategory::Time)
            .Authority(NetworkAuthority::Local)
            .Input("seconds", ValueType::Float, Value(1.0))
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                f64 seconds = s_TimeVM.GetSlotValue(block->GetInputSlot("seconds"), ctx).AsFloat();
                
                // Schedule delayed execution
                // TODO: Integrate with VM delayed execution system
                
                return Value();
            })
            .Register();
        
        //=====================================================================
        // Timer Blocks
        //=====================================================================
        
        registry.DefineBlock("time.set_timer")
            .DisplayName("Set Timer")
            .Description("Create a repeating timer")
            .Icon("🔁")
            .Shape(BlockShape::ScopedNested)
            .Category(BlockCategory::Time)
            .Authority(NetworkAuthority::Local)
            .Input("name", ValueType::String, Value("Timer1"))
            .Input("interval", ValueType::Float, Value(1.0))
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string name = s_TimeVM.GetSlotValue(block->GetInputSlot("name"), ctx).AsString();
                f64 interval = s_TimeVM.GetSlotValue(block->GetInputSlot("interval"), ctx).AsFloat();
                
                // TODO: Register timer with game loop
                
                return Value();
            })
            .Register();
        
        registry.DefineBlock("time.clear_timer")
            .DisplayName("Clear Timer")
            .Description("Stop and remove a timer")
            .Icon("⏹")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Time)
            .Authority(NetworkAuthority::Local)
            .Input("name", ValueType::String, Value("Timer1"))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string name = s_TimeVM.GetSlotValue(block->GetInputSlot("name"), ctx).AsString();
                
                // TODO: Clear timer
                
                return Value();
            })
            .Register();
        
        //=====================================================================
        // Cooldown Timer Blocks
        //=====================================================================
        
        registry.DefineBlock("time.cooldown_start")
            .DisplayName("Start Cooldown")
            .Description("Start a cooldown timer")
            .Icon("⏳")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::Time)
            .ChangesState(true)  // Server authoritative
            .Input("name", ValueType::String, Value("Cooldown1"))
            .Input("duration", ValueType::Float, Value(5.0))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string name = s_TimeVM.GetSlotValue(block->GetInputSlot("name"), ctx).AsString();
                f64 duration = s_TimeVM.GetSlotValue(block->GetInputSlot("duration"), ctx).AsFloat();
                
                // Store cooldown end time
                f64 endTime = ctx.GetGameTime() + duration;
                ctx.SetSyncedVariable("_cooldown_" + name, Value(endTime));
                
                return Value();
            })
            .Register();
        
        registry.DefineBlock("time.cooldown_ready")
            .DisplayName("Is Cooldown Ready")
            .Description("Check if a cooldown has finished")
            .Icon("✅")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Time)
            .ReturnsValue(ValueType::Bool)
            .Input("name", ValueType::String, Value("Cooldown1"))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string name = s_TimeVM.GetSlotValue(block->GetInputSlot("name"), ctx).AsString();
                
                Value endTimeValue = ctx.GetSyncedVariable("_cooldown_" + name);
                if (endTimeValue.IsVoid()) return Value(true);
                
                f64 endTime = endTimeValue.AsFloat();
                return Value(ctx.GetGameTime() >= endTime);
            })
            .Register();
        
        registry.DefineBlock("time.cooldown_remaining")
            .DisplayName("Cooldown Remaining")
            .Description("Get remaining time on a cooldown")
            .Icon("⏱")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Time)
            .ReturnsValue(ValueType::Float)
            .Input("name", ValueType::String, Value("Cooldown1"))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string name = s_TimeVM.GetSlotValue(block->GetInputSlot("name"), ctx).AsString();
                
                Value endTimeValue = ctx.GetSyncedVariable("_cooldown_" + name);
                if (endTimeValue.IsVoid()) return Value(0.0);
                
                f64 endTime = endTimeValue.AsFloat();
                f64 remaining = endTime - ctx.GetGameTime();
                return Value(remaining > 0 ? remaining : 0.0);
            })
            .Register();
        
        registry.DefineBlock("time.cooldown_reset")
            .DisplayName("Reset Cooldown")
            .Description("Reset a cooldown immediately")
            .Icon("🔄")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Time)
            .ChangesState(true)
            .Input("name", ValueType::String, Value("Cooldown1"))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string name = s_TimeVM.GetSlotValue(block->GetInputSlot("name"), ctx).AsString();
                ctx.SetSyncedVariable("_cooldown_" + name, Value(0.0));
                return Value();
            })
            .Register();
        
        //=====================================================================
        // Time Getters
        //=====================================================================
        
        registry.DefineBlock("time.get_delta")
            .DisplayName("Get Delta Time")
            .Description("Get time since last frame in seconds")
            .Icon("Δ")
            .Shape(BlockShape::Flat)
            .Category(BlockCategory::Time)
            .ReturnsValue(ValueType::Float)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                return Value(static_cast<f64>(ctx.GetDeltaTime()));
            })
            .Register();
        
        registry.DefineBlock("time.get_game_time")
            .DisplayName("Get Game Time")
            .Description("Get total elapsed game time in seconds")
            .Icon("🕐")
            .Shape(BlockShape::Flat)
            .Category(BlockCategory::Time)
            .ReturnsValue(ValueType::Float)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                return Value(ctx.GetGameTime());
            })
            .Register();
        
        registry.DefineBlock("time.get_server_time")
            .DisplayName("Get Server Time")
            .Description("Get synchronized server time")
            .Icon("🌐")
            .Shape(BlockShape::Flat)
            .Category(BlockCategory::Time)
            .ReturnsValue(ValueType::Float)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                // In multiplayer, this would be the synced server time
                return Value(ctx.GetGameTime());
            })
            .Register();
        
        //=====================================================================
        // Countdown Blocks
        //=====================================================================
        
        registry.DefineBlock("time.start_countdown")
            .DisplayName("Start Countdown")
            .Description("Start a countdown timer")
            .Icon("⏱")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::Time)
            .ChangesState(true)
            .Input("name", ValueType::String, Value("Countdown1"))
            .Input("from", ValueType::Float, Value(10.0))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string name = s_TimeVM.GetSlotValue(block->GetInputSlot("name"), ctx).AsString();
                f64 from = s_TimeVM.GetSlotValue(block->GetInputSlot("from"), ctx).AsFloat();
                
                // Store start time and duration
                ctx.SetSyncedVariable("_countdown_start_" + name, Value(ctx.GetGameTime()));
                ctx.SetSyncedVariable("_countdown_duration_" + name, Value(from));
                
                return Value();
            })
            .Register();
        
        registry.DefineBlock("time.get_countdown")
            .DisplayName("Get Countdown")
            .Description("Get remaining time on countdown")
            .Icon("⏱")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Time)
            .ReturnsValue(ValueType::Float)
            .Input("name", ValueType::String, Value("Countdown1"))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string name = s_TimeVM.GetSlotValue(block->GetInputSlot("name"), ctx).AsString();
                
                Value startValue = ctx.GetSyncedVariable("_countdown_start_" + name);
                Value durationValue = ctx.GetSyncedVariable("_countdown_duration_" + name);
                
                if (startValue.IsVoid() || durationValue.IsVoid()) return Value(0.0);
                
                f64 startTime = startValue.AsFloat();
                f64 duration = durationValue.AsFloat();
                f64 elapsed = ctx.GetGameTime() - startTime;
                f64 remaining = duration - elapsed;
                
                return Value(remaining > 0 ? remaining : 0.0);
            })
            .Register();
        
        registry.DefineBlock("time.is_countdown_finished")
            .DisplayName("Is Countdown Finished")
            .Description("Check if countdown has reached zero")
            .Icon("✓")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Time)
            .ReturnsValue(ValueType::Bool)
            .Input("name", ValueType::String, Value("Countdown1"))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string name = s_TimeVM.GetSlotValue(block->GetInputSlot("name"), ctx).AsString();
                
                Value startValue = ctx.GetSyncedVariable("_countdown_start_" + name);
                Value durationValue = ctx.GetSyncedVariable("_countdown_duration_" + name);
                
                if (startValue.IsVoid() || durationValue.IsVoid()) return Value(true);
                
                f64 startTime = startValue.AsFloat();
                f64 duration = durationValue.AsFloat();
                f64 elapsed = ctx.GetGameTime() - startTime;
                
                return Value(elapsed >= duration);
            })
            .Register();
    }
}
