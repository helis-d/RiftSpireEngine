#include "ExecutionStack.h"
#include "../Core/Block.h"

namespace RiftSpire
{
    //=========================================================================
    // ExecutionStack Implementation
    //=========================================================================
    
    ExecutionStack::ExecutionStack()
        : m_StackId(UUID::Generate())
    {
    }
    
    ExecutionStack::ExecutionStack(const UUID& id)
        : m_StackId(id)
    {
    }
    
    //-------------------------------------------------------------------------
    // Stack Frame Management
    //-------------------------------------------------------------------------
    
    void ExecutionStack::PushFrame(Block* block)
    {
        StackFrame frame;
        frame.Block = block;
        frame.ChildIndex = 0;
        frame.BranchIndex = 0;
        frame.LoopIteration = 0;
        m_Frames.push_back(frame);
    }
    
    void ExecutionStack::PopFrame()
    {
        if (!m_Frames.empty())
        {
            m_Frames.pop_back();
        }
    }
    
    StackFrame& ExecutionStack::CurrentFrame()
    {
        static StackFrame s_EmptyFrame;
        if (m_Frames.empty())
        {
            return s_EmptyFrame;
        }
        return m_Frames.back();
    }
    
    const StackFrame& ExecutionStack::CurrentFrame() const
    {
        static StackFrame s_EmptyFrame;
        if (m_Frames.empty())
        {
            return s_EmptyFrame;
        }
        return m_Frames.back();
    }
    
    //-------------------------------------------------------------------------
    // Wait Management
    //-------------------------------------------------------------------------
    
    void ExecutionStack::StartWait(WaitType type, float duration)
    {
        m_WaitType = type;
        m_WaitDuration = duration;
        m_WaitTimer = duration;
        m_State = ExecutionState::Waiting;
    }
    
    void ExecutionStack::StartWaitCondition(const WaitCondition& condition)
    {
        m_WaitType = WaitType::Condition;
        m_WaitCondition = condition;
        m_State = ExecutionState::Waiting;
    }
    
    void ExecutionStack::UpdateWait(float deltaTime)
    {
        if (m_State != ExecutionState::Waiting)
        {
            return;
        }
        
        switch (m_WaitType)
        {
            case WaitType::Seconds:
            case WaitType::CastTime:
            case WaitType::Channeling:
            case WaitType::Animation:
                m_WaitTimer -= deltaTime;
                if (m_WaitTimer <= 0.0f)
                {
                    ClearWait();
                }
                break;
                
            case WaitType::Condition:
                if (m_WaitCondition.Predicate && m_WaitCondition.Predicate())
                {
                    ClearWait();
                }
                break;
                
            case WaitType::NextFrame:
                // Her zaman bir sonraki frame'de tamamlanir
                ClearWait();
                break;
                
            case WaitType::None:
            default:
                break;
        }
    }
    
    bool ExecutionStack::IsWaitComplete() const
    {
        if (m_State != ExecutionState::Waiting)
        {
            return true;
        }
        
        switch (m_WaitType)
        {
            case WaitType::Seconds:
            case WaitType::CastTime:
            case WaitType::Channeling:
            case WaitType::Animation:
                return m_WaitTimer <= 0.0f;
                
            case WaitType::Condition:
                return m_WaitCondition.Predicate && m_WaitCondition.Predicate();
                
            case WaitType::NextFrame:
                return true;  // Her zaman tamamlandi kabul edilir
                
            case WaitType::None:
            default:
                return true;
        }
    }
    
    void ExecutionStack::ClearWait()
    {
        m_WaitType = WaitType::None;
        m_WaitTimer = 0.0f;
        m_WaitDuration = 0.0f;
        m_WaitCondition = WaitCondition{};
        
        // Durumu Active'e geri al
        if (m_State == ExecutionState::Waiting)
        {
            m_State = ExecutionState::Active;
        }
    }
    
    //-------------------------------------------------------------------------
    // Cancellation
    //-------------------------------------------------------------------------
    
    void ExecutionStack::Cancel(CancelReason reason)
    {
        m_CancelReason = reason;
        m_State = ExecutionState::Cancelled;
        
        // Tum frame'lere iptal bildirimi gonder
        for (auto& frame : m_Frames)
        {
            if (frame.Block)
            {
                // Block::OnExecutionCancelled varmı kontrol et
                // frame.Block->OnExecutionCancelled(reason);
            }
        }
        
        // Temizlik
        ClearWait();
    }
}
