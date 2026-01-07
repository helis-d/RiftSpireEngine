#include "NanoVGRenderer.h"
#include <Core/Logger.h>

//=============================================================================
// STUB IMPLEMENTATION
// NanoVG is not yet fully integrated. All functions are no-ops.
// This allows the editor layout to work while NanoVG integration is pending.
// The actual block rendering continues to use ImGui DrawList for now.
//=============================================================================

namespace RiftSpire
{
    // Global instance
    static NanoVGRenderer s_NanoVGRenderer;
    
    NanoVGRenderer& GetNanoVG()
    {
        return s_NanoVGRenderer;
    }
    
    NanoVGRenderer::~NanoVGRenderer()
    {
        Shutdown();
    }
    
    bool NanoVGRenderer::Initialize()
    {
        RS_INFO("NanoVGRenderer: Stub initialization (NanoVG not yet integrated)");
        // m_Context remains nullptr - all rendering is no-op
        return true;
    }
    
    void NanoVGRenderer::Shutdown()
    {
        m_Context = nullptr;
    }
    
    // Frame management - no-op
    void NanoVGRenderer::BeginFrame(float, float, float) {}
    void NanoVGRenderer::EndFrame() {}
    void NanoVGRenderer::CancelFrame() {}
    
    // State - no-op
    void NanoVGRenderer::Save() {}
    void NanoVGRenderer::Restore() {}
    void NanoVGRenderer::Reset() {}
    
    // Shapes - no-op
    void NanoVGRenderer::BeginPath() {}
    void NanoVGRenderer::ClosePath() {}
    void NanoVGRenderer::Rect(float, float, float, float) {}
    void NanoVGRenderer::RoundedRect(float, float, float, float, float) {}
    void NanoVGRenderer::RoundedRectVarying(float, float, float, float, float, float, float, float) {}
    void NanoVGRenderer::Circle(float, float, float) {}
    void NanoVGRenderer::Ellipse(float, float, float, float) {}
    
    // Path operations - no-op
    void NanoVGRenderer::MoveTo(float, float) {}
    void NanoVGRenderer::LineTo(float, float) {}
    void NanoVGRenderer::BezierTo(float, float, float, float, float, float) {}
    void NanoVGRenderer::QuadTo(float, float, float, float) {}
    void NanoVGRenderer::ArcTo(float, float, float, float, float) {}
    void NanoVGRenderer::Arc(float, float, float, float, float, int) {}
    
    // Fill & Stroke - no-op
    void NanoVGRenderer::Fill() {}
    void NanoVGRenderer::Stroke() {}
    void NanoVGRenderer::FillColor(float, float, float, float) {}
    void NanoVGRenderer::FillColor(unsigned int) {}
    void NanoVGRenderer::StrokeColor(float, float, float, float) {}
    void NanoVGRenderer::StrokeColor(unsigned int) {}
    void NanoVGRenderer::StrokeWidth(float) {}
    
    // Gradients - no-op
    void NanoVGRenderer::FillLinearGradient(float, float, float, float, float, float, float, float, float, float, float, float) {}
    void NanoVGRenderer::FillBoxGradient(float, float, float, float, float, float, float, float, float, float, float, float, float, float) {}
    void NanoVGRenderer::FillRadialGradient(float, float, float, float, float, float, float, float, float, float, float, float) {}
    
    // Text - no-op
    int NanoVGRenderer::LoadFont(const char*, const char*) { return -1; }
    void NanoVGRenderer::FontFace(const char*) {}
    void NanoVGRenderer::FontSize(float) {}
    void NanoVGRenderer::TextAlign(int) {}
    float NanoVGRenderer::Text(float, float, const char*) { return 0.0f; }
    void NanoVGRenderer::TextBox(float, float, float, const char*) {}
    
    // Scissoring - no-op
    void NanoVGRenderer::Scissor(float, float, float, float) {}
    void NanoVGRenderer::ResetScissor() {}
    
    // Transform - no-op
    void NanoVGRenderer::Translate(float, float) {}
    void NanoVGRenderer::Rotate(float) {}
    void NanoVGRenderer::Scale(float, float) {}
    void NanoVGRenderer::ResetTransform() {}
    
    // Block-specific drawing utilities - no-op (block rendering uses ImDrawList)
    void NanoVGRenderer::DrawScratchBlock(float, float, float, float, float, float, float, float, bool, bool) {}
    void NanoVGRenderer::DrawCBlock(float, float, float, float, float, float, float, float, float) {}
    void NanoVGRenderer::DrawConnectionBezier(float, float, float, float, float, float, float, float, float) {}
    void NanoVGRenderer::DrawBlockShadow(float, float, float, float, float) {}
}
