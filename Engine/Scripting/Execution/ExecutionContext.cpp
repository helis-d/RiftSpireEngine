#include "ExecutionContext.h"

namespace RiftSpire
{
    ExecutionContext::ExecutionContext(Scene* scene)
        : m_Scene(scene)
    {
        // Start with one global scope
        m_ScopeStack.push(Scope{});
    }
    
    //=========================================================================
    // Variable Management
    //=========================================================================
    
    void ExecutionContext::SetLocalVariable(const std::string& name, const Value& value)
    {
        if (m_ScopeStack.empty())
        {
            m_ScopeStack.push(Scope{});
        }
        m_ScopeStack.top().LocalVariables[name] = value;
    }
    
    Value ExecutionContext::GetLocalVariable(const std::string& name) const
    {
        // Search from current scope to outer scopes
        std::stack<Scope> tempStack = m_ScopeStack;
        
        while (!tempStack.empty())
        {
            const Scope& scope = tempStack.top();
            auto it = scope.LocalVariables.find(name);
            if (it != scope.LocalVariables.end())
            {
                return it->second;
            }
            tempStack.pop();
        }
        
        return Value();
    }
    
    bool ExecutionContext::HasLocalVariable(const std::string& name) const
    {
        std::stack<Scope> tempStack = m_ScopeStack;
        
        while (!tempStack.empty())
        {
            const Scope& scope = tempStack.top();
            if (scope.LocalVariables.find(name) != scope.LocalVariables.end())
            {
                return true;
            }
            tempStack.pop();
        }
        
        return false;
    }
    
    void ExecutionContext::SetSyncedVariable(const std::string& name, const Value& value)
    {
        m_SyncedVariables[name] = value;
        
        // TODO: Mark for network sync
    }
    
    Value ExecutionContext::GetSyncedVariable(const std::string& name) const
    {
        auto it = m_SyncedVariables.find(name);
        if (it != m_SyncedVariables.end())
        {
            return it->second;
        }
        return Value();
    }
    
    bool ExecutionContext::HasSyncedVariable(const std::string& name) const
    {
        return m_SyncedVariables.find(name) != m_SyncedVariables.end();
    }
    
    Value ExecutionContext::GetVariable(const std::string& name) const
    {
        // Check local first
        if (HasLocalVariable(name))
        {
            return GetLocalVariable(name);
        }
        
        // Then synced
        return GetSyncedVariable(name);
    }
    
    void ExecutionContext::SetVariable(const std::string& name, const Value& value)
    {
        // If it exists as synced, update synced
        if (HasSyncedVariable(name))
        {
            SetSyncedVariable(name, value);
        }
        else
        {
            SetLocalVariable(name, value);
        }
    }
    
    //=========================================================================
    // Scope Management
    //=========================================================================
    
    void ExecutionContext::PushScope()
    {
        m_ScopeStack.push(Scope{});
    }
    
    void ExecutionContext::PopScope()
    {
        if (m_ScopeStack.size() > 1)  // Keep at least one scope
        {
            m_ScopeStack.pop();
        }
    }
    
    //=========================================================================
    // Control Flow
    //=========================================================================
    
    void ExecutionContext::RequestReturn(const Value& value)
    {
        m_ReturnRequested = true;
        m_ReturnValue = value;
    }
    
    void ExecutionContext::ClearControlFlow()
    {
        m_BreakRequested = false;
        m_ContinueRequested = false;
        m_ReturnRequested = false;
        m_StopRequested = false;
        m_ReturnValue = Value();
    }
}
