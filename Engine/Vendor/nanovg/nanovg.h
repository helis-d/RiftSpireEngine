#ifndef NANOVG_H
#define NANOVG_H

#ifdef __cplusplus
extern "C" {
#endif

// NanoVG is a small antialiased vector graphics rendering library for OpenGL.
// This is a minimal stub header for RiftSpire integration.
// Full source: https://github.com/memononen/nanovg

#define NVG_PI 3.14159265358979323846264338327f

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4201)  // nonstandard extension used : nameless struct/union
#endif

typedef struct NVGcontext NVGcontext;

struct NVGcolor {
    union {
        float rgba[4];
        struct {
            float r,g,b,a;
        };
    };
};
typedef struct NVGcolor NVGcolor;

struct NVGpaint {
    float xform[6];
    float extent[2];
    float radius;
    float feather;
    NVGcolor innerColor;
    NVGcolor outerColor;
    int image;
};
typedef struct NVGpaint NVGpaint;

enum NVGwinding {
    NVG_CCW = 1,            // Winding for solid shapes
    NVG_CW = 2,             // Winding for holes
};

enum NVGsolidity {
    NVG_SOLID = 1,          // CCW
    NVG_HOLE = 2,           // CW
};

enum NVGlineCap {
    NVG_BUTT,
    NVG_ROUND,
    NVG_SQUARE,
    NVG_BEVEL,
    NVG_MITER,
};

enum NVGalign {
    // Horizontal align
    NVG_ALIGN_LEFT      = 1<<0, // Default, align text horizontally to left.
    NVG_ALIGN_CENTER    = 1<<1, // Align text horizontally to center.
    NVG_ALIGN_RIGHT     = 1<<2, // Align text horizontally to right.
    // Vertical align
    NVG_ALIGN_TOP       = 1<<3, // Align text vertically to top.
    NVG_ALIGN_MIDDLE    = 1<<4, // Align text vertically to middle.
    NVG_ALIGN_BOTTOM    = 1<<5, // Align text vertically to bottom.
    NVG_ALIGN_BASELINE  = 1<<6, // Default, align text vertically to baseline.
};

enum NVGblendFactor {
    NVG_ZERO = 1<<0,
    NVG_ONE = 1<<1,
    NVG_SRC_COLOR = 1<<2,
    NVG_ONE_MINUS_SRC_COLOR = 1<<3,
    NVG_DST_COLOR = 1<<4,
    NVG_ONE_MINUS_DST_COLOR = 1<<5,
    NVG_SRC_ALPHA = 1<<6,
    NVG_ONE_MINUS_SRC_ALPHA = 1<<7,
    NVG_DST_ALPHA = 1<<8,
    NVG_ONE_MINUS_DST_ALPHA = 1<<9,
    NVG_SRC_ALPHA_SATURATE = 1<<10,
};

enum NVGcompositeOperation {
    NVG_SOURCE_OVER,
    NVG_SOURCE_IN,
    NVG_SOURCE_OUT,
    NVG_ATOP,
    NVG_DESTINATION_OVER,
    NVG_DESTINATION_IN,
    NVG_DESTINATION_OUT,
    NVG_DESTINATION_ATOP,
    NVG_LIGHTER,
    NVG_COPY,
    NVG_XOR,
};

struct NVGcompositeOperationState {
    int srcRGB;
    int dstRGB;
    int srcAlpha;
    int dstAlpha;
};
typedef struct NVGcompositeOperationState NVGcompositeOperationState;

struct NVGglyphPosition {
    const char* str;    // Position of the glyph in the input string.
    float x;            // The x-coordinate of the logical glyph position.
    float minx, maxx;   // The bounds of the glyph shape.
};
typedef struct NVGglyphPosition NVGglyphPosition;

struct NVGtextRow {
    const char* start;  // Pointer to start of the row
    const char* end;    // Pointer to end of the row
    const char* next;   // Pointer to next row
    float width;        // Logical width of the row
    float minx, maxx;   // Actual bounds of row
};
typedef struct NVGtextRow NVGtextRow;

enum NVGimageFlags {
    NVG_IMAGE_GENERATE_MIPMAPS  = 1<<0,     // Generate mipmaps during creation of the image.
    NVG_IMAGE_REPEATX           = 1<<1,     // Repeat image in X direction.
    NVG_IMAGE_REPEATY           = 1<<2,     // Repeat image in Y direction.
    NVG_IMAGE_FLIPY             = 1<<3,     // Flips (inverses) image in Y direction when rendered.
    NVG_IMAGE_PREMULTIPLIED     = 1<<4,     // Image data has premultiplied alpha.
    NVG_IMAGE_NEAREST           = 1<<5,     // Image interpolation is Nearest instead Linear
};

// Begin drawing a new frame
void nvgBeginFrame(NVGcontext* ctx, float windowWidth, float windowHeight, float devicePixelRatio);

// Cancels drawing the current frame.
void nvgCancelFrame(NVGcontext* ctx);

// Ends drawing flushing remaining render state.
void nvgEndFrame(NVGcontext* ctx);

//
// Color utils
//
NVGcolor nvgRGB(unsigned char r, unsigned char g, unsigned char b);
NVGcolor nvgRGBf(float r, float g, float b);
NVGcolor nvgRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
NVGcolor nvgRGBAf(float r, float g, float b, float a);
NVGcolor nvgLerpRGBA(NVGcolor c0, NVGcolor c1, float u);
NVGcolor nvgTransRGBA(NVGcolor c0, unsigned char a);
NVGcolor nvgTransRGBAf(NVGcolor c0, float a);
NVGcolor nvgHSL(float h, float s, float l);
NVGcolor nvgHSLA(float h, float s, float l, unsigned char a);

//
// State Handling
//
void nvgSave(NVGcontext* ctx);
void nvgRestore(NVGcontext* ctx);
void nvgReset(NVGcontext* ctx);

//
// Render styles
//
void nvgShapeAntiAlias(NVGcontext* ctx, int enabled);
void nvgStrokeColor(NVGcontext* ctx, NVGcolor color);
void nvgStrokePaint(NVGcontext* ctx, NVGpaint paint);
void nvgFillColor(NVGcontext* ctx, NVGcolor color);
void nvgFillPaint(NVGcontext* ctx, NVGpaint paint);
void nvgMiterLimit(NVGcontext* ctx, float limit);
void nvgStrokeWidth(NVGcontext* ctx, float size);
void nvgLineCap(NVGcontext* ctx, int cap);
void nvgLineJoin(NVGcontext* ctx, int join);
void nvgGlobalAlpha(NVGcontext* ctx, float alpha);

//
// Transforms
//
void nvgResetTransform(NVGcontext* ctx);
void nvgTransform(NVGcontext* ctx, float a, float b, float c, float d, float e, float f);
void nvgTranslate(NVGcontext* ctx, float x, float y);
void nvgRotate(NVGcontext* ctx, float angle);
void nvgSkewX(NVGcontext* ctx, float angle);
void nvgSkewY(NVGcontext* ctx, float angle);
void nvgScale(NVGcontext* ctx, float x, float y);

//
// Paints
//
NVGpaint nvgLinearGradient(NVGcontext* ctx, float sx, float sy, float ex, float ey, NVGcolor icol, NVGcolor ocol);
NVGpaint nvgBoxGradient(NVGcontext* ctx, float x, float y, float w, float h, float r, float f, NVGcolor icol, NVGcolor ocol);
NVGpaint nvgRadialGradient(NVGcontext* ctx, float cx, float cy, float inr, float outr, NVGcolor icol, NVGcolor ocol);

//
// Scissoring
//
void nvgScissor(NVGcontext* ctx, float x, float y, float w, float h);
void nvgIntersectScissor(NVGcontext* ctx, float x, float y, float w, float h);
void nvgResetScissor(NVGcontext* ctx);

//
// Paths
//
void nvgBeginPath(NVGcontext* ctx);
void nvgMoveTo(NVGcontext* ctx, float x, float y);
void nvgLineTo(NVGcontext* ctx, float x, float y);
void nvgBezierTo(NVGcontext* ctx, float c1x, float c1y, float c2x, float c2y, float x, float y);
void nvgQuadTo(NVGcontext* ctx, float cx, float cy, float x, float y);
void nvgArcTo(NVGcontext* ctx, float x1, float y1, float x2, float y2, float radius);
void nvgClosePath(NVGcontext* ctx);
void nvgPathWinding(NVGcontext* ctx, int dir);
void nvgArc(NVGcontext* ctx, float cx, float cy, float r, float a0, float a1, int dir);
void nvgRect(NVGcontext* ctx, float x, float y, float w, float h);
void nvgRoundedRect(NVGcontext* ctx, float x, float y, float w, float h, float r);
void nvgRoundedRectVarying(NVGcontext* ctx, float x, float y, float w, float h, float radTopLeft, float radTopRight, float radBottomRight, float radBottomLeft);
void nvgEllipse(NVGcontext* ctx, float cx, float cy, float rx, float ry);
void nvgCircle(NVGcontext* ctx, float cx, float cy, float r);
void nvgFill(NVGcontext* ctx);
void nvgStroke(NVGcontext* ctx);

//
// Text
//
int nvgCreateFont(NVGcontext* ctx, const char* name, const char* path);
int nvgCreateFontAtIndex(NVGcontext* ctx, const char* name, const char* path, const int fontIndex);
int nvgCreateFontMem(NVGcontext* ctx, const char* name, unsigned char* data, int ndata, int freeData);
int nvgCreateFontMemAtIndex(NVGcontext* ctx, const char* name, unsigned char* data, int ndata, int freeData, const int fontIndex);
int nvgFindFont(NVGcontext* ctx, const char* name);
int nvgAddFallbackFontId(NVGcontext* ctx, int baseFont, int fallbackFont);
int nvgAddFallbackFont(NVGcontext* ctx, const char* baseFont, const char* fallbackFont);
void nvgResetFallbackFontsId(NVGcontext* ctx, int baseFont);
void nvgResetFallbackFonts(NVGcontext* ctx, const char* baseFont);
void nvgFontSize(NVGcontext* ctx, float size);
void nvgFontBlur(NVGcontext* ctx, float blur);
void nvgTextLetterSpacing(NVGcontext* ctx, float spacing);
void nvgTextLineHeight(NVGcontext* ctx, float lineHeight);
void nvgTextAlign(NVGcontext* ctx, int align);
void nvgFontFaceId(NVGcontext* ctx, int font);
void nvgFontFace(NVGcontext* ctx, const char* font);
float nvgText(NVGcontext* ctx, float x, float y, const char* string, const char* end);
void nvgTextBox(NVGcontext* ctx, float x, float y, float breakRowWidth, const char* string, const char* end);
float nvgTextBounds(NVGcontext* ctx, float x, float y, const char* string, const char* end, float* bounds);
void nvgTextBoxBounds(NVGcontext* ctx, float x, float y, float breakRowWidth, const char* string, const char* end, float* bounds);
int nvgTextGlyphPositions(NVGcontext* ctx, float x, float y, const char* string, const char* end, NVGglyphPosition* positions, int maxPositions);
void nvgTextMetrics(NVGcontext* ctx, float* ascender, float* descender, float* lineh);
int nvgTextBreakLines(NVGcontext* ctx, const char* string, const char* end, float breakRowWidth, NVGtextRow* rows, int maxRows);

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#ifdef __cplusplus
}
#endif

#endif // NANOVG_H
