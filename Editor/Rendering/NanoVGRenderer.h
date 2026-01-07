#pragma once

#include <memory>

namespace RiftSpire
{
    //=========================================================================
    // NanoVGRenderer - High-level wrapper for NanoVG rendering
    //=========================================================================
    // This class provides a clean C++ interface for NanoVG operations.
    // Used for premium block canvas rendering with gradients and anti-aliasing.
    // 
    // NOTE: This is currently a STUB implementation. Full NanoVG integration
    // is pending. All methods are no-ops. Block rendering continues to use
    // ImGui DrawList until NanoVG is properly integrated.
    //=========================================================================
    
    // Forward declaration (full NanoVG not yet integrated)
    struct NVGcontext;
    
    class NanoVGRenderer
    {
    public:
        NanoVGRenderer() = default;
        ~NanoVGRenderer();
        
        // Lifecycle
        bool Initialize();
        void Shutdown();
        bool IsInitialized() const { return m_Context != nullptr; }
        
        // Frame management
        void BeginFrame(float width, float height, float devicePixelRatio = 1.0f);
        void EndFrame();
        void CancelFrame();
        
        // State
        void Save();
        void Restore();
        void Reset();
        
        //---------------------------------------------------------------------
        // Shapes
        //---------------------------------------------------------------------
        
        void BeginPath();
        void ClosePath();
        
        // Basic shapes
        void Rect(float x, float y, float w, float h);
        void RoundedRect(float x, float y, float w, float h, float r);
        void RoundedRectVarying(float x, float y, float w, float h, 
                                float radiusTL, float radiusTR, 
                                float radiusBR, float radiusBL);
        void Circle(float cx, float cy, float r);
        void Ellipse(float cx, float cy, float rx, float ry);
        
        // Path operations
        void MoveTo(float x, float y);
        void LineTo(float x, float y);
        void BezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y);
        void QuadTo(float cx, float cy, float x, float y);
        void ArcTo(float x1, float y1, float x2, float y2, float radius);
        void Arc(float cx, float cy, float r, float a0, float a1, int dir);
        
        //---------------------------------------------------------------------
        // Fill & Stroke
        //---------------------------------------------------------------------
        
        void Fill();
        void Stroke();
        
        void FillColor(float r, float g, float b, float a = 1.0f);
        void FillColor(unsigned int rgba);
        void StrokeColor(float r, float g, float b, float a = 1.0f);
        void StrokeColor(unsigned int rgba);
        void StrokeWidth(float width);
        
        //---------------------------------------------------------------------
        // Gradients
        //---------------------------------------------------------------------
        
        void FillLinearGradient(float sx, float sy, float ex, float ey,
                                float r1, float g1, float b1, float a1,
                                float r2, float g2, float b2, float a2);
        
        void FillBoxGradient(float x, float y, float w, float h, float r, float f,
                             float r1, float g1, float b1, float a1,
                             float r2, float g2, float b2, float a2);
        
        void FillRadialGradient(float cx, float cy, float inr, float outr,
                                float r1, float g1, float b1, float a1,
                                float r2, float g2, float b2, float a2);
        
        //---------------------------------------------------------------------
        // Text
        //---------------------------------------------------------------------
        
        int LoadFont(const char* name, const char* path);
        void FontFace(const char* font);
        void FontSize(float size);
        void TextAlign(int align);
        float Text(float x, float y, const char* text);
        void TextBox(float x, float y, float breakRowWidth, const char* text);
        
        //---------------------------------------------------------------------
        // Scissoring (Clipping)
        //---------------------------------------------------------------------
        
        void Scissor(float x, float y, float w, float h);
        void ResetScissor();
        
        //---------------------------------------------------------------------
        // Transform
        //---------------------------------------------------------------------
        
        void Translate(float x, float y);
        void Rotate(float angle);
        void Scale(float x, float y);
        void ResetTransform();
        
        //---------------------------------------------------------------------
        // Higher-level drawing utilities for blocks
        //---------------------------------------------------------------------
        
        // Draw a Scratch-style block with notches
        void DrawScratchBlock(float x, float y, float w, float h,
                              float r, float g, float b, float a,
                              bool hasTopNotch, bool hasBottomNotch);
        
        // Draw a C-block wrapper
        void DrawCBlock(float x, float y, float w, float h,
                        float innerHeight, float r, float g, float b, float a);
        
        // Draw connection bezier
        void DrawConnectionBezier(float x1, float y1, float x2, float y2,
                                  float r, float g, float b, float a,
                                  float thickness = 2.0f);
        
        // Draw shadow under block
        void DrawBlockShadow(float x, float y, float w, float h, float radius = 8.0f);
        
        // Get raw context (for advanced usage)
        NVGcontext* GetContext() { return m_Context; }
        
    private:
        NVGcontext* m_Context = nullptr;
        int m_DefaultFont = -1;
    };
    
    // Global accessor (initialized in Application)
    NanoVGRenderer& GetNanoVG();
}
