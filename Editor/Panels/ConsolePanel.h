#pragma once

#include <imgui.h>
#include <Core/Logger.h>
#include <string>
#include <vector>
#include <mutex>

namespace RiftSpire
{
    // Custom spdlog sink for ImGui console
    class ImGuiConsoleSink
    {
    public:
        struct LogMessage
        {
            std::string Message;
            int Level; // 0=Trace, 1=Debug, 2=Info, 3=Warn, 4=Error, 5=Critical
        };

        static ImGuiConsoleSink& Get()
        {
            static ImGuiConsoleSink instance;
            return instance;
        }

        void AddMessage(const std::string& msg, int level)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Messages.push_back({ msg, level });
            if (m_Messages.size() > m_MaxMessages)
            {
                m_Messages.erase(m_Messages.begin());
            }
        }

        const std::vector<LogMessage>& GetMessages() const { return m_Messages; }
        void Clear()
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Messages.clear();
        }

    private:
        ImGuiConsoleSink() = default;
        std::vector<LogMessage> m_Messages;
        std::mutex m_Mutex;
        size_t m_MaxMessages = 1000;
    };

    class ConsolePanel
    {
    public:
        ConsolePanel() = default;

        void OnImGuiRender();
        void Clear() { ImGuiConsoleSink::Get().Clear(); }

    private:
        ImVec4 GetColorForLevel(int level);

    private:
        bool m_AutoScroll = true;
        bool m_ShowTrace = true;
        bool m_ShowDebug = true;
        bool m_ShowInfo = true;
        bool m_ShowWarning = true;
        bool m_ShowError = true;
    };
}
