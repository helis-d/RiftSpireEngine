#include "DataBlocks.h"
#include "../Execution/ExecutionContext.h"
#include "../Execution/ScriptVM.h"

namespace RiftSpire
{
    static ScriptVM s_DataVM;
    
    void RegisterDataBlocks()
    {
        auto& registry = BlockRegistry::Get();
        
        //=====================================================================
        // Variable Blocks
        //=====================================================================
        
        registry.DefineBlock("data.set")
            .DisplayName("Set Variable")
            .Description("Set a variable to a value")
            .Icon("📝")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::DataVariables)
            .ChangesState(true)
            .Input("name", ValueType::String, Value("myVar"))
            .Input("value", ValueType::Any)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string name = s_DataVM.GetSlotValue(block->GetInputSlot("name"), ctx).AsString();
                Value value = s_DataVM.GetSlotValue(block->GetInputSlot("value"), ctx);
                ctx.SetVariable(name, value);
                return Value();
            })
            .Register();
        
        registry.DefineBlock("data.get")
            .DisplayName("Get Variable")
            .Description("Get the value of a variable")
            .Icon("📖")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::DataVariables)
            .ReturnsValue(ValueType::Any)
            .Input("name", ValueType::String, Value("myVar"))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string name = s_DataVM.GetSlotValue(block->GetInputSlot("name"), ctx).AsString();
                return ctx.GetVariable(name);
            })
            .Register();
        
        registry.DefineBlock("data.change")
            .DisplayName("Change Variable By")
            .Description("Change a variable by an amount")
            .Icon("➕")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::DataVariables)
            .ChangesState(true)
            .Input("name", ValueType::String, Value("myVar"))
            .Input("amount", ValueType::Float, Value(1))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string name = s_DataVM.GetSlotValue(block->GetInputSlot("name"), ctx).AsString();
                Value amount = s_DataVM.GetSlotValue(block->GetInputSlot("amount"), ctx);
                Value current = ctx.GetVariable(name);
                ctx.SetVariable(name, current + amount);
                return Value();
            })
            .Register();
        
        registry.DefineBlock("data.create_local")
            .DisplayName("Create Local Variable")
            .Description("Create a local variable (scope-limited)")
            .Icon("📌")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::DataVariables)
            .Authority(NetworkAuthority::Local)
            .Input("name", ValueType::String, Value("localVar"))
            .Input("value", ValueType::Any)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string name = s_DataVM.GetSlotValue(block->GetInputSlot("name"), ctx).AsString();
                Value value = s_DataVM.GetSlotValue(block->GetInputSlot("value"), ctx);
                ctx.SetLocalVariable(name, value);
                return Value();
            })
            .Register();
        
        registry.DefineBlock("data.create_synced")
            .DisplayName("Create Synced Variable")
            .Description("Create a network-synced variable")
            .Icon("🌐")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::DataVariables)
            .ChangesState(true)
            .Input("name", ValueType::String, Value("syncedVar"))
            .Input("value", ValueType::Any)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                std::string name = s_DataVM.GetSlotValue(block->GetInputSlot("name"), ctx).AsString();
                Value value = s_DataVM.GetSlotValue(block->GetInputSlot("value"), ctx);
                ctx.SetSyncedVariable(name, value);
                return Value();
            })
            .Register();
        
        //=====================================================================
        // Entity References
        //=====================================================================
        
        registry.DefineBlock("data.self")
            .DisplayName("Self")
            .Description("Reference to this entity")
            .Icon("👤")
            .Shape(BlockShape::Flat)
            .Category(BlockCategory::DataVariables)
            .ReturnsValue(ValueType::Entity)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                return Value::FromEntityHandle(ctx.GetSelf());
            })
            .Register();
        
        registry.DefineBlock("data.target")
            .DisplayName("Target")
            .Description("Reference to current target entity")
            .Icon("🎯")
            .Shape(BlockShape::Flat)
            .Category(BlockCategory::DataVariables)
            .ReturnsValue(ValueType::Entity)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                return Value::FromEntityHandle(ctx.GetTarget());
            })
            .Register();
        
        registry.DefineBlock("data.owner")
            .DisplayName("Owner")
            .Description("Reference to owner entity (e.g., projectile owner)")
            .Icon("👑")
            .Shape(BlockShape::Flat)
            .Category(BlockCategory::DataVariables)
            .ReturnsValue(ValueType::Entity)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                return Value::FromEntityHandle(ctx.GetOwner());
            })
            .Register();
        
        //=====================================================================
        // List Operations
        //=====================================================================
        
        registry.DefineBlock("data.list_create")
            .DisplayName("Create List")
            .Description("Create an empty list")
            .Icon("📋")
            .Shape(BlockShape::Flat)
            .Category(BlockCategory::DataVariables)
            .ReturnsValue(ValueType::List)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                return Value::CreateList();
            })
            .Register();
        
        registry.DefineBlock("data.list_add")
            .DisplayName("Add to List")
            .Description("Add an item to a list")
            .Icon("➕")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::DataVariables)
            .ChangesState(true)
            .Input("list", ValueType::List)
            .Input("item", ValueType::Any)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value listValue = s_DataVM.GetSlotValue(block->GetInputSlot("list"), ctx);
                Value item = s_DataVM.GetSlotValue(block->GetInputSlot("item"), ctx);
                
                if (listValue.IsList())
                {
                    listValue.AsList().push_back(item);
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("data.list_get")
            .DisplayName("Get from List")
            .Description("Get an item from a list by index")
            .Icon("📍")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::DataVariables)
            .ReturnsValue(ValueType::Any)
            .Input("list", ValueType::List)
            .Input("index", ValueType::Int, Value(0))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value listValue = s_DataVM.GetSlotValue(block->GetInputSlot("list"), ctx);
                i64 index = s_DataVM.GetSlotValue(block->GetInputSlot("index"), ctx).AsInt();
                
                if (listValue.IsList())
                {
                    const auto& list = listValue.AsList();
                    if (index >= 0 && static_cast<size_t>(index) < list.size())
                    {
                        return list[index];
                    }
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("data.list_length")
            .DisplayName("List Length")
            .Description("Get the number of items in a list")
            .Icon("📏")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::DataVariables)
            .ReturnsValue(ValueType::Int)
            .Input("list", ValueType::List)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value listValue = s_DataVM.GetSlotValue(block->GetInputSlot("list"), ctx);
                
                if (listValue.IsList())
                {
                    return Value(static_cast<i64>(listValue.AsList().size()));
                }
                return Value(0LL);
            })
            .Register();
        
        registry.DefineBlock("data.list_remove")
            .DisplayName("Remove from List")
            .Description("Remove an item from a list by index")
            .Icon("➖")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::DataVariables)
            .ChangesState(true)
            .Input("list", ValueType::List)
            .Input("index", ValueType::Int, Value(0))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value listValue = s_DataVM.GetSlotValue(block->GetInputSlot("list"), ctx);
                i64 index = s_DataVM.GetSlotValue(block->GetInputSlot("index"), ctx).AsInt();
                
                if (listValue.IsList())
                {
                    auto& list = listValue.AsList();
                    if (index >= 0 && static_cast<size_t>(index) < list.size())
                    {
                        list.erase(list.begin() + index);
                    }
                }
                return Value();
            })
            .Register();
        
        registry.DefineBlock("data.list_clear")
            .DisplayName("Clear List")
            .Description("Remove all items from a list")
            .Icon("🗑")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::DataVariables)
            .ChangesState(true)
            .Input("list", ValueType::List)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value listValue = s_DataVM.GetSlotValue(block->GetInputSlot("list"), ctx);
                
                if (listValue.IsList())
                {
                    listValue.AsList().clear();
                }
                return Value();
            })
            .Register();
        
        //=====================================================================
        // Literal Values
        //=====================================================================
        
        registry.DefineBlock("data.number")
            .DisplayName("Number")
            .Description("A number value")
            .Icon("🔢")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::DataVariables)
            .ReturnsValue(ValueType::Float)
            .Input("value", ValueType::Float, Value(0.0))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                return s_DataVM.GetSlotValue(block->GetInputSlot("value"), ctx);
            })
            .Register();
        
        registry.DefineBlock("data.text")
            .DisplayName("Text")
            .Description("A text string value")
            .Icon("📝")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::DataVariables)
            .ReturnsValue(ValueType::String)
            .Input("value", ValueType::String, Value(""))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                return s_DataVM.GetSlotValue(block->GetInputSlot("value"), ctx);
            })
            .Register();
        
        registry.DefineBlock("data.true")
            .DisplayName("True")
            .Description("Boolean true value")
            .Icon("✓")
            .Shape(BlockShape::Flat)
            .Category(BlockCategory::DataVariables)
            .ReturnsValue(ValueType::Bool)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                return Value(true);
            })
            .Register();
        
        registry.DefineBlock("data.false")
            .DisplayName("False")
            .Description("Boolean false value")
            .Icon("✗")
            .Shape(BlockShape::Flat)
            .Category(BlockCategory::DataVariables)
            .ReturnsValue(ValueType::Bool)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                return Value(false);
            })
            .Register();
    }
}
