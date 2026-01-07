#pragma once

#include "../Core/Block.h"
#include "../Core/BlockTypes.h"
#include "../Core/Value.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <stack>

using i64 = std::int64_t;
using u64 = std::uint64_t;
using f32 = float;
using f64 = double;

namespace RiftSpire
{
    // Forward declarations
    class Entity;
    class Scene;
    class BlockScript;
    
    //=========================================================================
    // ExecutionContext - Runtime context for script execution
    //=========================================================================
    
    class ExecutionContext
    {
    public:
        ExecutionContext() = default;
        explicit ExecutionContext(Scene* scene);
        
        //---------------------------------------------------------------------
        // Entity context
        //---------------------------------------------------------------------
        
        void SetSelf(u64 entityHandle) { m_Self = entityHandle; }
        u64 GetSelf() const { return m_Self; }
        
        void SetTarget(u64 entityHandle) { m_Target = entityHandle; }
        u64 GetTarget() const { return m_Target; }
        
        void SetOwner(u64 entityHandle) { m_Owner = entityHandle; }
        u64 GetOwner() const { return m_Owner; }
        
        //---------------------------------------------------------------------
        // Scene access
        //---------------------------------------------------------------------
        
        Scene* GetScene() const { return m_Scene; }
        void SetScene(Scene* scene) { m_Scene = scene; }
        
        //---------------------------------------------------------------------
        // Variable management
        //---------------------------------------------------------------------
        
        // Local variables (script scope)
        void SetLocalVariable(const std::string& name, const Value& value);
        Value GetLocalVariable(const std::string& name) const;
        bool HasLocalVariable(const std::string& name) const;
        
        // Synced variables (network replicated)
        void SetSyncedVariable(const std::string& name, const Value& value);
        Value GetSyncedVariable(const std::string& name) const;
        bool HasSyncedVariable(const std::string& name) const;
        
        // Generic variable access (checks local first, then synced)
        Value GetVariable(const std::string& name) const;
        void SetVariable(const std::string& name, const Value& value);
        
        //---------------------------------------------------------------------
        // Scope management (for nested blocks)
        //---------------------------------------------------------------------
        
        void PushScope();
        void PopScope();
        size_t GetScopeDepth() const { return m_ScopeStack.size(); }
        
        //---------------------------------------------------------------------
        // Control flow
        //---------------------------------------------------------------------
        
        void RequestBreak() { m_BreakRequested = true; }
        void RequestContinue() { m_ContinueRequested = true; }
        void RequestReturn(const Value& value);
        void RequestStop() { m_StopRequested = true; }
        
        bool IsBreakRequested() const { return m_BreakRequested; }
        bool IsContinueRequested() const { return m_ContinueRequested; }
        bool IsReturnRequested() const { return m_ReturnRequested; }
        bool IsStopRequested() const { return m_StopRequested; }
        Value GetReturnValue() const { return m_ReturnValue; }
        
        void ClearBreak() { m_BreakRequested = false; }
        void ClearContinue() { m_ContinueRequested = false; }
        void ClearReturn() { m_ReturnRequested = false; m_ReturnValue = Value(); }
        void ClearControlFlow();
        
        //---------------------------------------------------------------------
        // Network context
        //---------------------------------------------------------------------
        
        bool IsServer() const { return m_IsServer; }
        bool IsClient() const { return !m_IsServer; }
        bool IsLocalPlayer() const { return m_IsLocalPlayer; }
        
        void SetIsServer(bool isServer) { m_IsServer = isServer; }
        void SetIsLocalPlayer(bool isLocal) { m_IsLocalPlayer = isLocal; }
        
        //---------------------------------------------------------------------
        // Iteration context (for loops)
        //---------------------------------------------------------------------
        
        void SetIterationIndex(i64 index) { m_IterationIndex = index; }
        i64 GetIterationIndex() const { return m_IterationIndex; }
        
        void SetIterationItem(const Value& item) { m_IterationItem = item; }
        Value GetIterationItem() const { return m_IterationItem; }
        
        //---------------------------------------------------------------------
        // Debug
        //---------------------------------------------------------------------
        
        void SetDebugMode(bool debug) { m_DebugMode = debug; }
        bool IsDebugMode() const { return m_DebugMode; }
        
        void SetCurrentBlock(Block* block) { m_CurrentBlock = block; }
        Block* GetCurrentBlock() const { return m_CurrentBlock; }
        
        //---------------------------------------------------------------------
        // Delta time (for time-based operations)
        //---------------------------------------------------------------------
        
        void SetDeltaTime(f32 dt) { m_DeltaTime = dt; }
        f32 GetDeltaTime() const { return m_DeltaTime; }
        
        void SetGameTime(f64 time) { m_GameTime = time; }
        f64 GetGameTime() const { return m_GameTime; }
        
    private:
        // Entity context
        u64 m_Self = 0;
        u64 m_Target = 0;
        u64 m_Owner = 0;
        
        // Scene
        Scene* m_Scene = nullptr;
        
        // Variables
        struct Scope
        {
            std::unordered_map<std::string, Value> LocalVariables;
        };
        std::stack<Scope> m_ScopeStack;
        std::unordered_map<std::string, Value> m_SyncedVariables;
        
        // Control flow
        bool m_BreakRequested = false;
        bool m_ContinueRequested = false;
        bool m_ReturnRequested = false;
        bool m_StopRequested = false;
        Value m_ReturnValue;
        
        // Network
        bool m_IsServer = true;
        bool m_IsLocalPlayer = true;
        
        // Iteration
        i64 m_IterationIndex = 0;
        Value m_IterationItem;
        
        // Debug
        bool m_DebugMode = false;
        Block* m_CurrentBlock = nullptr;
        
        // Time
        f32 m_DeltaTime = 0.0f;
        f64 m_GameTime = 0.0;
    };
}
