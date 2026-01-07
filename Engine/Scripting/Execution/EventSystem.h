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
    class ExecutionEngine;
    class BlockScript;
    class ExecutionStack;
    using ExecutionStackPtr = std::shared_ptr<ExecutionStack>;
    
    //=========================================================================
    // EventType - Olay tipleri
    //=========================================================================
    
    enum class EventType
    {
        // Yetkinlik olaylari
        AbilityCastStart,       // Yetkinlik kullanimi basladi
        AbilityCastComplete,    // Yetkinlik kullanimi tamamlandi
        AbilityChannelStart,    // Kanallama basladi
        AbilityChannelEnd,      // Kanallama bitti
        AbilityHit,             // Yetkinlik hedefe isabet etti
        AbilityMiss,            // Yetkinlik iskaladi
        
        // Hasar olaylari
        DamageDealt,            // Hasar verildi
        DamageTaken,            // Hasar alindi
        HealDealt,              // Iyilestirme verildi
        HealReceived,           // Iyilestirme alindi
        
        // Yasam olaylari
        Kill,                   // Oldurme
        Death,                  // Olum
        Respawn,                // Yeniden dogus
        
        // Buff olaylari
        BuffApplied,            // Buff uygulandi
        BuffRefreshed,          // Buff yenilendi
        BuffStacked,            // Buff yigildi
        BuffExpired,            // Buff sona erdi
        BuffRemoved,            // Buff kaldirildi
        
        // Crowd Control olaylari
        CCApplied,              // CC uygulandi
        CCExpired,              // CC sona erdi
        CCImmune,               // CC bagisikligi
        
        // Hareket olaylari
        DashStart,              // Atilma basladi
        DashEnd,                // Atilma bitti
        Teleport,               // Isinlanma
        
        // AI olaylari
        Tick,                   // AI tick (periyodik)
        TargetAcquired,         // Hedef alindi
        TargetLost,             // Hedef kaybedildi
        
        // Oyun olaylari
        GameStart,              // Oyun basladi
        GameEnd,                // Oyun bitti
        ObjectiveCapture,       // Hedef ele gecirildi
        
        // Ozel
        Custom                  // Ozel event
    };
    
    //=========================================================================
    // DamageType - Hasar tipi
    //=========================================================================
    
    enum class DamageType
    {
        Physical,       // Fiziksel hasar
        Magical,        // Sihirli hasar
        True,           // Gercek hasar (zirh yok sayar)
        Pure,           // Saf hasar (her seyi yok sayar)
        Mixed           // Karisik hasar
    };
    
    //=========================================================================
    // EventData - Olay verisi taban sinifi
    //=========================================================================
    
    struct EventData
    {
        EventType Type;
        UUID SourceEntityId;        // Olaya neden olan varlik
        UUID TargetEntityId;        // Hedef varlik (varsa)
        glm::vec3 Position;         // Olay konumu
        float Timestamp;            // Olay zamani
        
        EventData(EventType type = EventType::Custom)
            : Type(type)
            , Timestamp(0.0f)
        {}
        
        virtual ~EventData() = default;
    };
    
    //=========================================================================
    // AbilityEventData - Yetkinlik olay verisi
    //=========================================================================
    
    struct AbilityEventData : EventData
    {
        UUID AbilityId;
        std::string AbilityTypeId;
        int AbilityLevel = 1;
        glm::vec3 CastPosition;
        glm::vec3 TargetPosition;
        glm::vec3 Direction;
        float CastTime = 0.0f;
        
        AbilityEventData(EventType type = EventType::AbilityCastStart)
            : EventData(type)
        {}
    };
    
    //=========================================================================
    // DamageEventData - Hasar olay verisi
    //=========================================================================
    
    struct DamageEventData : EventData
    {
        float BaseDamage = 0.0f;
        float FinalDamage = 0.0f;
        float MitigatedDamage = 0.0f;
        DamageType Type = DamageType::Physical;
        bool IsCritical = false;
        bool IsKillingBlow = false;
        UUID AbilityId;
        
        DamageEventData(EventType type = EventType::DamageDealt)
            : EventData(type)
        {}
    };
    
    //=========================================================================
    // BuffEventData - Buff olay verisi
    //=========================================================================
    
    struct BuffEventData : EventData
    {
        UUID BuffId;
        std::string BuffTypeId;
        float Duration = 0.0f;
        int StackCount = 1;
        UUID AppliedBy;
        
        BuffEventData(EventType type = EventType::BuffApplied)
            : EventData(type)
        {}
    };
    
    //=========================================================================
    // TickEventData - AI tick olay verisi
    //=========================================================================
    
    struct TickEventData : EventData
    {
        float DeltaTime = 0.0f;
        int TickCount = 0;
        
        TickEventData()
            : EventData(EventType::Tick)
        {}
    };
    
    //=========================================================================
    // EventHandler - Olay dinleyici callback
    //=========================================================================
    
    using EventHandler = std::function<void(const EventData&)>;
    
    //=========================================================================
    // EventDispatcher - Merkezi olay dagitici
    //=========================================================================
    
    class EventDispatcher
    {
    public:
        static EventDispatcher& Get();
        
        // Olay dinleyici kaydet
        void Subscribe(EventType type, EventHandler handler);
        void Subscribe(EventType type, const std::string& id, EventHandler handler);
        
        // Olay dinleyici kaldir
        void Unsubscribe(EventType type, const std::string& id);
        void UnsubscribeAll(EventType type);
        
        // Olay tetikle
        void Dispatch(const EventData& data);
        
        // Kuyruklanmis olaylari isle
        void ProcessQueue();
        
        // Kuyruga olay ekle (thread-safe)
        void QueueEvent(const EventData& data);
        
    private:
        EventDispatcher() = default;
        
        struct HandlerEntry
        {
            std::string Id;
            EventHandler Handler;
        };
        
        std::unordered_map<EventType, std::vector<HandlerEntry>> m_Handlers;
        std::vector<std::unique_ptr<EventData>> m_EventQueue;
    };
}
