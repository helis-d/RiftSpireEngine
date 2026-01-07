#include "DebugBlocks.h"
#include "../Execution/ExecutionContext.h"
#include "../Execution/ScriptVM.h"
// #include <Core/Logger.h>  // TODO: Integrate logger

namespace RiftSpire
{
    static ScriptVM s_DebugVM;
    
    void RegisterDebugBlocks()
    {
        auto& registry = BlockRegistry::Get();
        
        registry.DefineBlock("debug.print")
            .DisplayName("Print")
            .Description("Print a message to the console")
            .Icon("📢")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::DebugLogging)
            .Authority(NetworkAuthority::Local)
            .Input("message", ValueType::String, Value("Hello!"))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string msg = s_DebugVM.GetSlotValue(block->GetInputSlot("message"), ctx).AsString();
                // TODO: Logger integration
                // RS_INFO("[Script] {}", msg);
                return Value();
            })
            .Register();
        
        registry.DefineBlock("debug.log_info")
            .DisplayName("Log Info")
            .Description("Log an info message")
            .Icon("ℹ")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::DebugLogging)
            .Authority(NetworkAuthority::Local)
            .Input("message", ValueType::String)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string msg = s_DebugVM.GetSlotValue(block->GetInputSlot("message"), ctx).AsString();
                // RS_INFO("[Script] {}", msg);
                return Value();
            })
            .Register();
        
        registry.DefineBlock("debug.log_warn")
            .DisplayName("Log Warning")
            .Description("Log a warning message")
            .Icon("⚠")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::DebugLogging)
            .Authority(NetworkAuthority::Local)
            .Input("message", ValueType::String)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string msg = s_DebugVM.GetSlotValue(block->GetInputSlot("message"), ctx).AsString();
                // RS_WARN("[Script] {}", msg);
                return Value();
            })
            .Register();
        
        registry.DefineBlock("debug.log_error")
            .DisplayName("Log Error")
            .Description("Log an error message")
            .Icon("❌")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::DebugLogging)
            .Authority(NetworkAuthority::Local)
            .Input("message", ValueType::String)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string msg = s_DebugVM.GetSlotValue(block->GetInputSlot("message"), ctx).AsString();
                // RS_ERROR("[Script] {}", msg);
                return Value();
            })
            .Register();
        
        registry.DefineBlock("debug.breakpoint")
            .DisplayName("Breakpoint")
            .Description("Pause execution in debug mode")
            .Icon("🔴")
            .Shape(BlockShape::Flat)
            .Category(BlockCategory::DebugLogging)
            .Authority(NetworkAuthority::Local)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                if (ctx.IsDebugMode())
                {
                    // RS_INFO("[Script] Breakpoint hit at block {}", block->GetId().ToString());
                    // VM would handle pausing
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("debug.assert")
            .DisplayName("Assert")
            .Description("Assert that a condition is true")
            .Icon("✓")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::DebugLogging)
            .Authority(NetworkAuthority::Local)
            .Input("condition", ValueType::Bool, Value(true))
            .Input("message", ValueType::String, Value("Assertion failed"))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                bool condition = s_DebugVM.GetSlotValue(block->GetInputSlot("condition"), ctx).AsBool();
                std::string msg = s_DebugVM.GetSlotValue(block->GetInputSlot("message"), ctx).AsString();
                
                if (!condition)
                {
                    // RS_ERROR("[Script Assert] {}", msg);
                }
                return Value();
            })
            .Register();
    }
}
