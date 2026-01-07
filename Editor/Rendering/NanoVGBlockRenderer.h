#pragma once

//=============================================================================
// NanoVG - Premium 2D Vector Graphics for RiftSpire Block Rendering
//=============================================================================
// NanoVG is used for:
// - Anti-aliased block shapes
// - Gradient fills (linear, radial, box)
// - Soft shadows
// - Bezier curve connections
// - Text rendering
//=============================================================================

#include <memory>
#include <string>
#include <glm/glm.hpp>

// NanoVG forward declaration
struct NVGcontext;

namespace RiftSpire
{
    //=========================================================================
    // NVGColor - NanoVG renk yapisi
    //=========================================================================
    
    struct NVGColor
    {
        float r, g, b, a;
        
        NVGColor() : r(0), g(0), b(0), a(1) {}
        NVGColor(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
        
        static NVGColor FromRGB(int r, int g, int b) 
        { 
            return NVGColor(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f); 
        }
        
        static NVGColor FromRGBA(int r, int g, int b, int a) 
        { 
            return NVGColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f); 
        }
        
        static NVGColor FromImVec4(float r, float g, float b, float a)
        {
            return NVGColor(r, g, b, a);
        }
        
        NVGColor Lighten(float amount) const
        {
            return NVGColor(
                std::min(1.0f, r + amount),
                std::min(1.0f, g + amount),
                std::min(1.0f, b + amount),
                a
            );
        }
        
        NVGColor Darken(float amount) const
        {
            return NVGColor(
                std::max(0.0f, r - amount),
                std::max(0.0f, g - amount),
                std::max(0.0f, b - amount),
                a
            );
        }
    };
    
    //=========================================================================
    // BlockShape Types for NanoVG Rendering
    //=========================================================================
    
    enum class NVGBlockShape
    {
        Flat,           // Standard command block
        Hat,            // Event block (rounded top)
        Cap,            // End block (no bottom notch)
        CBlock,         // Control block with inner slot
        Reporter,       // Oval/rounded reporter
        Boolean,        // Hexagonal boolean
        Menu            // Dropdown menu
    };
    
    //=========================================================================
    // NanoVGBlockRenderer - Premium block rendering with NanoVG
    //=========================================================================
    
    class NanoVGBlockRenderer
    {
    public:
        NanoVGBlockRenderer();
        ~NanoVGBlockRenderer();
        
        // Lifecycle
        bool Initialize();
        void Shutdown();
        bool IsInitialized() const { return m_Context != nullptr; }
        
        // Frame management
        void BeginFrame(float width, float height, float pixelRatio = 1.0f);
        void EndFrame();
        
        //---------------------------------------------------------------------
        // Block Rendering
        //---------------------------------------------------------------------
        
        // Command block (flat with notches)
        void DrawCommandBlock(
            float x, float y, float w, float h,
            const NVGColor& color,
            bool hasTopNotch = true,
            bool hasBottomNotch = true
        );
        
        // Hat block (event - rounded top)
        void DrawHatBlock(
            float x, float y, float w, float h,
            const NVGColor& color
        );
        
        // Cap block (end - no bottom connection)
        void DrawCapBlock(
            float x, float y, float w, float h,
            const NVGColor& color
        );
        
        // C-Block (control with inner slot)
        void DrawCBlock(
            float x, float y, float w,
            float headerHeight, float innerHeight, float footerHeight,
            const NVGColor& color
        );
        
        // Reporter block (oval)
        void DrawReporterBlock(
            float x, float y, float w, float h,
            const NVGColor& color
        );
        
        // Boolean block (hexagonal)
        void DrawBooleanBlock(
            float x, float y, float w, float h,
            const NVGColor& color
        );
        
        //---------------------------------------------------------------------
        // Block Components
        //---------------------------------------------------------------------
        
        // Input field (white rounded rect)
        void DrawInputField(
            float x, float y, float w, float h,
            const std::string& text,
            bool isNumber = false
        );
        
        // Dropdown field
        void DrawDropdownField(
            float x, float y, float w, float h,
            const std::string& selectedText
        );
        
        // Boolean toggle
        void DrawBooleanToggle(
            float x, float y, float size,
            bool value
        );
        
        // Color picker swatch
        void DrawColorSwatch(
            float x, float y, float size,
            const NVGColor& color
        );
        
        //---------------------------------------------------------------------
        // Visual Effects
        //---------------------------------------------------------------------
        
        // Drop shadow
        void DrawBlockShadow(
            float x, float y, float w, float h,
            float cornerRadius = 4.0f
        );
        
        // Selection glow
        void DrawSelectionGlow(
            float x, float y, float w, float h,
            float cornerRadius = 4.0f
        );
        
        // Execution highlight
        void DrawExecutionHighlight(
            float x, float y, float w, float h,
            float cornerRadius = 4.0f,
            float pulsePhase = 0.0f
        );
        
        // Error indicator
        void DrawErrorOverlay(
            float x, float y, float w, float h,
            float cornerRadius = 4.0f
        );
        
        // Debug highlight
        void DrawDebugHighlight(
            float x, float y, float w, float h,
            float cornerRadius = 4.0f
        );
        
        //---------------------------------------------------------------------
        // Connection Lines
        //---------------------------------------------------------------------
        
        // Bezier connection between blocks
        void DrawConnection(
            float x1, float y1, float x2, float y2,
            const NVGColor& color,
            float thickness = 2.0f
        );
        
        // Dashed connection (during drag)
        void DrawDashedConnection(
            float x1, float y1, float x2, float y2,
            const NVGColor& color
        );
        
        //---------------------------------------------------------------------
        // Text Rendering
        //---------------------------------------------------------------------
        
        void DrawText(
            float x, float y,
            const std::string& text,
            const NVGColor& color,
            float fontSize = 14.0f
        );
        
        void DrawTextWithShadow(
            float x, float y,
            const std::string& text,
            const NVGColor& color,
            float fontSize = 14.0f
        );
        
        // Icon (emoji or symbol)
        void DrawIcon(
            float x, float y,
            const std::string& icon,
            float size = 16.0f
        );
        
        //---------------------------------------------------------------------
        // Clipping
        //---------------------------------------------------------------------
        
        void PushClipRect(float x, float y, float w, float h);
        void PopClipRect();
        
        //---------------------------------------------------------------------
        // Colors
        //---------------------------------------------------------------------
        
        // Category colors (Scratch-style)
        static NVGColor GetCategoryColor(int category);
        
        // Block parts
        static NVGColor GetHighlightColor(const NVGColor& base);
        static NVGColor GetShadowColor(const NVGColor& base);
        static NVGColor GetBorderColor(const NVGColor& base);
        
    private:
        // Path building helpers
        void BuildCommandBlockPath(float x, float y, float w, float h, bool topNotch, bool bottomNotch);
        void BuildHatBlockPath(float x, float y, float w, float h);
        void BuildCapBlockPath(float x, float y, float w, float h);
        void BuildCBlockPath(float x, float y, float w, float headerH, float innerH, float footerH);
        void BuildReporterPath(float x, float y, float w, float h);
        void BuildBooleanPath(float x, float y, float w, float h);
        
        // Notch drawing
        void DrawTopNotch(float x, float y);
        void DrawBottomNotch(float x, float y);
        
        // Apply gradient fill
        void ApplyGradientFill(const NVGColor& top, const NVGColor& bottom, float y, float h);
        
    private:
        NVGcontext* m_Context = nullptr;
        int m_FontNormal = -1;
        int m_FontBold = -1;
        int m_FontIcons = -1;
        
        // Constants
        static constexpr float CORNER_RADIUS = 4.0f;
        static constexpr float NOTCH_WIDTH = 16.0f;
        static constexpr float NOTCH_HEIGHT = 4.0f;
        static constexpr float NOTCH_OFFSET = 16.0f;
        static constexpr float HAT_CURVE_HEIGHT = 20.0f;
        static constexpr float C_BLOCK_ARM_WIDTH = 20.0f;
    };
    
    //=========================================================================
    // Global Accessor
    //=========================================================================
    
    NanoVGBlockRenderer& GetBlockRenderer();
}
