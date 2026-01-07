#include "ConsolePanel.h"

namespace RiftSpire
{
    void ConsolePanel::OnImGuiRender()
    {
        ImGui::Begin("Console");

        // Toolbar
        if (ImGui::Button("Clear"))
        {
            Clear();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &m_AutoScroll);
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();

        // Filter toggles
        ImGui::PushStyleColor(ImGuiCol_Text, GetColorForLevel(0));
        ImGui::Checkbox("T", &m_ShowTrace);
        ImGui::PopStyleColor();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, GetColorForLevel(1));
        ImGui::Checkbox("D", &m_ShowDebug);
        ImGui::PopStyleColor();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, GetColorForLevel(2));
        ImGui::Checkbox("I", &m_ShowInfo);
        ImGui::PopStyleColor();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, GetColorForLevel(3));
        ImGui::Checkbox("W", &m_ShowWarning);
        ImGui::PopStyleColor();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, GetColorForLevel(4));
        ImGui::Checkbox("E", &m_ShowError);
        ImGui::PopStyleColor();

        ImGui::Separator();

        // Log area
        ImGui::BeginChild("LogScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        const auto& messages = ImGuiConsoleSink::Get().GetMessages();
        for (const auto& msg : messages)
        {
            // Filter by level
            if ((msg.Level == 0 && !m_ShowTrace) ||
                (msg.Level == 1 && !m_ShowDebug) ||
                (msg.Level == 2 && !m_ShowInfo) ||
                (msg.Level == 3 && !m_ShowWarning) ||
                (msg.Level >= 4 && !m_ShowError))
            {
                continue;
            }

            ImGui::PushStyleColor(ImGuiCol_Text, GetColorForLevel(msg.Level));
            ImGui::TextUnformatted(msg.Message.c_str());
            ImGui::PopStyleColor();
        }

        if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();
        ImGui::End();
    }

    ImVec4 ConsolePanel::GetColorForLevel(int level)
    {
        switch (level)
        {
        case 0:  return ImVec4(0.5f, 0.5f, 0.5f, 1.0f);  // Trace - Gray
        case 1:  return ImVec4(0.3f, 0.7f, 0.9f, 1.0f);  // Debug - Cyan
        case 2:  return ImVec4(0.2f, 0.9f, 0.2f, 1.0f);  // Info - Green
        case 3:  return ImVec4(1.0f, 0.9f, 0.2f, 1.0f);  // Warning - Yellow
        case 4:  return ImVec4(1.0f, 0.3f, 0.3f, 1.0f);  // Error - Red
        case 5:  return ImVec4(1.0f, 0.0f, 0.5f, 1.0f);  // Critical - Magenta
        default: return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // Default - White
        }
    }
}
