#include "ScriptVM.h"
#include "../Core/BlockScript.h"
// #include <Core/Logger.h>  // TODO: Integrate logger

namespace RiftSpire
{
    //=========================================================================
    // Main Execution
    //=========================================================================
    
    Value ScriptVM::Execute(BlockScript* script, ExecutionContext& context)
    {
        if (!script) return Value();
        
        m_ExecutionStart = std::chrono::steady_clock::now();
        m_CurrentIterations = 0;
        m_CurrentRecursionDepth = 0;
        
        context.SetDebugMode(m_DebugMode);
        
        // Execute all event handlers (they run in parallel in a real scenario)
        // For now, we just find and return the result of any "start" events
        auto eventBlocks = script->GetEventBlocks("events.on_start");
        
        Value result;
        for (const auto& eventBlock : eventBlocks)
        {
            result = ExecuteChain(eventBlock, context);
            
            if (context.IsStopRequested())
            {
                break;
            }
        }
        
        return result;
    }
    
    Value ScriptVM::ExecuteEvent(BlockScript* script, const std::string& eventName, ExecutionContext& context)
    {
        if (!script) return Value();
        
        m_ExecutionStart = std::chrono::steady_clock::now();
        m_CurrentIterations = 0;
        m_CurrentRecursionDepth = 0;
        
        auto eventBlocks = script->GetEventBlocks(eventName);
        
        Value result;
        for (const auto& eventBlock : eventBlocks)
        {
            result = ExecuteChain(eventBlock, context);
            
            if (context.IsStopRequested())
            {
                break;
            }
        }
        
        return result;
    }
    
    Value ScriptVM::ExecuteChain(BlockPtr startBlock, ExecutionContext& context)
    {
        if (!startBlock) return Value();
        
        BlockPtr current = startBlock;
        Value lastResult;
        
        while (current && !context.IsStopRequested())
        {
            // Check limits
            if (!CheckLimits())
            {
                // RS_ERROR("Script execution limit exceeded!");
                break;
            }
            
            // Execute the block
            lastResult = ExecuteBlock(current.get(), context);
            
            // Handle control flow
            if (context.IsBreakRequested() || context.IsContinueRequested() || context.IsReturnRequested())
            {
                break;
            }
            
            // Move to next block
            current = current->GetNextBlock();
        }
        
        return lastResult;
    }
    
    Value ScriptVM::ExecuteBlock(Block* block, ExecutionContext& context)
    {
        if (!block || block->IsDisabled()) return Value();
        
        m_CurrentIterations++;
        m_Stats.BlocksExecuted++;
        
        // Debug: before execution callback
        if (m_OnBeforeExecute)
        {
            m_OnBeforeExecute(block, context);
        }
        
        // Check for breakpoint
        if (m_DebugMode && HasBreakpoint(block->GetId()))
        {
            m_Paused = true;
            if (m_OnBreakpoint)
            {
                m_OnBreakpoint(block, context);
            }
            
            // Wait for continue (in real implementation, this would yield)
            while (m_Paused)
            {
                // In real implementation: yield to debug UI
                break;  // For now, just continue
            }
        }
        
        context.SetCurrentBlock(block);
        
        // Execute the block
        Value result = block->Execute(context);
        
        // Debug: after execution callback
        if (m_OnAfterExecute)
        {
            m_OnAfterExecute(block, context);
        }
        
        return result;
    }
    
    //=========================================================================
    // Value Evaluation
    //=========================================================================
    
    Value ScriptVM::EvaluateValue(Block* block, ExecutionContext& context)
    {
        if (!block) return Value();
        
        m_Stats.ValuesEvaluated++;
        
        // For value blocks, execution returns the value
        return ExecuteBlock(block, context);
    }
    
    Value ScriptVM::GetSlotValue(const BlockSlot* slot, ExecutionContext& context)
    {
        if (!slot) return Value();
        
        // If slot has a connected block, evaluate it
        BlockPtr connected = slot->GetConnectedBlock();
        if (connected)
        {
            return EvaluateValue(connected.get(), context);
        }
        
        // Otherwise return default value
        return slot->GetDefaultValue();
    }
    
    //=========================================================================
    // Nested Block Execution
    //=========================================================================
    
    Value ScriptVM::ExecuteNestedBlocks(const BlockSlot* slot, ExecutionContext& context)
    {
        if (!slot) return Value();
        
        const auto& nestedBlocks = slot->GetNestedBlocks();
        if (nestedBlocks.empty()) return Value();
        
        m_CurrentRecursionDepth++;
        context.PushScope();
        
        Value result;
        
        // Execute nested blocks as a chain
        // Find the first block (no previous block among the nested set)
        BlockPtr firstNested = nullptr;
        for (const auto& block : nestedBlocks)
        {
            bool isFirst = true;
            for (const auto& other : nestedBlocks)
            {
                if (other->GetNextBlock() == block)
                {
                    isFirst = false;
                    break;
                }
            }
            if (isFirst)
            {
                firstNested = block;
                break;
            }
        }
        
        if (firstNested)
        {
            result = ExecuteChain(firstNested, context);
        }
        
        context.PopScope();
        m_CurrentRecursionDepth--;
        
        return result;
    }
    
    //=========================================================================
    // Breakpoints
    //=========================================================================
    
    void ScriptVM::SetBreakpoint(const UUID& blockId)
    {
        m_Breakpoints.insert(blockId);
    }
    
    void ScriptVM::RemoveBreakpoint(const UUID& blockId)
    {
        m_Breakpoints.erase(blockId);
    }
    
    void ScriptVM::ClearBreakpoints()
    {
        m_Breakpoints.clear();
    }
    
    bool ScriptVM::HasBreakpoint(const UUID& blockId) const
    {
        return m_Breakpoints.find(blockId) != m_Breakpoints.end();
    }
    
    //=========================================================================
    // Step Execution (Debug)
    //=========================================================================
    
    void ScriptVM::StepOver(ExecutionContext& context)
    {
        // Execute current block and stop at next
        m_Paused = false;
        // Implementation would involve setting a temporary breakpoint
    }
    
    void ScriptVM::StepInto(ExecutionContext& context)
    {
        // Step into nested blocks
        m_Paused = false;
    }
    
    void ScriptVM::StepOut(ExecutionContext& context)
    {
        // Execute until returning from current nested scope
        m_Paused = false;
    }
    
    void ScriptVM::Continue(ExecutionContext& context)
    {
        m_Paused = false;
    }
    
    //=========================================================================
    // Delayed Execution
    //=========================================================================
    
    void ScriptVM::ScheduleDelayed(BlockPtr block, ExecutionContext context, f32 delaySeconds)
    {
        auto executeAt = std::chrono::steady_clock::now() + 
                         std::chrono::milliseconds(static_cast<i64>(delaySeconds * 1000));
        
        m_DelayedQueue.push({block, std::move(context), executeAt});
    }
    
    void ScriptVM::UpdateDelayed(f32 deltaTime)
    {
        auto now = std::chrono::steady_clock::now();
        
        while (!m_DelayedQueue.empty())
        {
            const auto& top = m_DelayedQueue.top();
            if (top.ExecuteAt <= now)
            {
                DelayedExecution exec = m_DelayedQueue.top();
                m_DelayedQueue.pop();
                
                ExecuteChain(exec.Block, exec.Context);
            }
            else
            {
                break;
            }
        }
    }
    
    void ScriptVM::ClearDelayed()
    {
        while (!m_DelayedQueue.empty())
        {
            m_DelayedQueue.pop();
        }
    }
    
    //=========================================================================
    // Limit Checking
    //=========================================================================
    
    bool ScriptVM::CheckLimits()
    {
        // Check iteration limit
        if (m_CurrentIterations >= m_MaxIterations)
        {
            // RS_ERROR("Script exceeded maximum iteration count: {}", m_MaxIterations);
            return false;
        }
        
        // Check recursion limit
        if (m_CurrentRecursionDepth >= m_MaxRecursionDepth)
        {
            // RS_ERROR("Script exceeded maximum recursion depth: {}", m_MaxRecursionDepth);
            return false;
        }
        
        // Check time limit
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<f64, std::milli>(now - m_ExecutionStart).count();
        m_Stats.TotalExecutionTimeMs = elapsed;
        
        if (elapsed >= m_MaxExecutionTimeMs)
        {
            // RS_ERROR("Script exceeded maximum execution time: {}ms", m_MaxExecutionTimeMs);
            return false;
        }
        
        // Update max depth stat
        if (m_CurrentRecursionDepth > m_Stats.MaxRecursionDepth)
        {
            m_Stats.MaxRecursionDepth = m_CurrentRecursionDepth;
        }
        
        return true;
    }
}
