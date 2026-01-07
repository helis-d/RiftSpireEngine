#include "ExecutionEngine.h"
#include "../Core/Block.h"
#include "../Core/BlockScript.h"
#include <Core/Logger.h>
#include <algorithm>

namespace RiftSpire
{
    //=========================================================================
    // Constructor
    //=========================================================================
    
    ExecutionEngine::ExecutionEngine()
        : m_Config{}
    {
    }
    
    ExecutionEngine::ExecutionEngine(const ExecutionEngineConfig& config)
        : m_Config(config)
    {
    }
    
    //=========================================================================
    // Yigin Yonetimi
    //=========================================================================
    
    ExecutionStackPtr ExecutionEngine::CreateStack()
    {
        if (m_ActiveStacks.size() >= static_cast<size_t>(m_Config.MaxActiveStacks))
        {
            RS_WARN("ExecutionEngine: Maksimum yigin sayisina ulasildi ({})", m_Config.MaxActiveStacks);
            return nullptr;
        }
        
        auto stack = std::make_shared<ExecutionStack>();
        stack->SetState(ExecutionState::Idle);
        
        m_StackMap[stack->GetId()] = stack;
        m_ActiveStacks.push_back(stack);
        
        m_Statistics.TotalStacksCreated++;
        
        if (m_Callbacks.OnStackCreated)
        {
            m_Callbacks.OnStackCreated(*stack);
        }
        
        return stack;
    }
    
    ExecutionStackPtr ExecutionEngine::CreateStack(const AbilityContext& abilityCtx)
    {
        auto stack = CreateStack();
        if (stack)
        {
            stack->SetAbilityContext(abilityCtx);
        }
        return stack;
    }
    
    void ExecutionEngine::RegisterStack(ExecutionStackPtr stack)
    {
        if (!stack) return;
        
        if (m_ActiveStacks.size() >= static_cast<size_t>(m_Config.MaxActiveStacks))
        {
            RS_WARN("ExecutionEngine: Maksimum yigin sayisina ulasildi");
            return;
        }
        
        m_StackMap[stack->GetId()] = stack;
        m_ActiveStacks.push_back(stack);
        
        m_Statistics.TotalStacksCreated++;
        
        if (m_Callbacks.OnStackCreated)
        {
            m_Callbacks.OnStackCreated(*stack);
        }
    }
    
    void ExecutionEngine::RemoveStack(const UUID& stackId)
    {
        auto it = m_StackMap.find(stackId);
        if (it != m_StackMap.end())
        {
            auto stack = it->second;
            
            // Vector'dan kaldir
            m_ActiveStacks.erase(
                std::remove_if(m_ActiveStacks.begin(), m_ActiveStacks.end(),
                    [&stackId](const ExecutionStackPtr& s) { return s->GetId() == stackId; }),
                m_ActiveStacks.end()
            );
            
            // Map'ten kaldir
            m_StackMap.erase(it);
        }
    }
    
    ExecutionStackPtr ExecutionEngine::GetStack(const UUID& stackId)
    {
        auto it = m_StackMap.find(stackId);
        if (it != m_StackMap.end())
        {
            return it->second;
        }
        return nullptr;
    }
    
    void ExecutionEngine::CancelStacksForEntity(const UUID& entityId, CancelReason reason)
    {
        for (auto& stack : m_ActiveStacks)
        {
            if (stack->GetAbilityContext().CasterId == entityId)
            {
                stack->Cancel(reason);
                m_Statistics.TotalStacksCancelled++;
                
                if (m_Callbacks.OnStackCancelled)
                {
                    m_Callbacks.OnStackCancelled(*stack, reason);
                }
            }
        }
    }
    
    void ExecutionEngine::CancelAllStacks(CancelReason reason)
    {
        for (auto& stack : m_ActiveStacks)
        {
            if (stack->GetState() != ExecutionState::Cancelled &&
                stack->GetState() != ExecutionState::Completed)
            {
                stack->Cancel(reason);
                m_Statistics.TotalStacksCancelled++;
                
                if (m_Callbacks.OnStackCancelled)
                {
                    m_Callbacks.OnStackCancelled(*stack, reason);
                }
            }
        }
    }
    
    //=========================================================================
    // Frame Guncelleme
    //=========================================================================
    
    void ExecutionEngine::Tick(float deltaTime, ExecutionContext& globalContext)
    {
        // Debug duraklatma kontrolu
        if (m_IsPaused && !m_StepRequested)
        {
            return;
        }
        m_StepRequested = false;
        
        // Bekleme durumlarini guncelle
        UpdateWaitingStacks(deltaTime);
        
        // Frame basina toplam talimat sayaci
        int instructionsRemaining = m_Config.MaxInstructionsPerFrame;
        
        // Aktif yiginlari calistir
        for (auto& stack : m_ActiveStacks)
        {
            if (instructionsRemaining <= 0)
            {
                // Frame limiti asildi
                break;
            }
            
            if (stack->GetState() == ExecutionState::Active)
            {
                ExecuteStack(*stack, globalContext, instructionsRemaining);
            }
        }
        
        // Tamamlanan yiginlari temizle
        CleanupCompletedStacks();
        
        m_Statistics.TotalExecutionTime += deltaTime;
    }
    
    //=========================================================================
    // Yigin Calistirma
    //=========================================================================
    
    void ExecutionEngine::ExecuteStack(ExecutionStack& stack, ExecutionContext& ctx, int& instructionsRemaining)
    {
        // Ilk kez calisiyorsa baslat callback'i
        if (stack.GetInstructionCount() == 0 && m_Callbacks.OnStackStarted)
        {
            m_Callbacks.OnStackStarted(stack);
        }
        
        while (stack.GetState() == ExecutionState::Active && instructionsRemaining > 0)
        {
            Block* currentBlock = stack.GetCurrentBlock();
            
            if (!currentBlock)
            {
                // Calistirilacak blok kalmadi
                stack.SetState(ExecutionState::Completed);
                m_Statistics.TotalStacksCompleted++;
                
                if (m_Callbacks.OnStackCompleted)
                {
                    m_Callbacks.OnStackCompleted(stack);
                }
                break;
            }
            
            // Blogu calistir
            if (!ExecuteCurrentBlock(stack, ctx))
            {
                // Blok bekleme durumuna gecti veya hata olustu
                break;
            }
            
            // Sonraki bloğa ilerle
            AdvanceToNextBlock(stack);
            
            instructionsRemaining--;
            stack.IncrementInstructionCount();
            m_Statistics.TotalInstructionsExecuted++;
        }
    }
    
    bool ExecutionEngine::ExecuteCurrentBlock(ExecutionStack& stack, ExecutionContext& ctx)
    {
        Block* block = stack.GetCurrentBlock();
        if (!block)
        {
            return false;
        }
        
        // Callback: Blok calistirilmaya basliyor
        if (m_Callbacks.OnBlockExecuting)
        {
            m_Callbacks.OnBlockExecuting(stack, block);
        }
        
        // Debug icin mevcut blogu ayarla
        ctx.SetCurrentBlock(block);
        
        try
        {
            // Blogu calistir
            block->Execute(ctx);
            
            // Bekleme durumu kontrolu
            if (ctx.IsStopRequested())
            {
                stack.SetState(ExecutionState::Completed);
                return false;
            }
            
            // Callback: Blok calistirildi
            if (m_Callbacks.OnBlockExecuted)
            {
                m_Callbacks.OnBlockExecuted(stack, block);
            }
            
            return true;
        }
        catch (const std::exception& e)
        {
            RS_ERROR("ExecutionEngine: Blok calistirma hatasi - {}", e.what());
            stack.SetState(ExecutionState::Error);
            
            if (m_Callbacks.OnError)
            {
                m_Callbacks.OnError(stack, e.what());
            }
            
            return false;
        }
    }
    
    void ExecutionEngine::AdvanceToNextBlock(ExecutionStack& stack)
    {
        Block* current = stack.GetCurrentBlock();
        if (!current)
        {
            return;
        }
        
        // Next Block baglantisini kontrol et
        Block* nextBlock = current->GetNextBlock();
        
        if (nextBlock)
        {
            stack.SetCurrentBlock(nextBlock);
        }
        else
        {
            // Ic bloktan cikis - parent frame'e don
            if (stack.HasFrames())
            {
                stack.PopFrame();
                
                if (stack.HasFrames())
                {
                    // Parent frame'deki sonraki bloga devam et
                    StackFrame& parentFrame = stack.CurrentFrame();
                    if (parentFrame.Block)
                    {
                        // Kontrol akis blogu ise, sonraki ic blogu veya sibling'i belirle
                        Block* parentNext = parentFrame.Block->GetNextBlock();
                        stack.SetCurrentBlock(parentNext);
                    }
                }
                else
                {
                    // Tum frame'ler bitti
                    stack.SetCurrentBlock(nullptr);
                }
            }
            else
            {
                // Kök seviyede ve sonraki blok yok
                stack.SetCurrentBlock(nullptr);
            }
        }
    }
    
    //=========================================================================
    // Bekleme Guncelleme
    //=========================================================================
    
    void ExecutionEngine::UpdateWaitingStacks(float deltaTime)
    {
        for (auto& stack : m_ActiveStacks)
        {
            if (stack->GetState() == ExecutionState::Waiting)
            {
                stack->UpdateWait(deltaTime);
                
                // Bekleme tamamlandiysa aktif yap
                if (stack->IsWaitComplete())
                {
                    stack->ClearWait();
                }
            }
        }
    }
    
    //=========================================================================
    // Temizlik
    //=========================================================================
    
    void ExecutionEngine::CleanupCompletedStacks()
    {
        // Tamamlanan veya iptal edilen yiginlari kaldir
        std::vector<UUID> toRemove;
        
        for (const auto& stack : m_ActiveStacks)
        {
            ExecutionState state = stack->GetState();
            if (state == ExecutionState::Completed ||
                state == ExecutionState::Cancelled ||
                state == ExecutionState::Error)
            {
                toRemove.push_back(stack->GetId());
            }
        }
        
        for (const UUID& id : toRemove)
        {
            RemoveStack(id);
        }
    }
    
    //=========================================================================
    // Debug
    //=========================================================================
    
    void ExecutionEngine::StepOne()
    {
        m_StepRequested = true;
    }
}
