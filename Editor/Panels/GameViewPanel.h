#pragma once

#include <string>

namespace RiftSpire
{
    //=========================================================================
    // GameViewPanel - Live runtime simulation view
    //=========================================================================
    // Fixed position: Bottom-right (1280, 820)
    // Fixed size: 640 x 260
    // Shows actual game state, FPS, and runtime controls
    //=========================================================================
    
    class GameViewPanel
    {
    public:
        GameViewPanel() = default;
        ~GameViewPanel() = default;
        
        void OnImGuiRender();
        
        // Runtime state
        void SetPlaying(bool playing) { m_IsPlaying = playing; }
        void SetPaused(bool paused) { m_IsPaused = paused; }
        bool IsPlaying() const { return m_IsPlaying; }
        bool IsPaused() const { return m_IsPaused; }
        
        // Game state info
        void SetGameState(const std::string& state) { m_GameState = state; }
        void SetFPS(float fps) { m_FPS = fps; }
        
    private:
        bool m_IsPlaying = false;
        bool m_IsPaused = false;
        
        std::string m_GameState = "Idle";
        float m_FPS = 0.0f;
        
        // Framebuffer for game rendering (future)
        // unsigned int m_FramebufferTexture = 0;
    };
}
