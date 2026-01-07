#include "OperatorBlocks.h"
#include "../Execution/ExecutionContext.h"
#include "../Execution/ScriptVM.h"

namespace RiftSpire
{
    // Helper to get VM for slot evaluation
    static ScriptVM s_VM;
    
    void RegisterOperatorBlocks()
    {
        auto& registry = BlockRegistry::Get();
        
        //=====================================================================
        // Arithmetic Operators
        //=====================================================================
        
        registry.DefineBlock("operators.add")
            .DisplayName("Add")
            .Description("Add two values together")
            .Icon("+")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Any)
            .Input("a", ValueType::Any, Value(0))
            .Input("b", ValueType::Any, Value(0))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value a = s_VM.GetSlotValue(block->GetInputSlot("a"), ctx);
                Value b = s_VM.GetSlotValue(block->GetInputSlot("b"), ctx);
                return a + b;
            })
            .Register();
        
        registry.DefineBlock("operators.subtract")
            .DisplayName("Subtract")
            .Description("Subtract second value from first")
            .Icon("-")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Any)
            .Input("a", ValueType::Any, Value(0))
            .Input("b", ValueType::Any, Value(0))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value a = s_VM.GetSlotValue(block->GetInputSlot("a"), ctx);
                Value b = s_VM.GetSlotValue(block->GetInputSlot("b"), ctx);
                return a - b;
            })
            .Register();
        
        registry.DefineBlock("operators.multiply")
            .DisplayName("Multiply")
            .Description("Multiply two values")
            .Icon("×")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Any)
            .Input("a", ValueType::Any, Value(1))
            .Input("b", ValueType::Any, Value(1))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value a = s_VM.GetSlotValue(block->GetInputSlot("a"), ctx);
                Value b = s_VM.GetSlotValue(block->GetInputSlot("b"), ctx);
                return a * b;
            })
            .Register();
        
        registry.DefineBlock("operators.divide")
            .DisplayName("Divide")
            .Description("Divide first value by second")
            .Icon("÷")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Any)
            .Input("a", ValueType::Any, Value(0))
            .Input("b", ValueType::Any, Value(1))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value a = s_VM.GetSlotValue(block->GetInputSlot("a"), ctx);
                Value b = s_VM.GetSlotValue(block->GetInputSlot("b"), ctx);
                return a / b;
            })
            .Register();
        
        registry.DefineBlock("operators.modulo")
            .DisplayName("Modulo")
            .Description("Get remainder of division")
            .Icon("%")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Int)
            .Input("a", ValueType::Any, Value(0))
            .Input("b", ValueType::Any, Value(1))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value a = s_VM.GetSlotValue(block->GetInputSlot("a"), ctx);
                Value b = s_VM.GetSlotValue(block->GetInputSlot("b"), ctx);
                return a % b;
            })
            .Register();
        
        registry.DefineBlock("operators.negate")
            .DisplayName("Negate")
            .Description("Negate a value (make positive negative or vice versa)")
            .Icon("−")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Any)
            .Input("value", ValueType::Any, Value(0))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value v = s_VM.GetSlotValue(block->GetInputSlot("value"), ctx);
                return -v;
            })
            .Register();
        
        //=====================================================================
        // Comparison Operators
        //=====================================================================
        
        registry.DefineBlock("operators.equals")
            .DisplayName("Equals")
            .Description("Check if two values are equal")
            .Icon("=")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Bool)
            .Input("a", ValueType::Any)
            .Input("b", ValueType::Any)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value a = s_VM.GetSlotValue(block->GetInputSlot("a"), ctx);
                Value b = s_VM.GetSlotValue(block->GetInputSlot("b"), ctx);
                return Value(a == b);
            })
            .Register();
        
        registry.DefineBlock("operators.not_equals")
            .DisplayName("Not Equals")
            .Description("Check if two values are not equal")
            .Icon("≠")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Bool)
            .Input("a", ValueType::Any)
            .Input("b", ValueType::Any)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value a = s_VM.GetSlotValue(block->GetInputSlot("a"), ctx);
                Value b = s_VM.GetSlotValue(block->GetInputSlot("b"), ctx);
                return Value(a != b);
            })
            .Register();
        
        registry.DefineBlock("operators.greater")
            .DisplayName("Greater Than")
            .Description("Check if first value is greater than second")
            .Icon(">")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Bool)
            .Input("a", ValueType::Any)
            .Input("b", ValueType::Any)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value a = s_VM.GetSlotValue(block->GetInputSlot("a"), ctx);
                Value b = s_VM.GetSlotValue(block->GetInputSlot("b"), ctx);
                return Value(a > b);
            })
            .Register();
        
        registry.DefineBlock("operators.less")
            .DisplayName("Less Than")
            .Description("Check if first value is less than second")
            .Icon("<")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Bool)
            .Input("a", ValueType::Any)
            .Input("b", ValueType::Any)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value a = s_VM.GetSlotValue(block->GetInputSlot("a"), ctx);
                Value b = s_VM.GetSlotValue(block->GetInputSlot("b"), ctx);
                return Value(a < b);
            })
            .Register();
        
        registry.DefineBlock("operators.greater_equal")
            .DisplayName("Greater or Equal")
            .Description("Check if first value is greater than or equal to second")
            .Icon("≥")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Bool)
            .Input("a", ValueType::Any)
            .Input("b", ValueType::Any)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value a = s_VM.GetSlotValue(block->GetInputSlot("a"), ctx);
                Value b = s_VM.GetSlotValue(block->GetInputSlot("b"), ctx);
                return Value(a >= b);
            })
            .Register();
        
        registry.DefineBlock("operators.less_equal")
            .DisplayName("Less or Equal")
            .Description("Check if first value is less than or equal to second")
            .Icon("≤")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Bool)
            .Input("a", ValueType::Any)
            .Input("b", ValueType::Any)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value a = s_VM.GetSlotValue(block->GetInputSlot("a"), ctx);
                Value b = s_VM.GetSlotValue(block->GetInputSlot("b"), ctx);
                return Value(a <= b);
            })
            .Register();
        
        //=====================================================================
        // Logical Operators
        //=====================================================================
        
        registry.DefineBlock("operators.and")
            .DisplayName("And")
            .Description("Returns true if both conditions are true")
            .Icon("AND")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Bool)
            .Input("a", ValueType::Bool, Value(false))
            .Input("b", ValueType::Bool, Value(false))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value a = s_VM.GetSlotValue(block->GetInputSlot("a"), ctx);
                Value b = s_VM.GetSlotValue(block->GetInputSlot("b"), ctx);
                return a && b;
            })
            .Register();
        
        registry.DefineBlock("operators.or")
            .DisplayName("Or")
            .Description("Returns true if either condition is true")
            .Icon("OR")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Bool)
            .Input("a", ValueType::Bool, Value(false))
            .Input("b", ValueType::Bool, Value(false))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value a = s_VM.GetSlotValue(block->GetInputSlot("a"), ctx);
                Value b = s_VM.GetSlotValue(block->GetInputSlot("b"), ctx);
                return a || b;
            })
            .Register();
        
        registry.DefineBlock("operators.not")
            .DisplayName("Not")
            .Description("Returns the opposite boolean value")
            .Icon("NOT")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Bool)
            .Input("value", ValueType::Bool, Value(false))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                Value v = s_VM.GetSlotValue(block->GetInputSlot("value"), ctx);
                return !v;
            })
            .Register();
        
        //=====================================================================
        // Utility Operators
        //=====================================================================
        
        registry.DefineBlock("operators.random")
            .DisplayName("Random")
            .Description("Get a random number between min and max")
            .Icon("🎲")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Float)
            .Input("min", ValueType::Float, Value(0.0))
            .Input("max", ValueType::Float, Value(1.0))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                f64 min = s_VM.GetSlotValue(block->GetInputSlot("min"), ctx).AsFloat();
                f64 max = s_VM.GetSlotValue(block->GetInputSlot("max"), ctx).AsFloat();
                
                f64 random = static_cast<f64>(rand()) / RAND_MAX;
                return Value(min + random * (max - min));
            })
            .Register();
        
        registry.DefineBlock("operators.random_int")
            .DisplayName("Random Int")
            .Description("Get a random integer between min and max (inclusive)")
            .Icon("🎲")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Int)
            .Input("min", ValueType::Int, Value(0))
            .Input("max", ValueType::Int, Value(100))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                i64 min = s_VM.GetSlotValue(block->GetInputSlot("min"), ctx).AsInt();
                i64 max = s_VM.GetSlotValue(block->GetInputSlot("max"), ctx).AsInt();
                
                if (max <= min) return Value(min);
                return Value(min + (rand() % (max - min + 1)));
            })
            .Register();
        
        registry.DefineBlock("operators.clamp")
            .DisplayName("Clamp")
            .Description("Constrain a value between min and max")
            .Icon("📏")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Float)
            .Input("value", ValueType::Float)
            .Input("min", ValueType::Float, Value(0.0))
            .Input("max", ValueType::Float, Value(1.0))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                f64 value = s_VM.GetSlotValue(block->GetInputSlot("value"), ctx).AsFloat();
                f64 min = s_VM.GetSlotValue(block->GetInputSlot("min"), ctx).AsFloat();
                f64 max = s_VM.GetSlotValue(block->GetInputSlot("max"), ctx).AsFloat();
                
                if (value < min) return Value(min);
                if (value > max) return Value(max);
                return Value(value);
            })
            .Register();
        
        registry.DefineBlock("operators.lerp")
            .DisplayName("Lerp")
            .Description("Linear interpolation between two values")
            .Icon("↔")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Float)
            .Input("a", ValueType::Float, Value(0.0))
            .Input("b", ValueType::Float, Value(1.0))
            .Input("t", ValueType::Float, Value(0.5))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                f64 a = s_VM.GetSlotValue(block->GetInputSlot("a"), ctx).AsFloat();
                f64 b = s_VM.GetSlotValue(block->GetInputSlot("b"), ctx).AsFloat();
                f64 t = s_VM.GetSlotValue(block->GetInputSlot("t"), ctx).AsFloat();
                
                return Value(a + t * (b - a));
            })
            .Register();
        
        registry.DefineBlock("operators.abs")
            .DisplayName("Absolute")
            .Description("Get the absolute value")
            .Icon("|x|")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Float)
            .Input("value", ValueType::Float)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                f64 v = s_VM.GetSlotValue(block->GetInputSlot("value"), ctx).AsFloat();
                return Value(v < 0 ? -v : v);
            })
            .Register();
        
        registry.DefineBlock("operators.floor")
            .DisplayName("Floor")
            .Description("Round down to nearest integer")
            .Icon("⌊x⌋")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Int)
            .Input("value", ValueType::Float)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                f64 v = s_VM.GetSlotValue(block->GetInputSlot("value"), ctx).AsFloat();
                return Value(static_cast<i64>(std::floor(v)));
            })
            .Register();
        
        registry.DefineBlock("operators.ceil")
            .DisplayName("Ceiling")
            .Description("Round up to nearest integer")
            .Icon("⌈x⌉")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Int)
            .Input("value", ValueType::Float)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                f64 v = s_VM.GetSlotValue(block->GetInputSlot("value"), ctx).AsFloat();
                return Value(static_cast<i64>(std::ceil(v)));
            })
            .Register();
        
        registry.DefineBlock("operators.round")
            .DisplayName("Round")
            .Description("Round to nearest integer")
            .Icon("≈")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Int)
            .Input("value", ValueType::Float)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                f64 v = s_VM.GetSlotValue(block->GetInputSlot("value"), ctx).AsFloat();
                return Value(static_cast<i64>(std::round(v)));
            })
            .Register();
        
        registry.DefineBlock("operators.sqrt")
            .DisplayName("Square Root")
            .Description("Calculate square root")
            .Icon("√")
            .Shape(BlockShape::ValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Float)
            .Input("value", ValueType::Float)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                f64 v = s_VM.GetSlotValue(block->GetInputSlot("value"), ctx).AsFloat();
                return Value(std::sqrt(v));
            })
            .Register();
        
        registry.DefineBlock("operators.pow")
            .DisplayName("Power")
            .Description("Raise base to exponent power")
            .Icon("^")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Float)
            .Input("base", ValueType::Float)
            .Input("exponent", ValueType::Float, Value(2.0))
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                f64 base = s_VM.GetSlotValue(block->GetInputSlot("base"), ctx).AsFloat();
                f64 exp = s_VM.GetSlotValue(block->GetInputSlot("exponent"), ctx).AsFloat();
                return Value(std::pow(base, exp));
            })
            .Register();
        
        registry.DefineBlock("operators.min")
            .DisplayName("Min")
            .Description("Get the smaller of two values")
            .Icon("↓")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Float)
            .Input("a", ValueType::Float)
            .Input("b", ValueType::Float)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                f64 a = s_VM.GetSlotValue(block->GetInputSlot("a"), ctx).AsFloat();
                f64 b = s_VM.GetSlotValue(block->GetInputSlot("b"), ctx).AsFloat();
                return Value(a < b ? a : b);
            })
            .Register();
        
        registry.DefineBlock("operators.max")
            .DisplayName("Max")
            .Description("Get the larger of two values")
            .Icon("↑")
            .Shape(BlockShape::MultiValueNested)
            .Category(BlockCategory::Operators)
            .ReturnsValue(ValueType::Float)
            .Input("a", ValueType::Float)
            .Input("b", ValueType::Float)
            .OnExecute([](Block* block, ExecutionContext& ctx) -> Value {
                f64 a = s_VM.GetSlotValue(block->GetInputSlot("a"), ctx).AsFloat();
                f64 b = s_VM.GetSlotValue(block->GetInputSlot("b"), ctx).AsFloat();
                return Value(a > b ? a : b);
            })
            .Register();
    }
}
