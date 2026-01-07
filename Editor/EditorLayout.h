#pragma once

namespace RiftSpire
{
    //=========================================================================
    // EditorLayout - Fixed pixel layout for 1920x1080 editor
    //=========================================================================
    // This file defines the EXACT pixel positions and sizes for all editor panels.
    // NO responsive behavior - all values are hardcoded for 1920x1080.
    //=========================================================================
    
    namespace EditorLayout
    {
        //---------------------------------------------------------------------
        // Editor Window (FIXED)
        //---------------------------------------------------------------------
        constexpr int EDITOR_WIDTH = 1920;
        constexpr int EDITOR_HEIGHT = 1080;
        
        //---------------------------------------------------------------------
        // Top Bars
        //---------------------------------------------------------------------
        constexpr int MENUBAR_HEIGHT = 26;
        constexpr int TOOLBAR_HEIGHT = 40;
        constexpr int CONTENT_TOP = MENUBAR_HEIGHT + TOOLBAR_HEIGHT;  // 66px
        
        //---------------------------------------------------------------------
        // Panel Widths
        //---------------------------------------------------------------------
        constexpr int LEFT_PANEL_WIDTH = 280;
        constexpr int RIGHT_PANEL_WIDTH = 320;
        constexpr int CENTER_WIDTH = EDITOR_WIDTH - LEFT_PANEL_WIDTH - RIGHT_PANEL_WIDTH;  // 1320
        
        //---------------------------------------------------------------------
        // Panel Heights
        //---------------------------------------------------------------------
        constexpr int BOTTOM_PANEL_HEIGHT = 260;
        constexpr int MAIN_CONTENT_HEIGHT = EDITOR_HEIGHT - CONTENT_TOP - BOTTOM_PANEL_HEIGHT;  // 754
        
        //---------------------------------------------------------------------
        // Scene Hierarchy Panel (Top-Left)
        //---------------------------------------------------------------------
        namespace SceneHierarchy
        {
            constexpr int X = 0;
            constexpr int Y = CONTENT_TOP;
            constexpr int WIDTH = LEFT_PANEL_WIDTH;
            constexpr int HEIGHT = MAIN_CONTENT_HEIGHT / 2;  // 377
        }
        
        //---------------------------------------------------------------------
        // Block Palette Panel (Bottom-Left)
        //---------------------------------------------------------------------
        namespace BlockPalette
        {
            constexpr int X = 0;
            constexpr int Y = SceneHierarchy::Y + SceneHierarchy::HEIGHT;
            constexpr int WIDTH = LEFT_PANEL_WIDTH;
            constexpr int HEIGHT = MAIN_CONTENT_HEIGHT - SceneHierarchy::HEIGHT + BOTTOM_PANEL_HEIGHT;  // 637
        }
        
        //---------------------------------------------------------------------
        // Block Canvas Panel (Center - LARGEST)
        //---------------------------------------------------------------------
        namespace BlockCanvas
        {
            constexpr int X = LEFT_PANEL_WIDTH;
            constexpr int Y = CONTENT_TOP;
            constexpr int WIDTH = CENTER_WIDTH;
            constexpr int HEIGHT = MAIN_CONTENT_HEIGHT;  // 754
        }
        
        //---------------------------------------------------------------------
        // Inspector Panel (Top-Right)
        //---------------------------------------------------------------------
        namespace Inspector
        {
            constexpr int X = EDITOR_WIDTH - RIGHT_PANEL_WIDTH;
            constexpr int Y = CONTENT_TOP;
            constexpr int WIDTH = RIGHT_PANEL_WIDTH;
            constexpr int HEIGHT = MAIN_CONTENT_HEIGHT;  // 754
        }
        
        //---------------------------------------------------------------------
        // Console Panel (Bottom-Center)
        //---------------------------------------------------------------------
        namespace Console
        {
            constexpr int X = LEFT_PANEL_WIDTH;
            constexpr int Y = CONTENT_TOP + MAIN_CONTENT_HEIGHT;
            constexpr int WIDTH = CENTER_WIDTH - RIGHT_PANEL_WIDTH;  // 1000
            constexpr int HEIGHT = BOTTOM_PANEL_HEIGHT;
        }
        
        //---------------------------------------------------------------------
        // Game View Panel (Bottom-Right)
        //---------------------------------------------------------------------
        namespace GameView
        {
            constexpr int X = Console::X + Console::WIDTH;
            constexpr int Y = Console::Y;
            constexpr int WIDTH = RIGHT_PANEL_WIDTH + RIGHT_PANEL_WIDTH;  // 640
            constexpr int HEIGHT = BOTTOM_PANEL_HEIGHT;
        }
        
        //---------------------------------------------------------------------
        // Toolbar (Runtime Controls)
        //---------------------------------------------------------------------
        namespace Toolbar
        {
            constexpr int X = 0;
            constexpr int Y = MENUBAR_HEIGHT;
            constexpr int WIDTH = EDITOR_WIDTH;
            constexpr int HEIGHT = TOOLBAR_HEIGHT;
            
            // Button dimensions
            constexpr int BUTTON_WIDTH = 80;
            constexpr int BUTTON_HEIGHT = 28;
            constexpr int BUTTON_SPACING = 8;
        }
    }
    
    //=========================================================================
    // Layout Diagram (1920x1080)
    //=========================================================================
    /*
    ┌──────────────────────────────────────────────────────────────────────────┐
    │ [File] [Entity] [View] [Blocks]          Menu Bar                   26px │
    ├──────────────────────────────────────────────────────────────────────────┤
    │ [▶ Play] [⏸ Pause] [⏭ Step] [🔁 Restart]   Toolbar                  40px │
    ├───────────┬────────────────────────────────────────────┬─────────────────┤
    │           │                                            │                 │
    │   Scene   │                                            │                 │
    │ Hierarchy │                                            │    Inspector    │
    │  280x377  │                                            │     320x754     │
    │           │              Block Canvas                  │                 │
    ├───────────┤               (NanoVG)                     │                 │
    │           │               1320x754                     │                 │
    │   Block   │                                            │                 │
    │  Palette  │                                            │                 │
    │  280x637  │                                            │                 │
    │           ├─────────────────────────────┬──────────────┴─────────────────┤
    │           │                             │                                │
    │           │          Console            │           Game View            │
    │           │          1000x260           │            640x260             │
    │           │                             │                                │
    └───────────┴─────────────────────────────┴────────────────────────────────┘
    */
}
