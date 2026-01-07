#include "EventSystem.h"
#include <Core/Logger.h>
#include <algorithm>

namespace RiftSpire
{
    //=========================================================================
    // EventDispatcher Singleton
    //=========================================================================
    
    EventDispatcher& EventDispatcher::Get()
    {
        static EventDispatcher s_Instance;
        return s_Instance;
    }
    
    //=========================================================================
    // Subscription
    //=========================================================================
    
    void EventDispatcher::Subscribe(EventType type, EventHandler handler)
    {
        static int s_AutoIdCounter = 0;
        std::string autoId = "__auto_" + std::to_string(++s_AutoIdCounter);
        Subscribe(type, autoId, handler);
    }
    
    void EventDispatcher::Subscribe(EventType type, const std::string& id, EventHandler handler)
    {
        HandlerEntry entry;
        entry.Id = id;
        entry.Handler = handler;
        m_Handlers[type].push_back(entry);
        
        RS_TRACE("EventDispatcher: Handler kayit edildi - Type: {}, Id: {}", 
                 static_cast<int>(type), id);
    }
    
    void EventDispatcher::Unsubscribe(EventType type, const std::string& id)
    {
        auto it = m_Handlers.find(type);
        if (it != m_Handlers.end())
        {
            auto& handlers = it->second;
            handlers.erase(
                std::remove_if(handlers.begin(), handlers.end(),
                    [&id](const HandlerEntry& entry) { return entry.Id == id; }),
                handlers.end()
            );
        }
    }
    
    void EventDispatcher::UnsubscribeAll(EventType type)
    {
        m_Handlers.erase(type);
    }
    
    //=========================================================================
    // Dispatch
    //=========================================================================
    
    void EventDispatcher::Dispatch(const EventData& data)
    {
        auto it = m_Handlers.find(data.Type);
        if (it != m_Handlers.end())
        {
            // Tum handler'lari cagir
            for (const auto& entry : it->second)
            {
                try
                {
                    entry.Handler(data);
                }
                catch (const std::exception& e)
                {
                    RS_ERROR("EventDispatcher: Handler hatasi - Id: {}, Hata: {}", 
                             entry.Id, e.what());
                }
            }
        }
    }
    
    void EventDispatcher::QueueEvent(const EventData& data)
    {
        // Basit kopya - gercek implementasyonda polymorphic copy gerekir
        auto copy = std::make_unique<EventData>(data);
        m_EventQueue.push_back(std::move(copy));
    }
    
    void EventDispatcher::ProcessQueue()
    {
        // Kuyruk kopyasini al (yeniden giris korumasi)
        auto queue = std::move(m_EventQueue);
        m_EventQueue.clear();
        
        for (const auto& event : queue)
        {
            Dispatch(*event);
        }
    }
}
