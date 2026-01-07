#include "NanoVGBlockRenderer.h"
#include <Core/Logger.h>

// OpenGL loader - must be included before NanoVG GL
#include <glad/glad.h>

// NanoVG implementation
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg.h>
#include <nanovg_gl.h>

#include <cmath>
#include <algorithm>

namespace RiftSpire
{
    //=========================================================================
    // Global Instance
    //=========================================================================
    
    static NanoVGBlockRenderer s_BlockRenderer;
    
    NanoVGBlockRenderer& GetBlockRenderer()
    {
        return s_BlockRenderer;
    }
    
    //=========================================================================
    // Constructor / Destructor
    //=========================================================================
    
    NanoVGBlockRenderer::NanoVGBlockRenderer()
    {
    }
    
    NanoVGBlockRenderer::~NanoVGBlockRenderer()
    {
        Shutdown();
    }
    
    //=========================================================================
    // Lifecycle
    //=========================================================================
    
    bool NanoVGBlockRenderer::Initialize()
    {
        if (m_Context)
        {
            return true; // Already initialized
        }
        
        // Create NanoVG context with OpenGL 3 backend
        m_Context = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
        
        if (!m_Context)
        {
            RS_ERROR("NanoVGBlockRenderer: Failed to create NanoVG context");
            return false;
        }
        
        RS_INFO("NanoVGBlockRenderer: Initialized with OpenGL3 backend");
        
        // Load default font (optional - will use ImGui font as fallback)
        // m_FontNormal = nvgCreateFont(m_Context, "sans", "fonts/Roboto-Regular.ttf");
        
        return true;
    }
    
    void NanoVGBlockRenderer::Shutdown()
    {
        if (m_Context)
        {
            nvgDeleteGL3(m_Context);
            m_Context = nullptr;
            RS_INFO("NanoVGBlockRenderer: Shutdown complete");
        }
    }
    
    //=========================================================================
    // Frame Management
    //=========================================================================
    
    void NanoVGBlockRenderer::BeginFrame(float width, float height, float pixelRatio)
    {
        if (m_Context)
        {
            nvgBeginFrame(m_Context, width, height, pixelRatio);
        }
    }
    
    void NanoVGBlockRenderer::EndFrame()
    {
        if (m_Context)
        {
            nvgEndFrame(m_Context);
        }
    }
    
    //=========================================================================
    // Block Rendering - Command Block
    //=========================================================================
    
    void NanoVGBlockRenderer::DrawCommandBlock(
        float x, float y, float w, float h,
        const NVGColor& color,
        bool hasTopNotch, bool hasBottomNotch)
    {
        if (!m_Context) return;
        
        nvgSave(m_Context);
        
        // Build path
        BuildCommandBlockPath(x, y, w, h, hasTopNotch, hasBottomNotch);
        
        // Gradient fill
        ApplyGradientFill(color.Lighten(0.15f), color.Darken(0.1f), y, h);
        nvgFill(m_Context);
        
        // Border
        nvgStrokeColor(m_Context, nvgRGBAf(
            color.r * 0.6f, color.g * 0.6f, color.b * 0.6f, 1.0f));
        nvgStrokeWidth(m_Context, 1.5f);
        nvgStroke(m_Context);
        
        // Highlight stripe at top
        nvgBeginPath(m_Context);
        nvgRoundedRect(m_Context, x + CORNER_RADIUS, y, w - CORNER_RADIUS * 2, 3.0f, 1.5f);
        nvgFillColor(m_Context, nvgRGBAf(1.0f, 1.0f, 1.0f, 0.3f));
        nvgFill(m_Context);
        
        nvgRestore(m_Context);
    }
    
    void NanoVGBlockRenderer::BuildCommandBlockPath(
        float x, float y, float w, float h, 
        bool topNotch, bool bottomNotch)
    {
        nvgBeginPath(m_Context);
        
        // Start from top-left
        nvgMoveTo(m_Context, x + CORNER_RADIUS, y);
        
        // Top edge with notch for input
        if (topNotch)
        {
            nvgLineTo(m_Context, x + NOTCH_OFFSET, y);
            nvgLineTo(m_Context, x + NOTCH_OFFSET, y + NOTCH_HEIGHT);
            nvgLineTo(m_Context, x + NOTCH_OFFSET + NOTCH_WIDTH, y + NOTCH_HEIGHT);
            nvgLineTo(m_Context, x + NOTCH_OFFSET + NOTCH_WIDTH, y);
        }
        
        // Top-right corner
        nvgLineTo(m_Context, x + w - CORNER_RADIUS, y);
        nvgArcTo(m_Context, x + w, y, x + w, y + CORNER_RADIUS, CORNER_RADIUS);
        
        // Right edge
        nvgLineTo(m_Context, x + w, y + h - CORNER_RADIUS);
        
        // Bottom-right corner
        nvgArcTo(m_Context, x + w, y + h, x + w - CORNER_RADIUS, y + h, CORNER_RADIUS);
        
        // Bottom edge with notch for output
        if (bottomNotch)
        {
            nvgLineTo(m_Context, x + NOTCH_OFFSET + NOTCH_WIDTH, y + h);
            nvgLineTo(m_Context, x + NOTCH_OFFSET + NOTCH_WIDTH, y + h + NOTCH_HEIGHT);
            nvgLineTo(m_Context, x + NOTCH_OFFSET, y + h + NOTCH_HEIGHT);
            nvgLineTo(m_Context, x + NOTCH_OFFSET, y + h);
        }
        
        // Bottom-left corner
        nvgLineTo(m_Context, x + CORNER_RADIUS, y + h);
        nvgArcTo(m_Context, x, y + h, x, y + h - CORNER_RADIUS, CORNER_RADIUS);
        
        // Left edge
        nvgLineTo(m_Context, x, y + CORNER_RADIUS);
        
        // Top-left corner
        nvgArcTo(m_Context, x, y, x + CORNER_RADIUS, y, CORNER_RADIUS);
        
        nvgClosePath(m_Context);
    }
    
    //=========================================================================
    // Block Rendering - Hat Block (Event)
    //=========================================================================
    
    void NanoVGBlockRenderer::DrawHatBlock(
        float x, float y, float w, float h,
        const NVGColor& color)
    {
        if (!m_Context) return;
        
        nvgSave(m_Context);
        
        BuildHatBlockPath(x, y, w, h);
        
        // Gradient fill
        ApplyGradientFill(color.Lighten(0.2f), color.Darken(0.05f), y, h);
        nvgFill(m_Context);
        
        // Border
        nvgStrokeColor(m_Context, nvgRGBAf(
            color.r * 0.6f, color.g * 0.6f, color.b * 0.6f, 1.0f));
        nvgStrokeWidth(m_Context, 1.5f);
        nvgStroke(m_Context);
        
        nvgRestore(m_Context);
    }
    
    void NanoVGBlockRenderer::BuildHatBlockPath(float x, float y, float w, float h)
    {
        nvgBeginPath(m_Context);
        
        // Hat curve at top
        nvgMoveTo(m_Context, x, y + HAT_CURVE_HEIGHT);
        nvgBezierTo(m_Context, 
            x, y, 
            x + w * 0.3f, y,
            x + w * 0.5f, y);
        nvgBezierTo(m_Context,
            x + w * 0.7f, y,
            x + w, y,
            x + w, y + HAT_CURVE_HEIGHT);
        
        // Right edge
        nvgLineTo(m_Context, x + w, y + h - CORNER_RADIUS);
        
        // Bottom-right corner
        nvgArcTo(m_Context, x + w, y + h, x + w - CORNER_RADIUS, y + h, CORNER_RADIUS);
        
        // Bottom with notch
        nvgLineTo(m_Context, x + NOTCH_OFFSET + NOTCH_WIDTH, y + h);
        nvgLineTo(m_Context, x + NOTCH_OFFSET + NOTCH_WIDTH, y + h + NOTCH_HEIGHT);
        nvgLineTo(m_Context, x + NOTCH_OFFSET, y + h + NOTCH_HEIGHT);
        nvgLineTo(m_Context, x + NOTCH_OFFSET, y + h);
        
        // Bottom-left corner
        nvgLineTo(m_Context, x + CORNER_RADIUS, y + h);
        nvgArcTo(m_Context, x, y + h, x, y + h - CORNER_RADIUS, CORNER_RADIUS);
        
        nvgClosePath(m_Context);
    }
    
    //=========================================================================
    // Block Rendering - Cap Block (End)
    //=========================================================================
    
    void NanoVGBlockRenderer::DrawCapBlock(
        float x, float y, float w, float h,
        const NVGColor& color)
    {
        if (!m_Context) return;
        
        nvgSave(m_Context);
        
        BuildCapBlockPath(x, y, w, h);
        
        ApplyGradientFill(color.Lighten(0.15f), color.Darken(0.1f), y, h);
        nvgFill(m_Context);
        
        nvgStrokeColor(m_Context, nvgRGBAf(
            color.r * 0.6f, color.g * 0.6f, color.b * 0.6f, 1.0f));
        nvgStrokeWidth(m_Context, 1.5f);
        nvgStroke(m_Context);
        
        nvgRestore(m_Context);
    }
    
    void NanoVGBlockRenderer::BuildCapBlockPath(float x, float y, float w, float h)
    {
        nvgBeginPath(m_Context);
        
        // Top with notch
        nvgMoveTo(m_Context, x + CORNER_RADIUS, y);
        nvgLineTo(m_Context, x + NOTCH_OFFSET, y);
        nvgLineTo(m_Context, x + NOTCH_OFFSET, y + NOTCH_HEIGHT);
        nvgLineTo(m_Context, x + NOTCH_OFFSET + NOTCH_WIDTH, y + NOTCH_HEIGHT);
        nvgLineTo(m_Context, x + NOTCH_OFFSET + NOTCH_WIDTH, y);
        
        // Top-right
        nvgLineTo(m_Context, x + w - CORNER_RADIUS, y);
        nvgArcTo(m_Context, x + w, y, x + w, y + CORNER_RADIUS, CORNER_RADIUS);
        
        // Right
        nvgLineTo(m_Context, x + w, y + h - CORNER_RADIUS);
        
        // Bottom-right (rounded, no notch)
        nvgArcTo(m_Context, x + w, y + h, x + w - CORNER_RADIUS, y + h, CORNER_RADIUS);
        
        // Bottom (flat, no notch)
        nvgLineTo(m_Context, x + CORNER_RADIUS, y + h);
        
        // Bottom-left
        nvgArcTo(m_Context, x, y + h, x, y + h - CORNER_RADIUS, CORNER_RADIUS);
        
        // Left
        nvgLineTo(m_Context, x, y + CORNER_RADIUS);
        
        // Top-left
        nvgArcTo(m_Context, x, y, x + CORNER_RADIUS, y, CORNER_RADIUS);
        
        nvgClosePath(m_Context);
    }
    
    //=========================================================================
    // Block Rendering - C-Block (Control)
    //=========================================================================
    
    void NanoVGBlockRenderer::DrawCBlock(
        float x, float y, float w,
        float headerHeight, float innerHeight, float footerHeight,
        const NVGColor& color)
    {
        if (!m_Context) return;
        
        nvgSave(m_Context);
        
        float totalHeight = headerHeight + innerHeight + footerHeight;
        
        BuildCBlockPath(x, y, w, headerHeight, innerHeight, footerHeight);
        
        ApplyGradientFill(color.Lighten(0.15f), color.Darken(0.1f), y, totalHeight);
        nvgFill(m_Context);
        
        // Border
        nvgStrokeColor(m_Context, nvgRGBAf(
            color.r * 0.6f, color.g * 0.6f, color.b * 0.6f, 1.0f));
        nvgStrokeWidth(m_Context, 1.5f);
        nvgStroke(m_Context);
        
        // Inner slot (darker background)
        nvgBeginPath(m_Context);
        nvgRoundedRect(m_Context, 
            x + C_BLOCK_ARM_WIDTH, y + headerHeight,
            w - C_BLOCK_ARM_WIDTH - CORNER_RADIUS, innerHeight,
            CORNER_RADIUS);
        nvgFillColor(m_Context, nvgRGBA(20, 22, 35, 180));
        nvgFill(m_Context);
        
        nvgRestore(m_Context);
    }
    
    void NanoVGBlockRenderer::BuildCBlockPath(
        float x, float y, float w, 
        float headerH, float innerH, float footerH)
    {
        nvgBeginPath(m_Context);
        
        // Top with input notch
        nvgMoveTo(m_Context, x + CORNER_RADIUS, y);
        nvgLineTo(m_Context, x + NOTCH_OFFSET, y);
        nvgLineTo(m_Context, x + NOTCH_OFFSET, y + NOTCH_HEIGHT);
        nvgLineTo(m_Context, x + NOTCH_OFFSET + NOTCH_WIDTH, y + NOTCH_HEIGHT);
        nvgLineTo(m_Context, x + NOTCH_OFFSET + NOTCH_WIDTH, y);
        nvgLineTo(m_Context, x + w - CORNER_RADIUS, y);
        nvgArcTo(m_Context, x + w, y, x + w, y + CORNER_RADIUS, CORNER_RADIUS);
        
        // Right side of header
        nvgLineTo(m_Context, x + w, y + headerH);
        
        // Step in for inner slot
        nvgLineTo(m_Context, x + C_BLOCK_ARM_WIDTH + CORNER_RADIUS, y + headerH);
        nvgArcTo(m_Context, x + C_BLOCK_ARM_WIDTH, y + headerH, x + C_BLOCK_ARM_WIDTH, y + headerH + CORNER_RADIUS, CORNER_RADIUS);
        
        // Inner slot left side
        nvgLineTo(m_Context, x + C_BLOCK_ARM_WIDTH, y + headerH + innerH - CORNER_RADIUS);
        nvgArcTo(m_Context, x + C_BLOCK_ARM_WIDTH, y + headerH + innerH, x + C_BLOCK_ARM_WIDTH + CORNER_RADIUS, y + headerH + innerH, CORNER_RADIUS);
        
        // Inner slot with output notch
        nvgLineTo(m_Context, x + C_BLOCK_ARM_WIDTH + NOTCH_OFFSET, y + headerH + innerH);
        nvgLineTo(m_Context, x + C_BLOCK_ARM_WIDTH + NOTCH_OFFSET, y + headerH + innerH + NOTCH_HEIGHT);
        nvgLineTo(m_Context, x + C_BLOCK_ARM_WIDTH + NOTCH_OFFSET + NOTCH_WIDTH, y + headerH + innerH + NOTCH_HEIGHT);
        nvgLineTo(m_Context, x + C_BLOCK_ARM_WIDTH + NOTCH_OFFSET + NOTCH_WIDTH, y + headerH + innerH);
        
        // Step out
        nvgLineTo(m_Context, x + w, y + headerH + innerH);
        
        // Right side of footer
        nvgLineTo(m_Context, x + w, y + headerH + innerH + footerH - CORNER_RADIUS);
        nvgArcTo(m_Context, x + w, y + headerH + innerH + footerH, x + w - CORNER_RADIUS, y + headerH + innerH + footerH, CORNER_RADIUS);
        
        // Bottom with output notch
        float bottomY = y + headerH + innerH + footerH;
        nvgLineTo(m_Context, x + NOTCH_OFFSET + NOTCH_WIDTH, bottomY);
        nvgLineTo(m_Context, x + NOTCH_OFFSET + NOTCH_WIDTH, bottomY + NOTCH_HEIGHT);
        nvgLineTo(m_Context, x + NOTCH_OFFSET, bottomY + NOTCH_HEIGHT);
        nvgLineTo(m_Context, x + NOTCH_OFFSET, bottomY);
        
        nvgLineTo(m_Context, x + CORNER_RADIUS, bottomY);
        nvgArcTo(m_Context, x, bottomY, x, bottomY - CORNER_RADIUS, CORNER_RADIUS);
        
        // Left side
        nvgLineTo(m_Context, x, y + CORNER_RADIUS);
        nvgArcTo(m_Context, x, y, x + CORNER_RADIUS, y, CORNER_RADIUS);
        
        nvgClosePath(m_Context);
    }
    
    //=========================================================================
    // Block Rendering - Reporter (Oval)
    //=========================================================================
    
    void NanoVGBlockRenderer::DrawReporterBlock(
        float x, float y, float w, float h,
        const NVGColor& color)
    {
        if (!m_Context) return;
        
        nvgSave(m_Context);
        
        float r = h / 2.0f;  // Fully rounded ends
        
        nvgBeginPath(m_Context);
        nvgRoundedRect(m_Context, x, y, w, h, r);
        
        ApplyGradientFill(color.Lighten(0.15f), color.Darken(0.1f), y, h);
        nvgFill(m_Context);
        
        nvgStrokeColor(m_Context, nvgRGBAf(
            color.r * 0.6f, color.g * 0.6f, color.b * 0.6f, 1.0f));
        nvgStrokeWidth(m_Context, 1.5f);
        nvgStroke(m_Context);
        
        nvgRestore(m_Context);
    }
    
    //=========================================================================
    // Block Rendering - Boolean (Hexagonal)
    //=========================================================================
    
    void NanoVGBlockRenderer::DrawBooleanBlock(
        float x, float y, float w, float h,
        const NVGColor& color)
    {
        if (!m_Context) return;
        
        nvgSave(m_Context);
        
        float pointWidth = h / 2.0f;
        
        nvgBeginPath(m_Context);
        nvgMoveTo(m_Context, x + pointWidth, y);
        nvgLineTo(m_Context, x + w - pointWidth, y);
        nvgLineTo(m_Context, x + w, y + h / 2.0f);
        nvgLineTo(m_Context, x + w - pointWidth, y + h);
        nvgLineTo(m_Context, x + pointWidth, y + h);
        nvgLineTo(m_Context, x, y + h / 2.0f);
        nvgClosePath(m_Context);
        
        ApplyGradientFill(color.Lighten(0.15f), color.Darken(0.1f), y, h);
        nvgFill(m_Context);
        
        nvgStrokeColor(m_Context, nvgRGBAf(
            color.r * 0.6f, color.g * 0.6f, color.b * 0.6f, 1.0f));
        nvgStrokeWidth(m_Context, 1.5f);
        nvgStroke(m_Context);
        
        nvgRestore(m_Context);
    }
    
    //=========================================================================
    // Visual Effects
    //=========================================================================
    
    void NanoVGBlockRenderer::DrawBlockShadow(
        float x, float y, float w, float h, float cornerRadius)
    {
        if (!m_Context) return;
        
        float shadowOffset = 3.0f;
        float shadowBlur = 8.0f;
        
        NVGpaint shadowPaint = nvgBoxGradient(m_Context,
            x + shadowOffset, y + shadowOffset + 2,
            w, h, cornerRadius, shadowBlur,
            nvgRGBA(0, 0, 0, 80),
            nvgRGBA(0, 0, 0, 0));
        
        nvgBeginPath(m_Context);
        nvgRoundedRect(m_Context, x + shadowOffset - shadowBlur, y + shadowOffset - shadowBlur,
            w + shadowBlur * 2, h + shadowBlur * 2, cornerRadius);
        nvgFillPaint(m_Context, shadowPaint);
        nvgFill(m_Context);
    }
    
    void NanoVGBlockRenderer::DrawSelectionGlow(
        float x, float y, float w, float h, float cornerRadius)
    {
        if (!m_Context) return;
        
        float glowSize = 4.0f;
        
        nvgBeginPath(m_Context);
        nvgRoundedRect(m_Context, x - glowSize, y - glowSize,
            w + glowSize * 2, h + glowSize * 2, cornerRadius + glowSize);
        nvgStrokeColor(m_Context, nvgRGBA(255, 255, 100, 255));
        nvgStrokeWidth(m_Context, 3.0f);
        nvgStroke(m_Context);
    }
    
    void NanoVGBlockRenderer::DrawExecutionHighlight(
        float x, float y, float w, float h, float cornerRadius, float pulsePhase)
    {
        if (!m_Context) return;
        
        float alpha = 0.5f + 0.5f * std::sin(pulsePhase);
        float glowSize = 3.0f + 2.0f * alpha;
        
        nvgBeginPath(m_Context);
        nvgRoundedRect(m_Context, x - glowSize, y - glowSize,
            w + glowSize * 2, h + glowSize * 2, cornerRadius + glowSize);
        nvgStrokeColor(m_Context, nvgRGBA(100, 255, 100, static_cast<int>(200 * alpha)));
        nvgStrokeWidth(m_Context, 2.0f);
        nvgStroke(m_Context);
    }
    
    void NanoVGBlockRenderer::DrawErrorOverlay(
        float x, float y, float w, float h, float cornerRadius)
    {
        if (!m_Context) return;
        
        nvgBeginPath(m_Context);
        nvgRoundedRect(m_Context, x, y, w, h, cornerRadius);
        nvgFillColor(m_Context, nvgRGBA(255, 0, 0, 50));
        nvgFill(m_Context);
        
        nvgStrokeColor(m_Context, nvgRGBA(255, 100, 100, 200));
        nvgStrokeWidth(m_Context, 2.0f);
        nvgStroke(m_Context);
    }
    
    void NanoVGBlockRenderer::DrawDebugHighlight(
        float x, float y, float w, float h, float cornerRadius)
    {
        if (!m_Context) return;
        
        nvgBeginPath(m_Context);
        nvgRoundedRect(m_Context, x - 2, y - 2, w + 4, h + 4, cornerRadius + 2);
        nvgStrokeColor(m_Context, nvgRGBA(255, 165, 0, 200));
        nvgStrokeWidth(m_Context, 2.0f);
        nvgStroke(m_Context);
    }
    
    //=========================================================================
    // Connection Lines
    //=========================================================================
    
    void NanoVGBlockRenderer::DrawConnection(
        float x1, float y1, float x2, float y2,
        const NVGColor& color, float thickness)
    {
        if (!m_Context) return;
        
        float dx = x2 - x1;
        float dy = y2 - y1;
        float controlDist = std::max(50.0f, std::abs(dx) * 0.5f);
        
        nvgBeginPath(m_Context);
        nvgMoveTo(m_Context, x1, y1);
        nvgBezierTo(m_Context,
            x1 + controlDist, y1,
            x2 - controlDist, y2,
            x2, y2);
        
        nvgStrokeColor(m_Context, nvgRGBAf(color.r, color.g, color.b, color.a));
        nvgStrokeWidth(m_Context, thickness);
        nvgStroke(m_Context);
    }
    
    void NanoVGBlockRenderer::DrawDashedConnection(
        float x1, float y1, float x2, float y2,
        const NVGColor& color)
    {
        if (!m_Context) return;
        
        // NanoVG doesn't have native dashed lines, simulate with dots
        float dx = x2 - x1;
        float dy = y2 - y1;
        float length = std::sqrt(dx * dx + dy * dy);
        int segments = static_cast<int>(length / 10.0f);
        
        nvgBeginPath(m_Context);
        for (int i = 0; i < segments; i += 2)
        {
            float t1 = static_cast<float>(i) / segments;
            float t2 = static_cast<float>(std::min(i + 1, segments)) / segments;
            
            nvgMoveTo(m_Context, x1 + dx * t1, y1 + dy * t1);
            nvgLineTo(m_Context, x1 + dx * t2, y1 + dy * t2);
        }
        
        nvgStrokeColor(m_Context, nvgRGBAf(color.r, color.g, color.b, color.a * 0.7f));
        nvgStrokeWidth(m_Context, 2.0f);
        nvgStroke(m_Context);
    }
    
    //=========================================================================
    // Text
    //=========================================================================
    
    void NanoVGBlockRenderer::DrawText(
        float x, float y, const std::string& text,
        const NVGColor& color, float fontSize)
    {
        if (!m_Context) return;
        
        nvgFontSize(m_Context, fontSize);
        nvgFillColor(m_Context, nvgRGBAf(color.r, color.g, color.b, color.a));
        nvgText(m_Context, x, y + fontSize, text.c_str(), nullptr);
    }
    
    void NanoVGBlockRenderer::DrawTextWithShadow(
        float x, float y, const std::string& text,
        const NVGColor& color, float fontSize)
    {
        if (!m_Context) return;
        
        nvgFontSize(m_Context, fontSize);
        
        // Shadow
        nvgFillColor(m_Context, nvgRGBA(0, 0, 0, 80));
        nvgText(m_Context, x + 1, y + fontSize + 1, text.c_str(), nullptr);
        
        // Text
        nvgFillColor(m_Context, nvgRGBAf(color.r, color.g, color.b, color.a));
        nvgText(m_Context, x, y + fontSize, text.c_str(), nullptr);
    }
    
    void NanoVGBlockRenderer::DrawIcon(float x, float y, const std::string& icon, float size)
    {
        if (!m_Context) return;
        
        nvgFontSize(m_Context, size);
        nvgFillColor(m_Context, nvgRGBA(255, 255, 255, 220));
        nvgText(m_Context, x, y + size, icon.c_str(), nullptr);
    }
    
    //=========================================================================
    // Input Fields
    //=========================================================================
    
    void NanoVGBlockRenderer::DrawInputField(
        float x, float y, float w, float h,
        const std::string& text, bool isNumber)
    {
        if (!m_Context) return;
        
        nvgBeginPath(m_Context);
        nvgRoundedRect(m_Context, x, y, w, h, 4.0f);
        nvgFillColor(m_Context, nvgRGBA(255, 255, 255, 230));
        nvgFill(m_Context);
        
        nvgFontSize(m_Context, 13.0f);
        nvgFillColor(m_Context, nvgRGBA(40, 40, 50, 255));
        nvgText(m_Context, x + 4, y + h * 0.7f, text.c_str(), nullptr);
    }
    
    void NanoVGBlockRenderer::DrawDropdownField(
        float x, float y, float w, float h,
        const std::string& selectedText)
    {
        if (!m_Context) return;
        
        nvgBeginPath(m_Context);
        nvgRoundedRect(m_Context, x, y, w, h, 4.0f);
        nvgFillColor(m_Context, nvgRGBA(240, 240, 250, 255));
        nvgFill(m_Context);
        
        // Text
        nvgFontSize(m_Context, 13.0f);
        nvgFillColor(m_Context, nvgRGBA(40, 40, 50, 255));
        nvgText(m_Context, x + 4, y + h * 0.7f, selectedText.c_str(), nullptr);
        
        // Arrow
        nvgText(m_Context, x + w - 14, y + h * 0.7f, "▼", nullptr);
    }
    
    void NanoVGBlockRenderer::DrawBooleanToggle(float x, float y, float size, bool value)
    {
        if (!m_Context) return;
        
        NVGcolor bgColor = value ? nvgRGB(80, 200, 120) : nvgRGB(200, 80, 80);
        
        nvgBeginPath(m_Context);
        nvgRoundedRect(m_Context, x, y, size, size, 4.0f);
        nvgFillColor(m_Context, bgColor);
        nvgFill(m_Context);
        
        const char* icon = value ? "✓" : "✗";
        nvgFontSize(m_Context, 14.0f);
        nvgFillColor(m_Context, nvgRGBA(255, 255, 255, 255));
        nvgText(m_Context, x + 4, y + size * 0.75f, icon, nullptr);
    }
    
    void NanoVGBlockRenderer::DrawColorSwatch(float x, float y, float size, const NVGColor& color)
    {
        if (!m_Context) return;
        
        nvgBeginPath(m_Context);
        nvgRoundedRect(m_Context, x, y, size, size, 4.0f);
        nvgFillColor(m_Context, nvgRGBAf(color.r, color.g, color.b, color.a));
        nvgFill(m_Context);
        
        nvgStrokeColor(m_Context, nvgRGBA(0, 0, 0, 100));
        nvgStrokeWidth(m_Context, 1.0f);
        nvgStroke(m_Context);
    }
    
    //=========================================================================
    // Clipping
    //=========================================================================
    
    void NanoVGBlockRenderer::PushClipRect(float x, float y, float w, float h)
    {
        if (m_Context)
        {
            nvgScissor(m_Context, x, y, w, h);
        }
    }
    
    void NanoVGBlockRenderer::PopClipRect()
    {
        if (m_Context)
        {
            nvgResetScissor(m_Context);
        }
    }
    
    //=========================================================================
    // Gradient Helper
    //=========================================================================
    
    void NanoVGBlockRenderer::ApplyGradientFill(
        const NVGColor& top, const NVGColor& bottom, float y, float h)
    {
        NVGpaint gradient = nvgLinearGradient(m_Context,
            0, y, 0, y + h,
            nvgRGBAf(top.r, top.g, top.b, top.a),
            nvgRGBAf(bottom.r, bottom.g, bottom.b, bottom.a));
        nvgFillPaint(m_Context, gradient);
    }
    
    //=========================================================================
    // Category Colors
    //=========================================================================
    
    NVGColor NanoVGBlockRenderer::GetCategoryColor(int category)
    {
        // Scratch-style category colors
        switch (category)
        {
            case 0:  return NVGColor::FromRGB(76, 151, 255);   // Motion - Blue
            case 1:  return NVGColor::FromRGB(255, 171, 25);   // Looks - Orange
            case 2:  return NVGColor::FromRGB(207, 99, 207);   // Sound - Purple
            case 3:  return NVGColor::FromRGB(255, 189, 38);   // Events - Yellow
            case 4:  return NVGColor::FromRGB(255, 171, 25);   // Control - Orange
            case 5:  return NVGColor::FromRGB(92, 177, 214);   // Sensing - Cyan
            case 6:  return NVGColor::FromRGB(89, 192, 89);    // Operators - Green
            case 7:  return NVGColor::FromRGB(255, 140, 26);   // Variables - Orange
            case 8:  return NVGColor::FromRGB(255, 102, 128);  // My Blocks - Pink
            default: return NVGColor::FromRGB(128, 128, 128);  // Default - Gray
        }
    }
    
    NVGColor NanoVGBlockRenderer::GetHighlightColor(const NVGColor& base)
    {
        return base.Lighten(0.2f);
    }
    
    NVGColor NanoVGBlockRenderer::GetShadowColor(const NVGColor& base)
    {
        return base.Darken(0.3f);
    }
    
    NVGColor NanoVGBlockRenderer::GetBorderColor(const NVGColor& base)
    {
        return base.Darken(0.4f);
    }
}
