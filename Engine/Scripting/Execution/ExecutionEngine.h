#pragma once

#include "ExecutionStack.h"
#include "ExecutionContext.h"
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace RiftSpire
{
    // Forward declarations
    class Block;
    class BlockScript;
    
    //=========================================================================
    // ExecutionEngineConfig - Motor yapilandirmasi
    //=========================================================================
    
    struct ExecutionEngineConfig
    {
        int MaxInstructionsPerFrame = 1000;    // Frame basina maksimum talimat (sonsuz dongu korumasi)
        int MaxActiveStacks = 100;             // Maksimum eşzamanlı yigin
        bool EnableDebugMode = false;          // Debug modu
        bool EnableProfiling = false;          // Performans olcumu
    };
    
    //=========================================================================
    // ExecutionCallback - Calistirma callback'leri
    //=========================================================================
    
    struct ExecutionCallbacks
    {
        std::function<void(ExecutionStack&)> OnStackCreated;
        std::function<void(ExecutionStack&)> OnStackStarted;
        std::function<void(ExecutionStack&)> OnStackCompleted;
        std::function<void(ExecutionStack&, CancelReason)> OnStackCancelled;
        std::function<void(ExecutionStack&, Block*)> OnBlockExecuting;
        std::function<void(ExecutionStack&, Block*)> OnBlockExecuted;
        std::function<void(ExecutionStack&, const std::string&)> OnError;
    };
    
    //=========================================================================
    // ExecutionEngine - Merkezi calistirma motoru
    //=========================================================================
    
    class ExecutionEngine
    {
    public:
        ExecutionEngine();
        explicit ExecutionEngine(const ExecutionEngineConfig& config);
        ~ExecutionEngine() = default;
        
        //---------------------------------------------------------------------
        // Yapilandirma
        //---------------------------------------------------------------------
        
        void SetConfig(const ExecutionEngineConfig& config) { m_Config = config; }
        const ExecutionEngineConfig& GetConfig() const { return m_Config; }
        
        void SetCallbacks(const ExecutionCallbacks& callbacks) { m_Callbacks = callbacks; }
        
        //---------------------------------------------------------------------
        // Yigin yonetimi
        //---------------------------------------------------------------------
        
        // Yeni yigin olustur ve kaydet
        ExecutionStackPtr CreateStack();
        ExecutionStackPtr CreateStack(const AbilityContext& abilityCtx);
        
        // Yigin kaydet (dis kaynaklardan)
        void RegisterStack(ExecutionStackPtr stack);
        
        // Yigin kaldir
        void RemoveStack(const UUID& stackId);
        
        // Yigin ara
        ExecutionStackPtr GetStack(const UUID& stackId);
        
        // Varlik icin yiginlari iptal et
        void CancelStacksForEntity(const UUID& entityId, CancelReason reason);
        
        // Tum yiginlari iptal et
        void CancelAllStacks(CancelReason reason);
        
        //---------------------------------------------------------------------
        // Frame guncelleme
        //---------------------------------------------------------------------
        
        // Her frame cagirilir
        void Tick(float deltaTime, ExecutionContext& globalContext);
        
        //---------------------------------------------------------------------
        // Sorgular
        //---------------------------------------------------------------------
        
        size_t GetActiveStackCount() const { return m_ActiveStacks.size(); }
        bool HasActiveStacks() const { return !m_ActiveStacks.empty(); }
        
        const std::vector<ExecutionStackPtr>& GetActiveStacks() const { return m_ActiveStacks; }
        
        //---------------------------------------------------------------------
        // Debug
        //---------------------------------------------------------------------
        
        void SetPaused(bool paused) { m_IsPaused = paused; }
        bool IsPaused() const { return m_IsPaused; }
        
        void StepOne();  // Tek adim ilerle (debug)
        
        //---------------------------------------------------------------------
        // Istatistikler
        //---------------------------------------------------------------------
        
        struct Statistics
        {
            int TotalStacksCreated = 0;
            int TotalStacksCompleted = 0;
            int TotalStacksCancelled = 0;
            int TotalInstructionsExecuted = 0;
            float TotalExecutionTime = 0.0f;
        };
        
        const Statistics& GetStatistics() const { return m_Statistics; }
        void ResetStatistics() { m_Statistics = Statistics{}; }
        
    private:
        // Tek yigini calistir
        void ExecuteStack(ExecutionStack& stack, ExecutionContext& ctx, int& instructionsRemaining);
        
        // Mevcut blogu calistir
        bool ExecuteCurrentBlock(ExecutionStack& stack, ExecutionContext& ctx);
        
        // Sonraki bloga ilerle
        void AdvanceToNextBlock(ExecutionStack& stack);
        
        // Bekleme durumlarini guncelle
        void UpdateWaitingStacks(float deltaTime);
        
        // Tamamlanan yiginlari temizle
        void CleanupCompletedStacks();
        
    private:
        ExecutionEngineConfig m_Config;
        ExecutionCallbacks m_Callbacks;
        
        // Aktif yiginlar
        std::vector<ExecutionStackPtr> m_ActiveStacks;
        
        // ID -> Stack haritasi (hizli lookup)
        std::unordered_map<UUID, ExecutionStackPtr> m_StackMap;
        
        // Debug durumu
        bool m_IsPaused = false;
        bool m_StepRequested = false;
        
        // Istatistikler
        Statistics m_Statistics;
    };
}
