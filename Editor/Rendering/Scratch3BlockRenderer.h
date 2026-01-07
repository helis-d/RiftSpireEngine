#pragma once

//=============================================================================
// Scratch3BlockRenderer - Pixel-Perfect Scratch 3.0 Style Block Rendering
//=============================================================================
// This renderer replicates the EXACT visual style of MIT Scratch 3.0 blocks
// using NanoVG for anti-aliased vector graphics.
//=============================================================================

#include <string>

// Forward declaration
struct NVGcontext;

namespace RiftSpire
{
    //=========================================================================
    // Block Shape Enum
    //=========================================================================
    
    enum class Scratch3Shape
    {
        HAT,        // Event blocks - convex arc top (When Game Starts)
        STACK,      // Command blocks - standard with top/bottom notches
        C_SHAPE,    // Control blocks - If/Repeat with inner mouth
        CAP,        // End blocks - Stop, no bottom notch
        REPORTER,   // Oval data blocks (rounded ends)
        BOOLEAN     // Hexagonal boolean blocks
    };
    
    //=========================================================================
    // Block Category Colors (Scratch 3.0 Official Palette)
    //=========================================================================
    
    namespace Scratch3Colors
    {
        // Primary category colors (vibrant, saturated)
        constexpr unsigned int Motion      = 0xFF4C97FF;  // Bright Blue
        constexpr unsigned int Looks       = 0xFF9966FF;  // Purple
        constexpr unsigned int Sound       = 0xFFCF63CF;  // Pink-Purple
        constexpr unsigned int Events      = 0xFFFFBF00;  // Gold/Yellow
        constexpr unsigned int Control     = 0xFFFFAB19;  // Orange
        constexpr unsigned int Sensing     = 0xFF5CB1D6;  // Cyan
        constexpr unsigned int Operators   = 0xFF59C059;  // Green
        constexpr unsigned int Variables   = 0xFFFF8C1A;  // Dark Orange
        constexpr unsigned int MyBlocks    = 0xFFFF6680;  // Pink/Red
        constexpr unsigned int Extensions  = 0xFF0FBD8C;  // Teal
        
        // Combat/MOBA specific
        constexpr unsigned int Combat      = 0xFFE74C3C;  // Red
        constexpr unsigned int Movement    = 0xFF3498DB;  // Blue
        constexpr unsigned int Buffs       = 0xFF9B59B6;  // Purple
        constexpr unsigned int Ultimate    = 0xFFF39C12;  // Gold
    }
    
    //=========================================================================
    // Scratch 3.0 Geometry Constants
    //=========================================================================
    
    namespace Scratch3Geometry
    {
        // Standard block dimensions
        constexpr float MIN_BLOCK_WIDTH   = 48.0f;
        constexpr float MIN_BLOCK_HEIGHT  = 40.0f;
        constexpr float CORNER_RADIUS     = 4.0f;
        
        // Puzzle notch specifications (THE KEY TO SCRATCH LOOK)
        constexpr float NOTCH_WIDTH       = 15.0f;
        constexpr float NOTCH_HEIGHT      = 4.0f;
        constexpr float NOTCH_START_X     = 15.0f;  // Offset from left edge
        
        // Hat block specifications
        constexpr float HAT_HEIGHT        = 20.0f;  // Height of the arc
        constexpr float HAT_WIDTH_RATIO   = 0.8f;   // Width of arc relative to block
        
        // C-Shape specifications
        constexpr float C_ARM_WIDTH       = 16.0f;  // Width of the left arm
        constexpr float C_MOUTH_MIN_H     = 24.0f;  // Minimum inner height
        constexpr float C_FOOTER_HEIGHT   = 24.0f;  // Height of bottom bar
        
        // Reporter/Boolean
        constexpr float REPORTER_RADIUS   = 10.0f;
        constexpr float BOOLEAN_POINT     = 10.0f;
        
        // Visual
        constexpr float STROKE_WIDTH      = 1.0f;
        constexpr float SHADOW_OFFSET_Y   = 1.0f;
        constexpr float ICON_SIZE         = 16.0f;
        constexpr float TEXT_SIZE         = 12.0f;
        constexpr float PADDING           = 8.0f;
    }
    
    //=========================================================================
    // Scratch3BlockRenderer Class
    //=========================================================================
    
    class Scratch3BlockRenderer
    {
    public:
        Scratch3BlockRenderer() = default;
        ~Scratch3BlockRenderer() = default;
        
        //---------------------------------------------------------------------
        // Initialization
        //---------------------------------------------------------------------
        
        void SetContext(NVGcontext* vg) { m_Vg = vg; }
        NVGcontext* GetContext() const { return m_Vg; }
        
        //---------------------------------------------------------------------
        // Main Block Drawing Functions
        //---------------------------------------------------------------------
        
        /// Draw a complete Scratch 3.0 style block
        void DrawBlock(
            float x, float y, 
            float w, float h,
            Scratch3Shape shape,
            unsigned int color,
            const char* label,
            const char* icon = nullptr
        );
        
        /// Draw a C-Shape block with inner mouth
        void DrawCShapeBlock(
            float x, float y,
            float w,
            float headerHeight,
            float mouthHeight,
            float footerHeight,
            unsigned int color,
            const char* label,
            const char* icon = nullptr
        );
        
        /// Draw a reporter (oval) block
        void DrawReporterBlock(
            float x, float y,
            float w, float h,
            unsigned int color,
            const char* label
        );
        
        /// Draw a boolean (hexagon) block
        void DrawBooleanBlock(
            float x, float y,
            float w, float h,
            unsigned int color,
            const char* label
        );
        
        //---------------------------------------------------------------------
        // Inline Input Fields
        //---------------------------------------------------------------------
        
        /// White rounded input field
        void DrawInputField(
            float x, float y,
            float w, float h,
            const char* value,
            bool isNumber = false
        );
        
        /// Dropdown selector
        void DrawDropdown(
            float x, float y,
            float w, float h,
            const char* selectedValue
        );
        
        /// Color swatch
        void DrawColorSwatch(
            float x, float y,
            float size,
            unsigned int color
        );
        
        //---------------------------------------------------------------------
        // Visual Effects
        //---------------------------------------------------------------------
        
        /// Selection highlight (yellow glow)
        void DrawSelectionHighlight(
            float x, float y,
            float w, float h,
            Scratch3Shape shape
        );
        
        /// Execution indicator (green pulse)
        void DrawExecutionIndicator(
            float x, float y,
            float w, float h,
            float pulsePhase
        );
        
        /// Error indicator (red overlay)
        void DrawErrorIndicator(
            float x, float y,
            float w, float h
        );
        
        /// Drop target highlight
        void DrawDropTarget(
            float x, float y,
            float w, float h,
            bool isActive
        );
        
        //---------------------------------------------------------------------
        // Utility
        //---------------------------------------------------------------------
        
        /// Calculate darker stroke color from fill color
        static unsigned int GetStrokeColor(unsigned int fillColor);
        
        /// Calculate block height based on content
        static float CalculateBlockHeight(
            int inputCount, 
            bool hasIcon,
            Scratch3Shape shape
        );
        
    private:
        //---------------------------------------------------------------------
        // Internal Path Builders
        //---------------------------------------------------------------------
        
        /// Draw the Scratch puzzle notch (THE KEY FEATURE)
        void DrawNotch(float x, float y, float direction, bool isMale);
        
        /// Build Hat block path (convex arc top)
        void BuildHatPath(float x, float y, float w, float h);
        
        /// Build Stack block path (standard command)
        void BuildStackPath(float x, float y, float w, float h);
        
        /// Build Cap block path (no bottom notch)
        void BuildCapPath(float x, float y, float w, float h);
        
        /// Build C-Shape path (control with mouth)
        void BuildCShapePath(
            float x, float y, float w,
            float headerH, float mouthH, float footerH
        );
        
        /// Draw subtle bottom shadow (Scratch 3.0 style)
        void DrawBlockShadow(float x, float y, float w, float h);
        
        /// Fill and stroke the current path
        void FillAndStroke(unsigned int fillColor);
        
        /// Draw label text with optional icon
        void DrawLabel(
            float x, float y, 
            float w, float h,
            const char* label,
            const char* icon
        );
        
    private:
        NVGcontext* m_Vg = nullptr;
    };
    
    //=========================================================================
    // Global Accessor
    //=========================================================================
    
    Scratch3BlockRenderer& GetScratch3Renderer();
}
