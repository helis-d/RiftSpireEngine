#pragma once

#include <imgui.h>

namespace RiftSpire
{
    //=========================================================================
    // Modern Dark Theme Colors
    //=========================================================================
    
    namespace EditorColors
    {
        // Base colors - Deep dark with blue undertones
        constexpr ImVec4 Background         = ImVec4(0.082f, 0.082f, 0.137f, 1.0f);    // #151523
        constexpr ImVec4 Surface            = ImVec4(0.110f, 0.110f, 0.180f, 0.95f);   // #1c1c2e
        constexpr ImVec4 SurfaceLight       = ImVec4(0.145f, 0.145f, 0.220f, 1.0f);    // #252538
        constexpr ImVec4 SurfaceHover       = ImVec4(0.180f, 0.180f, 0.260f, 1.0f);    // #2e2e42
        constexpr ImVec4 SurfaceActive      = ImVec4(0.200f, 0.200f, 0.300f, 1.0f);    // #33334d
        constexpr ImVec4 Border             = ImVec4(0.220f, 0.220f, 0.320f, 0.6f);    // #383852
        constexpr ImVec4 BorderHover        = ImVec4(0.400f, 0.400f, 0.500f, 0.8f);
        
        // Text colors
        constexpr ImVec4 Text               = ImVec4(0.950f, 0.950f, 0.970f, 1.0f);    // #f2f2f8
        constexpr ImVec4 TextDim            = ImVec4(0.650f, 0.650f, 0.700f, 1.0f);    // #a6a6b3
        constexpr ImVec4 TextDisabled       = ImVec4(0.450f, 0.450f, 0.500f, 1.0f);
        
        // Accent colors - Vibrant
        constexpr ImVec4 Primary            = ImVec4(0.545f, 0.361f, 0.965f, 1.0f);    // #8b5cf6 - Purple
        constexpr ImVec4 PrimaryHover       = ImVec4(0.639f, 0.478f, 0.984f, 1.0f);    // #a37afb
        constexpr ImVec4 Secondary          = ImVec4(0.235f, 0.729f, 0.988f, 1.0f);    // #3cbafb - Cyan
        constexpr ImVec4 SecondaryHover     = ImVec4(0.400f, 0.800f, 1.000f, 1.0f);
        
        // Status colors
        constexpr ImVec4 Success            = ImVec4(0.204f, 0.863f, 0.502f, 1.0f);    // #34dc80 - Green
        constexpr ImVec4 Warning            = ImVec4(0.976f, 0.757f, 0.275f, 1.0f);    // #f9c146 - Yellow
        constexpr ImVec4 Error              = ImVec4(0.937f, 0.325f, 0.314f, 1.0f);    // #ef5350 - Red
        constexpr ImVec4 Info               = ImVec4(0.357f, 0.678f, 0.996f, 1.0f);    // #5badfe - Blue
        
        // Title bar and menu
        constexpr ImVec4 TitleBar           = ImVec4(0.067f, 0.067f, 0.110f, 1.0f);    // #11111c
        constexpr ImVec4 TitleBarActive     = ImVec4(0.090f, 0.090f, 0.145f, 1.0f);
        constexpr ImVec4 MenuBar            = ImVec4(0.075f, 0.075f, 0.125f, 1.0f);
        
        // Scrollbar
        constexpr ImVec4 ScrollbarBg        = ImVec4(0.067f, 0.067f, 0.110f, 0.5f);
        constexpr ImVec4 ScrollbarGrab      = ImVec4(0.300f, 0.300f, 0.400f, 0.5f);
        constexpr ImVec4 ScrollbarGrabHover = ImVec4(0.400f, 0.400f, 0.520f, 0.7f);
        
        // Tab colors
        constexpr ImVec4 Tab                = ImVec4(0.120f, 0.120f, 0.190f, 1.0f);
        constexpr ImVec4 TabHovered         = ImVec4(0.180f, 0.180f, 0.280f, 1.0f);
        constexpr ImVec4 TabActive          = ImVec4(0.545f, 0.361f, 0.965f, 0.3f);
        constexpr ImVec4 TabUnfocused       = ImVec4(0.100f, 0.100f, 0.160f, 1.0f);
        
        // Header
        constexpr ImVec4 Header             = ImVec4(0.160f, 0.160f, 0.240f, 1.0f);
        constexpr ImVec4 HeaderHovered      = ImVec4(0.220f, 0.220f, 0.320f, 1.0f);
        constexpr ImVec4 HeaderActive       = ImVec4(0.280f, 0.280f, 0.400f, 1.0f);
    }

    //=========================================================================
    // Block Category Colors (Scratch 3.0 Official Palette)
    //=========================================================================
    
    namespace ScratchBlockColors
    {
        // Scratch 3.0 Official Colors (MIT Scratch)
        constexpr ImVec4 Motion         = ImVec4(0.298f, 0.592f, 1.000f, 1.0f);    // #4C97FF - Motion Blue
        constexpr ImVec4 Looks          = ImVec4(0.600f, 0.400f, 1.000f, 1.0f);    // #9966FF - Looks Purple
        constexpr ImVec4 Sound          = ImVec4(0.812f, 0.388f, 0.812f, 1.0f);    // #CF63CF - Sound Pink
        constexpr ImVec4 Events         = ImVec4(1.000f, 0.749f, 0.000f, 1.0f);    // #FFBF00 - Events Gold
        constexpr ImVec4 Control        = ImVec4(1.000f, 0.671f, 0.098f, 1.0f);    // #FFAB19 - Control Orange
        constexpr ImVec4 Sensing        = ImVec4(0.361f, 0.694f, 0.839f, 1.0f);    // #5CB1D6 - Sensing Cyan
        constexpr ImVec4 Operators      = ImVec4(0.349f, 0.753f, 0.349f, 1.0f);    // #59C059 - Operators Green
        constexpr ImVec4 Variables      = ImVec4(1.000f, 0.549f, 0.102f, 1.0f);    // #FF8C1A - Variables Orange
        constexpr ImVec4 MyBlocks       = ImVec4(1.000f, 0.400f, 0.502f, 1.0f);    // #FF6680 - My Blocks Pink
        
        // MOBA-specific colors (vibrant, game-ready)
        constexpr ImVec4 Attack         = ImVec4(0.906f, 0.298f, 0.235f, 1.0f);    // #E74C3C - Combat Red
        constexpr ImVec4 Defense        = ImVec4(0.102f, 0.737f, 0.612f, 1.0f);    // #1ABC9C - Shield Teal
        constexpr ImVec4 Health         = ImVec4(0.180f, 0.800f, 0.443f, 1.0f);    // #2ECC71 - Health Green
        constexpr ImVec4 Mana           = ImVec4(0.204f, 0.596f, 0.859f, 1.0f);    // #3498DB - Mana Blue
        constexpr ImVec4 Ultimate       = ImVec4(0.945f, 0.769f, 0.059f, 1.0f);    // #F1C40F - Ultimate Gold
        constexpr ImVec4 Targeting      = ImVec4(0.753f, 0.224f, 0.169f, 1.0f);    // #C0392B - Target Red
        constexpr ImVec4 Network        = ImVec4(0.557f, 0.267f, 0.678f, 1.0f);    // #8E44AD - Network Purple
        constexpr ImVec4 Debug          = ImVec4(0.584f, 0.647f, 0.651f, 1.0f);    // #95A5A6 - Debug Gray
    }
    
    //=========================================================================
    // Theme Application
    //=========================================================================
    
    inline void ApplyModernDarkTheme()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;
        
        // Rounding
        style.WindowRounding = 8.0f;
        style.ChildRounding = 6.0f;
        style.FrameRounding = 6.0f;
        style.PopupRounding = 6.0f;
        style.ScrollbarRounding = 8.0f;
        style.GrabRounding = 4.0f;
        style.TabRounding = 6.0f;
        
        // Borders
        style.WindowBorderSize = 1.0f;
        style.ChildBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;
        style.FrameBorderSize = 0.0f;
        style.TabBorderSize = 0.0f;
        
        // Padding and spacing
        style.WindowPadding = ImVec2(12.0f, 12.0f);
        style.FramePadding = ImVec2(10.0f, 6.0f);
        style.CellPadding = ImVec2(6.0f, 4.0f);
        style.ItemSpacing = ImVec2(8.0f, 6.0f);
        style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
        style.IndentSpacing = 20.0f;
        style.ScrollbarSize = 14.0f;
        style.GrabMinSize = 12.0f;
        
        // Window colors
        colors[ImGuiCol_WindowBg] = EditorColors::Surface;
        colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_PopupBg] = EditorColors::SurfaceLight;
        colors[ImGuiCol_Border] = EditorColors::Border;
        colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        
        // Title bar
        colors[ImGuiCol_TitleBg] = EditorColors::TitleBar;
        colors[ImGuiCol_TitleBgActive] = EditorColors::TitleBarActive;
        colors[ImGuiCol_TitleBgCollapsed] = EditorColors::TitleBar;
        
        // Menu bar
        colors[ImGuiCol_MenuBarBg] = EditorColors::MenuBar;
        
        // Text
        colors[ImGuiCol_Text] = EditorColors::Text;
        colors[ImGuiCol_TextDisabled] = EditorColors::TextDisabled;
        
        // Frame (input fields, etc)
        colors[ImGuiCol_FrameBg] = EditorColors::SurfaceLight;
        colors[ImGuiCol_FrameBgHovered] = EditorColors::SurfaceHover;
        colors[ImGuiCol_FrameBgActive] = EditorColors::SurfaceActive;
        
        // Buttons
        colors[ImGuiCol_Button] = EditorColors::SurfaceLight;
        colors[ImGuiCol_ButtonHovered] = EditorColors::SurfaceHover;
        colors[ImGuiCol_ButtonActive] = EditorColors::Primary;
        
        // Headers
        colors[ImGuiCol_Header] = EditorColors::Header;
        colors[ImGuiCol_HeaderHovered] = EditorColors::HeaderHovered;
        colors[ImGuiCol_HeaderActive] = EditorColors::HeaderActive;
        
        // Tabs
        colors[ImGuiCol_Tab] = EditorColors::Tab;
        colors[ImGuiCol_TabHovered] = EditorColors::TabHovered;
        colors[ImGuiCol_TabActive] = EditorColors::TabActive;
        colors[ImGuiCol_TabUnfocused] = EditorColors::TabUnfocused;
        colors[ImGuiCol_TabUnfocusedActive] = EditorColors::Tab;
        
        // Scrollbar
        colors[ImGuiCol_ScrollbarBg] = EditorColors::ScrollbarBg;
        colors[ImGuiCol_ScrollbarGrab] = EditorColors::ScrollbarGrab;
        colors[ImGuiCol_ScrollbarGrabHovered] = EditorColors::ScrollbarGrabHover;
        colors[ImGuiCol_ScrollbarGrabActive] = EditorColors::Primary;
        
        // Slider
        colors[ImGuiCol_SliderGrab] = EditorColors::Primary;
        colors[ImGuiCol_SliderGrabActive] = EditorColors::PrimaryHover;
        
        // Check/Radio
        colors[ImGuiCol_CheckMark] = EditorColors::Primary;
        
        // Separator
        colors[ImGuiCol_Separator] = EditorColors::Border;
        colors[ImGuiCol_SeparatorHovered] = EditorColors::Primary;
        colors[ImGuiCol_SeparatorActive] = EditorColors::PrimaryHover;
        
        // Resize grip
        colors[ImGuiCol_ResizeGrip] = EditorColors::Border;
        colors[ImGuiCol_ResizeGripHovered] = EditorColors::Primary;
        colors[ImGuiCol_ResizeGripActive] = EditorColors::PrimaryHover;
        
        // Docking (if enabled)
#ifdef IMGUI_HAS_DOCK
        colors[ImGuiCol_DockingPreview] = EditorColors::Primary;
        colors[ImGuiCol_DockingEmptyBg] = EditorColors::Background;
#endif
        
        // Table
        colors[ImGuiCol_TableHeaderBg] = EditorColors::Header;
        colors[ImGuiCol_TableBorderStrong] = EditorColors::Border;
        colors[ImGuiCol_TableBorderLight] = EditorColors::Border;
        colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.02f);
        
        // Nav highlight
        colors[ImGuiCol_NavHighlight] = EditorColors::Primary;
        colors[ImGuiCol_NavWindowingHighlight] = EditorColors::Primary;
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);
        
        // Modal dim
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.6f);
        
        // Drag drop
        colors[ImGuiCol_DragDropTarget] = EditorColors::Success;
        
        // Text selection
        colors[ImGuiCol_TextSelectedBg] = ImVec4(EditorColors::Primary.x, EditorColors::Primary.y, EditorColors::Primary.z, 0.35f);
    }
    
    //=========================================================================
    // Utility Functions
    //=========================================================================
    
    /// Get darker version of a color (for gradients)
    inline ImVec4 DarkenColor(const ImVec4& color, float amount = 0.3f)
    {
        return ImVec4(
            color.x * (1.0f - amount),
            color.y * (1.0f - amount),
            color.z * (1.0f - amount),
            color.w
        );
    }
    
    /// Get lighter version of a color (for highlights)
    inline ImVec4 LightenColor(const ImVec4& color, float amount = 0.3f)
    {
        return ImVec4(
            color.x + (1.0f - color.x) * amount,
            color.y + (1.0f - color.y) * amount,
            color.z + (1.0f - color.z) * amount,
            color.w
        );
    }
    
    /// Convert ImVec4 to ImU32
    inline ImU32 ColorToU32(const ImVec4& color)
    {
        return IM_COL32(
            static_cast<int>(color.x * 255),
            static_cast<int>(color.y * 255),
            static_cast<int>(color.z * 255),
            static_cast<int>(color.w * 255)
        );
    }
    
    /// Get color with modified alpha
    inline ImVec4 WithAlpha(const ImVec4& color, float alpha)
    {
        return ImVec4(color.x, color.y, color.z, alpha);
    }
}
