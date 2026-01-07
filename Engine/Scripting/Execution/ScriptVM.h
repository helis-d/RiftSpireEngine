#pragma once

#include "ExecutionContext.h"
#include "../Core/Block.h"
#include "../Core/BlockScript.h"
#include <Core/UUID.h>
#include <functional>
#include <queue>
#include <chrono>
#include <unordered_set>

namespace RiftSpire
{
    //=========================================================================
    // ScriptVM - Virtual Machine for executing block scripts
    //=========================================================================
    
    class ScriptVM
    {
    public:
        ScriptVM() = default;
        ~ScriptVM() = default;
        
        //---------------------------------------------------------------------
        // Execution
        //---------------------------------------------------------------------
        
        /// Execute a complete script from start
        Value Execute(BlockScript* script, ExecutionContext& context);
        
        /// Execute a single event handler
        Value ExecuteEvent(BlockScript* script, const std::string& eventName, ExecutionContext& context);
        
        /// Execute a chain of blocks starting from the given block
        Value ExecuteChain(BlockPtr startBlock, ExecutionContext& context);
        
        /// Execute a single block
        Value ExecuteBlock(Block* block, ExecutionContext& context);
        
        //---------------------------------------------------------------------
        // Value evaluation
        //---------------------------------------------------------------------
        
        /// Evaluate a value block (expression)
        Value EvaluateValue(Block* block, ExecutionContext& context);
        
        /// Get value from a slot (evaluates connected block or returns default)
        Value GetSlotValue(const BlockSlot* slot, ExecutionContext& context);
        
        //---------------------------------------------------------------------
        // Nested block execution
        //---------------------------------------------------------------------
        
        /// Execute all blocks in a nested slot
        Value ExecuteNestedBlocks(const BlockSlot* slot, ExecutionContext& context);
        
        //---------------------------------------------------------------------
        // Debug mode
        //---------------------------------------------------------------------
        
        void SetDebugMode(bool debug) { m_DebugMode = debug; }
        bool IsDebugMode() const { return m_DebugMode; }
        
        void SetBreakpoint(const UUID& blockId);
        void RemoveBreakpoint(const UUID& blockId);
        void ClearBreakpoints();
        bool HasBreakpoint(const UUID& blockId) const;
        
        /// Step to next block (debug mode)
        void StepOver(ExecutionContext& context);
        void StepInto(ExecutionContext& context);
        void StepOut(ExecutionContext& context);
        void Continue(ExecutionContext& context);
        
        /// Pause/resume execution
        bool IsPaused() const { return m_Paused; }
        void Pause() { m_Paused = true; }
        void Resume() { m_Paused = false; }
        
        //---------------------------------------------------------------------
        // Callbacks
        //---------------------------------------------------------------------
        
        using BlockCallback = std::function<void(Block*, ExecutionContext&)>;
        
        void SetOnBeforeExecute(BlockCallback callback) { m_OnBeforeExecute = callback; }
        void SetOnAfterExecute(BlockCallback callback) { m_OnAfterExecute = callback; }
        void SetOnBreakpoint(BlockCallback callback) { m_OnBreakpoint = callback; }
        
        //---------------------------------------------------------------------
        // Async/Delayed execution
        //---------------------------------------------------------------------
        
        struct DelayedExecution
        {
            BlockPtr Block;
            ExecutionContext Context;
            std::chrono::steady_clock::time_point ExecuteAt;
        };
        
        void ScheduleDelayed(BlockPtr block, ExecutionContext context, f32 delaySeconds);
        void UpdateDelayed(f32 deltaTime);
        void ClearDelayed();
        
        //---------------------------------------------------------------------
        // Statistics
        //---------------------------------------------------------------------
        
        struct ExecutionStats
        {
            u64 BlocksExecuted = 0;
            u64 ValuesEvaluated = 0;
            f64 TotalExecutionTimeMs = 0.0;
            u64 MaxRecursionDepth = 0;
        };
        
        const ExecutionStats& GetStats() const { return m_Stats; }
        void ResetStats() { m_Stats = ExecutionStats{}; }
        
        //---------------------------------------------------------------------
        // Limits (prevent infinite loops)
        //---------------------------------------------------------------------
        
        void SetMaxIterations(u64 max) { m_MaxIterations = max; }
        void SetMaxRecursionDepth(u64 max) { m_MaxRecursionDepth = max; }
        void SetMaxExecutionTimeMs(f64 max) { m_MaxExecutionTimeMs = max; }
        
    private:
        bool m_DebugMode = false;
        bool m_Paused = false;
        
        std::unordered_set<UUID> m_Breakpoints;
        
        BlockCallback m_OnBeforeExecute;
        BlockCallback m_OnAfterExecute;
        BlockCallback m_OnBreakpoint;
        
        // Comparator for delayed execution queue
        struct DelayedExecutionCompare
        {
            bool operator()(const DelayedExecution& a, const DelayedExecution& b) const
            {
                return a.ExecuteAt > b.ExecuteAt; // Later times have lower priority
            }
        };
        
        std::priority_queue<
            DelayedExecution,
            std::vector<DelayedExecution>,
            DelayedExecutionCompare
        > m_DelayedQueue;
        
        ExecutionStats m_Stats;
        
        // Limits
        u64 m_MaxIterations = 1000000;
        u64 m_MaxRecursionDepth = 100;
        f64 m_MaxExecutionTimeMs = 1000.0;  // 1 second
        
        // Current state
        u64 m_CurrentRecursionDepth = 0;
        u64 m_CurrentIterations = 0;
        std::chrono::steady_clock::time_point m_ExecutionStart;
        
        bool CheckLimits();
    };
}
