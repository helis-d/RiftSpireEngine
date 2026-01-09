#include "Scratch3BlockRenderer.h"
#include <Core/Logger.h>

// OpenGL loader - must come before NanoVG
#include <glad/glad.h>

// NanoVG
#include <nanovg.h>

#include <cmath>
#include <algorithm>

namespace RiftSpire
{
    using namespace Scratch3Geometry;
    
    //=========================================================================
    // Global Instance
    //=========================================================================
    
    static Scratch3BlockRenderer s_Renderer;
    
    Scratch3BlockRenderer& GetScratch3Renderer()
    {
        return s_Renderer;
    }
    
    //=========================================================================
    // Color Utilities
    //=========================================================================
    
    unsigned int Scratch3BlockRenderer::GetStrokeColor(unsigned int fillColor)
    {
        // Extract RGBA
        unsigned int r = (fillColor >> 16) & 0xFF;
        unsigned int g = (fillColor >> 8) & 0xFF;
        unsigned int b = fillColor & 0xFF;
        unsigned int a = (fillColor >> 24) & 0xFF;
        
        // Darken by 25% for more contrast
        r = static_cast<unsigned int>(r * 0.75f);
        g = static_cast<unsigned int>(g * 0.75f);
        b = static_cast<unsigned int>(b * 0.75f);
        
        return (a << 24) | (r << 16) | (g << 8) | b;
    }
    
    static unsigned int GetDarkenedColor(unsigned int fillColor, float factor)
    {
        unsigned int r = (fillColor >> 16) & 0xFF;
        unsigned int g = (fillColor >> 8) & 0xFF;
        unsigned int b = fillColor & 0xFF;
        unsigned int a = (fillColor >> 24) & 0xFF;
        
        r = static_cast<unsigned int>(r * factor);
        g = static_cast<unsigned int>(g * factor);
        b = static_cast<unsigned int>(b * factor);
        
        return (a << 24) | (r << 16) | (g << 8) | b;
    }
    
    static NVGcolor ColorFromU32(unsigned int color)
    {
        float r = ((color >> 16) & 0xFF) / 255.0f;
        float g = ((color >> 8) & 0xFF) / 255.0f;
        float b = (color & 0xFF) / 255.0f;
        float a = ((color >> 24) & 0xFF) / 255.0f;
        if (a == 0.0f) a = 1.0f;  // Default to opaque
        return nvgRGBAf(r, g, b, a);
    }
    
    //=========================================================================
    // THE SCRATCH PUZZLE NOTCH (Critical for authentic look)
    //=========================================================================
    // This creates the distinctive puzzle-piece connector that makes blocks
    // snap together. The notch is an S-curve that provides visual connection.
    //=========================================================================
    
    void Scratch3BlockRenderer::DrawNotch(float x, float y, float direction, bool isMale)
    {
        if (!m_Vg) return;
        
        // direction: 1.0 = going right, -1.0 = going left
        // isMale: true = protrusion (bottom), false = indentation (top)
        
        float notchDepth = isMale ? NOTCH_HEIGHT : -NOTCH_HEIGHT;
        
        // The notch profile:
        // Start -> curve down/up -> flat bottom -> curve up/down -> end
        //
        //     ___      ___
        //    /   \    /   \   <- Male (protrusion)
        // __|     |__|     |__
        //
        // Inverse for female (indentation)
        
        float hw = NOTCH_WIDTH / 2.0f;  // Half width
        float curveW = 3.0f;  // Width of the curved transition
        
        // Current point is at (x, y), we need to create the notch
        
        // Approach curve
        nvgBezierTo(m_Vg,
            x + curveW * direction, y,
            x + curveW * direction, y + notchDepth * 0.5f,
            x + curveW * direction, y + notchDepth
        );
        
        // Flat bottom of notch
        nvgLineTo(m_Vg, x + (hw - curveW) * direction, y + notchDepth);
        
        // Exit curve
        nvgBezierTo(m_Vg,
            x + (hw) * direction, y + notchDepth,
            x + (hw + curveW) * direction, y + notchDepth,
            x + (hw + curveW) * direction, y
        );
    }
    
    //=========================================================================
    // Path Builders
    //=========================================================================
    
    void Scratch3BlockRenderer::BuildStackPath(float x, float y, float w, float h)
    {
        if (!m_Vg) return;
        
        float r = CORNER_RADIUS;
        float notchX = NOTCH_START_X;
        
        nvgBeginPath(m_Vg);
        
        // === TOP EDGE (with female notch - indentation) ===
        nvgMoveTo(m_Vg, x + r, y);
        
        // Before notch
        nvgLineTo(m_Vg, x + notchX, y);
        
        // Female notch (goes UP into the block)
        nvgLineTo(m_Vg, x + notchX + 2, y);
        nvgLineTo(m_Vg, x + notchX + 2, y + NOTCH_HEIGHT);
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH - 2, y + NOTCH_HEIGHT);
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH - 2, y);
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH, y);
        
        // After notch to corner
        nvgLineTo(m_Vg, x + w - r, y);
        nvgArcTo(m_Vg, x + w, y, x + w, y + r, r);
        
        // === RIGHT EDGE ===
        nvgLineTo(m_Vg, x + w, y + h - r);
        nvgArcTo(m_Vg, x + w, y + h, x + w - r, y + h, r);
        
        // === BOTTOM EDGE (with male notch - protrusion) ===
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH, y + h);
        
        // Male notch (goes DOWN out of the block)
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH - 2, y + h);
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH - 2, y + h + NOTCH_HEIGHT);
        nvgLineTo(m_Vg, x + notchX + 2, y + h + NOTCH_HEIGHT);
        nvgLineTo(m_Vg, x + notchX + 2, y + h);
        nvgLineTo(m_Vg, x + notchX, y + h);
        
        // After notch to corner
        nvgLineTo(m_Vg, x + r, y + h);
        nvgArcTo(m_Vg, x, y + h, x, y + h - r, r);
        
        // === LEFT EDGE ===
        nvgLineTo(m_Vg, x, y + r);
        nvgArcTo(m_Vg, x, y, x + r, y, r);
        
        nvgClosePath(m_Vg);
    }
    
    void Scratch3BlockRenderer::BuildHatPath(float x, float y, float w, float h)
    {
        if (!m_Vg) return;
        
        float r = CORNER_RADIUS;
        float notchX = NOTCH_START_X;
        float hatH = HAT_HEIGHT;
        float hatW = w * HAT_WIDTH_RATIO;
        
        nvgBeginPath(m_Vg);
        
        // === TOP EDGE (Convex Arc - No notch, this is an event block) ===
        nvgMoveTo(m_Vg, x, y + hatH);
        
        // The famous "hat" curve - smooth camel hump
        nvgBezierTo(m_Vg,
            x, y,                           // Control point 1 (left side, curves up)
            x + hatW * 0.3f, y,             // Control point 2
            x + hatW * 0.5f, y              // Mid point (top of arc)
        );
        nvgBezierTo(m_Vg,
            x + hatW * 0.7f, y,             // Control point 1
            x + hatW, y,                    // Control point 2 (right side)
            x + w, y + hatH                 // End at right side
        );
        
        // === RIGHT EDGE ===
        nvgLineTo(m_Vg, x + w, y + h - r);
        nvgArcTo(m_Vg, x + w, y + h, x + w - r, y + h, r);
        
        // === BOTTOM EDGE (with male notch) ===
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH, y + h);
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH - 2, y + h);
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH - 2, y + h + NOTCH_HEIGHT);
        nvgLineTo(m_Vg, x + notchX + 2, y + h + NOTCH_HEIGHT);
        nvgLineTo(m_Vg, x + notchX + 2, y + h);
        nvgLineTo(m_Vg, x + notchX, y + h);
        
        nvgLineTo(m_Vg, x + r, y + h);
        nvgArcTo(m_Vg, x, y + h, x, y + h - r, r);
        
        // === LEFT EDGE ===
        nvgLineTo(m_Vg, x, y + hatH);
        
        nvgClosePath(m_Vg);
    }
    
    void Scratch3BlockRenderer::BuildCapPath(float x, float y, float w, float h)
    {
        if (!m_Vg) return;
        
        float r = CORNER_RADIUS;
        float notchX = NOTCH_START_X;
        
        nvgBeginPath(m_Vg);
        
        // === TOP EDGE (with female notch) ===
        nvgMoveTo(m_Vg, x + r, y);
        nvgLineTo(m_Vg, x + notchX, y);
        nvgLineTo(m_Vg, x + notchX + 2, y);
        nvgLineTo(m_Vg, x + notchX + 2, y + NOTCH_HEIGHT);
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH - 2, y + NOTCH_HEIGHT);
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH - 2, y);
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH, y);
        
        nvgLineTo(m_Vg, x + w - r, y);
        nvgArcTo(m_Vg, x + w, y, x + w, y + r, r);
        
        // === RIGHT EDGE ===
        nvgLineTo(m_Vg, x + w, y + h - r);
        nvgArcTo(m_Vg, x + w, y + h, x + w - r, y + h, r);
        
        // === BOTTOM EDGE (NO notch - this is a cap/end block) ===
        nvgLineTo(m_Vg, x + r, y + h);
        nvgArcTo(m_Vg, x, y + h, x, y + h - r, r);
        
        // === LEFT EDGE ===
        nvgLineTo(m_Vg, x, y + r);
        nvgArcTo(m_Vg, x, y, x + r, y, r);
        
        nvgClosePath(m_Vg);
    }
    
    void Scratch3BlockRenderer::BuildCShapePath(
        float x, float y, float w,
        float headerH, float mouthH, float footerH)
    {
        if (!m_Vg) return;
        
        float r = CORNER_RADIUS;
        float notchX = NOTCH_START_X;
        float armW = C_ARM_WIDTH;
        float innerX = x + armW;  // Left edge of inner area
        float totalH = headerH + mouthH + footerH;
        
        nvgBeginPath(m_Vg);
        
        // === TOP BAR ===
        // Top edge with female notch
        nvgMoveTo(m_Vg, x + r, y);
        nvgLineTo(m_Vg, x + notchX, y);
        nvgLineTo(m_Vg, x + notchX + 2, y);
        nvgLineTo(m_Vg, x + notchX + 2, y + NOTCH_HEIGHT);
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH - 2, y + NOTCH_HEIGHT);
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH - 2, y);
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH, y);
        nvgLineTo(m_Vg, x + w - r, y);
        nvgArcTo(m_Vg, x + w, y, x + w, y + r, r);
        
        // Right side of header
        nvgLineTo(m_Vg, x + w, y + headerH);
        
        // === STEP INTO MOUTH ===
        nvgLineTo(m_Vg, innerX + r, y + headerH);
        nvgArcTo(m_Vg, innerX, y + headerH, innerX, y + headerH + r, r);
        
        // === LEFT SIDE OF MOUTH ===
        nvgLineTo(m_Vg, innerX, y + headerH + mouthH - r);
        nvgArcTo(m_Vg, innerX, y + headerH + mouthH, innerX + r, y + headerH + mouthH, r);
        
        // === INNER BOTTOM NOTCH (for nesting blocks) ===
        float innerNotchX = innerX + notchX;
        nvgLineTo(m_Vg, innerNotchX, y + headerH + mouthH);
        nvgLineTo(m_Vg, innerNotchX + 2, y + headerH + mouthH);
        nvgLineTo(m_Vg, innerNotchX + 2, y + headerH + mouthH + NOTCH_HEIGHT);
        nvgLineTo(m_Vg, innerNotchX + NOTCH_WIDTH - 2, y + headerH + mouthH + NOTCH_HEIGHT);
        nvgLineTo(m_Vg, innerNotchX + NOTCH_WIDTH - 2, y + headerH + mouthH);
        nvgLineTo(m_Vg, innerNotchX + NOTCH_WIDTH, y + headerH + mouthH);
        
        // === STEP OUT OF MOUTH ===
        nvgLineTo(m_Vg, x + w, y + headerH + mouthH);
        
        // === FOOTER BAR ===
        // Right side of footer
        nvgLineTo(m_Vg, x + w, y + totalH - r);
        nvgArcTo(m_Vg, x + w, y + totalH, x + w - r, y + totalH, r);
        
        // Bottom edge with male notch
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH, y + totalH);
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH - 2, y + totalH);
        nvgLineTo(m_Vg, x + notchX + NOTCH_WIDTH - 2, y + totalH + NOTCH_HEIGHT);
        nvgLineTo(m_Vg, x + notchX + 2, y + totalH + NOTCH_HEIGHT);
        nvgLineTo(m_Vg, x + notchX + 2, y + totalH);
        nvgLineTo(m_Vg, x + notchX, y + totalH);
        
        nvgLineTo(m_Vg, x + r, y + totalH);
        nvgArcTo(m_Vg, x, y + totalH, x, y + totalH - r, r);
        
        // === LEFT EDGE (outer) ===
        nvgLineTo(m_Vg, x, y + r);
        nvgArcTo(m_Vg, x, y, x + r, y, r);
        
        nvgClosePath(m_Vg);
    }
    
    //=========================================================================
    // Fill and Stroke
    //=========================================================================
    
    void Scratch3BlockRenderer::FillAndStroke(unsigned int fillColor)
    {
        if (!m_Vg) return;
        
        // Fill with solid color (gradient applied in DrawBlock for positioned gradient)
        nvgFillColor(m_Vg, ColorFromU32(fillColor));
        nvgFill(m_Vg);
        
        // Stroke (25% darker for more contrast)
        unsigned int strokeColor = GetStrokeColor(fillColor);
        nvgStrokeColor(m_Vg, ColorFromU32(strokeColor));
        nvgStrokeWidth(m_Vg, STROKE_WIDTH);
        nvgStroke(m_Vg);
    }
    
    void Scratch3BlockRenderer::FillWithGradient(float x, float y, float h, unsigned int fillColor)
    {
        if (!m_Vg) return;
        
        // Create vertical gradient: lighter top -> darker bottom
        NVGcolor topColor = ColorFromU32(fillColor);
        NVGcolor bottomColor = ColorFromU32(GetDarkenedColor(fillColor, BOTTOM_DARKEN));
        
        NVGpaint gradient = nvgLinearGradient(m_Vg, x, y, x, y + h, topColor, bottomColor);
        nvgFillPaint(m_Vg, gradient);
        nvgFill(m_Vg);
        
        // Stroke (25% darker for more contrast)
        unsigned int strokeColor = GetStrokeColor(fillColor);
        nvgStrokeColor(m_Vg, ColorFromU32(strokeColor));
        nvgStrokeWidth(m_Vg, STROKE_WIDTH);
        nvgStroke(m_Vg);
    }
    
    void Scratch3BlockRenderer::DrawInnerHighlight(float x, float y, float w)
    {
        if (!m_Vg) return;
        
        // Subtle top-edge highlight for 3D effect
        nvgBeginPath(m_Vg);
        nvgMoveTo(m_Vg, x + CORNER_RADIUS + NOTCH_WIDTH + 5, y + 2);
        nvgLineTo(m_Vg, x + w - CORNER_RADIUS, y + 2);
        nvgStrokeColor(m_Vg, nvgRGBA(255, 255, 255, static_cast<int>(255 * INNER_GLOW_ALPHA)));
        nvgStrokeWidth(m_Vg, 1.0f);
        nvgStroke(m_Vg);
    }
    
    void Scratch3BlockRenderer::DrawBlockShadow(float x, float y, float w, float h)
    {
        if (!m_Vg) return;
        
        // Enhanced shadow with box gradient for blur effect
        NVGpaint shadowPaint = nvgBoxGradient(m_Vg,
            x, y + SHADOW_OFFSET_Y,   // Offset down
            w, h,
            CORNER_RADIUS + 2.0f,     // Corner radius
            SHADOW_BLUR,              // Blur/feather
            nvgRGBA(0, 0, 0, 50),     // Inner color (shadow)
            nvgRGBA(0, 0, 0, 0)       // Outer color (fade out)
        );
        
        nvgBeginPath(m_Vg);
        nvgRect(m_Vg, x - 6, y - 2, w + 12, h + 12);
        nvgFillPaint(m_Vg, shadowPaint);
        nvgFill(m_Vg);
    }
    
    //=========================================================================
    // Label Drawing
    //=========================================================================
    
    void Scratch3BlockRenderer::DrawLabel(
        float x, float y, 
        float w, float h,
        const char* label,
        const char* icon)
    {
        if (!m_Vg || !label) return;
        
        float textX = x + PADDING;
        float textY = y + h / 2.0f;
        
        // Icon (if provided)
        if (icon && icon[0] != '\0')
        {
            nvgFontSize(m_Vg, ICON_SIZE);
            nvgFillColor(m_Vg, nvgRGBA(255, 255, 255, 220));
            nvgTextAlign(m_Vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
            nvgText(m_Vg, textX, textY, icon, nullptr);
            textX += ICON_SIZE + 4.0f;
        }
        
        // Text shadow (subtle)
        nvgFontSize(m_Vg, TEXT_SIZE);
        nvgFillColor(m_Vg, nvgRGBA(0, 0, 0, 50));
        nvgTextAlign(m_Vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgText(m_Vg, textX + 0.5f, textY + 0.5f, label, nullptr);
        
        // Main text (white, bold)
        nvgFillColor(m_Vg, nvgRGBA(255, 255, 255, 255));
        nvgText(m_Vg, textX, textY, label, nullptr);
    }
    
    //=========================================================================
    // Main Block Drawing
    //=========================================================================
    
    void Scratch3BlockRenderer::DrawBlock(
        float x, float y, 
        float w, float h,
        Scratch3Shape shape,
        unsigned int color,
        const char* label,
        const char* icon)
    {
        if (!m_Vg) return;
        
        nvgSave(m_Vg);
        
        // Enhanced shadow with blur
        DrawBlockShadow(x, y, w, h);
        
        // Build path based on shape
        switch (shape)
        {
            case Scratch3Shape::HAT:
                BuildHatPath(x, y, w, h);
                break;
            case Scratch3Shape::STACK:
                BuildStackPath(x, y, w, h);
                break;
            case Scratch3Shape::CAP:
                BuildCapPath(x, y, w, h);
                break;
            default:
                BuildStackPath(x, y, w, h);
                break;
        }
        
        // Fill with gradient for premium 3D look
        FillWithGradient(x, y, h, color);
        
        // Inner highlight for extra polish (skip for hat blocks - top is curved)
        if (shape != Scratch3Shape::HAT)
        {
            DrawInnerHighlight(x, y, w);
        }
        
        // Draw label
        float labelY = (shape == Scratch3Shape::HAT) ? y + HAT_HEIGHT : y;
        float labelH = (shape == Scratch3Shape::HAT) ? h - HAT_HEIGHT : h;
        DrawLabel(x, labelY, w, labelH, label, icon);
        
        nvgRestore(m_Vg);
    }
    
    void Scratch3BlockRenderer::DrawCShapeBlock(
        float x, float y,
        float w,
        float headerHeight,
        float mouthHeight,
        float footerHeight,
        unsigned int color,
        const char* label,
        const char* icon)
    {
        if (!m_Vg) return;
        
        nvgSave(m_Vg);
        
        float totalH = headerHeight + mouthHeight + footerHeight;
        
        // Enhanced shadow
        DrawBlockShadow(x, y, w, totalH);
        
        // Build C-shape path
        BuildCShapePath(x, y, w, headerHeight, mouthHeight, footerHeight);
        
        // Fill with gradient for premium 3D look
        FillWithGradient(x, y, totalH, color);
        
        // Inner highlight
        DrawInnerHighlight(x, y, w);
        
        // Draw inner mouth background with subtle gradient
        float innerX = x + C_ARM_WIDTH;
        float innerY = y + headerHeight;
        float innerW = w - C_ARM_WIDTH - CORNER_RADIUS;
        
        NVGpaint mouthPaint = nvgBoxGradient(m_Vg,
            innerX, innerY, innerW, mouthHeight,
            CORNER_RADIUS, 2.0f,
            nvgRGBA(0, 0, 0, 45),   // Inner (darker)
            nvgRGBA(0, 0, 0, 20)    // Outer (lighter)
        );
        
        nvgBeginPath(m_Vg);
        nvgRoundedRect(m_Vg, innerX, innerY, innerW, mouthHeight, CORNER_RADIUS);
        nvgFillPaint(m_Vg, mouthPaint);
        nvgFill(m_Vg);
        
        // Label in header
        DrawLabel(x, y, w, headerHeight, label, icon);
        
        nvgRestore(m_Vg);
    }
    
    void Scratch3BlockRenderer::DrawReporterBlock(
        float x, float y,
        float w, float h,
        unsigned int color,
        const char* label)
    {
        if (!m_Vg) return;
        
        nvgSave(m_Vg);
        
        // Reporters are pill-shaped (fully rounded ends)
        float r = h / 2.0f;
        
        // Shadow
        DrawBlockShadow(x, y, w, h);
        
        nvgBeginPath(m_Vg);
        nvgRoundedRect(m_Vg, x, y, w, h, r);
        
        // Gradient fill
        FillWithGradient(x, y, h, color);
        
        // Centered text with shadow
        nvgFontSize(m_Vg, TEXT_SIZE);
        nvgFillColor(m_Vg, nvgRGBA(0, 0, 0, 50));
        nvgTextAlign(m_Vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgText(m_Vg, x + w / 2.0f + 0.5f, y + h / 2.0f + 0.5f, label, nullptr);
        
        nvgFillColor(m_Vg, nvgRGBA(255, 255, 255, 255));
        nvgText(m_Vg, x + w / 2.0f, y + h / 2.0f, label, nullptr);
        
        nvgRestore(m_Vg);
    }
    
    void Scratch3BlockRenderer::DrawBooleanBlock(
        float x, float y,
        float w, float h,
        unsigned int color,
        const char* label)
    {
        if (!m_Vg) return;
        
        nvgSave(m_Vg);
        
        // Shadow
        DrawBlockShadow(x, y, w, h);
        
        // Boolean blocks are hexagonal (pointed ends)
        float pointW = h / 2.0f;
        
        nvgBeginPath(m_Vg);
        nvgMoveTo(m_Vg, x + pointW, y);
        nvgLineTo(m_Vg, x + w - pointW, y);
        nvgLineTo(m_Vg, x + w, y + h / 2.0f);
        nvgLineTo(m_Vg, x + w - pointW, y + h);
        nvgLineTo(m_Vg, x + pointW, y + h);
        nvgLineTo(m_Vg, x, y + h / 2.0f);
        nvgClosePath(m_Vg);
        
        // Gradient fill
        FillWithGradient(x, y, h, color);
        
        // Centered text with shadow
        nvgFontSize(m_Vg, TEXT_SIZE);
        nvgFillColor(m_Vg, nvgRGBA(0, 0, 0, 50));
        nvgTextAlign(m_Vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgText(m_Vg, x + w / 2.0f + 0.5f, y + h / 2.0f + 0.5f, label, nullptr);
        
        nvgFillColor(m_Vg, nvgRGBA(255, 255, 255, 255));
        nvgText(m_Vg, x + w / 2.0f, y + h / 2.0f, label, nullptr);
        
        nvgRestore(m_Vg);
    }
    
    //=========================================================================
    // Input Fields
    //=========================================================================
    
    void Scratch3BlockRenderer::DrawInputField(
        float x, float y,
        float w, float h,
        const char* value,
        bool isNumber)
    {
        if (!m_Vg) return;
        
        float r = isNumber ? h / 2.0f : CORNER_RADIUS;  // Pill for numbers
        
        // White background
        nvgBeginPath(m_Vg);
        nvgRoundedRect(m_Vg, x, y, w, h, r);
        nvgFillColor(m_Vg, nvgRGBA(255, 255, 255, 255));
        nvgFill(m_Vg);
        
        // Text
        if (value)
        {
            nvgFontSize(m_Vg, TEXT_SIZE);
            nvgFillColor(m_Vg, nvgRGBA(87, 94, 117, 255));  // Scratch gray
            nvgTextAlign(m_Vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
            nvgText(m_Vg, x + w / 2.0f, y + h / 2.0f, value, nullptr);
        }
    }
    
    void Scratch3BlockRenderer::DrawDropdown(
        float x, float y,
        float w, float h,
        const char* selectedValue)
    {
        if (!m_Vg) return;
        
        // Rounded background
        nvgBeginPath(m_Vg);
        nvgRoundedRect(m_Vg, x, y, w, h, CORNER_RADIUS);
        nvgFillColor(m_Vg, nvgRGBA(255, 255, 255, 200));
        nvgFill(m_Vg);
        
        // Text
        if (selectedValue)
        {
            nvgFontSize(m_Vg, TEXT_SIZE);
            nvgFillColor(m_Vg, nvgRGBA(87, 94, 117, 255));
            nvgTextAlign(m_Vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
            nvgText(m_Vg, x + 6.0f, y + h / 2.0f, selectedValue, nullptr);
        }
        
        // Dropdown arrow
        nvgFontSize(m_Vg, 10.0f);
        nvgFillColor(m_Vg, nvgRGBA(87, 94, 117, 255));
        nvgTextAlign(m_Vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
        nvgText(m_Vg, x + w - 4.0f, y + h / 2.0f, "▼", nullptr);
    }
    
    void Scratch3BlockRenderer::DrawColorSwatch(
        float x, float y,
        float size,
        unsigned int color)
    {
        if (!m_Vg) return;
        
        nvgBeginPath(m_Vg);
        nvgRoundedRect(m_Vg, x, y, size, size, CORNER_RADIUS);
        nvgFillColor(m_Vg, ColorFromU32(color));
        nvgFill(m_Vg);
        
        // Border
        nvgStrokeColor(m_Vg, nvgRGBA(0, 0, 0, 50));
        nvgStrokeWidth(m_Vg, 1.0f);
        nvgStroke(m_Vg);
    }
    
    //=========================================================================
    // Visual Effects
    //=========================================================================
    
    void Scratch3BlockRenderer::DrawSelectionHighlight(
        float x, float y,
        float w, float h,
        Scratch3Shape shape)
    {
        if (!m_Vg) return;
        
        float pad = 3.0f;
        
        nvgBeginPath(m_Vg);
        nvgRoundedRect(m_Vg, x - pad, y - pad, w + pad * 2, h + pad * 2, CORNER_RADIUS + pad);
        nvgStrokeColor(m_Vg, nvgRGBA(255, 200, 50, 255));  // Yellow
        nvgStrokeWidth(m_Vg, 3.0f);
        nvgStroke(m_Vg);
    }
    
    void Scratch3BlockRenderer::DrawExecutionIndicator(
        float x, float y,
        float w, float h,
        float pulsePhase)
    {
        if (!m_Vg) return;
        
        float alpha = 0.5f + 0.5f * std::sin(pulsePhase);
        float pad = 2.0f + alpha * 2.0f;
        
        nvgBeginPath(m_Vg);
        nvgRoundedRect(m_Vg, x - pad, y - pad, w + pad * 2, h + pad * 2, CORNER_RADIUS + pad);
        nvgStrokeColor(m_Vg, nvgRGBA(50, 255, 50, static_cast<int>(200 * alpha)));
        nvgStrokeWidth(m_Vg, 2.0f);
        nvgStroke(m_Vg);
    }
    
    void Scratch3BlockRenderer::DrawErrorIndicator(
        float x, float y,
        float w, float h)
    {
        if (!m_Vg) return;
        
        nvgBeginPath(m_Vg);
        nvgRoundedRect(m_Vg, x, y, w, h, CORNER_RADIUS);
        nvgFillColor(m_Vg, nvgRGBA(255, 0, 0, 40));
        nvgFill(m_Vg);
        
        nvgStrokeColor(m_Vg, nvgRGBA(255, 80, 80, 200));
        nvgStrokeWidth(m_Vg, 2.0f);
        nvgStroke(m_Vg);
    }
    
    void Scratch3BlockRenderer::DrawDropTarget(
        float x, float y,
        float w, float h,
        bool isActive)
    {
        if (!m_Vg) return;
        
        unsigned int color = isActive ? 0xFF40FF40 : 0x50808080;
        
        nvgBeginPath(m_Vg);
        nvgRoundedRect(m_Vg, x, y, w, h, CORNER_RADIUS);
        nvgFillColor(m_Vg, ColorFromU32(color));
        nvgFill(m_Vg);
    }
    
    //=========================================================================
    // Utility
    //=========================================================================
    
    float Scratch3BlockRenderer::CalculateBlockHeight(
        int inputCount, 
        bool hasIcon,
        Scratch3Shape shape)
    {
        float baseHeight = MIN_BLOCK_HEIGHT;
        
        if (shape == Scratch3Shape::HAT)
        {
            baseHeight += HAT_HEIGHT;
        }
        
        if (inputCount > 1)
        {
            baseHeight += (inputCount - 1) * 24.0f;  // Each extra row
        }
        
        return baseHeight;
    }
}
