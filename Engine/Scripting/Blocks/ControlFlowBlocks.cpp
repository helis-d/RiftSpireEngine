#include "ControlFlowBlocks.h"
#include "../Execution/ExecutionContext.h"
#include "../Execution/ScriptVM.h"

namespace RiftSpire
{
    static ScriptVM s_ControlVM;
    
    void RegisterControlFlowBlocks()
    {
        auto& registry = BlockRegistry::Get();
        
        //=====================================================================
        // Conditional Blocks
        //=====================================================================
        
        registry.DefineBlock("control.if")
            .DisplayName("If")
            .Description("Execute blocks if condition is true")
            .Icon("❓")
            .Shape(BlockShape::ConditionalNested)
            .Category(BlockCategory::ControlFlow)
            .Input("condition", ValueType::Bool, Value(true))
            .NestedBody("then")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value condition = s_ControlVM.GetSlotValue(block->GetInputSlot("condition"), ctx);
                
                if (condition.AsBool())
                {
                    if (auto* slot = block->GetNestedSlot("then"))
                    {
                        return s_ControlVM.ExecuteNestedBlocks(slot, ctx);
                    }
                }
                
                return Value();
            })
            .Register();
        
        registry.DefineBlock("control.if_else")
            .DisplayName("If-Else")
            .Description("Execute different blocks based on condition")
            .Icon("❓")
            .Shape(BlockShape::MultiNested)
            .Category(BlockCategory::ControlFlow)
            .Input("condition", ValueType::Bool, Value(true))
            .NestedBody("then")
            .NestedBody("else")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value condition = s_ControlVM.GetSlotValue(block->GetInputSlot("condition"), ctx);
                
                if (condition.AsBool())
                {
                    if (auto* slot = block->GetNestedSlot("then"))
                    {
                        return s_ControlVM.ExecuteNestedBlocks(slot, ctx);
                    }
                }
                else
                {
                    if (auto* slot = block->GetNestedSlot("else"))
                    {
                        return s_ControlVM.ExecuteNestedBlocks(slot, ctx);
                    }
                }
                
                return Value();
            })
            .Register();
        
        //=====================================================================
        // Loop Blocks
        //=====================================================================
        
        registry.DefineBlock("control.repeat")
            .DisplayName("Repeat")
            .Description("Repeat blocks a number of times")
            .Icon("🔁")
            .Shape(BlockShape::LoopNested)
            .Category(BlockCategory::ControlFlow)
            .Input("count", ValueType::Int, Value(10))
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                i64 count = s_ControlVM.GetSlotValue(block->GetInputSlot("count"), ctx).AsInt();
                auto* bodySlot = block->GetNestedSlot("body");
                
                if (!bodySlot) return Value();
                
                Value lastResult;
                for (i64 i = 0; i < count; ++i)
                {
                    ctx.SetIterationIndex(i);
                    lastResult = s_ControlVM.ExecuteNestedBlocks(bodySlot, ctx);
                    
                    if (ctx.IsBreakRequested())
                    {
                        ctx.ClearBreak();
                        break;
                    }
                    if (ctx.IsContinueRequested())
                    {
                        ctx.ClearContinue();
                        continue;
                    }
                    if (ctx.IsReturnRequested() || ctx.IsStopRequested())
                    {
                        break;
                    }
                }
                
                return lastResult;
            })
            .Register();
        
        registry.DefineBlock("control.while")
            .DisplayName("While")
            .Description("Repeat blocks while condition is true")
            .Icon("🔄")
            .Shape(BlockShape::LoopNested)
            .Category(BlockCategory::ControlFlow)
            .Input("condition", ValueType::Bool, Value(true))
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                auto* bodySlot = block->GetNestedSlot("body");
                if (!bodySlot) return Value();
                
                Value lastResult;
                i64 iteration = 0;
                const i64 maxIterations = 1000000;  // Safety limit
                
                while (true)
                {
                    // Re-evaluate condition each iteration
                    Value condition = s_ControlVM.GetSlotValue(block->GetInputSlot("condition"), ctx);
                    if (!condition.AsBool()) break;
                    
                    if (iteration++ >= maxIterations)
                    {
                        // Prevent infinite loops
                        break;
                    }
                    
                    ctx.SetIterationIndex(iteration);
                    lastResult = s_ControlVM.ExecuteNestedBlocks(bodySlot, ctx);
                    
                    if (ctx.IsBreakRequested())
                    {
                        ctx.ClearBreak();
                        break;
                    }
                    if (ctx.IsContinueRequested())
                    {
                        ctx.ClearContinue();
                        continue;
                    }
                    if (ctx.IsReturnRequested() || ctx.IsStopRequested())
                    {
                        break;
                    }
                }
                
                return lastResult;
            })
            .Register();
        
        registry.DefineBlock("control.forever")
            .DisplayName("Forever")
            .Description("Repeat blocks forever (until stopped)")
            .Icon("∞")
            .Shape(BlockShape::LoopNested)
            .Category(BlockCategory::ControlFlow)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                auto* bodySlot = block->GetNestedSlot("body");
                if (!bodySlot) return Value();
                
                Value lastResult;
                i64 iteration = 0;
                const i64 maxIterations = 1000000;
                
                while (iteration++ < maxIterations)
                {
                    ctx.SetIterationIndex(iteration);
                    lastResult = s_ControlVM.ExecuteNestedBlocks(bodySlot, ctx);
                    
                    if (ctx.IsBreakRequested())
                    {
                        ctx.ClearBreak();
                        break;
                    }
                    if (ctx.IsContinueRequested())
                    {
                        ctx.ClearContinue();
                        continue;
                    }
                    if (ctx.IsReturnRequested() || ctx.IsStopRequested())
                    {
                        break;
                    }
                }
                
                return lastResult;
            })
            .Register();
        
        registry.DefineBlock("control.for_each")
            .DisplayName("For Each")
            .Description("Iterate over items in a list")
            .Icon("📝")
            .Shape(BlockShape::LoopNested)
            .Category(BlockCategory::ControlFlow)
            .Input("list", ValueType::List)
            .NestedBody("body")
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value listValue = s_ControlVM.GetSlotValue(block->GetInputSlot("list"), ctx);
                auto* bodySlot = block->GetNestedSlot("body");
                
                if (!listValue.IsList() || !bodySlot) return Value();
                
                const auto& list = listValue.AsList();
                Value lastResult;
                
                for (size_t i = 0; i < list.size(); ++i)
                {
                    ctx.SetIterationIndex(static_cast<i64>(i));
                    ctx.SetIterationItem(list[i]);
                    
                    lastResult = s_ControlVM.ExecuteNestedBlocks(bodySlot, ctx);
                    
                    if (ctx.IsBreakRequested())
                    {
                        ctx.ClearBreak();
                        break;
                    }
                    if (ctx.IsContinueRequested())
                    {
                        ctx.ClearContinue();
                        continue;
                    }
                    if (ctx.IsReturnRequested() || ctx.IsStopRequested())
                    {
                        break;
                    }
                }
                
                return lastResult;
            })
            .Register();
        
        //=====================================================================
        // Control Statements
        //=====================================================================
        
        registry.DefineBlock("control.break")
            .DisplayName("Break")
            .Description("Exit the current loop")
            .Icon("⛔")
            .Shape(BlockShape::Flat)
            .Category(BlockCategory::ControlFlow)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                ctx.RequestBreak();
                return Value();
            })
            .Register();
        
        registry.DefineBlock("control.continue")
            .DisplayName("Continue")
            .Description("Skip to the next loop iteration")
            .Icon("⏭")
            .Shape(BlockShape::Flat)
            .Category(BlockCategory::ControlFlow)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                ctx.RequestContinue();
                return Value();
            })
            .Register();
        
        registry.DefineBlock("control.return")
            .DisplayName("Return")
            .Description("Return a value from the script")
            .Icon("↩")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::ControlFlow)
            .Input("value", ValueType::Any)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value returnValue = s_ControlVM.GetSlotValue(block->GetInputSlot("value"), ctx);
                ctx.RequestReturn(returnValue);
                return returnValue;
            })
            .Register();
        
        registry.DefineBlock("control.stop")
            .DisplayName("Stop Script")
            .Description("Stop executing this script entirely")
            .Icon("🛑")
            .Shape(BlockShape::Flat)
            .Category(BlockCategory::ControlFlow)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                ctx.RequestStop();
                return Value();
            })
            .Register();
        
        //=====================================================================
        // Utility Blocks
        //=====================================================================
        
        registry.DefineBlock("control.get_iteration")
            .DisplayName("Get Iteration Index")
            .Description("Get the current loop iteration index (0-based)")
            .Icon("🔢")
            .Shape(BlockShape::Flat)
            .Category(BlockCategory::ControlFlow)
            .ReturnsValue(ValueType::Int)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                return Value(ctx.GetIterationIndex());
            })
            .Register();
        
        registry.DefineBlock("control.get_item")
            .DisplayName("Get Current Item")
            .Description("Get the current item in a for-each loop")
            .Icon("📦")
            .Shape(BlockShape::Flat)
            .Category(BlockCategory::ControlFlow)
            .ReturnsValue(ValueType::Any)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                return ctx.GetIterationItem();
            })
            .Register();
    }
}
