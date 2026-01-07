#pragma once

#include "../Core/Block.h"
#include "../Core/Value.h"
#include <Core/UUID.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace RiftSpire
{
    // Forward declarations
    class Entity;
    class Block;
    class BlockScript;
    class ExecutionEngine;
    
    //=========================================================================
    // ExecutionState - Calistirma yigini durumu
    //=========================================================================
    
    enum class ExecutionState
    {
        Idle,           // Henuz baslamadi
        Active,         // Aktif olarak calisiyor
        Waiting,        // Bekleme durumunda (Wait, CastTime)
        Paused,         // Debug icin duraklatildi
        Completed,      // Basariyla tamamlandi
        Cancelled,      // Iptal edildi (olum, stun, vb.)
        Error           // Hata ile sonlandi
    };
    
    //=========================================================================
    // CancelReason - Iptal nedenleri
    //=========================================================================
    
    enum class CancelReason
    {
        None,
        Death,          // Varlik oldu
        Stun,           // Sersemletme etkisi
        Silence,        // Susturma etkisi
        Knockup,        // Havaya kaldirma
        ManualCancel,   // Manuel iptal (oyuncu)
        Interrupt,      // Kesme yetkinligi
        OutOfRange,     // Menzil disi
        InsufficientResources  // Yetersiz kaynak (mana, enerji)
    };
    
    //=========================================================================
    // WaitType - Bekleme tipi
    //=========================================================================
    
    enum class WaitType
    {
        None,
        Seconds,        // Belirli sure bekle
        Condition,      // Kosul saglanana kadar bekle
        Animation,      // Animasyon bitene kadar bekle
        CastTime,       // Kullanim suresi dolana kadar bekle
        Channeling,     // Kanallama tamamlanana kadar bekle
        NextFrame       // Sonraki frame'e kadar bekle
    };
    
    //=========================================================================
    // WaitCondition - Bekleme kosulu
    //=========================================================================
    
    struct WaitCondition
    {
        std::function<bool()> Predicate;  // Kosul fonksiyonu
        std::string Description;           // Debug icin aciklama
    };
    
    //=========================================================================
    // AbilityContext - Yetkinlik baglami
    //=========================================================================
    
    struct AbilityContext
    {
        // Varlik referanslari
        UUID CasterId;              // Kullanan varlik
        UUID TargetId;              // Hedef varlik (opsiyonel)
        std::vector<UUID> HitTargets;  // Vurulan hedefler
        
        // Konum bilgileri
        glm::vec3 CastPosition;     // Kullanim konumu
        glm::vec3 TargetPosition;   // Hedef konum
        glm::vec3 Direction;        // Yetkinlik yonu
        
        // Yetkinlik bilgileri
        UUID AbilityId;             // Yetkinlik kimlik
        std::string AbilityTypeId;  // Yetkinlik tipi ("Fireball", "Dash", vb.)
        int AbilityLevel;           // Yetkinlik seviyesi (1-5)
        int AbilityRank;            // Yetkinlik rütbesi
        
        // Zaman bilgileri
        float CastTime;             // Kullanim suresi
        float Cooldown;             // Bekleme suresi
        float ChannelDuration;      // Kanallama suresi
        
        // Hasar/Etki bilgileri
        float BaseDamage;           // Temel hasar
        float BonusDamage;          // Bonus hasar
        float HealAmount;           // Iyilestirme miktari
        
        // Durum bayraklari
        bool IsCrit;                // Kritik vurus mu
        bool IsEmpowered;           // Güçlendirilmiş mi
        bool CanBeCancelled;        // İptal edilebilir mi
        
        // Varsayilan degerler
        AbilityContext()
            : AbilityLevel(1)
            , AbilityRank(0)
            , CastTime(0.0f)
            , Cooldown(0.0f)
            , ChannelDuration(0.0f)
            , BaseDamage(0.0f)
            , BonusDamage(0.0f)
            , HealAmount(0.0f)
            , IsCrit(false)
            , IsEmpowered(false)
            , CanBeCancelled(true)
        {}
    };
    
    //=========================================================================
    // InstructionPointer - Talimat isaretcisi
    //=========================================================================
    
    struct InstructionPointer
    {
        Block* CurrentBlock = nullptr;    // Mevcut blok
        int ChildIndex = 0;               // Ic blok endeksi (kontrol akisi icin)
        int BranchIndex = 0;              // Dallanma endeksi (If/Else icin)
        int LoopIteration = 0;            // Dongu yineleme sayaci
        bool AdvancePending = false;      // Sonraki bloga gecis bekliyor
        
        void Reset()
        {
            CurrentBlock = nullptr;
            ChildIndex = 0;
            BranchIndex = 0;
            LoopIteration = 0;
            AdvancePending = false;
        }
    };
    
    //=========================================================================
    // StackFrame - Cagri cercevesi (ic bloklar icin)
    //=========================================================================
    
    struct StackFrame
    {
        Block* Block = nullptr;           // Bu frame'in blogu
        int ChildIndex = 0;               // Ic blok endeksi
        int BranchIndex = 0;              // Dallanma endeksi
        int LoopIteration = 0;            // Dongu sayaci
        std::unordered_map<std::string, Value> LocalVariables;  // Frame-local degiskenler
    };
    
    //=========================================================================
    // ExecutionStack - Calistirma yigini
    //=========================================================================
    
    class ExecutionStack
    {
    public:
        ExecutionStack();
        explicit ExecutionStack(const UUID& id);
        ~ExecutionStack() = default;
        
        //---------------------------------------------------------------------
        // Kimlik ve durum
        //---------------------------------------------------------------------
        
        const UUID& GetId() const { return m_StackId; }
        ExecutionState GetState() const { return m_State; }
        void SetState(ExecutionState state) { m_State = state; }
        
        //---------------------------------------------------------------------
        // Ability Context
        //---------------------------------------------------------------------
        
        AbilityContext& GetAbilityContext() { return m_AbilityContext; }
        const AbilityContext& GetAbilityContext() const { return m_AbilityContext; }
        void SetAbilityContext(const AbilityContext& ctx) { m_AbilityContext = ctx; }
        
        //---------------------------------------------------------------------
        // Instruction Pointer
        //---------------------------------------------------------------------
        
        InstructionPointer& GetIP() { return m_IP; }
        const InstructionPointer& GetIP() const { return m_IP; }
        
        Block* GetCurrentBlock() const { return m_IP.CurrentBlock; }
        void SetCurrentBlock(Block* block) { m_IP.CurrentBlock = block; }
        
        //---------------------------------------------------------------------
        // Stack Frames (nested blocks)
        //---------------------------------------------------------------------
        
        void PushFrame(Block* block);
        void PopFrame();
        StackFrame& CurrentFrame();
        const StackFrame& CurrentFrame() const;
        size_t GetFrameDepth() const { return m_Frames.size(); }
        bool HasFrames() const { return !m_Frames.empty(); }
        
        //---------------------------------------------------------------------
        // Wait management
        //---------------------------------------------------------------------
        
        void StartWait(WaitType type, float duration);
        void StartWaitCondition(const WaitCondition& condition);
        void UpdateWait(float deltaTime);
        bool IsWaitComplete() const;
        void ClearWait();
        
        WaitType GetWaitType() const { return m_WaitType; }
        float GetWaitTimer() const { return m_WaitTimer; }
        float GetWaitDuration() const { return m_WaitDuration; }
        
        //---------------------------------------------------------------------
        // Cancellation
        //---------------------------------------------------------------------
        
        void Cancel(CancelReason reason);
        CancelReason GetCancelReason() const { return m_CancelReason; }
        bool IsCancelled() const { return m_State == ExecutionState::Cancelled; }
        
        //---------------------------------------------------------------------
        // Statistics
        //---------------------------------------------------------------------
        
        int GetInstructionCount() const { return m_InstructionCount; }
        void IncrementInstructionCount() { m_InstructionCount++; }
        void ResetInstructionCount() { m_InstructionCount = 0; }
        
        float GetTotalExecutionTime() const { return m_TotalExecutionTime; }
        void AddExecutionTime(float time) { m_TotalExecutionTime += time; }
        
        //---------------------------------------------------------------------
        // Script reference
        //---------------------------------------------------------------------
        
        void SetScript(BlockScript* script) { m_Script = script; }
        BlockScript* GetScript() const { return m_Script; }
        
    private:
        UUID m_StackId;
        ExecutionState m_State = ExecutionState::Idle;
        
        // Ability context
        AbilityContext m_AbilityContext;
        
        // Instruction pointer
        InstructionPointer m_IP;
        
        // Call stack
        std::vector<StackFrame> m_Frames;
        
        // Wait state
        WaitType m_WaitType = WaitType::None;
        float m_WaitTimer = 0.0f;
        float m_WaitDuration = 0.0f;
        WaitCondition m_WaitCondition;
        
        // Cancellation
        CancelReason m_CancelReason = CancelReason::None;
        
        // Script reference
        BlockScript* m_Script = nullptr;
        
        // Statistics
        int m_InstructionCount = 0;
        float m_TotalExecutionTime = 0.0f;
    };
    
    //=========================================================================
    // ExecutionStackPtr - Akilli isareticisi
    //=========================================================================
    
    using ExecutionStackPtr = std::shared_ptr<ExecutionStack>;
}
