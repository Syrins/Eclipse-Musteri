// dear MsTrG, v1.88
// (headers)

// Help:
// - Read FAQ at http://dearMsTrG.org/faq
// - Newcomers, read 'Programmer guide' in MsTrG.cpp for notes on how to setup Dear MsTrG in your codebase.
// - Call and read MsTrG::ShowDemoWindow() in MsTrG_demo.cpp. All applications in examples/ are doing that.
// Read MsTrG.cpp for details, links and comments.

// Resources:
// - FAQ                   http://dearMsTrG.org/faq
// - Homepage & latest     https://github.com/ocornut/MsTrG
// - Releases & changelog  https://github.com/ocornut/MsTrG/releases
// - Gallery               https://github.com/ocornut/MsTrG/issues/5243 (please post your screenshots/video there!)
// - Wiki                  https://github.com/ocornut/MsTrG/wiki (lots of good stuff there)
// - Glossary              https://github.com/ocornut/MsTrG/wiki/Glossary
// - Issues & support      https://github.com/ocornut/MsTrG/issues

// Getting Started?
// - For first-time users having issues compiling/linking/running or issues loading fonts:
//   please post in https://github.com/ocornut/MsTrG/discussions if you cannot find a solution in resources above.

/*

Index of this file:
// [SECTION] Header mess
// [SECTION] Forward declarations and basic types
// [SECTION] Dear MsTrG end-user API functions
// [SECTION] Flags & Enumerations
// [SECTION] Helpers: Memory allocations macros, ImVector<>
// [SECTION] MsTrGStyle
// [SECTION] MsTrGIO
// [SECTION] Misc data structures (MsTrGInputTextCallbackData, MsTrGSizeCallbackData, MsTrGPayload, MsTrGTableSortSpecs, MsTrGTableColumnSortSpecs)
// [SECTION] Helpers (MsTrGOnceUponAFrame, MsTrGTextFilter, MsTrGTextBuffer, MsTrGStorage, MsTrGListClipper, ImColor)
// [SECTION] Drawing API (ImDrawCallback, ImDrawCmd, ImDrawIdx, ImDrawVert, ImDrawChannel, ImDrawListSplitter, ImDrawFlags, ImDrawListFlags, ImDrawList, ImDrawData)
// [SECTION] Font API (ImFontConfig, ImFontGlyph, ImFontGlyphRangesBuilder, ImFontAtlasFlags, ImFontAtlas, ImFont)
// [SECTION] Viewports (MsTrGViewportFlags, MsTrGViewport)
// [SECTION] Platform Dependent Interfaces (MsTrGPlatformImeData)
// [SECTION] Obsolete functions and types

*/

#pragma once

// Configuration file with compile-time options (edit imconfig.h or '#define MsTrG_USER_CONFIG "myfilename.h" from your build system')
#ifdef MsTrG_USER_CONFIG
#include MsTrG_USER_CONFIG
#endif
#if !defined(MsTrG_DISABLE_INCLUDE_IMCONFIG_H) || defined(MsTrG_INCLUDE_IMCONFIG_H)
#include "imconfig.h"
#endif

#ifndef MsTrG_DISABLE

//-----------------------------------------------------------------------------
// [SECTION] Header mess
//-----------------------------------------------------------------------------

// Includes
#include <float.h>                  // FLT_MIN, FLT_MAX
#include <stdarg.h>                 // va_list, va_start, va_end
#include <stddef.h>                 // ptrdiff_t, NULL
#include <string.h>                 // memset, memmove, memcpy, strlen, strchr, strcpy, strcmp


// Version
// (Integer encoded as XYYZZ for use in #if preprocessor conditionals. Work in progress versions typically starts at XYY99 then bounce up to XYY00, XYY01 etc. when release tagging happens)
#define MsTrG_VERSION               "1.88"
#define MsTrG_VERSION_NUM           18800
#define MsTrG_CHECKVERSION()        MsTrG::DebugCheckVersionAndDataLayout(MsTrG_VERSION, sizeof(MsTrGIO), sizeof(MsTrGStyle), sizeof(ImVec2), sizeof(ImVec4), sizeof(ImDrawVert), sizeof(ImDrawIdx))
#define MsTrG_HAS_TABLE

// Define attributes of all API symbols declarations (e.g. for DLL under Windows)
// MsTrG_API is used for core MsTrG functions, MsTrG_IMPL_API is used for the default backends files (MsTrG_impl_xxx.h)
// Using dear MsTrG via a shared library is not recommended, because we don't guarantee backward nor forward ABI compatibility (also function call overhead, as dear MsTrG is a call-heavy API)
#ifndef MsTrG_API
#define MsTrG_API
#endif
#ifndef MsTrG_IMPL_API
#define MsTrG_IMPL_API              MsTrG_API
#endif

// Helper Macros
#ifndef IM_ASSERT
#include <assert.h>
#define IM_ASSERT(_EXPR)            assert(_EXPR)                               // You can override the default assert handler by editing imconfig.h
#endif
#define IM_ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR) / sizeof(*(_ARR))))     // Size of a static C-style array. Don't use on pointers!
#define IM_UNUSED(_VAR)             ((void)(_VAR))                              // Used to silence "unused variable warnings". Often useful as asserts may be stripped out from final builds.
#define IM_OFFSETOF(_TYPE,_MEMBER)  offsetof(_TYPE, _MEMBER)                    // Offset of _MEMBER within _TYPE. Standardized as offsetof() in C++11

// Helper Macros - IM_FMTARGS, IM_FMTLIST: Apply printf-style warnings to our formatting functions.
#if !defined(MsTrG_USE_STB_SPRINTF) && defined(__MINGW32__) && !defined(__clang__)
#define IM_FMTARGS(FMT)             __attribute__((format(gnu_printf, FMT, FMT+1)))
#define IM_FMTLIST(FMT)             __attribute__((format(gnu_printf, FMT, 0)))
#elif !defined(MsTrG_USE_STB_SPRINTF) && (defined(__clang__) || defined(__GNUC__))
#define IM_FMTARGS(FMT)             __attribute__((format(printf, FMT, FMT+1)))
#define IM_FMTLIST(FMT)             __attribute__((format(printf, FMT, 0)))
#else
#define IM_FMTARGS(FMT)
#define IM_FMTLIST(FMT)
#endif

// Disable some of MSVC most aggressive Debug runtime checks in function header/footer (used in some simple/low-level functions)
#if defined(_MSC_VER) && !defined(__clang__)  && !defined(__INTEL_COMPILER) && !defined(MsTrG_DEBUG_PARANOID)
#define IM_MSVC_RUNTIME_CHECKS_OFF      __pragma(runtime_checks("",off))     __pragma(check_stack(off)) __pragma(strict_gs_check(push,off))
#define IM_MSVC_RUNTIME_CHECKS_RESTORE  __pragma(runtime_checks("",restore)) __pragma(check_stack())    __pragma(strict_gs_check(pop))
#else
#define IM_MSVC_RUNTIME_CHECKS_OFF
#define IM_MSVC_RUNTIME_CHECKS_RESTORE
#endif

// Warnings
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 26495)    // [Static Analyzer] Variable 'XXX' is uninitialized. Always initialize a member variable (type.6).
#endif
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"          // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wclass-memaccess"  // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

//-----------------------------------------------------------------------------
// [SECTION] Forward declarations and basic types
//-----------------------------------------------------------------------------

// Forward declarations
struct ImDrawChannel;               // Temporary storage to output draw commands out of order, used by ImDrawListSplitter and ImDrawList::ChannelsSplit()
struct ImDrawCmd;                   // A single draw command within a parent ImDrawList (generally maps to 1 GPU draw call, unless it is a callback)
struct ImDrawData;                  // All draw command lists required to render the frame + pos/size coordinates to use for the projection matrix.
struct ImDrawList;                  // A single draw command list (generally one per window, conceptually you may see this as a dynamic "mesh" builder)
struct ImDrawListSharedData;        // Data shared among multiple draw lists (typically owned by parent MsTrG context, but you may create one yourself)
struct ImDrawListSplitter;          // Helper to split a draw list into different layers which can be drawn into out of order, then flattened back.
struct ImDrawVert;                  // A single vertex (pos + uv + col = 20 bytes by default. Override layout with MsTrG_OVERRIDE_DRAWVERT_STRUCT_LAYOUT)
struct ImFont;                      // Runtime data for a single font within a parent ImFontAtlas
struct ImFontAtlas;                 // Runtime data for multiple fonts, bake multiple fonts into a single texture, TTF/OTF font loader
struct ImFontBuilderIO;             // Opaque interface to a font builder (stb_truetype or FreeType).
struct ImFontConfig;                // Configuration data when adding a font or merging fonts
struct ImFontGlyph;                 // A single font glyph (code point + coordinates within in ImFontAtlas + offset)
struct ImFontGlyphRangesBuilder;    // Helper to build glyph ranges from text/string data
struct ImColor;                     // Helper functions to create a color that can be converted to either u32 or float4 (*OBSOLETE* please avoid using)
struct MsTrGContext;                // Dear MsTrG context (opaque structure, unless including MsTrG_internal.h)
struct MsTrGIO;                     // Main configuration and I/O between your application and MsTrG
struct MsTrGInputTextCallbackData;  // Shared state of InputText() when using custom MsTrGInputTextCallback (rare/advanced use)
struct MsTrGKeyData;                // Storage for MsTrGIO and IsKeyDown(), IsKeyPressed() etc functions.
struct MsTrGListClipper;            // Helper to manually clip large list of items
struct MsTrGOnceUponAFrame;         // Helper for running a block of code not more than once a frame
struct MsTrGPayload;                // User data payload for drag and drop operations
struct MsTrGPlatformImeData;        // Platform IME data for io.SetPlatformImeDataFn() function.
struct MsTrGSizeCallbackData;       // Callback data when using SetNextWindowSizeConstraints() (rare/advanced use)
struct MsTrGStorage;                // Helper for key->value storage
struct MsTrGStyle;                  // Runtime data for styling/colors
struct MsTrGTableSortSpecs;         // Sorting specifications for a table (often handling sort specs for a single column, occasionally more)
struct MsTrGTableColumnSortSpecs;   // Sorting specification for one column of a table
struct MsTrGTextBuffer;             // Helper to hold and append into a text buffer (~string builder)
struct MsTrGTextFilter;             // Helper to parse and apply text filters (e.g. "aaaaa[,bbbbb][,ccccc]")
struct MsTrGViewport;               // A Platform Window (always only one in 'master' branch), in the future may represent Platform Monitor

// Enums/Flags (declared as int for compatibility with old C++, to allow using as flags without overhead, and to not pollute the top of this file)
// - Tip: Use your programming IDE navigation facilities on the names in the _central column_ below to find the actual flags/enum lists!
//   In Visual Studio IDE: CTRL+comma ("Edit.GoToAll") can follow symbols in comments, whereas CTRL+F12 ("Edit.GoToImplementation") cannot.
//   With Visual Assist installed: ALT+G ("VAssistX.GoToImplementation") can also follow symbols in comments.
typedef int MsTrGCol;               // -> enum MsTrGCol_             // Enum: A color identifier for styling
typedef int MsTrGCond;              // -> enum MsTrGCond_            // Enum: A condition for many Set*() functions
typedef int MsTrGDataType;          // -> enum MsTrGDataType_        // Enum: A primary data type
typedef int MsTrGDir;               // -> enum MsTrGDir_             // Enum: A cardinal direction
typedef int MsTrGKey;               // -> enum MsTrGKey_             // Enum: A key identifier
typedef int MsTrGNavInput;          // -> enum MsTrGNavInput_        // Enum: An input identifier for navigation
typedef int MsTrGMouseButton;       // -> enum MsTrGMouseButton_     // Enum: A mouse button identifier (0=left, 1=right, 2=middle)
typedef int MsTrGMouseCursor;       // -> enum MsTrGMouseCursor_     // Enum: A mouse cursor identifier
typedef int MsTrGSortDirection;     // -> enum MsTrGSortDirection_   // Enum: A sorting direction (ascending or descending)
typedef int MsTrGStyleVar;          // -> enum MsTrGStyleVar_        // Enum: A variable identifier for styling
typedef int MsTrGTableBgTarget;     // -> enum MsTrGTableBgTarget_   // Enum: A color target for TableSetBgColor()
typedef int ImDrawFlags;            // -> enum ImDrawFlags_          // Flags: for ImDrawList functions
typedef int ImDrawListFlags;        // -> enum ImDrawListFlags_      // Flags: for ImDrawList instance
typedef int ImFontAtlasFlags;       // -> enum ImFontAtlasFlags_     // Flags: for ImFontAtlas build
typedef int MsTrGBackendFlags;      // -> enum MsTrGBackendFlags_    // Flags: for io.BackendFlags
typedef int MsTrGButtonFlags;       // -> enum MsTrGButtonFlags_     // Flags: for InvisibleButton()
typedef int MsTrGColorEditFlags;    // -> enum MsTrGColorEditFlags_  // Flags: for ColorEdit4(), ColorPicker4() etc.
typedef int MsTrGConfigFlags;       // -> enum MsTrGConfigFlags_     // Flags: for io.ConfigFlags
typedef int MsTrGComboFlags;        // -> enum MsTrGComboFlags_      // Flags: for BeginCombo()
typedef int MsTrGDragDropFlags;     // -> enum MsTrGDragDropFlags_   // Flags: for BeginDragDropSource(), AcceptDragDropPayload()
typedef int MsTrGFocusedFlags;      // -> enum MsTrGFocusedFlags_    // Flags: for IsWindowFocused()
typedef int MsTrGHoveredFlags;      // -> enum MsTrGHoveredFlags_    // Flags: for IsItemHovered(), IsWindowHovered() etc.
typedef int MsTrGInputTextFlags;    // -> enum MsTrGInputTextFlags_  // Flags: for InputText(), InputTextMultiline()
typedef int MsTrGModFlags;          // -> enum MsTrGModFlags_        // Flags: for io.KeyMods (Ctrl/Shift/Alt/Super)
typedef int MsTrGPopupFlags;        // -> enum MsTrGPopupFlags_      // Flags: for OpenPopup*(), BeginPopupContext*(), IsPopupOpen()
typedef int MsTrGSelectableFlags;   // -> enum MsTrGSelectableFlags_ // Flags: for Selectable()
typedef int MsTrGSliderFlags;       // -> enum MsTrGSliderFlags_     // Flags: for DragFloat(), DragInt(), SliderFloat(), SliderInt() etc.
typedef int MsTrGTabBarFlags;       // -> enum MsTrGTabBarFlags_     // Flags: for BeginTabBar()
typedef int MsTrGTabItemFlags;      // -> enum MsTrGTabItemFlags_    // Flags: for BeginTabItem()
typedef int MsTrGTableFlags;        // -> enum MsTrGTableFlags_      // Flags: For BeginTable()
typedef int MsTrGTableColumnFlags;  // -> enum MsTrGTableColumnFlags_// Flags: For TableSetupColumn()
typedef int MsTrGTableRowFlags;     // -> enum MsTrGTableRowFlags_   // Flags: For TableNextRow()
typedef int MsTrGTreeNodeFlags;     // -> enum MsTrGTreeNodeFlags_   // Flags: for TreeNode(), TreeNodeEx(), CollapsingHeader()
typedef int MsTrGViewportFlags;     // -> enum MsTrGViewportFlags_   // Flags: for MsTrGViewport
typedef int MsTrGWindowFlags;       // -> enum MsTrGWindowFlags_     // Flags: for Begin(), BeginChild()

// ImTexture: user data for renderer backend to identify a texture [Compile-time configurable type]
// - To use something else than an opaque void* pointer: override with e.g. '#define ImTextureID MyTextureType*' in your imconfig.h file.
// - This can be whatever to you want it to be! read the FAQ about ImTextureID for details.
#ifndef ImTextureID
typedef void* ImTextureID;          // Default: store a pointer or an integer fitting in a pointer (most renderer backends are ok with that)
#endif

// ImDrawIdx: vertex index. [Compile-time configurable type]
// - To use 16-bit indices + allow large meshes: backend need to set 'io.BackendFlags |= MsTrGBackendFlags_RendererHasVtxOffset' and handle ImDrawCmd::VtxOffset (recommended).
// - To use 32-bit indices: override with '#define ImDrawIdx unsigned int' in your imconfig.h file.
#ifndef ImDrawIdx
typedef unsigned short ImDrawIdx;   // Default: 16-bit (for maximum compatibility with renderer backends)
#endif

// Scalar data types
typedef unsigned int        MsTrGID;// A unique ID used by widgets (typically the result of hashing a stack of string)
typedef signed char         ImS8;   // 8-bit signed integer
typedef unsigned char       ImU8;   // 8-bit unsigned integer
typedef signed short        ImS16;  // 16-bit signed integer
typedef unsigned short      ImU16;  // 16-bit unsigned integer
typedef signed int          ImS32;  // 32-bit signed integer == int
typedef unsigned int        ImU32;  // 32-bit unsigned integer (often used to store packed colors)
typedef signed   long long  ImS64;  // 64-bit signed integer
typedef unsigned long long  ImU64;  // 64-bit unsigned integer

// Character types
// (we generally use UTF-8 encoded string in the API. This is storage specifically for a decoded character used for keyboard input and display)
typedef unsigned short ImWchar16;   // A single decoded U16 character/code point. We encode them as multi bytes UTF-8 when used in strings.
typedef unsigned int ImWchar32;     // A single decoded U32 character/code point. We encode them as multi bytes UTF-8 when used in strings.
#ifdef MsTrG_USE_WCHAR32            // ImWchar [configurable type: override in imconfig.h with '#define MsTrG_USE_WCHAR32' to support Unicode planes 1-16]
typedef ImWchar32 ImWchar;
#else
typedef ImWchar16 ImWchar;
#endif

// Callback and functions types
typedef int     (*MsTrGInputTextCallback)(MsTrGInputTextCallbackData* data);    // Callback function for MsTrG::InputText()
typedef void    (*MsTrGSizeCallback)(MsTrGSizeCallbackData* data);              // Callback function for MsTrG::SetNextWindowSizeConstraints()
typedef void*   (*MsTrGMemAllocFunc)(size_t sz, void* user_data);               // Function signature for MsTrG::SetAllocatorFunctions()
typedef void    (*MsTrGMemFreeFunc)(void* ptr, void* user_data);                // Function signature for MsTrG::SetAllocatorFunctions()

// ImVec2: 2D vector used to store positions, sizes etc. [Compile-time configurable type]
// This is a frequently used type in the API. Consider using IM_VEC2_CLASS_EXTRA to create implicit cast from/to our preferred type.
IM_MSVC_RUNTIME_CHECKS_OFF
struct ImVec2
{
    float                                   x, y;
    constexpr ImVec2()                      : x(0.0f), y(0.0f) { }
    constexpr ImVec2(float _x, float _y)    : x(_x), y(_y) { }
    float  operator[] (size_t idx) const    { IM_ASSERT(idx <= 1); return (&x)[idx]; }    // We very rarely use this [] operator, the assert overhead is fine.
    float& operator[] (size_t idx)          { IM_ASSERT(idx <= 1); return (&x)[idx]; }    // We very rarely use this [] operator, the assert overhead is fine.
#ifdef IM_VEC2_CLASS_EXTRA
    IM_VEC2_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and ImVec2.
#endif
};

// ImVec4: 4D vector used to store clipping rectangles, colors etc. [Compile-time configurable type]
struct ImVec4
{
    float                                                     x, y, z, w;
    constexpr ImVec4()                                        : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
    constexpr ImVec4(float _x, float _y, float _z, float _w)  : x(_x), y(_y), z(_z), w(_w) { }
#ifdef IM_VEC4_CLASS_EXTRA
    IM_VEC4_CLASS_EXTRA     // Define additional constructors and implicit cast operators in imconfig.h to convert back and forth between your math types and ImVec4.
#endif
};
IM_MSVC_RUNTIME_CHECKS_RESTORE

//-----------------------------------------------------------------------------
// [SECTION] Dear MsTrG end-user API functions
// (Note that MsTrG:: being a namespace, you can add extra MsTrG:: functions in your own separate file. Please don't modify MsTrG source files!)
//-----------------------------------------------------------------------------

namespace MsTrG
{
    // Context creation and access
    // - Each context create its own ImFontAtlas by default. You may instance one yourself and pass it to CreateContext() to share a font atlas between contexts.
    // - DLL users: heaps and globals are not shared across DLL boundaries! You will need to call SetCurrentContext() + SetAllocatorFunctions()
    //   for each static/DLL boundary you are calling from. Read "Context and Memory Allocators" section of MsTrG.cpp for details.
    MsTrG_API MsTrGContext* CreateContext(ImFontAtlas* shared_font_atlas = NULL);
    MsTrG_API void          DestroyContext(MsTrGContext* ctx = NULL);   // NULL = destroy current context
    MsTrG_API MsTrGContext* GetCurrentContext();
    MsTrG_API void          SetCurrentContext(MsTrGContext* ctx);

    // Main
    MsTrG_API MsTrGIO&      GetIO();                                    // access the IO structure (mouse/keyboard/gamepad inputs, time, various configuration options/flags)
    MsTrG_API MsTrGStyle&   GetStyle();                                 // access the Style structure (colors, sizes). Always use PushStyleCol(), PushStyleVar() to modify style mid-frame!
    MsTrG_API void          NewFrame();                                 // start a new Dear MsTrG frame, you can submit any command from this point until Render()/EndFrame().
    MsTrG_API void          EndFrame();                                 // ends the Dear MsTrG frame. automatically called by Render(). If you don't need to render data (skipping rendering) you may call EndFrame() without Render()... but you'll have wasted CPU already! If you don't need to render, better to not create any windows and not call NewFrame() at all!
    MsTrG_API void          Render();                                   // ends the Dear MsTrG frame, finalize the draw data. You can then get call GetDrawData().
    MsTrG_API ImDrawData*   GetDrawData();                              // valid after Render() and until the next call to NewFrame(). this is what you have to render.

    // Demo, Debug, Information
    MsTrG_API void          ShowDemoWindow(bool* p_open = NULL);        // create Demo window. demonstrate most MsTrG features. call this to learn about the library! try to make it always available in your application!
    MsTrG_API void          ShowMetricsWindow(bool* p_open = NULL);     // create Metrics/Debugger window. display Dear MsTrG internals: windows, draw commands, various internal state, etc.
    MsTrG_API void          ShowDebugLogWindow(bool* p_open = NULL);    // create Debug Log window. display a simplified log of important dear MsTrG events.
    MsTrG_API void          ShowStackToolWindow(bool* p_open = NULL);   // create Stack Tool window. hover items with mouse to query information about the source of their unique ID.
    MsTrG_API void          ShowAboutWindow(bool* p_open = NULL);       // create About window. display Dear MsTrG version, credits and build/system information.
    MsTrG_API void          ShowStyleEditor(MsTrGStyle* ref = NULL);    // add style editor block (not a window). you can pass in a reference MsTrGStyle structure to compare to, revert to and save to (else it uses the default style)
    MsTrG_API bool          ShowStyleSelector(const char* label);       // add style selector block (not a window), essentially a combo listing the default styles.
    MsTrG_API void          ShowFontSelector(const char* label);        // add font selector block (not a window), essentially a combo listing the loaded fonts.
    MsTrG_API void          ShowUserGuide();                            // add basic help/info block (not a window): how to manipulate MsTrG as a end-user (mouse/keyboard controls).
    MsTrG_API const char*   GetVersion();                               // get the compiled version string e.g. "1.80 WIP" (essentially the value for MsTrG_VERSION from the compiled version of MsTrG.cpp)

    // Styles
    MsTrG_API void          StyleColorsDark(MsTrGStyle* dst = NULL);    // new, recommended style (default)
    MsTrG_API void          StyleColorsLight(MsTrGStyle* dst = NULL);   // best used with borders and a custom, thicker font
    MsTrG_API void          StyleColorsClassic(MsTrGStyle* dst = NULL); // classic MsTrG style

    // Windows
    // - Begin() = push window to the stack and start appending to it. End() = pop window from the stack.
    // - Passing 'bool* p_open != NULL' shows a window-closing widget in the upper-right corner of the window,
    //   which clicking will set the boolean to false when clicked.
    // - You may append multiple times to the same window during the same frame by calling Begin()/End() pairs multiple times.
    //   Some information such as 'flags' or 'p_open' will only be considered by the first call to Begin().
    // - Begin() return false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting
    //   anything to the window. Always call a matching End() for each Begin() call, regardless of its return value!
    //   [Important: due to legacy reason, this is inconsistent with most other functions such as BeginMenu/EndMenu,
    //    BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding BeginXXX function
    //    returned true. Begin and BeginChild are the only odd ones out. Will be fixed in a future update.]
    // - Note that the bottom of window stack always contains a window called "Debug".
    MsTrG_API bool          Begin(const char* name, bool* p_open = NULL, MsTrGWindowFlags flags = 0);
    MsTrG_API void          End();

    // Child Windows
    // - Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window. Child windows can embed their own child.
    // - For each independent axis of 'size': ==0.0f: use remaining host window size / >0.0f: fixed size / <0.0f: use remaining window size minus abs(size) / Each axis can use a different mode, e.g. ImVec2(0,400).
    // - BeginChild() returns false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting anything to the window.
    //   Always call a matching EndChild() for each BeginChild() call, regardless of its return value.
    //   [Important: due to legacy reason, this is inconsistent with most other functions such as BeginMenu/EndMenu,
    //    BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding BeginXXX function
    //    returned true. Begin and BeginChild are the only odd ones out. Will be fixed in a future update.]
    MsTrG_API bool          BeginChild(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = false, MsTrGWindowFlags flags = 0);
    MsTrG_API bool          BeginChild(MsTrGID id, const ImVec2& size = ImVec2(0, 0), bool border = false, MsTrGWindowFlags flags = 0);
    MsTrG_API bool          beginchildex(const char* name, MsTrGID id, const ImVec2& size_arg, bool border, MsTrGWindowFlags flags);
    MsTrG_API bool          MenuChild(const char* str_id, const ImVec2& size_arg = ImVec2(0, 0), bool sub_tab = false, MsTrGWindowFlags extra_flags = NULL);
    MsTrG_API void          EndChild();

    // Windows Utilities
    // - 'current window' = the window we are appending into while inside a Begin()/End() block. 'next window' = next window we will Begin() into.
    MsTrG_API bool          IsWindowAppearing();
    MsTrG_API bool          IsWindowCollapsed();
    MsTrG_API bool          IsWindowFocused(MsTrGFocusedFlags flags=0); // is current window focused? or its root/child, depending on flags. see flags for options.
    MsTrG_API bool          IsWindowHovered(MsTrGHoveredFlags flags=0); // is current window hovered (and typically: not blocked by a popup/modal)? see flags for options. NB: If you are trying to check whether your mouse should be dispatched to MsTrG or to your app, you should use the 'io.WantCaptureMouse' boolean for that! Please read the FAQ!
    MsTrG_API ImDrawList*   GetWindowDrawList();                        // get draw list associated to the current window, to append your own drawing primitives
    MsTrG_API ImVec2        GetWindowPos();                             // get current window position in screen space (useful if you want to do your own drawing via the DrawList API)
    MsTrG_API ImVec2        GetWindowSize();                            // get current window size
    MsTrG_API float         GetWindowWidth();                           // get current window width (shortcut for GetWindowSize().x)
    MsTrG_API float         GetWindowHeight();                          // get current window height (shortcut for GetWindowSize().y)

    // Window manipulation
    // - Prefer using SetNextXXX functions (before Begin) rather that SetXXX functions (after Begin).
    MsTrG_API void          SetNextWindowPos(const ImVec2& pos, MsTrGCond cond = 0, const ImVec2& pivot = ImVec2(0, 0)); // set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc.
    MsTrG_API void          SetNextWindowSize(const ImVec2& size, MsTrGCond cond = 0);                  // set next window size. set axis to 0.0f to force an auto-fit on this axis. call before Begin()
    MsTrG_API void          SetNextWindowSizeConstraints(const ImVec2& size_min, const ImVec2& size_max, MsTrGSizeCallback custom_callback = NULL, void* custom_callback_data = NULL); // set next window size limits. use -1,-1 on either X/Y axis to preserve the current size. Sizes will be rounded down. Use callback to apply non-trivial programmatic constraints.
    MsTrG_API void          SetNextWindowContentSize(const ImVec2& size);                               // set next window content size (~ scrollable client area, which enforce the range of scrollbars). Not including window decorations (title bar, menu bar, etc.) nor WindowPadding. set an axis to 0.0f to leave it automatic. call before Begin()
    MsTrG_API void          SetNextWindowCollapsed(bool collapsed, MsTrGCond cond = 0);                 // set next window collapsed state. call before Begin()
    MsTrG_API void          SetNextWindowFocus();                                                       // set next window to be focused / top-most. call before Begin()
    MsTrG_API void          SetNextWindowBgAlpha(float alpha);                                          // set next window background color alpha. helper to easily override the Alpha component of MsTrGCol_WindowBg/ChildBg/PopupBg. you may also use MsTrGWindowFlags_NoBackground.
    MsTrG_API void          SetWindowPos(const ImVec2& pos, MsTrGCond cond = 0);                        // (not recommended) set current window position - call within Begin()/End(). prefer using SetNextWindowPos(), as this may incur tearing and side-effects.
    MsTrG_API void          SetWindowSize(const ImVec2& size, MsTrGCond cond = 0);                      // (not recommended) set current window size - call within Begin()/End(). set to ImVec2(0, 0) to force an auto-fit. prefer using SetNextWindowSize(), as this may incur tearing and minor side-effects.
    MsTrG_API void          SetWindowCollapsed(bool collapsed, MsTrGCond cond = 0);                     // (not recommended) set current window collapsed state. prefer using SetNextWindowCollapsed().
    MsTrG_API void          SetWindowFocus();                                                           // (not recommended) set current window to be focused / top-most. prefer using SetNextWindowFocus().
    MsTrG_API void          SetWindowFontScale(float scale);                                            // [OBSOLETE] set font scale. Adjust IO.FontGlobalScale if you want to scale all windows. This is an old API! For correct scaling, prefer to reload font + rebuild ImFontAtlas + call style.ScaleAllSizes().
    MsTrG_API void          SetWindowPos(const char* name, const ImVec2& pos, MsTrGCond cond = 0);      // set named window position.
    MsTrG_API void          SetWindowSize(const char* name, const ImVec2& size, MsTrGCond cond = 0);    // set named window size. set axis to 0.0f to force an auto-fit on this axis.
    MsTrG_API void          SetWindowCollapsed(const char* name, bool collapsed, MsTrGCond cond = 0);   // set named window collapsed state
    MsTrG_API void          SetWindowFocus(const char* name);                                           // set named window to be focused / top-most. use NULL to remove focus.

    // Content region
    // - Retrieve available space from a given point. GetContentRegionAvail() is frequently useful.
    // - Those functions are bound to be redesigned (they are confusing, incomplete and the Min/Max return values are in local window coordinates which increases confusion)
    MsTrG_API ImVec2        GetContentRegionAvail();                                        // == GetContentRegionMax() - GetCursorPos()
    MsTrG_API ImVec2        GetContentRegionMax();                                          // current content boundaries (typically window boundaries including scrolling, or current column boundaries), in windows coordinates
    MsTrG_API ImVec2        GetWindowContentRegionMin();                                    // content boundaries min for the full window (roughly (0,0)-Scroll), in window coordinates
    MsTrG_API ImVec2        GetWindowContentRegionMax();                                    // content boundaries max for the full window (roughly (0,0)+Size-Scroll) where Size can be override with SetNextWindowContentSize(), in window coordinates

    // Windows Scrolling
    MsTrG_API float         GetScrollX();                                                   // get scrolling amount [0 .. GetScrollMaxX()]
    MsTrG_API float         GetScrollY();                                                   // get scrolling amount [0 .. GetScrollMaxY()]
    MsTrG_API void          SetScrollX(float scroll_x);                                     // set scrolling amount [0 .. GetScrollMaxX()]
    MsTrG_API void          SetScrollY(float scroll_y);                                     // set scrolling amount [0 .. GetScrollMaxY()]
    MsTrG_API float         GetScrollMaxX();                                                // get maximum scrolling amount ~~ ContentSize.x - WindowSize.x - DecorationsSize.x
    MsTrG_API float         GetScrollMaxY();                                                // get maximum scrolling amount ~~ ContentSize.y - WindowSize.y - DecorationsSize.y
    MsTrG_API void          SetScrollHereX(float center_x_ratio = 0.5f);                    // adjust scrolling amount to make current cursor position visible. center_x_ratio=0.0: left, 0.5: center, 1.0: right. When using to make a "default/current item" visible, consider using SetItemDefaultFocus() instead.
    MsTrG_API void          SetScrollHereY(float center_y_ratio = 0.5f);                    // adjust scrolling amount to make current cursor position visible. center_y_ratio=0.0: top, 0.5: center, 1.0: bottom. When using to make a "default/current item" visible, consider using SetItemDefaultFocus() instead.
    MsTrG_API void          SetScrollFromPosX(float local_x, float center_x_ratio = 0.5f);  // adjust scrolling amount to make given position visible. Generally GetCursorStartPos() + offset to compute a valid position.
    MsTrG_API void          SetScrollFromPosY(float local_y, float center_y_ratio = 0.5f);  // adjust scrolling amount to make given position visible. Generally GetCursorStartPos() + offset to compute a valid position.

    // Parameters stacks (shared)
    MsTrG_API void          PushFont(ImFont* font);                                         // use NULL as a shortcut to push default font
    MsTrG_API void          PopFont();
    MsTrG_API void          PushStyleColor(MsTrGCol idx, ImU32 col);                        // modify a style color. always use this if you modify the style after NewFrame().
    MsTrG_API void          PushStyleColor(MsTrGCol idx, const ImVec4& col);
    MsTrG_API void          PopStyleColor(int count = 1);
    MsTrG_API void          PushStyleVar(MsTrGStyleVar idx, float val);                     // modify a style float variable. always use this if you modify the style after NewFrame().
    MsTrG_API void          PushStyleVar(MsTrGStyleVar idx, const ImVec2& val);             // modify a style ImVec2 variable. always use this if you modify the style after NewFrame().
    MsTrG_API void          PopStyleVar(int count = 1);
    MsTrG_API void          PushAllowKeyboardFocus(bool allow_keyboard_focus);              // == tab stop enable. Allow focusing using TAB/Shift-TAB, enabled by default but you can disable it for certain widgets
    MsTrG_API void          PopAllowKeyboardFocus();
    MsTrG_API void          PushButtonRepeat(bool repeat);                                  // in 'repeat' mode, Button*() functions return repeated true in a typematic manner (using io.KeyRepeatDelay/io.KeyRepeatRate setting). Note that you can call IsItemActive() after any Button() to tell if the button is held in the current frame.
    MsTrG_API void          PopButtonRepeat();

    // Parameters stacks (current window)
    MsTrG_API void          PushItemWidth(float item_width);                                // push width of items for common large "item+label" widgets. >0.0f: width in pixels, <0.0f align xx pixels to the right of window (so -FLT_MIN always align width to the right side).
    MsTrG_API void          PopItemWidth();
    MsTrG_API void          SetNextItemWidth(float item_width);                             // set width of the _next_ common large "item+label" widget. >0.0f: width in pixels, <0.0f align xx pixels to the right of window (so -FLT_MIN always align width to the right side)
    MsTrG_API float         CalcItemWidth();                                                // width of item given pushed settings and current cursor position. NOT necessarily the width of last item unlike most 'Item' functions.
    MsTrG_API void          PushTextWrapPos(float wrap_local_pos_x = 0.0f);                 // push word-wrapping position for Text*() commands. < 0.0f: no wrapping; 0.0f: wrap to end of window (or column); > 0.0f: wrap at 'wrap_pos_x' position in window local space
    MsTrG_API void          PopTextWrapPos();

    // Style read access
    // - Use the style editor (ShowStyleEditor() function) to interactively see what the colors are)
    MsTrG_API ImFont*       GetFont();                                                      // get current font
    MsTrG_API float         GetFontSize();                                                  // get current font size (= height in pixels) of current font with current scale applied
    MsTrG_API ImVec2        GetFontTexUvWhitePixel();                                       // get UV coordinate for a while pixel, useful to draw custom shapes via the ImDrawList API
    MsTrG_API ImU32         GetColorU32(MsTrGCol idx, float alpha_mul = 1.0f);              // retrieve given style color with style alpha applied and optional extra alpha multiplier, packed as a 32-bit value suitable for ImDrawList
    MsTrG_API ImU32         GetColorU32(const ImVec4& col);                                 // retrieve given color with style alpha applied, packed as a 32-bit value suitable for ImDrawList
    MsTrG_API ImU32         GetColorU32(ImU32 col);                                         // retrieve given color with style alpha applied, packed as a 32-bit value suitable for ImDrawList
    MsTrG_API const ImVec4& GetStyleColorVec4(MsTrGCol idx);                                // retrieve style color as stored in MsTrGStyle structure. use to feed back into PushStyleColor(), otherwise use GetColorU32() to get style color with style alpha baked in.

    // Cursor / Layout
    // - By "cursor" we mean the current output position.
    // - The typical widget behavior is to output themselves at the current cursor position, then move the cursor one line down.
    // - You can call SameLine() between widgets to undo the last carriage return and output at the right of the preceding widget.
    // - Attention! We currently have inconsistencies between window-local and absolute positions we will aim to fix with future API:
    //    Window-local coordinates:   SameLine(), GetCursorPos(), SetCursorPos(), GetCursorStartPos(), GetContentRegionMax(), GetWindowContentRegion*(), PushTextWrapPos()
    //    Absolute coordinate:        GetCursorScreenPos(), SetCursorScreenPos(), all ImDrawList:: functions.
    MsTrG_API void          Separator();                                                    // separator, generally horizontal. inside a menu bar or in horizontal layout mode, this becomes a vertical separator.
    MsTrG_API void          SameLine(float offset_from_start_x=0.0f, float spacing=-1.0f);  // call between widgets or groups to layout them horizontally. X position given in window coordinates.
    MsTrG_API void          NewLine();                                                      // undo a SameLine() or force a new line when in an horizontal-layout context.
    MsTrG_API void          Spacing();                                                      // add vertical spacing.
    MsTrG_API void          Dummy(const ImVec2& size);                                      // add a dummy item of given size. unlike InvisibleButton(), Dummy() won't take the mouse click or be navigable into.
    MsTrG_API void          Indent(float indent_w = 0.0f);                                  // move content position toward the right, by indent_w, or style.IndentSpacing if indent_w <= 0
    MsTrG_API void          Unindent(float indent_w = 0.0f);                                // move content position back to the left, by indent_w, or style.IndentSpacing if indent_w <= 0
    MsTrG_API void          BeginGroup();                                                   // lock horizontal starting position
    MsTrG_API void          EndGroup();                                                     // unlock horizontal starting position + capture the whole group bounding box into one "item" (so you can use IsItemHovered() or layout primitives such as SameLine() on whole group, etc.)
    MsTrG_API ImVec2        GetCursorPos();                                                 // cursor position in window coordinates (relative to window position)
    MsTrG_API float         GetCursorPosX();                                                //   (some functions are using window-relative coordinates, such as: GetCursorPos, GetCursorStartPos, GetContentRegionMax, GetWindowContentRegion* etc.
    MsTrG_API float         GetCursorPosY();                                                //    other functions such as GetCursorScreenPos or everything in ImDrawList::
    MsTrG_API void          SetCursorPos(const ImVec2& local_pos);                          //    are using the main, absolute coordinate system.
    MsTrG_API void          SetCursorPosX(float local_x);                                   //    GetWindowPos() + GetCursorPos() == GetCursorScreenPos() etc.)
    MsTrG_API void          SetCursorPosY(float local_y);                                   //
    MsTrG_API ImVec2        GetCursorStartPos();                                            // initial cursor position in window coordinates
    MsTrG_API ImVec2        GetCursorScreenPos();                                           // cursor position in absolute coordinates (useful to work with ImDrawList API). generally top-left == GetMainViewport()->Pos == (0,0) in single viewport mode, and bottom-right == GetMainViewport()->Pos+Size == io.DisplaySize in single-viewport mode.
    MsTrG_API void          SetCursorScreenPos(const ImVec2& pos);                          // cursor position in absolute coordinates
    MsTrG_API void          AlignTextToFramePadding();                                      // vertically align upcoming text baseline to FramePadding.y so that it will align properly to regularly framed items (call if you have text on a line before a framed item)
    MsTrG_API float         GetTextLineHeight();                                            // ~ FontSize
    MsTrG_API float         GetTextLineHeightWithSpacing();                                 // ~ FontSize + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of text)
    MsTrG_API float         GetFrameHeight();                                               // ~ FontSize + style.FramePadding.y * 2
    MsTrG_API float         GetFrameHeightWithSpacing();                                    // ~ FontSize + style.FramePadding.y * 2 + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of framed widgets)

    // ID stack/scopes
    // Read the FAQ (docs/FAQ.md or http://dearMsTrG.org/faq) for more details about how ID are handled in dear MsTrG.
    // - Those questions are answered and impacted by understanding of the ID stack system:
    //   - "Q: Why is my widget not reacting when I click on it?"
    //   - "Q: How can I have widgets with an empty label?"
    //   - "Q: How can I have multiple widgets with the same label?"
    // - Short version: ID are hashes of the entire ID stack. If you are creating widgets in a loop you most likely
    //   want to push a unique identifier (e.g. object pointer, loop index) to uniquely differentiate them.
    // - You can also use the "Label##foobar" syntax within widget label to distinguish them from each others.
    // - In this header file we use the "label"/"name" terminology to denote a string that will be displayed + used as an ID,
    //   whereas "str_id" denote a string that is only used as an ID and not normally displayed.
    MsTrG_API void          PushID(const char* str_id);                                     // push string into the ID stack (will hash string).
    MsTrG_API void          PushID(const char* str_id_begin, const char* str_id_end);       // push string into the ID stack (will hash string).
    MsTrG_API void          PushID(const void* ptr_id);                                     // push pointer into the ID stack (will hash pointer).
    MsTrG_API void          PushID(int int_id);                                             // push integer into the ID stack (will hash integer).
    MsTrG_API void          PopID();                                                        // pop from the ID stack.
    MsTrG_API MsTrGID       GetID(const char* str_id);                                      // calculate unique ID (hash of whole ID stack + given parameter). e.g. if you want to query into MsTrGStorage yourself
    MsTrG_API MsTrGID       GetID(const char* str_id_begin, const char* str_id_end);
    MsTrG_API MsTrGID       GetID(const void* ptr_id);

    // Widgets: Text
    MsTrG_API void          TextUnformatted(const char* text, const char* text_end = NULL); // raw text without formatting. Roughly equivalent to Text("%s", text) but: A) doesn't require null terminated string if 'text_end' is specified, B) it's faster, no memory copy is done, no buffer size limits, recommended for long chunks of text.
    MsTrG_API void          Text(const char* fmt, ...)                                      IM_FMTARGS(1); // formatted text
    MsTrG_API void          TextV(const char* fmt, va_list args)                            IM_FMTLIST(1);
    MsTrG_API void          TextColored(const ImVec4& col, const char* fmt, ...)            IM_FMTARGS(2); // shortcut for PushStyleColor(MsTrGCol_Text, col); Text(fmt, ...); PopStyleColor();
    MsTrG_API void          TextColoredV(const ImVec4& col, const char* fmt, va_list args)  IM_FMTLIST(2);
    MsTrG_API void          TextDisabled(const char* fmt, ...)                              IM_FMTARGS(1); // shortcut for PushStyleColor(MsTrGCol_Text, style.Colors[MsTrGCol_TextDisabled]); Text(fmt, ...); PopStyleColor();
    MsTrG_API void          TextDisabledV(const char* fmt, va_list args)                    IM_FMTLIST(1);
    MsTrG_API void          TextWrapped(const char* fmt, ...)                               IM_FMTARGS(1); // shortcut for PushTextWrapPos(0.0f); Text(fmt, ...); PopTextWrapPos();. Note that this won't work on an auto-resizing window if there's no other widgets to extend the window width, yoy may need to set a size using SetNextWindowSize().
    MsTrG_API void          TextWrappedV(const char* fmt, va_list args)                     IM_FMTLIST(1);
    MsTrG_API void          LabelText(const char* label, const char* fmt, ...)              IM_FMTARGS(2); // display text+label aligned the same way as value+label widgets
    MsTrG_API void          LabelTextV(const char* label, const char* fmt, va_list args)    IM_FMTLIST(2);
    MsTrG_API void          BulletText(const char* fmt, ...)                                IM_FMTARGS(1); // shortcut for Bullet()+Text()
    MsTrG_API void          BulletTextV(const char* fmt, va_list args)                      IM_FMTLIST(1);

    // Widgets: Main
    // - Most widgets return true when the value has been changed or when pressed/selected
    // - You may also use one of the many IsItemXXX functions (e.g. IsItemActive, IsItemHovered, etc.) to query widget state.
    MsTrG_API bool          Button(const char* label, const ImVec2& size = ImVec2(0, 0));   // button
    MsTrG_API bool          SmallButton(const char* label);                                 // button with FramePadding=(0,0) to easily embed within text
    MsTrG_API bool          InvisibleButton(const char* str_id, const ImVec2& size, MsTrGButtonFlags flags = 0); // flexible button behavior without the visuals, frequently useful to build custom behaviors using the public api (along with IsItemActive, IsItemHovered, etc.)
    MsTrG_API bool          ArrowButton(const char* str_id, MsTrGDir dir);                  // square button with an arrow shape
    MsTrG_API void          Image(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1,1), const ImVec4& tint_col = ImVec4(1,1,1,1), const ImVec4& border_col = ImVec4(0,0,0,0));
    MsTrG_API bool          ImageButton(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0),  const ImVec2& uv1 = ImVec2(1,1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0,0,0,0), const ImVec4& tint_col = ImVec4(1,1,1,1));    // <0 frame_padding uses default frame padding settings. 0 for no padding
    MsTrG_API bool          Checkbox(const char* label, bool* v);
    MsTrG_API bool          tab(const char* label, bool selected);
    MsTrG_API bool          KeyBind(const char* label, int& k, const ImVec2& size_arg = ImVec2(0, 0));
    MsTrG_API bool          Keybind(const char* label, int* key, int* mode);
    MsTrG_API bool          CheckboxFlags(const char* label, int* flags, int flags_value);
    MsTrG_API bool          CheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value);
    MsTrG_API bool          RadioButton(const char* label, bool active);                    // use with e.g. if (RadioButton("one", my_value==1)) { my_value = 1; }
    MsTrG_API bool          RadioButton(const char* label, int* v, int v_button);           // shortcut to handle the above pattern when value is an integer
    MsTrG_API void          ProgressBar(float fraction, const ImVec2& size_arg = ImVec2(-FLT_MIN, 0), const char* overlay = NULL);
    MsTrG_API void          Bullet();                                                       // draw a small circle + keep the cursor on the same line. advance cursor x position by GetTreeNodeToLabelSpacing(), same distance that TreeNode() uses

    // Widgets: Combo Box
    // - The BeginCombo()/EndCombo() api allows you to manage your contents and selection state however you want it, by creating e.g. Selectable() items.
    // - The old Combo() api are helpers over BeginCombo()/EndCombo() which are kept available for convenience purpose. This is analogous to how ListBox are created.
    MsTrG_API bool          BeginCombo(const char* label, const char* preview_value, MsTrGComboFlags flags = 0);
    MsTrG_API void          EndCombo(); // only call EndCombo() if BeginCombo() returns true!
    MsTrG_API bool          Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
    MsTrG_API bool          Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);      // Separate items with \0 within a string, end item-list with \0\0. e.g. "One\0Two\0Three\0"
    MsTrG_API bool          Combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1);

    // Widgets: Drag Sliders
    // - CTRL+Click on any drag box to turn them into an input box. Manually input values aren't clamped by default and can go off-bounds. Use MsTrGSliderFlags_AlwaysClamp to always clamp.
    // - For all the Float2/Float3/Float4/Int2/Int3/Int4 versions of every functions, note that a 'float v[X]' function argument is the same as 'float* v',
    //   the array syntax is just a way to document the number of elements that are expected to be accessible. You can pass address of your first element out of a contiguous set, e.g. &myvector.x
    // - Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
    // - Format string may also be set to NULL or use the default format ("%f" or "%d").
    // - Speed are per-pixel of mouse movement (v_speed=0.2f: mouse needs to move by 5 pixels to increase value by 1). For gamepad/keyboard navigation, minimum speed is Max(v_speed, minimum_step_at_given_precision).
    // - Use v_min < v_max to clamp edits to given limits. Note that CTRL+Click manual input can override those limits if MsTrGSliderFlags_AlwaysClamp is not used.
    // - Use v_max = FLT_MAX / INT_MAX etc to avoid clamping to a maximum, same with v_min = -FLT_MAX / INT_MIN to avoid clamping to a minimum.
    // - We use the same sets of flags for DragXXX() and SliderXXX() functions as the features are the same and it makes it easier to swap them.
    // - Legacy: Pre-1.78 there are DragXXX() function signatures that takes a final `float power=1.0f' argument instead of the `MsTrGSliderFlags flags=0' argument.
    //   If you get a warning converting a float to MsTrGSliderFlags, read https://github.com/ocornut/MsTrG/issues/3361
    MsTrG_API bool          DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", MsTrGSliderFlags flags = 0);     // If v_min >= v_max we have no bound
    MsTrG_API bool          DragFloat2(const char* label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", MsTrGSliderFlags flags = 0);
    MsTrG_API bool          DragFloat3(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", MsTrGSliderFlags flags = 0);
    MsTrG_API bool          DragFloat4(const char* label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", MsTrGSliderFlags flags = 0);
    MsTrG_API bool          DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", const char* format_max = NULL, MsTrGSliderFlags flags = 0);
    MsTrG_API bool          DragInt(const char* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", MsTrGSliderFlags flags = 0);  // If v_min >= v_max we have no bound
    MsTrG_API bool          DragInt2(const char* label, int v[2], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", MsTrGSliderFlags flags = 0);
    MsTrG_API bool          DragInt3(const char* label, int v[3], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", MsTrGSliderFlags flags = 0);
    MsTrG_API bool          DragInt4(const char* label, int v[4], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", MsTrGSliderFlags flags = 0);
    MsTrG_API bool          DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* format = "%d", const char* format_max = NULL, MsTrGSliderFlags flags = 0);
    MsTrG_API bool          DragScalar(const char* label, MsTrGDataType data_type, void* p_data, float v_speed = 1.0f, const void* p_min = NULL, const void* p_max = NULL, const char* format = NULL, MsTrGSliderFlags flags = 0);
    MsTrG_API bool          DragScalarN(const char* label, MsTrGDataType data_type, void* p_data, int components, float v_speed = 1.0f, const void* p_min = NULL, const void* p_max = NULL, const char* format = NULL, MsTrGSliderFlags flags = 0);

    // Widgets: Regular Sliders
    // - CTRL+Click on any slider to turn them into an input box. Manually input values aren't clamped by default and can go off-bounds. Use MsTrGSliderFlags_AlwaysClamp to always clamp.
    // - Adjust format string to decorate the value with a prefix, a suffix, or adapt the editing and display precision e.g. "%.3f" -> 1.234; "%5.2f secs" -> 01.23 secs; "Biscuit: %.0f" -> Biscuit: 1; etc.
    // - Format string may also be set to NULL or use the default format ("%f" or "%d").
    // - Legacy: Pre-1.78 there are SliderXXX() function signatures that takes a final `float power=1.0f' argument instead of the `MsTrGSliderFlags flags=0' argument.
    //   If you get a warning converting a float to MsTrGSliderFlags, read https://github.com/ocornut/MsTrG/issues/3361
    MsTrG_API bool          SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", MsTrGSliderFlags flags = 0);     // adjust format to decorate the value with a prefix or a suffix for in-slider labels or unit display.
    MsTrG_API bool          SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format = "%.3f", MsTrGSliderFlags flags = 0);
    MsTrG_API bool          SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format = "%.3f", MsTrGSliderFlags flags = 0);
    MsTrG_API bool          SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format = "%.3f", MsTrGSliderFlags flags = 0);
    MsTrG_API bool          SliderAngle(const char* label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f, const char* format = "%.0f deg", MsTrGSliderFlags flags = 0);
    MsTrG_API bool          SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d", MsTrGSliderFlags flags = 0);
    MsTrG_API bool          SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format = "%d", MsTrGSliderFlags flags = 0);
    MsTrG_API bool          SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format = "%d", MsTrGSliderFlags flags = 0);
    MsTrG_API bool          SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format = "%d", MsTrGSliderFlags flags = 0);
    MsTrG_API bool          SliderScalar(const char* label, MsTrGDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = NULL, MsTrGSliderFlags flags = 0);
    MsTrG_API bool          SliderScalarN(const char* label, MsTrGDataType data_type, void* p_data, int components, const void* p_min, const void* p_max, const char* format = NULL, MsTrGSliderFlags flags = 0);
    MsTrG_API bool          VSliderFloat(const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* format = "%.3f", MsTrGSliderFlags flags = 0);
    MsTrG_API bool          VSliderInt(const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* format = "%d", MsTrGSliderFlags flags = 0);
    MsTrG_API bool          VSliderScalar(const char* label, const ImVec2& size, MsTrGDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = NULL, MsTrGSliderFlags flags = 0);

    // Widgets: Input with Keyboard
    // - If you want to use InputText() with std::string or any custom dynamic string type, see misc/cpp/MsTrG_stdlib.h and comments in MsTrG_demo.cpp.
    // - Most of the MsTrGInputTextFlags flags are only useful for InputText() and not for InputFloatX, InputIntX, InputDouble etc.
    MsTrG_API bool          InputText(const char* label, char* buf, size_t buf_size, MsTrGInputTextFlags flags = 0, MsTrGInputTextCallback callback = NULL, void* user_data = NULL);
    MsTrG_API bool          InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size = ImVec2(0, 0), MsTrGInputTextFlags flags = 0, MsTrGInputTextCallback callback = NULL, void* user_data = NULL);
    MsTrG_API bool          InputTextWithHint(const char* label, const char* hint, char* buf, size_t buf_size, MsTrGInputTextFlags flags = 0, MsTrGInputTextCallback callback = NULL, void* user_data = NULL);
    MsTrG_API bool          InputFloat(const char* label, float* v, float step = 0.0f, float step_fast = 0.0f, const char* format = "%.3f", MsTrGInputTextFlags flags = 0);
    MsTrG_API bool          InputFloat2(const char* label, float v[2], const char* format = "%.3f", MsTrGInputTextFlags flags = 0);
    MsTrG_API bool          InputFloat3(const char* label, float v[3], const char* format = "%.3f", MsTrGInputTextFlags flags = 0);
    MsTrG_API bool          InputFloat4(const char* label, float v[4], const char* format = "%.3f", MsTrGInputTextFlags flags = 0);
    MsTrG_API bool          InputInt(const char* label, int* v, int step = 1, int step_fast = 100, MsTrGInputTextFlags flags = 0);
    MsTrG_API bool          InputInt2(const char* label, int v[2], MsTrGInputTextFlags flags = 0);
    MsTrG_API bool          InputInt3(const char* label, int v[3], MsTrGInputTextFlags flags = 0);
    MsTrG_API bool          InputInt4(const char* label, int v[4], MsTrGInputTextFlags flags = 0);
    MsTrG_API bool          InputDouble(const char* label, double* v, double step = 0.0, double step_fast = 0.0, const char* format = "%.6f", MsTrGInputTextFlags flags = 0);
    MsTrG_API bool          InputScalar(const char* label, MsTrGDataType data_type, void* p_data, const void* p_step = NULL, const void* p_step_fast = NULL, const char* format = NULL, MsTrGInputTextFlags flags = 0);
    MsTrG_API bool          InputScalarN(const char* label, MsTrGDataType data_type, void* p_data, int components, const void* p_step = NULL, const void* p_step_fast = NULL, const char* format = NULL, MsTrGInputTextFlags flags = 0);

    // Widgets: Color Editor/Picker (tip: the ColorEdit* functions have a little color square that can be left-clicked to open a picker, and right-clicked to open an option menu.)
    // - Note that in C++ a 'float v[X]' function argument is the _same_ as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible.
    // - You can pass the address of a first float element out of a contiguous structure, e.g. &myvector.x
    MsTrG_API bool          ColorEdit3(const char* label, float col[3], MsTrGColorEditFlags flags = 0);
    MsTrG_API bool          ColorEdit4(const char* label, float col[4], MsTrGColorEditFlags flags = 0);
    MsTrG_API bool          ColorPicker3(const char* label, float col[3], MsTrGColorEditFlags flags = 0);
    MsTrG_API bool          ColorPicker4(const char* label, float col[4], MsTrGColorEditFlags flags = 0, const float* ref_col = NULL);
    MsTrG_API bool          ColorButton(const char* desc_id, const ImVec4& col, MsTrGColorEditFlags flags = 0, const ImVec2& size = ImVec2(0, 0)); // display a color square/button, hover for details, return true when pressed.
    MsTrG_API void          SetColorEditOptions(MsTrGColorEditFlags flags);                     // initialize current options (generally on application startup) if you want to select a default format, picker type, etc. User will be able to change many settings, unless you pass the _NoOptions flag to your calls.

    // Widgets: Trees
    // - TreeNode functions return true when the node is open, in which case you need to also call TreePop() when you are finished displaying the tree node contents.
    MsTrG_API bool          TreeNode(const char* label);
    MsTrG_API bool          TreeNode(const char* str_id, const char* fmt, ...) IM_FMTARGS(2);   // helper variation to easily decorelate the id from the displayed string. Read the FAQ about why and how to use ID. to align arbitrary text at the same level as a TreeNode() you can use Bullet().
    MsTrG_API bool          TreeNode(const void* ptr_id, const char* fmt, ...) IM_FMTARGS(2);   // "
    MsTrG_API bool          TreeNodeV(const char* str_id, const char* fmt, va_list args) IM_FMTLIST(2);
    MsTrG_API bool          TreeNodeV(const void* ptr_id, const char* fmt, va_list args) IM_FMTLIST(2);
    MsTrG_API bool          TreeNodeEx(const char* label, MsTrGTreeNodeFlags flags = 0);
    MsTrG_API bool          TreeNodeEx(const char* str_id, MsTrGTreeNodeFlags flags, const char* fmt, ...) IM_FMTARGS(3);
    MsTrG_API bool          TreeNodeEx(const void* ptr_id, MsTrGTreeNodeFlags flags, const char* fmt, ...) IM_FMTARGS(3);
    MsTrG_API bool          TreeNodeExV(const char* str_id, MsTrGTreeNodeFlags flags, const char* fmt, va_list args) IM_FMTLIST(3);
    MsTrG_API bool          TreeNodeExV(const void* ptr_id, MsTrGTreeNodeFlags flags, const char* fmt, va_list args) IM_FMTLIST(3);
    MsTrG_API void          TreePush(const char* str_id);                                       // ~ Indent()+PushId(). Already called by TreeNode() when returning true, but you can call TreePush/TreePop yourself if desired.
    MsTrG_API void          TreePush(const void* ptr_id = NULL);                                // "
    MsTrG_API void          TreePop();                                                          // ~ Unindent()+PopId()
    MsTrG_API float         GetTreeNodeToLabelSpacing();                                        // horizontal distance preceding label when using TreeNode*() or Bullet() == (g.FontSize + style.FramePadding.x*2) for a regular unframed TreeNode
    MsTrG_API bool          CollapsingHeader(const char* label, MsTrGTreeNodeFlags flags = 0);  // if returning 'true' the header is open. doesn't indent nor push on ID stack. user doesn't have to call TreePop().
    MsTrG_API bool          CollapsingHeader(const char* label, bool* p_visible, MsTrGTreeNodeFlags flags = 0); // when 'p_visible != NULL': if '*p_visible==true' display an additional small close button on upper right of the header which will set the bool to false when clicked, if '*p_visible==false' don't display the header.
    MsTrG_API void          SetNextItemOpen(bool is_open, MsTrGCond cond = 0);                  // set next TreeNode/CollapsingHeader open state.

    // Widgets: Selectables
    // - A selectable highlights when hovered, and can display another color when selected.
    // - Neighbors selectable extend their highlight bounds in order to leave no gap between them. This is so a series of selected Selectable appear contiguous.
    MsTrG_API bool          Selectable(const char* label, bool selected = false, MsTrGSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0)); // "bool selected" carry the selection state (read-only). Selectable() is clicked is returns true so you can modify your selection state. size.x==0.0: use remaining width, size.x>0.0: specify width. size.y==0.0: use label height, size.y>0.0: specify height
    MsTrG_API bool          Selectable(const char* label, bool* p_selected, MsTrGSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));      // "bool* p_selected" point to the selection state (read-write), as a convenient helper.

    // Widgets: List Boxes
    // - This is essentially a thin wrapper to using BeginChild/EndChild with some stylistic changes.
    // - The BeginListBox()/EndListBox() api allows you to manage your contents and selection state however you want it, by creating e.g. Selectable() or any items.
    // - The simplified/old ListBox() api are helpers over BeginListBox()/EndListBox() which are kept available for convenience purpose. This is analoguous to how Combos are created.
    // - Choose frame width:   size.x > 0.0f: custom  /  size.x < 0.0f or -FLT_MIN: right-align   /  size.x = 0.0f (default): use current ItemWidth
    // - Choose frame height:  size.y > 0.0f: custom  /  size.y < 0.0f or -FLT_MIN: bottom-align  /  size.y = 0.0f (default): arbitrary default height which can fit ~7 items
    MsTrG_API bool          BeginListBox(const char* label, const ImVec2& size = ImVec2(0, 0)); // open a framed scrolling region
    MsTrG_API void          EndListBox();                                                       // only call EndListBox() if BeginListBox() returned true!
    MsTrG_API bool          ListBox(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items = -1);
    MsTrG_API bool          ListBox(const char* label, int* current_item, bool (*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1);

    // Widgets: Data Plotting
    // - Consider using ImPlot (https://github.com/epezent/implot) which is much better!
    MsTrG_API void          PlotLines(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));
    MsTrG_API void          PlotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0));
    MsTrG_API void          PlotHistogram(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));
    MsTrG_API void          PlotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0));

    // Widgets: Value() Helpers.
    // - Those are merely shortcut to calling Text() with a format string. Output single value in "name: value" format (tip: freely declare more in your code to handle your types. you can add functions to the MsTrG namespace)
    MsTrG_API void          Value(const char* prefix, bool b);
    MsTrG_API void          Value(const char* prefix, int v);
    MsTrG_API void          Value(const char* prefix, unsigned int v);
    MsTrG_API void          Value(const char* prefix, float v, const char* float_format = NULL);

    // Widgets: Menus
    // - Use BeginMenuBar() on a window MsTrGWindowFlags_MenuBar to append to its menu bar.
    // - Use BeginMainMenuBar() to create a menu bar at the top of the screen and append to it.
    // - Use BeginMenu() to create a menu. You can call BeginMenu() multiple time with the same identifier to append more items to it.
    // - Not that MenuItem() keyboardshortcuts are displayed as a convenience but _not processed_ by Dear MsTrG at the moment.
    MsTrG_API bool          BeginMenuBar();                                                     // append to menu-bar of current window (requires MsTrGWindowFlags_MenuBar flag set on parent window).
    MsTrG_API void          EndMenuBar();                                                       // only call EndMenuBar() if BeginMenuBar() returns true!
    MsTrG_API bool          BeginMainMenuBar();                                                 // create and append to a full screen menu-bar.
    MsTrG_API void          EndMainMenuBar();                                                   // only call EndMainMenuBar() if BeginMainMenuBar() returns true!
    MsTrG_API bool          BeginMenu(const char* label, bool enabled = true);                  // create a sub-menu entry. only call EndMenu() if this returns true!
    MsTrG_API void          EndMenu();                                                          // only call EndMenu() if BeginMenu() returns true!
    MsTrG_API bool          MenuItem(const char* label, const char* shortcut = NULL, bool selected = false, bool enabled = true);  // return true when activated.
    MsTrG_API bool          MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled = true);              // return true when activated + toggle (*p_selected) if p_selected != NULL

    // Tooltips
    // - Tooltip are windows following the mouse. They do not take focus away.
    MsTrG_API void          BeginTooltip();                                                     // begin/append a tooltip window. to create full-featured tooltip (with any kind of items).
    MsTrG_API void          EndTooltip();
    MsTrG_API void          SetTooltip(const char* fmt, ...) IM_FMTARGS(1);                     // set a text-only tooltip, typically use with MsTrG::IsItemHovered(). override any previous call to SetTooltip().
    MsTrG_API void          SetTooltipV(const char* fmt, va_list args) IM_FMTLIST(1);

    // Popups, Modals
    //  - They block normal mouse hovering detection (and therefore most mouse interactions) behind them.
    //  - If not modal: they can be closed by clicking anywhere outside them, or by pressing ESCAPE.
    //  - Their visibility state (~bool) is held internally instead of being held by the programmer as we are used to with regular Begin*() calls.
    //  - The 3 properties above are related: we need to retain popup visibility state in the library because popups may be closed as any time.
    //  - You can bypass the hovering restriction by using MsTrGHoveredFlags_AllowWhenBlockedByPopup when calling IsItemHovered() or IsWindowHovered().
    //  - IMPORTANT: Popup identifiers are relative to the current ID stack, so OpenPopup and BeginPopup generally needs to be at the same level of the stack.
    //    This is sometimes leading to confusing mistakes. May rework this in the future.

    // Popups: begin/end functions
    //  - BeginPopup(): query popup state, if open start appending into the window. Call EndPopup() afterwards. MsTrGWindowFlags are forwarded to the window.
    //  - BeginPopupModal(): block every interactions behind the window, cannot be closed by user, add a dimming background, has a title bar.
    MsTrG_API bool          BeginPopup(const char* str_id, MsTrGWindowFlags flags = 0);                         // return true if the popup is open, and you can start outputting to it.
    MsTrG_API bool          BeginPopupModal(const char* name, bool* p_open = NULL, MsTrGWindowFlags flags = 0); // return true if the modal is open, and you can start outputting to it.
    MsTrG_API void          EndPopup();                                                                         // only call EndPopup() if BeginPopupXXX() returns true!

    // Popups: open/close functions
    //  - OpenPopup(): set popup state to open. MsTrGPopupFlags are available for opening options.
    //  - If not modal: they can be closed by clicking anywhere outside them, or by pressing ESCAPE.
    //  - CloseCurrentPopup(): use inside the BeginPopup()/EndPopup() scope to close manually.
    //  - CloseCurrentPopup() is called by default by Selectable()/MenuItem() when activated (FIXME: need some options).
    //  - Use MsTrGPopupFlags_NoOpenOverExistingPopup to avoid opening a popup if there's already one at the same level. This is equivalent to e.g. testing for !IsAnyPopupOpen() prior to OpenPopup().
    //  - Use IsWindowAppearing() after BeginPopup() to tell if a window just opened.
    //  - IMPORTANT: Notice that for OpenPopupOnItemClick() we exceptionally default flags to 1 (== MsTrGPopupFlags_MouseButtonRight) for backward compatibility with older API taking 'int mouse_button = 1' parameter
    MsTrG_API void          OpenPopup(const char* str_id, MsTrGPopupFlags popup_flags = 0);                     // call to mark popup as open (don't call every frame!).
    MsTrG_API void          OpenPopup(MsTrGID id, MsTrGPopupFlags popup_flags = 0);                             // id overload to facilitate calling from nested stacks
    MsTrG_API void          OpenPopupOnItemClick(const char* str_id = NULL, MsTrGPopupFlags popup_flags = 1);   // helper to open popup when clicked on last item. Default to MsTrGPopupFlags_MouseButtonRight == 1. (note: actually triggers on the mouse _released_ event to be consistent with popup behaviors)
    MsTrG_API void          CloseCurrentPopup();                                                                // manually close the popup we have begin-ed into.

    // Popups: open+begin combined functions helpers
    //  - Helpers to do OpenPopup+BeginPopup where the Open action is triggered by e.g. hovering an item and right-clicking.
    //  - They are convenient to easily create context menus, hence the name.
    //  - IMPORTANT: Notice that BeginPopupContextXXX takes MsTrGPopupFlags just like OpenPopup() and unlike BeginPopup(). For full consistency, we may add MsTrGWindowFlags to the BeginPopupContextXXX functions in the future.
    //  - IMPORTANT: Notice that we exceptionally default their flags to 1 (== MsTrGPopupFlags_MouseButtonRight) for backward compatibility with older API taking 'int mouse_button = 1' parameter, so if you add other flags remember to re-add the MsTrGPopupFlags_MouseButtonRight.
    MsTrG_API bool          BeginPopupContextItem(const char* str_id = NULL, MsTrGPopupFlags popup_flags = 1);  // open+begin popup when clicked on last item. Use str_id==NULL to associate the popup to previous item. If you want to use that on a non-interactive item such as Text() you need to pass in an explicit ID here. read comments in .cpp!
    MsTrG_API bool          BeginPopupContextWindow(const char* str_id = NULL, MsTrGPopupFlags popup_flags = 1);// open+begin popup when clicked on current window.
    MsTrG_API bool          BeginPopupContextVoid(const char* str_id = NULL, MsTrGPopupFlags popup_flags = 1);  // open+begin popup when clicked in void (where there are no windows).

    // Popups: query functions
    //  - IsPopupOpen(): return true if the popup is open at the current BeginPopup() level of the popup stack.
    //  - IsPopupOpen() with MsTrGPopupFlags_AnyPopupId: return true if any popup is open at the current BeginPopup() level of the popup stack.
    //  - IsPopupOpen() with MsTrGPopupFlags_AnyPopupId + MsTrGPopupFlags_AnyPopupLevel: return true if any popup is open.
    MsTrG_API bool          IsPopupOpen(const char* str_id, MsTrGPopupFlags flags = 0);                         // return true if the popup is open.

    // Tables
    // - Full-featured replacement for old Columns API.
    // - See Demo->Tables for demo code. See top of MsTrG_tables.cpp for general commentary.
    // - See MsTrGTableFlags_ and MsTrGTableColumnFlags_ enums for a description of available flags.
    // The typical call flow is:
    // - 1. Call BeginTable(), early out if returning false.
    // - 2. Optionally call TableSetupColumn() to submit column name/flags/defaults.
    // - 3. Optionally call TableSetupScrollFreeze() to request scroll freezing of columns/rows.
    // - 4. Optionally call TableHeadersRow() to submit a header row. Names are pulled from TableSetupColumn() data.
    // - 5. Populate contents:
    //    - In most situations you can use TableNextRow() + TableSetColumnIndex(N) to start appending into a column.
    //    - If you are using tables as a sort of grid, where every columns is holding the same type of contents,
    //      you may prefer using TableNextColumn() instead of TableNextRow() + TableSetColumnIndex().
    //      TableNextColumn() will automatically wrap-around into the next row if needed.
    //    - IMPORTANT: Comparatively to the old Columns() API, we need to call TableNextColumn() for the first column!
    //    - Summary of possible call flow:
    //        --------------------------------------------------------------------------------------------------------
    //        TableNextRow() -> TableSetColumnIndex(0) -> Text("Hello 0") -> TableSetColumnIndex(1) -> Text("Hello 1")  // OK
    //        TableNextRow() -> TableNextColumn()      -> Text("Hello 0") -> TableNextColumn()      -> Text("Hello 1")  // OK
    //                          TableNextColumn()      -> Text("Hello 0") -> TableNextColumn()      -> Text("Hello 1")  // OK: TableNextColumn() automatically gets to next row!
    //        TableNextRow()                           -> Text("Hello 0")                                               // Not OK! Missing TableSetColumnIndex() or TableNextColumn()! Text will not appear!
    //        --------------------------------------------------------------------------------------------------------
    // - 5. Call EndTable()
    MsTrG_API bool          BeginTable(const char* str_id, int column, MsTrGTableFlags flags = 0, const ImVec2& outer_size = ImVec2(0.0f, 0.0f), float inner_width = 0.0f);
    MsTrG_API void          EndTable();                                         // only call EndTable() if BeginTable() returns true!
    MsTrG_API void          TableNextRow(MsTrGTableRowFlags row_flags = 0, float min_row_height = 0.0f); // append into the first cell of a new row.
    MsTrG_API bool          TableNextColumn();                                  // append into the next column (or first column of next row if currently in last column). Return true when column is visible.
    MsTrG_API bool          TableSetColumnIndex(int column_n);                  // append into the specified column. Return true when column is visible.

    // Tables: Headers & Columns declaration
    // - Use TableSetupColumn() to specify label, resizing policy, default width/weight, id, various other flags etc.
    // - Use TableHeadersRow() to create a header row and automatically submit a TableHeader() for each column.
    //   Headers are required to perform: reordering, sorting, and opening the context menu.
    //   The context menu can also be made available in columns body using MsTrGTableFlags_ContextMenuInBody.
    // - You may manually submit headers using TableNextRow() + TableHeader() calls, but this is only useful in
    //   some advanced use cases (e.g. adding custom widgets in header row).
    // - Use TableSetupScrollFreeze() to lock columns/rows so they stay visible when scrolled.
    MsTrG_API void          TableSetupColumn(const char* label, MsTrGTableColumnFlags flags = 0, float init_width_or_weight = 0.0f, MsTrGID user_id = 0);
    MsTrG_API void          TableSetupScrollFreeze(int cols, int rows);         // lock columns/rows so they stay visible when scrolled.
    MsTrG_API void          TableHeadersRow();                                  // submit all headers cells based on data provided to TableSetupColumn() + submit context menu
    MsTrG_API void          TableHeader(const char* label);                     // submit one header cell manually (rarely used)

    // Tables: Sorting & Miscellaneous functions
    // - Sorting: call TableGetSortSpecs() to retrieve latest sort specs for the table. NULL when not sorting.
    //   When 'sort_specs->SpecsDirty == true' you should sort your data. It will be true when sorting specs have
    //   changed since last call, or the first time. Make sure to set 'SpecsDirty = false' after sorting,
    //   else you may wastefully sort your data every frame!
    // - Functions args 'int column_n' treat the default value of -1 as the same as passing the current column index.
    MsTrG_API MsTrGTableSortSpecs*  TableGetSortSpecs();                        // get latest sort specs for the table (NULL if not sorting).  Lifetime: don't hold on this pointer over multiple frames or past any subsequent call to BeginTable().
    MsTrG_API int                   TableGetColumnCount();                      // return number of columns (value passed to BeginTable)
    MsTrG_API int                   TableGetColumnIndex();                      // return current column index.
    MsTrG_API int                   TableGetRowIndex();                         // return current row index.
    MsTrG_API const char*           TableGetColumnName(int column_n = -1);      // return "" if column didn't have a name declared by TableSetupColumn(). Pass -1 to use current column.
    MsTrG_API MsTrGTableColumnFlags TableGetColumnFlags(int column_n = -1);     // return column flags so you can query their Enabled/Visible/Sorted/Hovered status flags. Pass -1 to use current column.
    MsTrG_API void                  TableSetColumnEnabled(int column_n, bool v);// change user accessible enabled/disabled state of a column. Set to false to hide the column. User can use the context menu to change this themselves (right-click in headers, or right-click in columns body with MsTrGTableFlags_ContextMenuInBody)
    MsTrG_API void                  TableSetBgColor(MsTrGTableBgTarget target, ImU32 color, int column_n = -1);  // change the color of a cell, row, or column. See MsTrGTableBgTarget_ flags for details.

    // Legacy Columns API (prefer using Tables!)
    // - You can also use SameLine(pos_x) to mimic simplified columns.
    MsTrG_API void          Columns(int count = 1, const char* id = NULL, bool border = true);
    MsTrG_API void          NextColumn();                                                       // next column, defaults to current row or next row if the current row is finished
    MsTrG_API int           GetColumnIndex();                                                   // get current column index
    MsTrG_API float         GetColumnWidth(int column_index = -1);                              // get column width (in pixels). pass -1 to use current column
    MsTrG_API void          SetColumnWidth(int column_index, float width);                      // set column width (in pixels). pass -1 to use current column
    MsTrG_API float         GetColumnOffset(int column_index = -1);                             // get position of column line (in pixels, from the left side of the contents region). pass -1 to use current column, otherwise 0..GetColumnsCount() inclusive. column 0 is typically 0.0f
    MsTrG_API void          SetColumnOffset(int column_index, float offset_x);                  // set position of column line (in pixels, from the left side of the contents region). pass -1 to use current column
    MsTrG_API int           GetColumnsCount();

    // Tab Bars, Tabs
    MsTrG_API bool          BeginTabBar(const char* str_id, MsTrGTabBarFlags flags = 0);        // create and append into a TabBar
    MsTrG_API void          EndTabBar();                                                        // only call EndTabBar() if BeginTabBar() returns true!
    MsTrG_API bool          BeginTabItem(const char* label, bool* p_open = NULL, MsTrGTabItemFlags flags = 0); // create a Tab. Returns true if the Tab is selected.
    MsTrG_API void          EndTabItem();                                                       // only call EndTabItem() if BeginTabItem() returns true!
    MsTrG_API bool          TabItemButton(const char* label, MsTrGTabItemFlags flags = 0);      // create a Tab behaving like a button. return true when clicked. cannot be selected in the tab bar.
    MsTrG_API void          SetTabItemClosed(const char* tab_or_docked_window_label);           // notify TabBar or Docking system of a closed tab/window ahead (useful to reduce visual flicker on reorderable tab bars). For tab-bar: call after BeginTabBar() and before Tab submissions. Otherwise call with a window name.

    // Logging/Capture
    // - All text output from the interface can be captured into tty/file/clipboard. By default, tree nodes are automatically opened during logging.
    MsTrG_API void          LogToTTY(int auto_open_depth = -1);                                 // start logging to tty (stdout)
    MsTrG_API void          LogToFile(int auto_open_depth = -1, const char* filename = NULL);   // start logging to file
    MsTrG_API void          LogToClipboard(int auto_open_depth = -1);                           // start logging to OS clipboard
    MsTrG_API void          LogFinish();                                                        // stop logging (close file, etc.)
    MsTrG_API void          LogButtons();                                                       // helper to display buttons for logging to tty/file/clipboard
    MsTrG_API void          LogText(const char* fmt, ...) IM_FMTARGS(1);                        // pass text data straight to log (without being displayed)
    MsTrG_API void          LogTextV(const char* fmt, va_list args) IM_FMTLIST(1);

    // Drag and Drop
    // - On source items, call BeginDragDropSource(), if it returns true also call SetDragDropPayload() + EndDragDropSource().
    // - On target candidates, call BeginDragDropTarget(), if it returns true also call AcceptDragDropPayload() + EndDragDropTarget().
    // - If you stop calling BeginDragDropSource() the payload is preserved however it won't have a preview tooltip (we currently display a fallback "..." tooltip, see #1725)
    // - An item can be both drag source and drop target.
    MsTrG_API bool          BeginDragDropSource(MsTrGDragDropFlags flags = 0);                                      // call after submitting an item which may be dragged. when this return true, you can call SetDragDropPayload() + EndDragDropSource()
    MsTrG_API bool          SetDragDropPayload(const char* type, const void* data, size_t sz, MsTrGCond cond = 0);  // type is a user defined string of maximum 32 characters. Strings starting with '_' are reserved for dear MsTrG internal types. Data is copied and held by MsTrG. Return true when payload has been accepted.
    MsTrG_API void          EndDragDropSource();                                                                    // only call EndDragDropSource() if BeginDragDropSource() returns true!
    MsTrG_API bool                  BeginDragDropTarget();                                                          // call after submitting an item that may receive a payload. If this returns true, you can call AcceptDragDropPayload() + EndDragDropTarget()
    MsTrG_API const MsTrGPayload*   AcceptDragDropPayload(const char* type, MsTrGDragDropFlags flags = 0);          // accept contents of a given type. If MsTrGDragDropFlags_AcceptBeforeDelivery is set you can peek into the payload before the mouse button is released.
    MsTrG_API void                  EndDragDropTarget();                                                            // only call EndDragDropTarget() if BeginDragDropTarget() returns true!
    MsTrG_API const MsTrGPayload*   GetDragDropPayload();                                                           // peek directly into the current payload from anywhere. may return NULL. use MsTrGPayload::IsDataType() to test for the payload type.

    // Disabling [BETA API]
    // - Disable all user interactions and dim items visuals (applying style.DisabledAlpha over current colors)
    // - Those can be nested but it cannot be used to enable an already disabled section (a single BeginDisabled(true) in the stack is enough to keep everything disabled)
    // - BeginDisabled(false) essentially does nothing useful but is provided to facilitate use of boolean expressions. If you can avoid calling BeginDisabled(False)/EndDisabled() best to avoid it.
    MsTrG_API void          BeginDisabled(bool disabled = true);
    MsTrG_API void          EndDisabled();

    // Clipping
    // - Mouse hovering is affected by MsTrG::PushClipRect() calls, unlike direct calls to ImDrawList::PushClipRect() which are render only.
    MsTrG_API void          PushClipRect(const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect);
    MsTrG_API void          PopClipRect();

    // Focus, Activation
    // - Prefer using "SetItemDefaultFocus()" over "if (IsWindowAppearing()) SetScrollHereY()" when applicable to signify "this is the default item"
    MsTrG_API void          SetItemDefaultFocus();                                              // make last item the default focused item of a window.
    MsTrG_API void          SetKeyboardFocusHere(int offset = 0);                               // focus keyboard on the next widget. Use positive 'offset' to access sub components of a multiple component widget. Use -1 to access previous widget.

    // Item/Widgets Utilities and Query Functions
    // - Most of the functions are referring to the previous Item that has been submitted.
    // - See Demo Window under "Widgets->Querying Status" for an interactive visualization of most of those functions.
    MsTrG_API bool          IsItemHovered(MsTrGHoveredFlags flags = 0);                         // is the last item hovered? (and usable, aka not blocked by a popup, etc.). See MsTrGHoveredFlags for more options.
    MsTrG_API bool          IsItemActive();                                                     // is the last item active? (e.g. button being held, text field being edited. This will continuously return true while holding mouse button on an item. Items that don't interact will always return false)
    MsTrG_API bool          IsItemFocused();                                                    // is the last item focused for keyboard/gamepad navigation?
    MsTrG_API bool          IsItemClicked(MsTrGMouseButton mouse_button = 0);                   // is the last item hovered and mouse clicked on? (**)  == IsMouseClicked(mouse_button) && IsItemHovered()Important. (**) this it NOT equivalent to the behavior of e.g. Button(). Read comments in function definition.
    MsTrG_API bool          IsItemVisible();                                                    // is the last item visible? (items may be out of sight because of clipping/scrolling)
    MsTrG_API bool          IsItemEdited();                                                     // did the last item modify its underlying value this frame? or was pressed? This is generally the same as the "bool" return value of many widgets.
    MsTrG_API bool          IsItemActivated();                                                  // was the last item just made active (item was previously inactive).
    MsTrG_API bool          IsItemDeactivated();                                                // was the last item just made inactive (item was previously active). Useful for Undo/Redo patterns with widgets that requires continuous editing.
    MsTrG_API bool          IsItemDeactivatedAfterEdit();                                       // was the last item just made inactive and made a value change when it was active? (e.g. Slider/Drag moved). Useful for Undo/Redo patterns with widgets that requires continuous editing. Note that you may get false positives (some widgets such as Combo()/ListBox()/Selectable() will return true even when clicking an already selected item).
    MsTrG_API bool          IsItemToggledOpen();                                                // was the last item open state toggled? set by TreeNode().
    MsTrG_API bool          IsAnyItemHovered();                                                 // is any item hovered?
    MsTrG_API bool          IsAnyItemActive();                                                  // is any item active?
    MsTrG_API bool          IsAnyItemFocused();                                                 // is any item focused?
    MsTrG_API ImVec2        GetItemRectMin();                                                   // get upper-left bounding rectangle of the last item (screen space)
    MsTrG_API ImVec2        GetItemRectMax();                                                   // get lower-right bounding rectangle of the last item (screen space)
    MsTrG_API ImVec2        GetItemRectSize();                                                  // get size of last item
    MsTrG_API void          SetItemAllowOverlap();                                              // allow last item to be overlapped by a subsequent item. sometimes useful with invisible buttons, selectables, etc. to catch unused area.

    // Viewports
    // - Currently represents the Platform Window created by the application which is hosting our Dear MsTrG windows.
    // - In 'docking' branch with multi-viewport enabled, we extend this concept to have multiple active viewports.
    // - In the future we will extend this concept further to also represent Platform Monitor and support a "no main platform window" operation mode.
    MsTrG_API MsTrGViewport* GetMainViewport();                                                 // return primary/default viewport. This can never be NULL.

    // Background/Foreground Draw Lists
    MsTrG_API ImDrawList*   GetBackgroundDrawList();                                            // this draw list will be the first rendered one. Useful to quickly draw shapes/text behind dear MsTrG contents.
    MsTrG_API ImDrawList*   GetForegroundDrawList();                                            // this draw list will be the last rendered one. Useful to quickly draw shapes/text over dear MsTrG contents.

    // Miscellaneous Utilities
    MsTrG_API bool          IsRectVisible(const ImVec2& size);                                  // test if rectangle (of given size, starting from cursor position) is visible / not clipped.
    MsTrG_API bool          IsRectVisible(const ImVec2& rect_min, const ImVec2& rect_max);      // test if rectangle (in screen space) is visible / not clipped. to perform coarse clipping on user's side.
    MsTrG_API double        GetTime();                                                          // get global MsTrG time. incremented by io.DeltaTime every frame.
    MsTrG_API int           GetFrameCount();                                                    // get global MsTrG frame count. incremented by 1 every frame.
    MsTrG_API ImDrawListSharedData* GetDrawListSharedData();                                    // you may use this when creating your own ImDrawList instances.
    MsTrG_API const char*   GetStyleColorName(MsTrGCol idx);                                    // get a string corresponding to the enum value (for display, saving, etc.).
    MsTrG_API void          SetStateStorage(MsTrGStorage* storage);                             // replace current window storage with our own (if you want to manipulate it yourself, typically clear subsection of it)
    MsTrG_API MsTrGStorage* GetStateStorage();
    MsTrG_API bool          BeginChildFrame(MsTrGID id, const ImVec2& size, MsTrGWindowFlags flags = 0); // helper to create a child window / scrolling region that looks like a normal widget frame
    MsTrG_API void          EndChildFrame();                                                    // always call EndChildFrame() regardless of BeginChildFrame() return values (which indicates a collapsed/clipped window)

    // Text Utilities
    MsTrG_API ImVec2        CalcTextSize(const char* text, const char* text_end = NULL, bool hide_text_after_double_hash = false, float wrap_width = -1.0f);

    // Color Utilities
    MsTrG_API ImVec4        ColorConvertU32ToFloat4(ImU32 in);
    MsTrG_API ImU32         ColorConvertFloat4ToU32(const ImVec4& in);
    MsTrG_API void          ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v);
    MsTrG_API void          ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b);

    // Inputs Utilities: Keyboard
    // Without MsTrG_DISABLE_OBSOLETE_KEYIO: (legacy support)
    //   - For 'MsTrGKey key' you can still use your legacy native/user indices according to how your backend/engine stored them in io.KeysDown[].
    // With MsTrG_DISABLE_OBSOLETE_KEYIO: (this is the way forward)
    //   - Any use of 'MsTrGKey' will assert when key < 512 will be passed, previously reserved as native/user keys indices
    //   - GetKeyIndex() is pass-through and therefore deprecated (gone if MsTrG_DISABLE_OBSOLETE_KEYIO is defined)
    MsTrG_API bool          IsKeyDown(MsTrGKey key);                                            // is key being held.
    MsTrG_API bool          IsKeyPressed(MsTrGKey key, bool repeat = true);                     // was key pressed (went from !Down to Down)? if repeat=true, uses io.KeyRepeatDelay / KeyRepeatRate
    MsTrG_API bool          IsKeyReleased(MsTrGKey key);                                        // was key released (went from Down to !Down)?
    MsTrG_API int           GetKeyPressedAmount(MsTrGKey key, float repeat_delay, float rate);  // uses provided repeat rate/delay. return a count, most often 0 or 1 but might be >1 if RepeatRate is small enough that DeltaTime > RepeatRate
    MsTrG_API const char*   GetKeyName(MsTrGKey key);                                           // [DEBUG] returns English name of the key. Those names a provided for debugging purpose and are not meant to be saved persistently not compared.
    MsTrG_API void          SetNextFrameWantCaptureKeyboard(bool want_capture_keyboard);        // Override io.WantCaptureKeyboard flag next frame (said flag is left for your application to handle, typically when true it instructs your app to ignore inputs). e.g. force capture keyboard when your widget is being hovered. This is equivalent to setting "io.WantCaptureKeyboard = want_capture_keyboard"; after the next NewFrame() call.

    // Inputs Utilities: Mouse
    // - To refer to a mouse button, you may use named enums in your code e.g. MsTrGMouseButton_Left, MsTrGMouseButton_Right.
    // - You can also use regular integer: it is forever guaranteed that 0=Left, 1=Right, 2=Middle.
    // - Dragging operations are only reported after mouse has moved a certain distance away from the initial clicking position (see 'lock_threshold' and 'io.MouseDraggingThreshold')
    MsTrG_API bool          IsMouseDown(MsTrGMouseButton button);                               // is mouse button held?
    MsTrG_API bool          IsMouseClicked(MsTrGMouseButton button, bool repeat = false);       // did mouse button clicked? (went from !Down to Down). Same as GetMouseClickedCount() == 1.
    MsTrG_API bool          IsMouseReleased(MsTrGMouseButton button);                           // did mouse button released? (went from Down to !Down)
    MsTrG_API bool          IsMouseDoubleClicked(MsTrGMouseButton button);                      // did mouse button double-clicked? Same as GetMouseClickedCount() == 2. (note that a double-click will also report IsMouseClicked() == true)
    MsTrG_API int           GetMouseClickedCount(MsTrGMouseButton button);                      // return the number of successive mouse-clicks at the time where a click happen (otherwise 0).
    MsTrG_API bool          IsMouseHoveringRect(const ImVec2& r_min, const ImVec2& r_max, bool clip = true);// is mouse hovering given bounding rect (in screen space). clipped by current clipping settings, but disregarding of other consideration of focus/window ordering/popup-block.
    MsTrG_API bool          IsMousePosValid(const ImVec2* mouse_pos = NULL);                    // by convention we use (-FLT_MAX,-FLT_MAX) to denote that there is no mouse available
    MsTrG_API bool          IsAnyMouseDown();                                                   // [WILL OBSOLETE] is any mouse button held? This was designed for backends, but prefer having backend maintain a mask of held mouse buttons, because upcoming input queue system will make this invalid.
    MsTrG_API ImVec2        GetMousePos();                                                      // shortcut to MsTrG::GetIO().MousePos provided by user, to be consistent with other calls
    MsTrG_API ImVec2        GetMousePosOnOpeningCurrentPopup();                                 // retrieve mouse position at the time of opening popup we have BeginPopup() into (helper to avoid user backing that value themselves)
    MsTrG_API bool          IsMouseDragging(MsTrGMouseButton button, float lock_threshold = -1.0f);         // is mouse dragging? (if lock_threshold < -1.0f, uses io.MouseDraggingThreshold)
    MsTrG_API ImVec2        GetMouseDragDelta(MsTrGMouseButton button = 0, float lock_threshold = -1.0f);   // return the delta from the initial clicking position while the mouse button is pressed or was just released. This is locked and return 0.0f until the mouse moves past a distance threshold at least once (if lock_threshold < -1.0f, uses io.MouseDraggingThreshold)
    MsTrG_API void          ResetMouseDragDelta(MsTrGMouseButton button = 0);                   //
    MsTrG_API MsTrGMouseCursor GetMouseCursor();                                                // get desired cursor type, reset in MsTrG::NewFrame(), this is updated during the frame. valid before Render(). If you use software rendering by setting io.MouseDrawCursor MsTrG will render those for you
    MsTrG_API void          SetMouseCursor(MsTrGMouseCursor cursor_type);                       // set desired cursor type
    MsTrG_API void          SetNextFrameWantCaptureMouse(bool want_capture_mouse);              // Override io.WantCaptureMouse flag next frame (said flag is left for your application to handle, typical when true it instucts your app to ignore inputs). This is equivalent to setting "io.WantCaptureMouse = want_capture_mouse;" after the next NewFrame() call.

    // Clipboard Utilities
    // - Also see the LogToClipboard() function to capture GUI into clipboard, or easily output text data to the clipboard.
    MsTrG_API const char*   GetClipboardText();
    MsTrG_API void          SetClipboardText(const char* text);

    // Settings/.Ini Utilities
    // - The disk functions are automatically called if io.IniFilename != NULL (default is "MsTrG.ini").
    // - Set io.IniFilename to NULL to load/save manually. Read io.WantSaveIniSettings description about handling .ini saving manually.
    // - Important: default value "MsTrG.ini" is relative to current working dir! Most apps will want to lock this to an absolute path (e.g. same path as executables).
    MsTrG_API void          LoadIniSettingsFromDisk(const char* ini_filename);                  // call after CreateContext() and before the first call to NewFrame(). NewFrame() automatically calls LoadIniSettingsFromDisk(io.IniFilename).
    MsTrG_API void          LoadIniSettingsFromMemory(const char* ini_data, size_t ini_size=0); // call after CreateContext() and before the first call to NewFrame() to provide .ini data from your own data source.
    MsTrG_API void          SaveIniSettingsToDisk(const char* ini_filename);                    // this is automatically called (if io.IniFilename is not empty) a few seconds after any modification that should be reflected in the .ini file (and also by DestroyContext).
    MsTrG_API const char*   SaveIniSettingsToMemory(size_t* out_ini_size = NULL);               // return a zero-terminated string with the .ini data which you can save by your own mean. call when io.WantSaveIniSettings is set, then save data by your own mean and clear io.WantSaveIniSettings.

    // Debug Utilities
    MsTrG_API void          DebugTextEncoding(const char* text);
    MsTrG_API bool          DebugCheckVersionAndDataLayout(const char* version_str, size_t sz_io, size_t sz_style, size_t sz_vec2, size_t sz_vec4, size_t sz_drawvert, size_t sz_drawidx); // This is called by MsTrG_CHECKVERSION() macro.

    // Memory Allocators
    // - Those functions are not reliant on the current context.
    // - DLL users: heaps and globals are not shared across DLL boundaries! You will need to call SetCurrentContext() + SetAllocatorFunctions()
    //   for each static/DLL boundary you are calling from. Read "Context and Memory Allocators" section of MsTrG.cpp for more details.
    MsTrG_API void          SetAllocatorFunctions(MsTrGMemAllocFunc alloc_func, MsTrGMemFreeFunc free_func, void* user_data = NULL);
    MsTrG_API void          GetAllocatorFunctions(MsTrGMemAllocFunc* p_alloc_func, MsTrGMemFreeFunc* p_free_func, void** p_user_data);
    MsTrG_API void*         MemAlloc(size_t size);
    MsTrG_API void          MemFree(void* ptr);

} // namespace MsTrG

//-----------------------------------------------------------------------------
// [SECTION] Flags & Enumerations
//-----------------------------------------------------------------------------

// Flags for MsTrG::Begin()
enum MsTrGWindowFlags_
{
    MsTrGWindowFlags_None                   = 0,
    MsTrGWindowFlags_NoTitleBar             = 1 << 0,   // Disable title-bar
    MsTrGWindowFlags_NoResize               = 1 << 1,   // Disable user resizing with the lower-right grip
    MsTrGWindowFlags_NoMove                 = 1 << 2,   // Disable user moving the window
    MsTrGWindowFlags_NoScrollbar            = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
    MsTrGWindowFlags_NoScrollWithMouse      = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
    MsTrGWindowFlags_NoCollapse             = 1 << 5,   // Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
    MsTrGWindowFlags_AlwaysAutoResize       = 1 << 6,   // Resize every window to its content every frame
    MsTrGWindowFlags_NoBackground           = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
    MsTrGWindowFlags_NoSavedSettings        = 1 << 8,   // Never load/save settings in .ini file
    MsTrGWindowFlags_NoMouseInputs          = 1 << 9,   // Disable catching mouse, hovering test with pass through.
    MsTrGWindowFlags_MenuBar                = 1 << 10,  // Has a menu-bar
    MsTrGWindowFlags_HorizontalScrollbar    = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in MsTrG_demo in the "Horizontal Scrolling" section.
    MsTrGWindowFlags_NoFocusOnAppearing     = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
    MsTrGWindowFlags_NoBringToFrontOnFocus  = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
    MsTrGWindowFlags_AlwaysVerticalScrollbar= 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
    MsTrGWindowFlags_AlwaysHorizontalScrollbar=1<< 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
    MsTrGWindowFlags_AlwaysUseWindowPadding = 1 << 16,  // Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
    MsTrGWindowFlags_NoNavInputs            = 1 << 18,  // No gamepad/keyboard navigation within the window
    MsTrGWindowFlags_NoNavFocus             = 1 << 19,  // No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
    MsTrGWindowFlags_UnsavedDocument        = 1 << 20,  // Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
    MsTrGWindowFlags_NoNav                  = MsTrGWindowFlags_NoNavInputs | MsTrGWindowFlags_NoNavFocus,
    MsTrGWindowFlags_NoDecoration           = MsTrGWindowFlags_NoTitleBar | MsTrGWindowFlags_NoResize | MsTrGWindowFlags_NoScrollbar | MsTrGWindowFlags_NoCollapse,
    MsTrGWindowFlags_NoInputs               = MsTrGWindowFlags_NoMouseInputs | MsTrGWindowFlags_NoNavInputs | MsTrGWindowFlags_NoNavFocus,

    // [Internal]
    MsTrGWindowFlags_NavFlattened           = 1 << 23,  // [BETA] On child window: allow gamepad/keyboard navigation to cross over parent border to this child or between sibling child windows.
    MsTrGWindowFlags_ChildWindow            = 1 << 24,  // Don't use! For internal use by BeginChild()
    MsTrGWindowFlags_Tooltip                = 1 << 25,  // Don't use! For internal use by BeginTooltip()
    MsTrGWindowFlags_Popup                  = 1 << 26,  // Don't use! For internal use by BeginPopup()
    MsTrGWindowFlags_Modal                  = 1 << 27,  // Don't use! For internal use by BeginPopupModal()
    MsTrGWindowFlags_ChildMenu              = 1 << 28   // Don't use! For internal use by BeginMenu()
    //MsTrGWindowFlags_ResizeFromAnySide    = 1 << 17,  // [Obsolete] --> Set io.ConfigWindowsResizeFromEdges=true and make sure mouse cursors are supported by backend (io.BackendFlags & MsTrGBackendFlags_HasMouseCursors)
};

// Flags for MsTrG::InputText()
enum MsTrGInputTextFlags_
{
    MsTrGInputTextFlags_None                = 0,
    MsTrGInputTextFlags_CharsDecimal        = 1 << 0,   // Allow 0123456789.+-*/
    MsTrGInputTextFlags_CharsHexadecimal    = 1 << 1,   // Allow 0123456789ABCDEFabcdef
    MsTrGInputTextFlags_CharsUppercase      = 1 << 2,   // Turn a..z into A..Z
    MsTrGInputTextFlags_CharsNoBlank        = 1 << 3,   // Filter out spaces, tabs
    MsTrGInputTextFlags_AutoSelectAll       = 1 << 4,   // Select entire text when first taking mouse focus
    MsTrGInputTextFlags_EnterReturnsTrue    = 1 << 5,   // Return 'true' when Enter is pressed (as opposed to every time the value was modified). Consider looking at the IsItemDeactivatedAfterEdit() function.
    MsTrGInputTextFlags_CallbackCompletion  = 1 << 6,   // Callback on pressing TAB (for completion handling)
    MsTrGInputTextFlags_CallbackHistory     = 1 << 7,   // Callback on pressing Up/Down arrows (for history handling)
    MsTrGInputTextFlags_CallbackAlways      = 1 << 8,   // Callback on each iteration. User code may query cursor position, modify text buffer.
    MsTrGInputTextFlags_CallbackCharFilter  = 1 << 9,   // Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
    MsTrGInputTextFlags_AllowTabInput       = 1 << 10,  // Pressing TAB input a '\t' character into the text field
    MsTrGInputTextFlags_CtrlEnterForNewLine = 1 << 11,  // In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter (default is opposite: unfocus with Ctrl+Enter, add line with Enter).
    MsTrGInputTextFlags_NoHorizontalScroll  = 1 << 12,  // Disable following the cursor horizontally
    MsTrGInputTextFlags_AlwaysOverwrite     = 1 << 13,  // Overwrite mode
    MsTrGInputTextFlags_ReadOnly            = 1 << 14,  // Read-only mode
    MsTrGInputTextFlags_Password            = 1 << 15,  // Password mode, display all characters as '*'
    MsTrGInputTextFlags_NoUndoRedo          = 1 << 16,  // Disable undo/redo. Note that input text owns the text data while active, if you want to provide your own undo/redo stack you need e.g. to call ClearActiveID().
    MsTrGInputTextFlags_CharsScientific     = 1 << 17,  // Allow 0123456789.+-*/eE (Scientific notation input)
    MsTrGInputTextFlags_CallbackResize      = 1 << 18,  // Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow. Notify when the string wants to be resized (for string types which hold a cache of their Size). You will be provided a new BufSize in the callback and NEED to honor it. (see misc/cpp/MsTrG_stdlib.h for an example of using this)
    MsTrGInputTextFlags_CallbackEdit        = 1 << 19   // Callback on any edit (note that InputText() already returns true on edit, the callback is useful mainly to manipulate the underlying buffer while focus is active)

    // Obsolete names (will be removed soon)
#ifndef MsTrG_DISABLE_OBSOLETE_FUNCTIONS
    , MsTrGInputTextFlags_AlwaysInsertMode    = MsTrGInputTextFlags_AlwaysOverwrite   // [renamed in 1.82] name was not matching behavior
#endif
};

// Flags for MsTrG::TreeNodeEx(), MsTrG::CollapsingHeader*()
enum MsTrGTreeNodeFlags_
{
    MsTrGTreeNodeFlags_None                 = 0,
    MsTrGTreeNodeFlags_Selected             = 1 << 0,   // Draw as selected
    MsTrGTreeNodeFlags_Framed               = 1 << 1,   // Draw frame with background (e.g. for CollapsingHeader)
    MsTrGTreeNodeFlags_AllowItemOverlap     = 1 << 2,   // Hit testing to allow subsequent widgets to overlap this one
    MsTrGTreeNodeFlags_NoTreePushOnOpen     = 1 << 3,   // Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
    MsTrGTreeNodeFlags_NoAutoOpenOnLog      = 1 << 4,   // Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
    MsTrGTreeNodeFlags_DefaultOpen          = 1 << 5,   // Default node to be open
    MsTrGTreeNodeFlags_OpenOnDoubleClick    = 1 << 6,   // Need double-click to open node
    MsTrGTreeNodeFlags_OpenOnArrow          = 1 << 7,   // Only open when clicking on the arrow part. If MsTrGTreeNodeFlags_OpenOnDoubleClick is also set, single-click arrow or double-click all box to open.
    MsTrGTreeNodeFlags_Leaf                 = 1 << 8,   // No collapsing, no arrow (use as a convenience for leaf nodes).
    MsTrGTreeNodeFlags_Bullet               = 1 << 9,   // Display a bullet instead of arrow
    MsTrGTreeNodeFlags_FramePadding         = 1 << 10,  // Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding().
    MsTrGTreeNodeFlags_SpanAvailWidth       = 1 << 11,  // Extend hit box to the right-most edge, even if not framed. This is not the default in order to allow adding other items on the same line. In the future we may refactor the hit system to be front-to-back, allowing natural overlaps and then this can become the default.
    MsTrGTreeNodeFlags_SpanFullWidth        = 1 << 12,  // Extend hit box to the left-most and right-most edges (bypass the indented area).
    MsTrGTreeNodeFlags_NavLeftJumpsBackHere = 1 << 13,  // (WIP) Nav: left direction may move to this TreeNode() from any of its child (items submitted between TreeNode and TreePop)
    //MsTrGTreeNodeFlags_NoScrollOnOpen     = 1 << 14,  // FIXME: TODO: Disable automatic scroll on TreePop() if node got just open and contents is not visible
    MsTrGTreeNodeFlags_CollapsingHeader     = MsTrGTreeNodeFlags_Framed | MsTrGTreeNodeFlags_NoTreePushOnOpen | MsTrGTreeNodeFlags_NoAutoOpenOnLog
};

// Flags for OpenPopup*(), BeginPopupContext*(), IsPopupOpen() functions.
// - To be backward compatible with older API which took an 'int mouse_button = 1' argument, we need to treat
//   small flags values as a mouse button index, so we encode the mouse button in the first few bits of the flags.
//   It is therefore guaranteed to be legal to pass a mouse button index in MsTrGPopupFlags.
// - For the same reason, we exceptionally default the MsTrGPopupFlags argument of BeginPopupContextXXX functions to 1 instead of 0.
//   IMPORTANT: because the default parameter is 1 (==MsTrGPopupFlags_MouseButtonRight), if you rely on the default parameter
//   and want to another another flag, you need to pass in the MsTrGPopupFlags_MouseButtonRight flag.
// - Multiple buttons currently cannot be combined/or-ed in those functions (we could allow it later).
enum MsTrGPopupFlags_
{
    MsTrGPopupFlags_None                    = 0,
    MsTrGPopupFlags_MouseButtonLeft         = 0,        // For BeginPopupContext*(): open on Left Mouse release. Guaranteed to always be == 0 (same as MsTrGMouseButton_Left)
    MsTrGPopupFlags_MouseButtonRight        = 1,        // For BeginPopupContext*(): open on Right Mouse release. Guaranteed to always be == 1 (same as MsTrGMouseButton_Right)
    MsTrGPopupFlags_MouseButtonMiddle       = 2,        // For BeginPopupContext*(): open on Middle Mouse release. Guaranteed to always be == 2 (same as MsTrGMouseButton_Middle)
    MsTrGPopupFlags_MouseButtonMask_        = 0x1F,
    MsTrGPopupFlags_MouseButtonDefault_     = 1,
    MsTrGPopupFlags_NoOpenOverExistingPopup = 1 << 5,   // For OpenPopup*(), BeginPopupContext*(): don't open if there's already a popup at the same level of the popup stack
    MsTrGPopupFlags_NoOpenOverItems         = 1 << 6,   // For BeginPopupContextWindow(): don't return true when hovering items, only when hovering empty space
    MsTrGPopupFlags_AnyPopupId              = 1 << 7,   // For IsPopupOpen(): ignore the MsTrGID parameter and test for any popup.
    MsTrGPopupFlags_AnyPopupLevel           = 1 << 8,   // For IsPopupOpen(): search/test at any level of the popup stack (default test in the current level)
    MsTrGPopupFlags_AnyPopup                = MsTrGPopupFlags_AnyPopupId | MsTrGPopupFlags_AnyPopupLevel
};

// Flags for MsTrG::Selectable()
enum MsTrGSelectableFlags_
{
    MsTrGSelectableFlags_None               = 0,
    MsTrGSelectableFlags_DontClosePopups    = 1 << 0,   // Clicking this don't close parent popup window
    MsTrGSelectableFlags_SpanAllColumns     = 1 << 1,   // Selectable frame can span all columns (text will still fit in current column)
    MsTrGSelectableFlags_AllowDoubleClick   = 1 << 2,   // Generate press events on double clicks too
    MsTrGSelectableFlags_Disabled           = 1 << 3,   // Cannot be selected, display grayed out text
    MsTrGSelectableFlags_AllowItemOverlap   = 1 << 4    // (WIP) Hit testing to allow subsequent widgets to overlap this one
};

// Flags for MsTrG::BeginCombo()
enum MsTrGComboFlags_
{
    MsTrGComboFlags_None                    = 0,
    MsTrGComboFlags_PopupAlignLeft          = 1 << 0,   // Align the popup toward the left by default
    MsTrGComboFlags_HeightSmall             = 1 << 1,   // Max ~4 items visible. Tip: If you want your combo popup to be a specific size you can use SetNextWindowSizeConstraints() prior to calling BeginCombo()
    MsTrGComboFlags_HeightRegular           = 1 << 2,   // Max ~8 items visible (default)
    MsTrGComboFlags_HeightLarge             = 1 << 3,   // Max ~20 items visible
    MsTrGComboFlags_HeightLargest           = 1 << 4,   // As many fitting items as possible
    MsTrGComboFlags_NoArrowButton           = 1 << 5,   // Display on the preview box without the square arrow button
    MsTrGComboFlags_NoPreview               = 1 << 6,   // Display only a square arrow button
    MsTrGComboFlags_HeightMask_             = MsTrGComboFlags_HeightSmall | MsTrGComboFlags_HeightRegular | MsTrGComboFlags_HeightLarge | MsTrGComboFlags_HeightLargest
};

// Flags for MsTrG::BeginTabBar()
enum MsTrGTabBarFlags_
{
    MsTrGTabBarFlags_None                           = 0,
    MsTrGTabBarFlags_Reorderable                    = 1 << 0,   // Allow manually dragging tabs to re-order them + New tabs are appended at the end of list
    MsTrGTabBarFlags_AutoSelectNewTabs              = 1 << 1,   // Automatically select new tabs when they appear
    MsTrGTabBarFlags_TabListPopupButton             = 1 << 2,   // Disable buttons to open the tab list popup
    MsTrGTabBarFlags_NoCloseWithMiddleMouseButton   = 1 << 3,   // Disable behavior of closing tabs (that are submitted with p_open != NULL) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
    MsTrGTabBarFlags_NoTabListScrollingButtons      = 1 << 4,   // Disable scrolling buttons (apply when fitting policy is MsTrGTabBarFlags_FittingPolicyScroll)
    MsTrGTabBarFlags_NoTooltip                      = 1 << 5,   // Disable tooltips when hovering a tab
    MsTrGTabBarFlags_FittingPolicyResizeDown        = 1 << 6,   // Resize tabs when they don't fit
    MsTrGTabBarFlags_FittingPolicyScroll            = 1 << 7,   // Add scroll buttons when tabs don't fit
    MsTrGTabBarFlags_FittingPolicyMask_             = MsTrGTabBarFlags_FittingPolicyResizeDown | MsTrGTabBarFlags_FittingPolicyScroll,
    MsTrGTabBarFlags_FittingPolicyDefault_          = MsTrGTabBarFlags_FittingPolicyResizeDown
};

// Flags for MsTrG::BeginTabItem()
enum MsTrGTabItemFlags_
{
    MsTrGTabItemFlags_None                          = 0,
    MsTrGTabItemFlags_UnsavedDocument               = 1 << 0,   // Display a dot next to the title + tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
    MsTrGTabItemFlags_SetSelected                   = 1 << 1,   // Trigger flag to programmatically make the tab selected when calling BeginTabItem()
    MsTrGTabItemFlags_NoCloseWithMiddleMouseButton  = 1 << 2,   // Disable behavior of closing tabs (that are submitted with p_open != NULL) with middle mouse button. You can still repro this behavior on user's side with if (IsItemHovered() && IsMouseClicked(2)) *p_open = false.
    MsTrGTabItemFlags_NoPushId                      = 1 << 3,   // Don't call PushID(tab->ID)/PopID() on BeginTabItem()/EndTabItem()
    MsTrGTabItemFlags_NoTooltip                     = 1 << 4,   // Disable tooltip for the given tab
    MsTrGTabItemFlags_NoReorder                     = 1 << 5,   // Disable reordering this tab or having another tab cross over this tab
    MsTrGTabItemFlags_Leading                       = 1 << 6,   // Enforce the tab position to the left of the tab bar (after the tab list popup button)
    MsTrGTabItemFlags_Trailing                      = 1 << 7    // Enforce the tab position to the right of the tab bar (before the scrolling buttons)
};

// Flags for MsTrG::BeginTable()
// - Important! Sizing policies have complex and subtle side effects, much more so than you would expect.
//   Read comments/demos carefully + experiment with live demos to get acquainted with them.
// - The DEFAULT sizing policies are:
//    - Default to MsTrGTableFlags_SizingFixedFit    if ScrollX is on, or if host window has MsTrGWindowFlags_AlwaysAutoResize.
//    - Default to MsTrGTableFlags_SizingStretchSame if ScrollX is off.
// - When ScrollX is off:
//    - Table defaults to MsTrGTableFlags_SizingStretchSame -> all Columns defaults to MsTrGTableColumnFlags_WidthStretch with same weight.
//    - Columns sizing policy allowed: Stretch (default), Fixed/Auto.
//    - Fixed Columns (if any) will generally obtain their requested width (unless the table cannot fit them all).
//    - Stretch Columns will share the remaining width according to their respective weight.
//    - Mixed Fixed/Stretch columns is possible but has various side-effects on resizing behaviors.
//      The typical use of mixing sizing policies is: any number of LEADING Fixed columns, followed by one or two TRAILING Stretch columns.
//      (this is because the visible order of columns have subtle but necessary effects on how they react to manual resizing).
// - When ScrollX is on:
//    - Table defaults to MsTrGTableFlags_SizingFixedFit -> all Columns defaults to MsTrGTableColumnFlags_WidthFixed
//    - Columns sizing policy allowed: Fixed/Auto mostly.
//    - Fixed Columns can be enlarged as needed. Table will show an horizontal scrollbar if needed.
//    - When using auto-resizing (non-resizable) fixed columns, querying the content width to use item right-alignment e.g. SetNextItemWidth(-FLT_MIN) doesn't make sense, would create a feedback loop.
//    - Using Stretch columns OFTEN DOES NOT MAKE SENSE if ScrollX is on, UNLESS you have specified a value for 'inner_width' in BeginTable().
//      If you specify a value for 'inner_width' then effectively the scrolling space is known and Stretch or mixed Fixed/Stretch columns become meaningful again.
// - Read on documentation at the top of MsTrG_tables.cpp for details.
enum MsTrGTableFlags_
{
    // Features
    MsTrGTableFlags_None                       = 0,
    MsTrGTableFlags_Resizable                  = 1 << 0,   // Enable resizing columns.
    MsTrGTableFlags_Reorderable                = 1 << 1,   // Enable reordering columns in header row (need calling TableSetupColumn() + TableHeadersRow() to display headers)
    MsTrGTableFlags_Hideable                   = 1 << 2,   // Enable hiding/disabling columns in context menu.
    MsTrGTableFlags_Sortable                   = 1 << 3,   // Enable sorting. Call TableGetSortSpecs() to obtain sort specs. Also see MsTrGTableFlags_SortMulti and MsTrGTableFlags_SortTristate.
    MsTrGTableFlags_NoSavedSettings            = 1 << 4,   // Disable persisting columns order, width and sort settings in the .ini file.
    MsTrGTableFlags_ContextMenuInBody          = 1 << 5,   // Right-click on columns body/contents will display table context menu. By default it is available in TableHeadersRow().
    // Decorations
    MsTrGTableFlags_RowBg                      = 1 << 6,   // Set each RowBg color with MsTrGCol_TableRowBg or MsTrGCol_TableRowBgAlt (equivalent of calling TableSetBgColor with MsTrGTableBgFlags_RowBg0 on each row manually)
    MsTrGTableFlags_BordersInnerH              = 1 << 7,   // Draw horizontal borders between rows.
    MsTrGTableFlags_BordersOuterH              = 1 << 8,   // Draw horizontal borders at the top and bottom.
    MsTrGTableFlags_BordersInnerV              = 1 << 9,   // Draw vertical borders between columns.
    MsTrGTableFlags_BordersOuterV              = 1 << 10,  // Draw vertical borders on the left and right sides.
    MsTrGTableFlags_BordersH                   = MsTrGTableFlags_BordersInnerH | MsTrGTableFlags_BordersOuterH, // Draw horizontal borders.
    MsTrGTableFlags_BordersV                   = MsTrGTableFlags_BordersInnerV | MsTrGTableFlags_BordersOuterV, // Draw vertical borders.
    MsTrGTableFlags_BordersInner               = MsTrGTableFlags_BordersInnerV | MsTrGTableFlags_BordersInnerH, // Draw inner borders.
    MsTrGTableFlags_BordersOuter               = MsTrGTableFlags_BordersOuterV | MsTrGTableFlags_BordersOuterH, // Draw outer borders.
    MsTrGTableFlags_Borders                    = MsTrGTableFlags_BordersInner | MsTrGTableFlags_BordersOuter,   // Draw all borders.
    MsTrGTableFlags_NoBordersInBody            = 1 << 11,  // [ALPHA] Disable vertical borders in columns Body (borders will always appears in Headers). -> May move to style
    MsTrGTableFlags_NoBordersInBodyUntilResize = 1 << 12,  // [ALPHA] Disable vertical borders in columns Body until hovered for resize (borders will always appears in Headers). -> May move to style
    // Sizing Policy (read above for defaults)
    MsTrGTableFlags_SizingFixedFit             = 1 << 13,  // Columns default to _WidthFixed or _WidthAuto (if resizable or not resizable), matching contents width.
    MsTrGTableFlags_SizingFixedSame            = 2 << 13,  // Columns default to _WidthFixed or _WidthAuto (if resizable or not resizable), matching the maximum contents width of all columns. Implicitly enable MsTrGTableFlags_NoKeepColumnsVisible.
    MsTrGTableFlags_SizingStretchProp          = 3 << 13,  // Columns default to _WidthStretch with default weights proportional to each columns contents widths.
    MsTrGTableFlags_SizingStretchSame          = 4 << 13,  // Columns default to _WidthStretch with default weights all equal, unless overridden by TableSetupColumn().
    // Sizing Extra Options
    MsTrGTableFlags_NoHostExtendX              = 1 << 16,  // Make outer width auto-fit to columns, overriding outer_size.x value. Only available when ScrollX/ScrollY are disabled and Stretch columns are not used.
    MsTrGTableFlags_NoHostExtendY              = 1 << 17,  // Make outer height stop exactly at outer_size.y (prevent auto-extending table past the limit). Only available when ScrollX/ScrollY are disabled. Data below the limit will be clipped and not visible.
    MsTrGTableFlags_NoKeepColumnsVisible       = 1 << 18,  // Disable keeping column always minimally visible when ScrollX is off and table gets too small. Not recommended if columns are resizable.
    MsTrGTableFlags_PreciseWidths              = 1 << 19,  // Disable distributing remainder width to stretched columns (width allocation on a 100-wide table with 3 columns: Without this flag: 33,33,34. With this flag: 33,33,33). With larger number of columns, resizing will appear to be less smooth.
    // Clipping
    MsTrGTableFlags_NoClip                     = 1 << 20,  // Disable clipping rectangle for every individual columns (reduce draw command count, items will be able to overflow into other columns). Generally incompatible with TableSetupScrollFreeze().
    // Padding
    MsTrGTableFlags_PadOuterX                  = 1 << 21,  // Default if BordersOuterV is on. Enable outer-most padding. Generally desirable if you have headers.
    MsTrGTableFlags_NoPadOuterX                = 1 << 22,  // Default if BordersOuterV is off. Disable outer-most padding.
    MsTrGTableFlags_NoPadInnerX                = 1 << 23,  // Disable inner padding between columns (double inner padding if BordersOuterV is on, single inner padding if BordersOuterV is off).
    // Scrolling
    MsTrGTableFlags_ScrollX                    = 1 << 24,  // Enable horizontal scrolling. Require 'outer_size' parameter of BeginTable() to specify the container size. Changes default sizing policy. Because this create a child window, ScrollY is currently generally recommended when using ScrollX.
    MsTrGTableFlags_ScrollY                    = 1 << 25,  // Enable vertical scrolling. Require 'outer_size' parameter of BeginTable() to specify the container size.
    // Sorting
    MsTrGTableFlags_SortMulti                  = 1 << 26,  // Hold shift when clicking headers to sort on multiple column. TableGetSortSpecs() may return specs where (SpecsCount > 1).
    MsTrGTableFlags_SortTristate               = 1 << 27,  // Allow no sorting, disable default sorting. TableGetSortSpecs() may return specs where (SpecsCount == 0).

    // [Internal] Combinations and masks
    MsTrGTableFlags_SizingMask_                = MsTrGTableFlags_SizingFixedFit | MsTrGTableFlags_SizingFixedSame | MsTrGTableFlags_SizingStretchProp | MsTrGTableFlags_SizingStretchSame

    // Obsolete names (will be removed soon)
#ifndef MsTrG_DISABLE_OBSOLETE_FUNCTIONS
    //, MsTrGTableFlags_ColumnsWidthFixed = MsTrGTableFlags_SizingFixedFit, MsTrGTableFlags_ColumnsWidthStretch = MsTrGTableFlags_SizingStretchSame   // WIP Tables 2020/12
    //, MsTrGTableFlags_SizingPolicyFixed = MsTrGTableFlags_SizingFixedFit, MsTrGTableFlags_SizingPolicyStretch = MsTrGTableFlags_SizingStretchSame   // WIP Tables 2021/01
#endif
};

// Flags for MsTrG::TableSetupColumn()
enum MsTrGTableColumnFlags_
{
    // Input configuration flags
    MsTrGTableColumnFlags_None                  = 0,
    MsTrGTableColumnFlags_Disabled              = 1 << 0,   // Overriding/master disable flag: hide column, won't show in context menu (unlike calling TableSetColumnEnabled() which manipulates the user accessible state)
    MsTrGTableColumnFlags_DefaultHide           = 1 << 1,   // Default as a hidden/disabled column.
    MsTrGTableColumnFlags_DefaultSort           = 1 << 2,   // Default as a sorting column.
    MsTrGTableColumnFlags_WidthStretch          = 1 << 3,   // Column will stretch. Preferable with horizontal scrolling disabled (default if table sizing policy is _SizingStretchSame or _SizingStretchProp).
    MsTrGTableColumnFlags_WidthFixed            = 1 << 4,   // Column will not stretch. Preferable with horizontal scrolling enabled (default if table sizing policy is _SizingFixedFit and table is resizable).
    MsTrGTableColumnFlags_NoResize              = 1 << 5,   // Disable manual resizing.
    MsTrGTableColumnFlags_NoReorder             = 1 << 6,   // Disable manual reordering this column, this will also prevent other columns from crossing over this column.
    MsTrGTableColumnFlags_NoHide                = 1 << 7,   // Disable ability to hide/disable this column.
    MsTrGTableColumnFlags_NoClip                = 1 << 8,   // Disable clipping for this column (all NoClip columns will render in a same draw command).
    MsTrGTableColumnFlags_NoSort                = 1 << 9,   // Disable ability to sort on this field (even if MsTrGTableFlags_Sortable is set on the table).
    MsTrGTableColumnFlags_NoSortAscending       = 1 << 10,  // Disable ability to sort in the ascending direction.
    MsTrGTableColumnFlags_NoSortDescending      = 1 << 11,  // Disable ability to sort in the descending direction.
    MsTrGTableColumnFlags_NoHeaderLabel         = 1 << 12,  // TableHeadersRow() will not submit label for this column. Convenient for some small columns. Name will still appear in context menu.
    MsTrGTableColumnFlags_NoHeaderWidth         = 1 << 13,  // Disable header text width contribution to automatic column width.
    MsTrGTableColumnFlags_PreferSortAscending   = 1 << 14,  // Make the initial sort direction Ascending when first sorting on this column (default).
    MsTrGTableColumnFlags_PreferSortDescending  = 1 << 15,  // Make the initial sort direction Descending when first sorting on this column.
    MsTrGTableColumnFlags_IndentEnable          = 1 << 16,  // Use current Indent value when entering cell (default for column 0).
    MsTrGTableColumnFlags_IndentDisable         = 1 << 17,  // Ignore current Indent value when entering cell (default for columns > 0). Indentation changes _within_ the cell will still be honored.

    // Output status flags, read-only via TableGetColumnFlags()
    MsTrGTableColumnFlags_IsEnabled             = 1 << 24,  // Status: is enabled == not hidden by user/api (referred to as "Hide" in _DefaultHide and _NoHide) flags.
    MsTrGTableColumnFlags_IsVisible             = 1 << 25,  // Status: is visible == is enabled AND not clipped by scrolling.
    MsTrGTableColumnFlags_IsSorted              = 1 << 26,  // Status: is currently part of the sort specs
    MsTrGTableColumnFlags_IsHovered             = 1 << 27,  // Status: is hovered by mouse

    // [Internal] Combinations and masks
    MsTrGTableColumnFlags_WidthMask_            = MsTrGTableColumnFlags_WidthStretch | MsTrGTableColumnFlags_WidthFixed,
    MsTrGTableColumnFlags_IndentMask_           = MsTrGTableColumnFlags_IndentEnable | MsTrGTableColumnFlags_IndentDisable,
    MsTrGTableColumnFlags_StatusMask_           = MsTrGTableColumnFlags_IsEnabled | MsTrGTableColumnFlags_IsVisible | MsTrGTableColumnFlags_IsSorted | MsTrGTableColumnFlags_IsHovered,
    MsTrGTableColumnFlags_NoDirectResize_       = 1 << 30   // [Internal] Disable user resizing this column directly (it may however we resized indirectly from its left edge)

    // Obsolete names (will be removed soon)
#ifndef MsTrG_DISABLE_OBSOLETE_FUNCTIONS
    //MsTrGTableColumnFlags_WidthAuto           = MsTrGTableColumnFlags_WidthFixed | MsTrGTableColumnFlags_NoResize, // Column will not stretch and keep resizing based on submitted contents.
#endif
};

// Flags for MsTrG::TableNextRow()
enum MsTrGTableRowFlags_
{
    MsTrGTableRowFlags_None                         = 0,
    MsTrGTableRowFlags_Headers                      = 1 << 0    // Identify header row (set default background color + width of its contents accounted differently for auto column width)
};

// Enum for MsTrG::TableSetBgColor()
// Background colors are rendering in 3 layers:
//  - Layer 0: draw with RowBg0 color if set, otherwise draw with ColumnBg0 if set.
//  - Layer 1: draw with RowBg1 color if set, otherwise draw with ColumnBg1 if set.
//  - Layer 2: draw with CellBg color if set.
// The purpose of the two row/columns layers is to let you decide if a background color changes should override or blend with the existing color.
// When using MsTrGTableFlags_RowBg on the table, each row has the RowBg0 color automatically set for odd/even rows.
// If you set the color of RowBg0 target, your color will override the existing RowBg0 color.
// If you set the color of RowBg1 or ColumnBg1 target, your color will blend over the RowBg0 color.
enum MsTrGTableBgTarget_
{
    MsTrGTableBgTarget_None                         = 0,
    MsTrGTableBgTarget_RowBg0                       = 1,        // Set row background color 0 (generally used for background, automatically set when MsTrGTableFlags_RowBg is used)
    MsTrGTableBgTarget_RowBg1                       = 2,        // Set row background color 1 (generally used for selection marking)
    MsTrGTableBgTarget_CellBg                       = 3         // Set cell background color (top-most color)
};

// Flags for MsTrG::IsWindowFocused()
enum MsTrGFocusedFlags_
{
    MsTrGFocusedFlags_None                          = 0,
    MsTrGFocusedFlags_ChildWindows                  = 1 << 0,   // Return true if any children of the window is focused
    MsTrGFocusedFlags_RootWindow                    = 1 << 1,   // Test from root window (top most parent of the current hierarchy)
    MsTrGFocusedFlags_AnyWindow                     = 1 << 2,   // Return true if any window is focused. Important: If you are trying to tell how to dispatch your low-level inputs, do NOT use this. Use 'io.WantCaptureMouse' instead! Please read the FAQ!
    MsTrGFocusedFlags_NoPopupHierarchy              = 1 << 3,   // Do not consider popup hierarchy (do not treat popup emitter as parent of popup) (when used with _ChildWindows or _RootWindow)
    //MsTrGFocusedFlags_DockHierarchy               = 1 << 4,   // Consider docking hierarchy (treat dockspace host as parent of docked window) (when used with _ChildWindows or _RootWindow)
    MsTrGFocusedFlags_RootAndChildWindows           = MsTrGFocusedFlags_RootWindow | MsTrGFocusedFlags_ChildWindows
};

// Flags for MsTrG::IsItemHovered(), MsTrG::IsWindowHovered()
// Note: if you are trying to check whether your mouse should be dispatched to Dear MsTrG or to your app, you should use 'io.WantCaptureMouse' instead! Please read the FAQ!
// Note: windows with the MsTrGWindowFlags_NoInputs flag are ignored by IsWindowHovered() calls.
enum MsTrGHoveredFlags_
{
    MsTrGHoveredFlags_None                          = 0,        // Return true if directly over the item/window, not obstructed by another window, not obstructed by an active popup or modal blocking inputs under them.
    MsTrGHoveredFlags_ChildWindows                  = 1 << 0,   // IsWindowHovered() only: Return true if any children of the window is hovered
    MsTrGHoveredFlags_RootWindow                    = 1 << 1,   // IsWindowHovered() only: Test from root window (top most parent of the current hierarchy)
    MsTrGHoveredFlags_AnyWindow                     = 1 << 2,   // IsWindowHovered() only: Return true if any window is hovered
    MsTrGHoveredFlags_NoPopupHierarchy              = 1 << 3,   // IsWindowHovered() only: Do not consider popup hierarchy (do not treat popup emitter as parent of popup) (when used with _ChildWindows or _RootWindow)
    //MsTrGHoveredFlags_DockHierarchy               = 1 << 4,   // IsWindowHovered() only: Consider docking hierarchy (treat dockspace host as parent of docked window) (when used with _ChildWindows or _RootWindow)
    MsTrGHoveredFlags_AllowWhenBlockedByPopup       = 1 << 5,   // Return true even if a popup window is normally blocking access to this item/window
    //MsTrGHoveredFlags_AllowWhenBlockedByModal     = 1 << 6,   // Return true even if a modal popup window is normally blocking access to this item/window. FIXME-TODO: Unavailable yet.
    MsTrGHoveredFlags_AllowWhenBlockedByActiveItem  = 1 << 7,   // Return true even if an active item is blocking access to this item/window. Useful for Drag and Drop patterns.
    MsTrGHoveredFlags_AllowWhenOverlapped           = 1 << 8,   // IsItemHovered() only: Return true even if the position is obstructed or overlapped by another window
    MsTrGHoveredFlags_AllowWhenDisabled             = 1 << 9,   // IsItemHovered() only: Return true even if the item is disabled
    MsTrGHoveredFlags_NoNavOverride                 = 1 << 10,  // Disable using gamepad/keyboard navigation state when active, always query mouse.
    MsTrGHoveredFlags_RectOnly                      = MsTrGHoveredFlags_AllowWhenBlockedByPopup | MsTrGHoveredFlags_AllowWhenBlockedByActiveItem | MsTrGHoveredFlags_AllowWhenOverlapped,
    MsTrGHoveredFlags_RootAndChildWindows           = MsTrGHoveredFlags_RootWindow | MsTrGHoveredFlags_ChildWindows
};

// Flags for MsTrG::BeginDragDropSource(), MsTrG::AcceptDragDropPayload()
enum MsTrGDragDropFlags_
{
    MsTrGDragDropFlags_None                         = 0,
    // BeginDragDropSource() flags
    MsTrGDragDropFlags_SourceNoPreviewTooltip       = 1 << 0,   // By default, a successful call to BeginDragDropSource opens a tooltip so you can display a preview or description of the source contents. This flag disable this behavior.
    MsTrGDragDropFlags_SourceNoDisableHover         = 1 << 1,   // By default, when dragging we clear data so that IsItemHovered() will return false, to avoid subsequent user code submitting tooltips. This flag disable this behavior so you can still call IsItemHovered() on the source item.
    MsTrGDragDropFlags_SourceNoHoldToOpenOthers     = 1 << 2,   // Disable the behavior that allows to open tree nodes and collapsing header by holding over them while dragging a source item.
    MsTrGDragDropFlags_SourceAllowNullID            = 1 << 3,   // Allow items such as Text(), Image() that have no unique identifier to be used as drag source, by manufacturing a temporary identifier based on their window-relative position. This is extremely unusual within the dear MsTrG ecosystem and so we made it explicit.
    MsTrGDragDropFlags_SourceExtern                 = 1 << 4,   // External source (from outside of dear MsTrG), won't attempt to read current item/window info. Will always return true. Only one Extern source can be active simultaneously.
    MsTrGDragDropFlags_SourceAutoExpirePayload      = 1 << 5,   // Automatically expire the payload if the source cease to be submitted (otherwise payloads are persisting while being dragged)
    // AcceptDragDropPayload() flags
    MsTrGDragDropFlags_AcceptBeforeDelivery         = 1 << 10,  // AcceptDragDropPayload() will returns true even before the mouse button is released. You can then call IsDelivery() to test if the payload needs to be delivered.
    MsTrGDragDropFlags_AcceptNoDrawDefaultRect      = 1 << 11,  // Do not draw the default highlight rectangle when hovering over target.
    MsTrGDragDropFlags_AcceptNoPreviewTooltip       = 1 << 12,  // Request hiding the BeginDragDropSource tooltip from the BeginDragDropTarget site.
    MsTrGDragDropFlags_AcceptPeekOnly               = MsTrGDragDropFlags_AcceptBeforeDelivery | MsTrGDragDropFlags_AcceptNoDrawDefaultRect  // For peeking ahead and inspecting the payload before delivery.
};

// Standard Drag and Drop payload types. You can define you own payload types using short strings. Types starting with '_' are defined by Dear MsTrG.
#define MsTrG_PAYLOAD_TYPE_COLOR_3F     "_COL3F"    // float[3]: Standard type for colors, without alpha. User code may use this type.
#define MsTrG_PAYLOAD_TYPE_COLOR_4F     "_COL4F"    // float[4]: Standard type for colors. User code may use this type.

// A primary data type
enum MsTrGDataType_
{
    MsTrGDataType_S8,       // signed char / char (with sensible compilers)
    MsTrGDataType_U8,       // unsigned char
    MsTrGDataType_S16,      // short
    MsTrGDataType_U16,      // unsigned short
    MsTrGDataType_S32,      // int
    MsTrGDataType_U32,      // unsigned int
    MsTrGDataType_S64,      // long long / __int64
    MsTrGDataType_U64,      // unsigned long long / unsigned __int64
    MsTrGDataType_Float,    // float
    MsTrGDataType_Double,   // double
    MsTrGDataType_COUNT
};

// A cardinal direction
enum MsTrGDir_
{
    MsTrGDir_None    = -1,
    MsTrGDir_Left    = 0,
    MsTrGDir_Right   = 1,
    MsTrGDir_Up      = 2,
    MsTrGDir_Down    = 3,
    MsTrGDir_COUNT
};

// A sorting direction
enum MsTrGSortDirection_
{
    MsTrGSortDirection_None         = 0,
    MsTrGSortDirection_Ascending    = 1,    // Ascending = 0->9, A->Z etc.
    MsTrGSortDirection_Descending   = 2     // Descending = 9->0, Z->A etc.
};

// Keys value 0 to 511 are left unused as legacy native/opaque key values (< 1.87)
// Keys value >= 512 are named keys (>= 1.87)
enum MsTrGKey_
{
    // Keyboard
    MsTrGKey_None = 0,
    MsTrGKey_Tab = 512,             // == MsTrGKey_NamedKey_BEGIN
    MsTrGKey_LeftArrow,
    MsTrGKey_RightArrow,
    MsTrGKey_UpArrow,
    MsTrGKey_DownArrow,
    MsTrGKey_PageUp,
    MsTrGKey_PageDown,
    MsTrGKey_Home,
    MsTrGKey_End,
    MsTrGKey_Insert,
    MsTrGKey_Delete,
    MsTrGKey_Backspace,
    MsTrGKey_Space,
    MsTrGKey_Enter,
    MsTrGKey_Escape,
    MsTrGKey_LeftCtrl, MsTrGKey_LeftShift, MsTrGKey_LeftAlt, MsTrGKey_LeftSuper,
    MsTrGKey_RightCtrl, MsTrGKey_RightShift, MsTrGKey_RightAlt, MsTrGKey_RightSuper,
    MsTrGKey_Menu,
    MsTrGKey_0, MsTrGKey_1, MsTrGKey_2, MsTrGKey_3, MsTrGKey_4, MsTrGKey_5, MsTrGKey_6, MsTrGKey_7, MsTrGKey_8, MsTrGKey_9,
    MsTrGKey_A, MsTrGKey_B, MsTrGKey_C, MsTrGKey_D, MsTrGKey_E, MsTrGKey_F, MsTrGKey_G, MsTrGKey_H, MsTrGKey_I, MsTrGKey_J,
    MsTrGKey_K, MsTrGKey_L, MsTrGKey_M, MsTrGKey_N, MsTrGKey_O, MsTrGKey_P, MsTrGKey_Q, MsTrGKey_R, MsTrGKey_S, MsTrGKey_T,
    MsTrGKey_U, MsTrGKey_V, MsTrGKey_W, MsTrGKey_X, MsTrGKey_Y, MsTrGKey_Z,
    MsTrGKey_F1, MsTrGKey_F2, MsTrGKey_F3, MsTrGKey_F4, MsTrGKey_F5, MsTrGKey_F6,
    MsTrGKey_F7, MsTrGKey_F8, MsTrGKey_F9, MsTrGKey_F10, MsTrGKey_F11, MsTrGKey_F12,
    MsTrGKey_Apostrophe,        // '
    MsTrGKey_Comma,             // ,
    MsTrGKey_Minus,             // -
    MsTrGKey_Period,            // .
    MsTrGKey_Slash,             // /
    MsTrGKey_Semicolon,         // ;
    MsTrGKey_Equal,             // =
    MsTrGKey_LeftBracket,       // [
    MsTrGKey_Backslash,         // \ (this text inhibit multiline comment caused by backslash)
    MsTrGKey_RightBracket,      // ]
    MsTrGKey_GraveAccent,       // `
    MsTrGKey_CapsLock,
    MsTrGKey_ScrollLock,
    MsTrGKey_NumLock,
    MsTrGKey_PrintScreen,
    MsTrGKey_Pause,
    MsTrGKey_Keypad0, MsTrGKey_Keypad1, MsTrGKey_Keypad2, MsTrGKey_Keypad3, MsTrGKey_Keypad4,
    MsTrGKey_Keypad5, MsTrGKey_Keypad6, MsTrGKey_Keypad7, MsTrGKey_Keypad8, MsTrGKey_Keypad9,
    MsTrGKey_KeypadDecimal,
    MsTrGKey_KeypadDivide,
    MsTrGKey_KeypadMultiply,
    MsTrGKey_KeypadSubtract,
    MsTrGKey_KeypadAdd,
    MsTrGKey_KeypadEnter,
    MsTrGKey_KeypadEqual,

    // Gamepad (some of those are analog values, 0.0f to 1.0f)                              // NAVIGATION action
    MsTrGKey_GamepadStart,          // Menu (Xbox)          + (Switch)   Start/Options (PS) // --
    MsTrGKey_GamepadBack,           // View (Xbox)          - (Switch)   Share (PS)         // --
    MsTrGKey_GamepadFaceUp,         // Y (Xbox)             X (Switch)   Triangle (PS)      // -> MsTrGNavInput_Input
    MsTrGKey_GamepadFaceDown,       // A (Xbox)             B (Switch)   Cross (PS)         // -> MsTrGNavInput_Activate
    MsTrGKey_GamepadFaceLeft,       // X (Xbox)             Y (Switch)   Square (PS)        // -> MsTrGNavInput_Menu
    MsTrGKey_GamepadFaceRight,      // B (Xbox)             A (Switch)   Circle (PS)        // -> MsTrGNavInput_Cancel
    MsTrGKey_GamepadDpadUp,         // D-pad Up                                             // -> MsTrGNavInput_DpadUp
    MsTrGKey_GamepadDpadDown,       // D-pad Down                                           // -> MsTrGNavInput_DpadDown
    MsTrGKey_GamepadDpadLeft,       // D-pad Left                                           // -> MsTrGNavInput_DpadLeft
    MsTrGKey_GamepadDpadRight,      // D-pad Right                                          // -> MsTrGNavInput_DpadRight
    MsTrGKey_GamepadL1,             // L Bumper (Xbox)      L (Switch)   L1 (PS)            // -> MsTrGNavInput_FocusPrev + MsTrGNavInput_TweakSlow
    MsTrGKey_GamepadR1,             // R Bumper (Xbox)      R (Switch)   R1 (PS)            // -> MsTrGNavInput_FocusNext + MsTrGNavInput_TweakFast
    MsTrGKey_GamepadL2,             // L Trigger (Xbox)     ZL (Switch)  L2 (PS) [Analog]
    MsTrGKey_GamepadR2,             // R Trigger (Xbox)     ZR (Switch)  R2 (PS) [Analog]
    MsTrGKey_GamepadL3,             // L Thumbstick (Xbox)  L3 (Switch)  L3 (PS)
    MsTrGKey_GamepadR3,             // R Thumbstick (Xbox)  R3 (Switch)  R3 (PS)
    MsTrGKey_GamepadLStickUp,       // [Analog]                                             // -> MsTrGNavInput_LStickUp
    MsTrGKey_GamepadLStickDown,     // [Analog]                                             // -> MsTrGNavInput_LStickDown
    MsTrGKey_GamepadLStickLeft,     // [Analog]                                             // -> MsTrGNavInput_LStickLeft
    MsTrGKey_GamepadLStickRight,    // [Analog]                                             // -> MsTrGNavInput_LStickRight
    MsTrGKey_GamepadRStickUp,       // [Analog]
    MsTrGKey_GamepadRStickDown,     // [Analog]
    MsTrGKey_GamepadRStickLeft,     // [Analog]
    MsTrGKey_GamepadRStickRight,    // [Analog]

    // Keyboard Modifiers (explicitly submitted by backend via AddKeyEvent() calls)
    // - This is mirroring the data also written to io.KeyCtrl, io.KeyShift, io.KeyAlt, io.KeySuper, in a format allowing
    //   them to be accessed via standard key API, allowing calls such as IsKeyPressed(), IsKeyReleased(), querying duration etc.
    // - Code polling every keys (e.g. an interface to detect a key press for input mapping) might want to ignore those
    //   and prefer using the real keys (e.g. MsTrGKey_LeftCtrl, MsTrGKey_RightCtrl instead of MsTrGKey_ModCtrl).
    // - In theory the value of keyboard modifiers should be roughly equivalent to a logical or of the equivalent left/right keys.
    //   In practice: it's complicated; mods are often provided from different sources. Keyboard layout, IME, sticky keys and
    //   backends tend to interfere and break that equivalence. The safer decision is to relay that ambiguity down to the end-user...
    MsTrGKey_ModCtrl, MsTrGKey_ModShift, MsTrGKey_ModAlt, MsTrGKey_ModSuper,

    // End of list
    MsTrGKey_COUNT,                 // No valid MsTrGKey is ever greater than this value

    // [Internal] Prior to 1.87 we required user to fill io.KeysDown[512] using their own native index + a io.KeyMap[] array.
    // We are ditching this method but keeping a legacy path for user code doing e.g. IsKeyPressed(MY_NATIVE_KEY_CODE)
    MsTrGKey_NamedKey_BEGIN         = 512,
    MsTrGKey_NamedKey_END           = MsTrGKey_COUNT,
    MsTrGKey_NamedKey_COUNT         = MsTrGKey_NamedKey_END - MsTrGKey_NamedKey_BEGIN,
#ifdef MsTrG_DISABLE_OBSOLETE_KEYIO
    MsTrGKey_KeysData_SIZE          = MsTrGKey_NamedKey_COUNT,          // Size of KeysData[]: only hold named keys
    MsTrGKey_KeysData_OFFSET        = MsTrGKey_NamedKey_BEGIN           // First key stored in io.KeysData[0]. Accesses to io.KeysData[] must use (key - MsTrGKey_KeysData_OFFSET).
#else
    MsTrGKey_KeysData_SIZE          = MsTrGKey_COUNT,                   // Size of KeysData[]: hold legacy 0..512 keycodes + named keys
    MsTrGKey_KeysData_OFFSET        = 0                                 // First key stored in io.KeysData[0]. Accesses to io.KeysData[] must use (key - MsTrGKey_KeysData_OFFSET).
#endif

#ifndef MsTrG_DISABLE_OBSOLETE_FUNCTIONS
    , MsTrGKey_KeyPadEnter = MsTrGKey_KeypadEnter   // Renamed in 1.87
#endif
};

// Helper "flags" version of key-mods to store and compare multiple key-mods easily. Sometimes used for storage (e.g. io.KeyMods) but otherwise not much used in public API.
enum MsTrGModFlags_
{
    MsTrGModFlags_None              = 0,
    MsTrGModFlags_Ctrl              = 1 << 0,
    MsTrGModFlags_Shift             = 1 << 1,
    MsTrGModFlags_Alt               = 1 << 2,   // Menu
    MsTrGModFlags_Super             = 1 << 3    // Cmd/Super/Windows key
};

// Gamepad/Keyboard navigation
// Since >= 1.87 backends you generally don't need to care about this enum since io.NavInputs[] is setup automatically. This might become private/internal some day.
// Keyboard: Set io.ConfigFlags |= MsTrGConfigFlags_NavEnableKeyboard to enable. NewFrame() will automatically fill io.NavInputs[] based on your io.AddKeyEvent() calls.
// Gamepad:  Set io.ConfigFlags |= MsTrGConfigFlags_NavEnableGamepad to enable. Backend: set MsTrGBackendFlags_HasGamepad and fill the io.NavInputs[] fields before calling NewFrame(). Note that io.NavInputs[] is cleared by EndFrame().
// Read instructions in MsTrG.cpp for more details. Download PNG/PSD at http://dearMsTrG.org/controls_sheets.
enum MsTrGNavInput_
{
    // Gamepad Mapping
    MsTrGNavInput_Activate,      // Activate / Open / Toggle / Tweak value       // e.g. Cross  (PS4), A (Xbox), A (Switch), Space (Keyboard)
    MsTrGNavInput_Cancel,        // Cancel / Close / Exit                        // e.g. Circle (PS4), B (Xbox), B (Switch), Escape (Keyboard)
    MsTrGNavInput_Input,         // Text input / On-Screen keyboard              // e.g. Triang.(PS4), Y (Xbox), X (Switch), Return (Keyboard)
    MsTrGNavInput_Menu,          // Tap: Toggle menu / Hold: Focus, Move, Resize // e.g. Square (PS4), X (Xbox), Y (Switch), Alt (Keyboard)
    MsTrGNavInput_DpadLeft,      // Move / Tweak / Resize window (w/ PadMenu)    // e.g. D-pad Left/Right/Up/Down (Gamepads), Arrow keys (Keyboard)
    MsTrGNavInput_DpadRight,     //
    MsTrGNavInput_DpadUp,        //
    MsTrGNavInput_DpadDown,      //
    MsTrGNavInput_LStickLeft,    // Scroll / Move window (w/ PadMenu)            // e.g. Left Analog Stick Left/Right/Up/Down
    MsTrGNavInput_LStickRight,   //
    MsTrGNavInput_LStickUp,      //
    MsTrGNavInput_LStickDown,    //
    MsTrGNavInput_FocusPrev,     // Focus Next window (w/ PadMenu)               // e.g. L1 or L2 (PS4), LB or LT (Xbox), L or ZL (Switch)
    MsTrGNavInput_FocusNext,     // Focus Prev window (w/ PadMenu)               // e.g. R1 or R2 (PS4), RB or RT (Xbox), R or ZL (Switch)
    MsTrGNavInput_TweakSlow,     // Slower tweaks                                // e.g. L1 or L2 (PS4), LB or LT (Xbox), L or ZL (Switch)
    MsTrGNavInput_TweakFast,     // Faster tweaks                                // e.g. R1 or R2 (PS4), RB or RT (Xbox), R or ZL (Switch)

    // [Internal] Don't use directly! This is used internally to differentiate keyboard from gamepad inputs for behaviors that require to differentiate them.
    // Keyboard behavior that have no corresponding gamepad mapping (e.g. CTRL+TAB) will be directly reading from keyboard keys instead of io.NavInputs[].
    MsTrGNavInput_KeyLeft_,      // Move left                                    // = Arrow keys
    MsTrGNavInput_KeyRight_,     // Move right
    MsTrGNavInput_KeyUp_,        // Move up
    MsTrGNavInput_KeyDown_,      // Move down
    MsTrGNavInput_COUNT
};

// Configuration flags stored in io.ConfigFlags. Set by user/application.
enum MsTrGConfigFlags_
{
    MsTrGConfigFlags_None                   = 0,
    MsTrGConfigFlags_NavEnableKeyboard      = 1 << 0,   // Master keyboard navigation enable flag. NewFrame() will automatically fill io.NavInputs[] based on io.AddKeyEvent() calls
    MsTrGConfigFlags_NavEnableGamepad       = 1 << 1,   // Master gamepad navigation enable flag. This is mostly to instruct your MsTrG backend to fill io.NavInputs[]. Backend also needs to set MsTrGBackendFlags_HasGamepad.
    MsTrGConfigFlags_NavEnableSetMousePos   = 1 << 2,   // Instruct navigation to move the mouse cursor. May be useful on TV/console systems where moving a virtual mouse is awkward. Will update io.MousePos and set io.WantSetMousePos=true. If enabled you MUST honor io.WantSetMousePos requests in your backend, otherwise MsTrG will react as if the mouse is jumping around back and forth.
    MsTrGConfigFlags_NavNoCaptureKeyboard   = 1 << 3,   // Instruct navigation to not set the io.WantCaptureKeyboard flag when io.NavActive is set.
    MsTrGConfigFlags_NoMouse                = 1 << 4,   // Instruct MsTrG to clear mouse position/buttons in NewFrame(). This allows ignoring the mouse information set by the backend.
    MsTrGConfigFlags_NoMouseCursorChange    = 1 << 5,   // Instruct backend to not alter mouse cursor shape and visibility. Use if the backend cursor changes are interfering with yours and you don't want to use SetMouseCursor() to change mouse cursor. You may want to honor requests from MsTrG by reading GetMouseCursor() yourself instead.

    // User storage (to allow your backend/engine to communicate to code that may be shared between multiple projects. Those flags are NOT used by core Dear MsTrG)
    MsTrGConfigFlags_IsSRGB                 = 1 << 20,  // Application is SRGB-aware.
    MsTrGConfigFlags_IsTouchScreen          = 1 << 21   // Application is using a touch screen instead of a mouse.
};

// Backend capabilities flags stored in io.BackendFlags. Set by MsTrG_impl_xxx or custom backend.
enum MsTrGBackendFlags_
{
    MsTrGBackendFlags_None                  = 0,
    MsTrGBackendFlags_HasGamepad            = 1 << 0,   // Backend Platform supports gamepad and currently has one connected.
    MsTrGBackendFlags_HasMouseCursors       = 1 << 1,   // Backend Platform supports honoring GetMouseCursor() value to change the OS cursor shape.
    MsTrGBackendFlags_HasSetMousePos        = 1 << 2,   // Backend Platform supports io.WantSetMousePos requests to reposition the OS mouse position (only used if MsTrGConfigFlags_NavEnableSetMousePos is set).
    MsTrGBackendFlags_RendererHasVtxOffset  = 1 << 3    // Backend Renderer supports ImDrawCmd::VtxOffset. This enables output of large meshes (64K+ vertices) while still using 16-bit indices.
};

// Enumeration for PushStyleColor() / PopStyleColor()
enum MsTrGCol_
{
    MsTrGCol_Text,
    MsTrGCol_TextDisabled,
    MsTrGCol_WindowBg,              // Background of normal windows
    MsTrGCol_ChildBg,               // Background of child windows
    MsTrGCol_PopupBg,               // Background of popups, menus, tooltips windows
    MsTrGCol_Border,
    MsTrGCol_BorderShadow,
    MsTrGCol_FrameBg,               // Background of checkbox, radio button, plot, slider, text input
    MsTrGCol_FrameBgHovered,
    MsTrGCol_FrameBgActive,
    MsTrGCol_TitleBg,
    MsTrGCol_TitleBgActive,
    MsTrGCol_TitleBgCollapsed,
    MsTrGCol_MenuBarBg,
    MsTrGCol_ScrollbarBg,
    MsTrGCol_ScrollbarGrab,
    MsTrGCol_ScrollbarGrabHovered,
    MsTrGCol_ScrollbarGrabActive,
    MsTrGCol_CheckMark,
    MsTrGCol_SliderGrab,
    MsTrGCol_SliderGrabActive,
    MsTrGCol_Button,
    MsTrGCol_ButtonHovered,
    MsTrGCol_ButtonActive,
    MsTrGCol_Header,                // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
    MsTrGCol_HeaderHovered,
    MsTrGCol_HeaderActive,
    MsTrGCol_Separator,
    MsTrGCol_SeparatorHovered,
    MsTrGCol_SeparatorActive,
    MsTrGCol_ResizeGrip,            // Resize grip in lower-right and lower-left corners of windows.
    MsTrGCol_ResizeGripHovered,
    MsTrGCol_ResizeGripActive,
    MsTrGCol_Tab,                   // TabItem in a TabBar
    MsTrGCol_TabHovered,
    MsTrGCol_TabActive,
    MsTrGCol_TabUnfocused,
    MsTrGCol_TabUnfocusedActive,
    MsTrGCol_PlotLines,
    MsTrGCol_PlotLinesHovered,
    MsTrGCol_PlotHistogram,
    MsTrGCol_PlotHistogramHovered,
    MsTrGCol_TableHeaderBg,         // Table header background
    MsTrGCol_TableBorderStrong,     // Table outer and header borders (prefer using Alpha=1.0 here)
    MsTrGCol_TableBorderLight,      // Table inner borders (prefer using Alpha=1.0 here)
    MsTrGCol_TableRowBg,            // Table row background (even rows)
    MsTrGCol_TableRowBgAlt,         // Table row background (odd rows)
    MsTrGCol_TextSelectedBg,
    MsTrGCol_DragDropTarget,        // Rectangle highlighting a drop target
    MsTrGCol_NavHighlight,          // Gamepad/keyboard: current highlighted item
    MsTrGCol_NavWindowingHighlight, // Highlight window when using CTRL+TAB
    MsTrGCol_NavWindowingDimBg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
    MsTrGCol_ModalWindowDimBg,      // Darken/colorize entire screen behind a modal window, when one is active
    MsTrGCol_COUNT
};

// Enumeration for PushStyleVar() / PopStyleVar() to temporarily modify the MsTrGStyle structure.
// - The enum only refers to fields of MsTrGStyle which makes sense to be pushed/popped inside UI code.
//   During initialization or between frames, feel free to just poke into MsTrGStyle directly.
// - Tip: Use your programming IDE navigation facilities on the names in the _second column_ below to find the actual members and their description.
//   In Visual Studio IDE: CTRL+comma ("Edit.GoToAll") can follow symbols in comments, whereas CTRL+F12 ("Edit.GoToImplementation") cannot.
//   With Visual Assist installed: ALT+G ("VAssistX.GoToImplementation") can also follow symbols in comments.
// - When changing this enum, you need to update the associated internal table GStyleVarInfo[] accordingly. This is where we link enum values to members offset/type.
enum MsTrGStyleVar_
{
    // Enum name --------------------- // Member in MsTrGStyle structure (see MsTrGStyle for descriptions)
    MsTrGStyleVar_Alpha,               // float     Alpha
    MsTrGStyleVar_DisabledAlpha,       // float     DisabledAlpha
    MsTrGStyleVar_WindowPadding,       // ImVec2    WindowPadding
    MsTrGStyleVar_WindowRounding,      // float     WindowRounding
    MsTrGStyleVar_WindowBorderSize,    // float     WindowBorderSize
    MsTrGStyleVar_WindowMinSize,       // ImVec2    WindowMinSize
    MsTrGStyleVar_WindowTitleAlign,    // ImVec2    WindowTitleAlign
    MsTrGStyleVar_ChildRounding,       // float     ChildRounding
    MsTrGStyleVar_ChildBorderSize,     // float     ChildBorderSize
    MsTrGStyleVar_PopupRounding,       // float     PopupRounding
    MsTrGStyleVar_PopupBorderSize,     // float     PopupBorderSize
    MsTrGStyleVar_FramePadding,        // ImVec2    FramePadding
    MsTrGStyleVar_FrameRounding,       // float     FrameRounding
    MsTrGStyleVar_FrameBorderSize,     // float     FrameBorderSize
    MsTrGStyleVar_ItemSpacing,         // ImVec2    ItemSpacing
    MsTrGStyleVar_ItemInnerSpacing,    // ImVec2    ItemInnerSpacing
    MsTrGStyleVar_IndentSpacing,       // float     IndentSpacing
    MsTrGStyleVar_CellPadding,         // ImVec2    CellPadding
    MsTrGStyleVar_ScrollbarSize,       // float     ScrollbarSize
    MsTrGStyleVar_ScrollbarRounding,   // float     ScrollbarRounding
    MsTrGStyleVar_GrabMinSize,         // float     GrabMinSize
    MsTrGStyleVar_GrabRounding,        // float     GrabRounding
    MsTrGStyleVar_TabRounding,         // float     TabRounding
    MsTrGStyleVar_ButtonTextAlign,     // ImVec2    ButtonTextAlign
    MsTrGStyleVar_SelectableTextAlign, // ImVec2    SelectableTextAlign
    MsTrGStyleVar_COUNT
};

// Flags for InvisibleButton() [extended in MsTrG_internal.h]
enum MsTrGButtonFlags_
{
    MsTrGButtonFlags_None                   = 0,
    MsTrGButtonFlags_MouseButtonLeft        = 1 << 0,   // React on left mouse button (default)
    MsTrGButtonFlags_MouseButtonRight       = 1 << 1,   // React on right mouse button
    MsTrGButtonFlags_MouseButtonMiddle      = 1 << 2,   // React on center mouse button

    // [Internal]
    MsTrGButtonFlags_MouseButtonMask_       = MsTrGButtonFlags_MouseButtonLeft | MsTrGButtonFlags_MouseButtonRight | MsTrGButtonFlags_MouseButtonMiddle,
    MsTrGButtonFlags_MouseButtonDefault_    = MsTrGButtonFlags_MouseButtonLeft
};

// Flags for ColorEdit3() / ColorEdit4() / ColorPicker3() / ColorPicker4() / ColorButton()
enum MsTrGColorEditFlags_
{
    MsTrGColorEditFlags_None            = 0,
    MsTrGColorEditFlags_NoAlpha         = 1 << 1,   //              // ColorEdit, ColorPicker, ColorButton: ignore Alpha component (will only read 3 components from the input pointer).
    MsTrGColorEditFlags_NoPicker        = 1 << 2,   //              // ColorEdit: disable picker when clicking on color square.
    MsTrGColorEditFlags_NoOptions       = 1 << 3,   //              // ColorEdit: disable toggling options menu when right-clicking on inputs/small preview.
    MsTrGColorEditFlags_NoSmallPreview  = 1 << 4,   //              // ColorEdit, ColorPicker: disable color square preview next to the inputs. (e.g. to show only the inputs)
    MsTrGColorEditFlags_NoInputs        = 1 << 5,   //              // ColorEdit, ColorPicker: disable inputs sliders/text widgets (e.g. to show only the small preview color square).
    MsTrGColorEditFlags_NoTooltip       = 1 << 6,   //              // ColorEdit, ColorPicker, ColorButton: disable tooltip when hovering the preview.
    MsTrGColorEditFlags_NoLabel         = 1 << 7,   //              // ColorEdit, ColorPicker: disable display of inline text label (the label is still forwarded to the tooltip and picker).
    MsTrGColorEditFlags_NoSidePreview   = 1 << 8,   //              // ColorPicker: disable bigger color preview on right side of the picker, use small color square preview instead.
    MsTrGColorEditFlags_NoDragDrop      = 1 << 9,   //              // ColorEdit: disable drag and drop target. ColorButton: disable drag and drop source.
    MsTrGColorEditFlags_NoBorder        = 1 << 10,  //              // ColorButton: disable border (which is enforced by default)

    // User Options (right-click on widget to change some of them).
    MsTrGColorEditFlags_AlphaBar        = 1 << 16,  //              // ColorEdit, ColorPicker: show vertical alpha bar/gradient in picker.
    MsTrGColorEditFlags_AlphaPreview    = 1 << 17,  //              // ColorEdit, ColorPicker, ColorButton: display preview as a transparent color over a checkerboard, instead of opaque.
    MsTrGColorEditFlags_AlphaPreviewHalf= 1 << 18,  //              // ColorEdit, ColorPicker, ColorButton: display half opaque / half checkerboard, instead of opaque.
    MsTrGColorEditFlags_HDR             = 1 << 19,  //              // (WIP) ColorEdit: Currently only disable 0.0f..1.0f limits in RGBA edition (note: you probably want to use MsTrGColorEditFlags_Float flag as well).
    MsTrGColorEditFlags_DisplayRGB      = 1 << 20,  // [Display]    // ColorEdit: override _display_ type among RGB/HSV/Hex. ColorPicker: select any combination using one or more of RGB/HSV/Hex.
    MsTrGColorEditFlags_DisplayHSV      = 1 << 21,  // [Display]    // "
    MsTrGColorEditFlags_DisplayHex      = 1 << 22,  // [Display]    // "
    MsTrGColorEditFlags_Uint8           = 1 << 23,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0..255.
    MsTrGColorEditFlags_Float           = 1 << 24,  // [DataType]   // ColorEdit, ColorPicker, ColorButton: _display_ values formatted as 0.0f..1.0f floats instead of 0..255 integers. No round-trip of value via integers.
    MsTrGColorEditFlags_PickerHueBar    = 1 << 25,  // [Picker]     // ColorPicker: bar for Hue, rectangle for Sat/Value.
    MsTrGColorEditFlags_PickerHueWheel  = 1 << 26,  // [Picker]     // ColorPicker: wheel for Hue, triangle for Sat/Value.
    MsTrGColorEditFlags_InputRGB        = 1 << 27,  // [Input]      // ColorEdit, ColorPicker: input and output data in RGB format.
    MsTrGColorEditFlags_InputHSV        = 1 << 28,  // [Input]      // ColorEdit, ColorPicker: input and output data in HSV format.

    // Defaults Options. You can set application defaults using SetColorEditOptions(). The intent is that you probably don't want to
    // override them in most of your calls. Let the user choose via the option menu and/or call SetColorEditOptions() once during startup.
    MsTrGColorEditFlags_DefaultOptions_ = MsTrGColorEditFlags_Uint8 | MsTrGColorEditFlags_DisplayRGB | MsTrGColorEditFlags_InputRGB | MsTrGColorEditFlags_PickerHueBar,

    // [Internal] Masks
    MsTrGColorEditFlags_DisplayMask_    = MsTrGColorEditFlags_DisplayRGB | MsTrGColorEditFlags_DisplayHSV | MsTrGColorEditFlags_DisplayHex,
    MsTrGColorEditFlags_DataTypeMask_   = MsTrGColorEditFlags_Uint8 | MsTrGColorEditFlags_Float,
    MsTrGColorEditFlags_PickerMask_     = MsTrGColorEditFlags_PickerHueWheel | MsTrGColorEditFlags_PickerHueBar,
    MsTrGColorEditFlags_InputMask_      = MsTrGColorEditFlags_InputRGB | MsTrGColorEditFlags_InputHSV

    // Obsolete names (will be removed)
    // MsTrGColorEditFlags_RGB = MsTrGColorEditFlags_DisplayRGB, MsTrGColorEditFlags_HSV = MsTrGColorEditFlags_DisplayHSV, MsTrGColorEditFlags_HEX = MsTrGColorEditFlags_DisplayHex  // [renamed in 1.69]
};

// Flags for DragFloat(), DragInt(), SliderFloat(), SliderInt() etc.
// We use the same sets of flags for DragXXX() and SliderXXX() functions as the features are the same and it makes it easier to swap them.
enum MsTrGSliderFlags_
{
    MsTrGSliderFlags_None                   = 0,
    MsTrGSliderFlags_AlwaysClamp            = 1 << 4,       // Clamp value to min/max bounds when input manually with CTRL+Click. By default CTRL+Click allows going out of bounds.
    MsTrGSliderFlags_Logarithmic            = 1 << 5,       // Make the widget logarithmic (linear otherwise). Consider using MsTrGSliderFlags_NoRoundToFormat with this if using a format-string with small amount of digits.
    MsTrGSliderFlags_NoRoundToFormat        = 1 << 6,       // Disable rounding underlying value to match precision of the display format string (e.g. %.3f values are rounded to those 3 digits)
    MsTrGSliderFlags_NoInput                = 1 << 7,       // Disable CTRL+Click or Enter key allowing to input text directly into the widget
    MsTrGSliderFlags_InvalidMask_           = 0x7000000F    // [Internal] We treat using those bits as being potentially a 'float power' argument from the previous API that has got miscast to this enum, and will trigger an assert if needed.

    // Obsolete names (will be removed)
#ifndef MsTrG_DISABLE_OBSOLETE_FUNCTIONS
    , MsTrGSliderFlags_ClampOnInput = MsTrGSliderFlags_AlwaysClamp // [renamed in 1.79]
#endif
};

// Identify a mouse button.
// Those values are guaranteed to be stable and we frequently use 0/1 directly. Named enums provided for convenience.
enum MsTrGMouseButton_
{
    MsTrGMouseButton_Left = 0,
    MsTrGMouseButton_Right = 1,
    MsTrGMouseButton_Middle = 2,
    MsTrGMouseButton_COUNT = 5
};

// Enumeration for GetMouseCursor()
// User code may request backend to display given cursor by calling SetMouseCursor(), which is why we have some cursors that are marked unused here
enum MsTrGMouseCursor_
{
    MsTrGMouseCursor_None = -1,
    MsTrGMouseCursor_Arrow = 0,
    MsTrGMouseCursor_TextInput,         // When hovering over InputText, etc.
    MsTrGMouseCursor_ResizeAll,         // (Unused by Dear MsTrG functions)
    MsTrGMouseCursor_ResizeNS,          // When hovering over an horizontal border
    MsTrGMouseCursor_ResizeEW,          // When hovering over a vertical border or a column
    MsTrGMouseCursor_ResizeNESW,        // When hovering over the bottom-left corner of a window
    MsTrGMouseCursor_ResizeNWSE,        // When hovering over the bottom-right corner of a window
    MsTrGMouseCursor_Hand,              // (Unused by Dear MsTrG functions. Use for e.g. hyperlinks)
    MsTrGMouseCursor_NotAllowed,        // When hovering something with disallowed interaction. Usually a crossed circle.
    MsTrGMouseCursor_COUNT
};

// Enumeration for MsTrG::SetWindow***(), SetNextWindow***(), SetNextItem***() functions
// Represent a condition.
// Important: Treat as a regular enum! Do NOT combine multiple values using binary operators! All the functions above treat 0 as a shortcut to MsTrGCond_Always.
enum MsTrGCond_
{
    MsTrGCond_None          = 0,        // No condition (always set the variable), same as _Always
    MsTrGCond_Always        = 1 << 0,   // No condition (always set the variable)
    MsTrGCond_Once          = 1 << 1,   // Set the variable once per runtime session (only the first call will succeed)
    MsTrGCond_FirstUseEver  = 1 << 2,   // Set the variable if the object/window has no persistently saved data (no entry in .ini file)
    MsTrGCond_Appearing     = 1 << 3    // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)
};

//-----------------------------------------------------------------------------
// [SECTION] Helpers: Memory allocations macros, ImVector<>
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// IM_MALLOC(), IM_FREE(), IM_NEW(), IM_PLACEMENT_NEW(), IM_DELETE()
// We call C++ constructor on own allocated memory via the placement "new(ptr) Type()" syntax.
// Defining a custom placement new() with a custom parameter allows us to bypass including <new> which on some platforms complains when user has disabled exceptions.
//-----------------------------------------------------------------------------

struct ImNewWrapper {};
inline void* operator new(size_t, ImNewWrapper, void* ptr) { return ptr; }
inline void  operator delete(void*, ImNewWrapper, void*)   {} // This is only required so we can use the symmetrical new()
#define IM_ALLOC(_SIZE)                     MsTrG::MemAlloc(_SIZE)
#define IM_FREE(_PTR)                       MsTrG::MemFree(_PTR)
#define IM_PLACEMENT_NEW(_PTR)              new(ImNewWrapper(), _PTR)
#define IM_NEW(_TYPE)                       new(ImNewWrapper(), MsTrG::MemAlloc(sizeof(_TYPE))) _TYPE
template<typename T> void IM_DELETE(T* p)   { if (p) { p->~T(); MsTrG::MemFree(p); } }

//-----------------------------------------------------------------------------
// ImVector<>
// Lightweight std::vector<>-like class to avoid dragging dependencies (also, some implementations of STL with debug enabled are absurdly slow, we bypass it so our code runs fast in debug).
//-----------------------------------------------------------------------------
// - You generally do NOT need to care or use this ever. But we need to make it available in MsTrG.h because some of our public structures are relying on it.
// - We use std-like naming convention here, which is a little unusual for this codebase.
// - Important: clear() frees memory, resize(0) keep the allocated buffer. We use resize(0) a lot to intentionally recycle allocated buffers across frames and amortize our costs.
// - Important: our implementation does NOT call C++ constructors/destructors, we treat everything as raw data! This is intentional but be extra mindful of that,
//   Do NOT use this class as a std::vector replacement in your own code! Many of the structures used by dear MsTrG can be safely initialized by a zero-memset.
//-----------------------------------------------------------------------------

IM_MSVC_RUNTIME_CHECKS_OFF
template<typename T>
struct ImVector
{
    int                 Size;
    int                 Capacity;
    T*                  Data;

    // Provide standard typedefs but we don't use them ourselves.
    typedef T                   value_type;
    typedef value_type*         iterator;
    typedef const value_type*   const_iterator;

    // Constructors, destructor
    inline ImVector()                                       { Size = Capacity = 0; Data = NULL; }
    inline ImVector(const ImVector<T>& src)                 { Size = Capacity = 0; Data = NULL; operator=(src); }
    inline ImVector<T>& operator=(const ImVector<T>& src)   { clear(); resize(src.Size); memcpy(Data, src.Data, (size_t)Size * sizeof(T)); return *this; }
    inline ~ImVector()                                      { if (Data) IM_FREE(Data); } // Important: does not destruct anything

    inline void         clear()                             { if (Data) { Size = Capacity = 0; IM_FREE(Data); Data = NULL; } }  // Important: does not destruct anything
    inline void         clear_delete()                      { for (int n = 0; n < Size; n++) IM_DELETE(Data[n]); clear(); }     // Important: never called automatically! always explicit.
    inline void         clear_destruct()                    { for (int n = 0; n < Size; n++) Data[n].~T(); clear(); }           // Important: never called automatically! always explicit.

    inline bool         empty() const                       { return Size == 0; }
    inline int          size() const                        { return Size; }
    inline int          size_in_bytes() const               { return Size * (int)sizeof(T); }
    inline int          max_size() const                    { return 0x7FFFFFFF / (int)sizeof(T); }
    inline int          capacity() const                    { return Capacity; }
    inline T&           operator[](int i)                   { IM_ASSERT(i >= 0 && i < Size); return Data[i]; }
    inline const T&     operator[](int i) const             { IM_ASSERT(i >= 0 && i < Size); return Data[i]; }

    inline T*           begin()                             { return Data; }
    inline const T*     begin() const                       { return Data; }
    inline T*           end()                               { return Data + Size; }
    inline const T*     end() const                         { return Data + Size; }
    inline T&           front()                             { IM_ASSERT(Size > 0); return Data[0]; }
    inline const T&     front() const                       { IM_ASSERT(Size > 0); return Data[0]; }
    inline T&           back()                              { IM_ASSERT(Size > 0); return Data[Size - 1]; }
    inline const T&     back() const                        { IM_ASSERT(Size > 0); return Data[Size - 1]; }
    inline void         swap(ImVector<T>& rhs)              { int rhs_size = rhs.Size; rhs.Size = Size; Size = rhs_size; int rhs_cap = rhs.Capacity; rhs.Capacity = Capacity; Capacity = rhs_cap; T* rhs_data = rhs.Data; rhs.Data = Data; Data = rhs_data; }

    inline int          _grow_capacity(int sz) const        { int new_capacity = Capacity ? (Capacity + Capacity / 2) : 8; return new_capacity > sz ? new_capacity : sz; }
    inline void         resize(int new_size)                { if (new_size > Capacity) reserve(_grow_capacity(new_size)); Size = new_size; }
    inline void         resize(int new_size, const T& v)    { if (new_size > Capacity) reserve(_grow_capacity(new_size)); if (new_size > Size) for (int n = Size; n < new_size; n++) memcpy(&Data[n], &v, sizeof(v)); Size = new_size; }
    inline void         shrink(int new_size)                { IM_ASSERT(new_size <= Size); Size = new_size; } // Resize a vector to a smaller size, guaranteed not to cause a reallocation
    inline void         reserve(int new_capacity)           { if (new_capacity <= Capacity) return; T* new_data = (T*)IM_ALLOC((size_t)new_capacity * sizeof(T)); if (Data) { memcpy(new_data, Data, (size_t)Size * sizeof(T)); IM_FREE(Data); } Data = new_data; Capacity = new_capacity; }
    inline void         reserve_discard(int new_capacity)   { if (new_capacity <= Capacity) return; if (Data) IM_FREE(Data); Data = (T*)IM_ALLOC((size_t)new_capacity * sizeof(T)); Capacity = new_capacity; }

    // NB: It is illegal to call push_back/push_front/insert with a reference pointing inside the ImVector data itself! e.g. v.push_back(v[10]) is forbidden.
    inline void         push_back(const T& v)               { if (Size == Capacity) reserve(_grow_capacity(Size + 1)); memcpy(&Data[Size], &v, sizeof(v)); Size++; }
    inline void         pop_back()                          { IM_ASSERT(Size > 0); Size--; }
    inline void         push_front(const T& v)              { if (Size == 0) push_back(v); else insert(Data, v); }
    inline T*           erase(const T* it)                  { IM_ASSERT(it >= Data && it < Data + Size); const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + 1, ((size_t)Size - (size_t)off - 1) * sizeof(T)); Size--; return Data + off; }
    inline T*           erase(const T* it, const T* it_last){ IM_ASSERT(it >= Data && it < Data + Size && it_last >= it && it_last <= Data + Size); const ptrdiff_t count = it_last - it; const ptrdiff_t off = it - Data; memmove(Data + off, Data + off + count, ((size_t)Size - (size_t)off - (size_t)count) * sizeof(T)); Size -= (int)count; return Data + off; }
    inline T*           erase_unsorted(const T* it)         { IM_ASSERT(it >= Data && it < Data + Size);  const ptrdiff_t off = it - Data; if (it < Data + Size - 1) memcpy(Data + off, Data + Size - 1, sizeof(T)); Size--; return Data + off; }
    inline T*           insert(const T* it, const T& v)     { IM_ASSERT(it >= Data && it <= Data + Size); const ptrdiff_t off = it - Data; if (Size == Capacity) reserve(_grow_capacity(Size + 1)); if (off < (int)Size) memmove(Data + off + 1, Data + off, ((size_t)Size - (size_t)off) * sizeof(T)); memcpy(&Data[off], &v, sizeof(v)); Size++; return Data + off; }
    inline bool         contains(const T& v) const          { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data++ == v) return true; return false; }
    inline T*           find(const T& v)                    { T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data == v) break; else ++data; return data; }
    inline const T*     find(const T& v) const              { const T* data = Data;  const T* data_end = Data + Size; while (data < data_end) if (*data == v) break; else ++data; return data; }
    inline bool         find_erase(const T& v)              { const T* it = find(v); if (it < Data + Size) { erase(it); return true; } return false; }
    inline bool         find_erase_unsorted(const T& v)     { const T* it = find(v); if (it < Data + Size) { erase_unsorted(it); return true; } return false; }
    inline int          index_from_ptr(const T* it) const   { IM_ASSERT(it >= Data && it < Data + Size); const ptrdiff_t off = it - Data; return (int)off; }
};
IM_MSVC_RUNTIME_CHECKS_RESTORE

//-----------------------------------------------------------------------------
// [SECTION] MsTrGStyle
//-----------------------------------------------------------------------------
// You may modify the MsTrG::GetStyle() main instance during initialization and before NewFrame().
// During the frame, use MsTrG::PushStyleVar(MsTrGStyleVar_XXXX)/PopStyleVar() to alter the main style values,
// and MsTrG::PushStyleColor(MsTrGCol_XXX)/PopStyleColor() for colors.
//-----------------------------------------------------------------------------

struct MsTrGStyle
{
    float       Alpha;                      // Global alpha applies to everything in Dear MsTrG.
    float       DisabledAlpha;              // Additional alpha multiplier applied by BeginDisabled(). Multiply over current value of Alpha.
    ImVec2      WindowPadding;              // Padding within a window.
    float       WindowRounding;             // Radius of window corners rounding. Set to 0.0f to have rectangular windows. Large values tend to lead to variety of artifacts and are not recommended.
    float       WindowBorderSize;           // Thickness of border around windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    ImVec2      WindowMinSize;              // Minimum window size. This is a global setting. If you want to constraint individual windows, use SetNextWindowSizeConstraints().
    ImVec2      WindowTitleAlign;           // Alignment for title bar text. Defaults to (0.0f,0.5f) for left-aligned,vertically centered.
    MsTrGDir    WindowMenuButtonPosition;   // Side of the collapsing/docking button in the title bar (None/Left/Right). Defaults to MsTrGDir_Left.
    float       ChildRounding;              // Radius of child window corners rounding. Set to 0.0f to have rectangular windows.
    float       ChildBorderSize;            // Thickness of border around child windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    float       PopupRounding;              // Radius of popup window corners rounding. (Note that tooltip windows use WindowRounding)
    float       PopupBorderSize;            // Thickness of border around popup/tooltip windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    ImVec2      FramePadding;               // Padding within a framed rectangle (used by most widgets).
    float       FrameRounding;              // Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets).
    float       FrameBorderSize;            // Thickness of border around frames. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly).
    ImVec2      ItemSpacing;                // Horizontal and vertical spacing between widgets/lines.
    ImVec2      ItemInnerSpacing;           // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label).
    ImVec2      CellPadding;                // Padding within a table cell
    ImVec2      TouchExtraPadding;          // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
    float       IndentSpacing;              // Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
    float       ColumnsMinSpacing;          // Minimum horizontal spacing between two columns. Preferably > (FramePadding.x + 1).
    float       ScrollbarSize;              // Width of the vertical scrollbar, Height of the horizontal scrollbar.
    float       ScrollbarRounding;          // Radius of grab corners for scrollbar.
    float       GrabMinSize;                // Minimum width/height of a grab box for slider/scrollbar.
    float       GrabRounding;               // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
    float       LogSliderDeadzone;          // The size in pixels of the dead-zone around zero on logarithmic sliders that cross zero.
    float       TabRounding;                // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
    float       TabBorderSize;              // Thickness of border around tabs.
    float       TabMinWidthForCloseButton;  // Minimum width for close button to appears on an unselected tab when hovered. Set to 0.0f to always show when hovering, set to FLT_MAX to never show close button unless selected.
    MsTrGDir    ColorButtonPosition;        // Side of the color button in the ColorEdit4 widget (left/right). Defaults to MsTrGDir_Right.
    ImVec2      ButtonTextAlign;            // Alignment of button text when button is larger than text. Defaults to (0.5f, 0.5f) (centered).
    ImVec2      SelectableTextAlign;        // Alignment of selectable text. Defaults to (0.0f, 0.0f) (top-left aligned). It's generally important to keep this left-aligned if you want to lay multiple items on a same line.
    ImVec2      DisplayWindowPadding;       // Window position are clamped to be visible within the display area or monitors by at least this amount. Only applies to regular windows.
    ImVec2      DisplaySafeAreaPadding;     // If you cannot see the edges of your screen (e.g. on a TV) increase the safe area padding. Apply to popups/tooltips as well regular windows. NB: Prefer configuring your TV sets correctly!
    float       MouseCursorScale;           // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). May be removed later.
    bool        AntiAliasedLines;           // Enable anti-aliased lines/borders. Disable if you are really tight on CPU/GPU. Latched at the beginning of the frame (copied to ImDrawList).
    bool        AntiAliasedLinesUseTex;     // Enable anti-aliased lines/borders using textures where possible. Require backend to render with bilinear filtering (NOT point/nearest filtering). Latched at the beginning of the frame (copied to ImDrawList).
    bool        AntiAliasedFill;            // Enable anti-aliased edges around filled shapes (rounded rectangles, circles, etc.). Disable if you are really tight on CPU/GPU. Latched at the beginning of the frame (copied to ImDrawList).
    float       CurveTessellationTol;       // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
    float       CircleTessellationMaxError; // Maximum error (in pixels) allowed when using AddCircle()/AddCircleFilled() or drawing rounded corner rectangles with no explicit segment count specified. Decrease for higher quality but more geometry.
    ImVec4      Colors[MsTrGCol_COUNT];

    MsTrG_API MsTrGStyle();
    MsTrG_API void ScaleAllSizes(float scale_factor);
};

//-----------------------------------------------------------------------------
// [SECTION] MsTrGIO
//-----------------------------------------------------------------------------
// Communicate most settings and inputs/outputs to Dear MsTrG using this structure.
// Access via MsTrG::GetIO(). Read 'Programmer guide' section in .cpp file for general usage.
//-----------------------------------------------------------------------------

// [Internal] Storage used by IsKeyDown(), IsKeyPressed() etc functions.
// If prior to 1.87 you used io.KeysDownDuration[] (which was marked as internal), you should use GetKeyData(key)->DownDuration and not io.KeysData[key]->DownDuration.
struct MsTrGKeyData
{
    bool        Down;               // True for if key is down
    float       DownDuration;       // Duration the key has been down (<0.0f: not pressed, 0.0f: just pressed, >0.0f: time held)
    float       DownDurationPrev;   // Last frame duration the key has been down
    float       AnalogValue;        // 0.0f..1.0f for gamepad values
};

struct MsTrGIO
{
    //------------------------------------------------------------------
    // Configuration                            // Default value
    //------------------------------------------------------------------

    MsTrGConfigFlags   ConfigFlags;             // = 0              // See MsTrGConfigFlags_ enum. Set by user/application. Gamepad/keyboard navigation options, etc.
    MsTrGBackendFlags  BackendFlags;            // = 0              // See MsTrGBackendFlags_ enum. Set by backend (MsTrG_impl_xxx files or custom backend) to communicate features supported by the backend.
    ImVec2      DisplaySize;                    // <unset>          // Main display size, in pixels (generally == GetMainViewport()->Size). May change every frame.
    float       DeltaTime;                      // = 1.0f/60.0f     // Time elapsed since last frame, in seconds. May change every frame.
    float       IniSavingRate;                  // = 5.0f           // Minimum time between saving positions/sizes to .ini file, in seconds.
    const char* IniFilename;                    // = "MsTrG.ini"    // Path to .ini file (important: default "MsTrG.ini" is relative to current working dir!). Set NULL to disable automatic .ini loading/saving or if you want to manually call LoadIniSettingsXXX() / SaveIniSettingsXXX() functions.
    const char* LogFilename;                    // = "MsTrG_log.txt"// Path to .log file (default parameter to MsTrG::LogToFile when no file is specified).
    float       MouseDoubleClickTime;           // = 0.30f          // Time for a double-click, in seconds.
    float       MouseDoubleClickMaxDist;        // = 6.0f           // Distance threshold to stay in to validate a double-click, in pixels.
    float       MouseDragThreshold;             // = 6.0f           // Distance threshold before considering we are dragging.
    float       KeyRepeatDelay;                 // = 0.250f         // When holding a key/button, time before it starts repeating, in seconds (for buttons in Repeat mode, etc.).
    float       KeyRepeatRate;                  // = 0.050f         // When holding a key/button, rate at which it repeats, in seconds.
    void*       UserData;                       // = NULL           // Store your own data for retrieval by callbacks.

    ImFontAtlas*Fonts;                          // <auto>           // Font atlas: load, rasterize and pack one or more fonts into a single texture.
    float       FontGlobalScale;                // = 1.0f           // Global scale all fonts
    bool        FontAllowUserScaling;           // = false          // Allow user scaling text of individual window with CTRL+Wheel.
    ImFont*     FontDefault;                    // = NULL           // Font to use on NewFrame(). Use NULL to uses Fonts->Fonts[0].
    ImVec2      DisplayFramebufferScale;        // = (1, 1)         // For retina display or other situations where window coordinates are different from framebuffer coordinates. This generally ends up in ImDrawData::FramebufferScale.

    // Miscellaneous options
    bool        MouseDrawCursor;                // = false          // Request MsTrG to draw a mouse cursor for you (if you are on a platform without a mouse cursor). Cannot be easily renamed to 'io.ConfigXXX' because this is frequently used by backend implementations.
    bool        ConfigMacOSXBehaviors;          // = defined(__APPLE__) // OS X style: Text editing cursor movement using Alt instead of Ctrl, Shortcuts using Cmd/Super instead of Ctrl, Line/Text Start and End using Cmd+Arrows instead of Home/End, Double click selects by word instead of selecting whole text, Multi-selection in lists uses Cmd/Super instead of Ctrl.
    bool        ConfigInputTrickleEventQueue;   // = true           // Enable input queue trickling: some types of events submitted during the same frame (e.g. button down + up) will be spread over multiple frames, improving interactions with low framerates.
    bool        ConfigInputTextCursorBlink;     // = true           // Enable blinking cursor (optional as some users consider it to be distracting).
    bool        ConfigDragClickToInputText;     // = false          // [BETA] Enable turning DragXXX widgets into text input with a simple mouse click-release (without moving). Not desirable on devices without a keyboard.
    bool        ConfigWindowsResizeFromEdges;   // = true           // Enable resizing of windows from their edges and from the lower-left corner. This requires (io.BackendFlags & MsTrGBackendFlags_HasMouseCursors) because it needs mouse cursor feedback. (This used to be a per-window MsTrGWindowFlags_ResizeFromAnySide flag)
    bool        ConfigWindowsMoveFromTitleBarOnly; // = false       // Enable allowing to move windows only when clicking on their title bar. Does not apply to windows without a title bar.
    float       ConfigMemoryCompactTimer;       // = 60.0f          // Timer (in seconds) to free transient windows/tables memory buffers when unused. Set to -1.0f to disable.

    //------------------------------------------------------------------
    // Platform Functions
    // (the MsTrG_impl_xxxx backend files are setting those up for you)
    //------------------------------------------------------------------

    // Optional: Platform/Renderer backend name (informational only! will be displayed in About Window) + User data for backend/wrappers to store their own stuff.
    const char* BackendPlatformName;            // = NULL
    const char* BackendRendererName;            // = NULL
    void*       BackendPlatformUserData;        // = NULL           // User data for platform backend
    void*       BackendRendererUserData;        // = NULL           // User data for renderer backend
    void*       BackendLanguageUserData;        // = NULL           // User data for non C++ programming language backend

    // Optional: Access OS clipboard
    // (default to use native Win32 clipboard on Windows, otherwise uses a private clipboard. Override to access OS clipboard on other architectures)
    const char* (*GetClipboardTextFn)(void* user_data);
    void        (*SetClipboardTextFn)(void* user_data, const char* text);
    void*       ClipboardUserData;

    // Optional: Notify OS Input Method Editor of the screen position of your cursor for text input position (e.g. when using Japanese/Chinese IME on Windows)
    // (default to use native imm32 api on Windows)
    void        (*SetPlatformImeDataFn)(MsTrGViewport* viewport, MsTrGPlatformImeData* data);
#ifndef MsTrG_DISABLE_OBSOLETE_FUNCTIONS
    void*       ImeWindowHandle;                // = NULL           // [Obsolete] Set MsTrGViewport::PlatformHandleRaw instead. Set this to your HWND to get automatic IME cursor positioning.
#else
    void*       _UnusedPadding;                                     // Unused field to keep data structure the same size.
#endif

    //------------------------------------------------------------------
    // Input - Call before calling NewFrame()
    //------------------------------------------------------------------

    // Input Functions
    MsTrG_API void  AddKeyEvent(MsTrGKey key, bool down);                   // Queue a new key down/up event. Key should be "translated" (as in, generally MsTrGKey_A matches the key end-user would use to emit an 'A' character)
    MsTrG_API void  AddKeyAnalogEvent(MsTrGKey key, bool down, float v);    // Queue a new key down/up event for analog values (e.g. MsTrGKey_Gamepad_ values). Dead-zones should be handled by the backend.
    MsTrG_API void  AddMousePosEvent(float x, float y);                     // Queue a mouse position update. Use -FLT_MAX,-FLT_MAX to signify no mouse (e.g. app not focused and not hovered)
    MsTrG_API void  AddMouseButtonEvent(int button, bool down);             // Queue a mouse button change
    MsTrG_API void  AddMouseWheelEvent(float wh_x, float wh_y);             // Queue a mouse wheel update
    MsTrG_API void  AddFocusEvent(bool focused);                            // Queue a gain/loss of focus for the application (generally based on OS/platform focus of your window)
    MsTrG_API void  AddInputCharacter(unsigned int c);                      // Queue a new character input
    MsTrG_API void  AddInputCharacterUTF16(ImWchar16 c);                    // Queue a new character input from an UTF-16 character, it can be a surrogate
    MsTrG_API void  AddInputCharactersUTF8(const char* str);                // Queue a new characters input from an UTF-8 string

    MsTrG_API void  SetKeyEventNativeData(MsTrGKey key, int native_keycode, int native_scancode, int native_legacy_index = -1); // [Optional] Specify index for legacy <1.87 IsKeyXXX() functions with native indices + specify native keycode, scancode.
    MsTrG_API void  SetAppAcceptingEvents(bool accepting_events);           // Set master flag for accepting key/mouse/text events (default to true). Useful if you have native dialog boxes that are interrupting your application loop/refresh, and you want to disable events being queued while your app is frozen.
    MsTrG_API void  ClearInputCharacters();                                 // [Internal] Clear the text input buffer manually
    MsTrG_API void  ClearInputKeys();                                       // [Internal] Release all keys

    //------------------------------------------------------------------
    // Output - Updated by NewFrame() or EndFrame()/Render()
    // (when reading from the io.WantCaptureMouse, io.WantCaptureKeyboard flags to dispatch your inputs, it is
    //  generally easier and more correct to use their state BEFORE calling NewFrame(). See FAQ for details!)
    //------------------------------------------------------------------

    bool        WantCaptureMouse;                   // Set when Dear MsTrG will use mouse inputs, in this case do not dispatch them to your main game/application (either way, always pass on mouse inputs to MsTrG). (e.g. unclicked mouse is hovering over an MsTrG window, widget is active, mouse was clicked over an MsTrG window, etc.).
    bool        WantCaptureKeyboard;                // Set when Dear MsTrG will use keyboard inputs, in this case do not dispatch them to your main game/application (either way, always pass keyboard inputs to MsTrG). (e.g. InputText active, or an MsTrG window is focused and navigation is enabled, etc.).
    bool        WantTextInput;                      // Mobile/console: when set, you may display an on-screen keyboard. This is set by Dear MsTrG when it wants textual keyboard input to happen (e.g. when a InputText widget is active).
    bool        WantSetMousePos;                    // MousePos has been altered, backend should reposition mouse on next frame. Rarely used! Set only when MsTrGConfigFlags_NavEnableSetMousePos flag is enabled.
    bool        WantSaveIniSettings;                // When manual .ini load/save is active (io.IniFilename == NULL), this will be set to notify your application that you can call SaveIniSettingsToMemory() and save yourself. Important: clear io.WantSaveIniSettings yourself after saving!
    bool        NavActive;                          // Keyboard/Gamepad navigation is currently allowed (will handle MsTrGKey_NavXXX events) = a window is focused and it doesn't use the MsTrGWindowFlags_NoNavInputs flag.
    bool        NavVisible;                         // Keyboard/Gamepad navigation is visible and allowed (will handle MsTrGKey_NavXXX events).
    float       Framerate;                          // Rough estimate of application framerate, in frame per second. Solely for convenience. Rolling average estimation based on io.DeltaTime over 120 frames.
    int         MetricsRenderVertices;              // Vertices output during last call to Render()
    int         MetricsRenderIndices;               // Indices output during last call to Render() = number of triangles * 3
    int         MetricsRenderWindows;               // Number of visible windows
    int         MetricsActiveWindows;               // Number of active windows
    int         MetricsActiveAllocations;           // Number of active allocations, updated by MemAlloc/MemFree based on current context. May be off if you have multiple MsTrG contexts.
    ImVec2      MouseDelta;                         // Mouse delta. Note that this is zero if either current or previous position are invalid (-FLT_MAX,-FLT_MAX), so a disappearing/reappearing mouse won't have a huge delta.

    // Legacy: before 1.87, we required backend to fill io.KeyMap[] (MsTrG->native map) during initialization and io.KeysDown[] (native indices) every frame.
    // This is still temporarily supported as a legacy feature. However the new preferred scheme is for backend to call io.AddKeyEvent().
#ifndef MsTrG_DISABLE_OBSOLETE_KEYIO
    int         KeyMap[MsTrGKey_COUNT];             // [LEGACY] Input: map of indices into the KeysDown[512] entries array which represent your "native" keyboard state. The first 512 are now unused and should be kept zero. Legacy backend will write into KeyMap[] using MsTrGKey_ indices which are always >512.
    bool        KeysDown[MsTrGKey_COUNT];           // [LEGACY] Input: Keyboard keys that are pressed (ideally left in the "native" order your engine has access to keyboard keys, so you can use your own defines/enums for keys). This used to be [512] sized. It is now MsTrGKey_COUNT to allow legacy io.KeysDown[GetKeyIndex(...)] to work without an overflow.
#endif

    //------------------------------------------------------------------
    // [Internal] Dear MsTrG will maintain those fields. Forward compatibility not guaranteed!
    //------------------------------------------------------------------

    // Main Input State
    // (this block used to be written by backend, since 1.87 it is best to NOT write to those directly, call the AddXXX functions above instead)
    // (reading from those variables is fair game, as they are extremely unlikely to be moving anywhere)
    ImVec2      MousePos;                           // Mouse position, in pixels. Set to ImVec2(-FLT_MAX, -FLT_MAX) if mouse is unavailable (on another screen, etc.)
    bool        MouseDown[5];                       // Mouse buttons: 0=left, 1=right, 2=middle + extras (MsTrGMouseButton_COUNT == 5). Dear MsTrG mostly uses left and right buttons. Others buttons allows us to track if the mouse is being used by your application + available to user as a convenience via IsMouse** API.
    float       MouseWheel;                         // Mouse wheel Vertical: 1 unit scrolls about 5 lines text.
    float       MouseWheelH;                        // Mouse wheel Horizontal. Most users don't have a mouse with an horizontal wheel, may not be filled by all backends.
    bool        KeyCtrl;                            // Keyboard modifier down: Control
    bool        KeyShift;                           // Keyboard modifier down: Shift
    bool        KeyAlt;                             // Keyboard modifier down: Alt
    bool        KeySuper;                           // Keyboard modifier down: Cmd/Super/Windows
    float       NavInputs[MsTrGNavInput_COUNT];     // Gamepad inputs. Cleared back to zero by EndFrame(). Keyboard keys will be auto-mapped and be written here by NewFrame().

    // Other state maintained from data above + IO function calls
    MsTrGModFlags KeyMods;                          // Key mods flags (same as io.KeyCtrl/KeyShift/KeyAlt/KeySuper but merged into flags), updated by NewFrame()
    MsTrGKeyData KeysData[MsTrGKey_KeysData_SIZE];  // Key state for all known keys. Use IsKeyXXX() functions to access this.
    bool        WantCaptureMouseUnlessPopupClose;   // Alternative to WantCaptureMouse: (WantCaptureMouse == true && WantCaptureMouseUnlessPopupClose == false) when a click over void is expected to close a popup.
    ImVec2      MousePosPrev;                       // Previous mouse position (note that MouseDelta is not necessary == MousePos-MousePosPrev, in case either position is invalid)
    ImVec2      MouseClickedPos[5];                 // Position at time of clicking
    double      MouseClickedTime[5];                // Time of last click (used to figure out double-click)
    bool        MouseClicked[5];                    // Mouse button went from !Down to Down (same as MouseClickedCount[x] != 0)
    bool        MouseDoubleClicked[5];              // Has mouse button been double-clicked? (same as MouseClickedCount[x] == 2)
    ImU16       MouseClickedCount[5];               // == 0 (not clicked), == 1 (same as MouseClicked[]), == 2 (double-clicked), == 3 (triple-clicked) etc. when going from !Down to Down
    ImU16       MouseClickedLastCount[5];           // Count successive number of clicks. Stays valid after mouse release. Reset after another click is done.
    bool        MouseReleased[5];                   // Mouse button went from Down to !Down
    bool        MouseDownOwned[5];                  // Track if button was clicked inside a dear MsTrG window or over void blocked by a popup. We don't request mouse capture from the application if click started outside MsTrG bounds.
    bool        MouseDownOwnedUnlessPopupClose[5];  // Track if button was clicked inside a dear MsTrG window.
    float       MouseDownDuration[5];               // Duration the mouse button has been down (0.0f == just clicked)
    float       MouseDownDurationPrev[5];           // Previous time the mouse button has been down
    float       MouseDragMaxDistanceSqr[5];         // Squared maximum distance of how much mouse has traveled from the clicking point (used for moving thresholds)
    float       NavInputsDownDuration[MsTrGNavInput_COUNT];
    float       NavInputsDownDurationPrev[MsTrGNavInput_COUNT];
    float       PenPressure;                        // Touch/Pen pressure (0.0f to 1.0f, should be >0.0f only when MouseDown[0] == true). Helper storage currently unused by Dear MsTrG.
    bool        AppFocusLost;                       // Only modify via AddFocusEvent()
    bool        AppAcceptingEvents;                 // Only modify via SetAppAcceptingEvents()
    ImS8        BackendUsingLegacyKeyArrays;        // -1: unknown, 0: using AddKeyEvent(), 1: using legacy io.KeysDown[]
    bool        BackendUsingLegacyNavInputArray;    // 0: using AddKeyAnalogEvent(), 1: writing to legacy io.NavInputs[] directly
    ImWchar16   InputQueueSurrogate;                // For AddInputCharacterUTF16()
    ImVector<ImWchar> InputQueueCharacters;         // Queue of _characters_ input (obtained by platform backend). Fill using AddInputCharacter() helper.

    MsTrG_API   MsTrGIO();
};

//-----------------------------------------------------------------------------
// [SECTION] Misc data structures
//-----------------------------------------------------------------------------

// Shared state of InputText(), passed as an argument to your callback when a MsTrGInputTextFlags_Callback* flag is used.
// The callback function should return 0 by default.
// Callbacks (follow a flag name and see comments in MsTrGInputTextFlags_ declarations for more details)
// - MsTrGInputTextFlags_CallbackEdit:        Callback on buffer edit (note that InputText() already returns true on edit, the callback is useful mainly to manipulate the underlying buffer while focus is active)
// - MsTrGInputTextFlags_CallbackAlways:      Callback on each iteration
// - MsTrGInputTextFlags_CallbackCompletion:  Callback on pressing TAB
// - MsTrGInputTextFlags_CallbackHistory:     Callback on pressing Up/Down arrows
// - MsTrGInputTextFlags_CallbackCharFilter:  Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
// - MsTrGInputTextFlags_CallbackResize:      Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow.
struct MsTrGInputTextCallbackData
{
    MsTrGInputTextFlags EventFlag;      // One MsTrGInputTextFlags_Callback*    // Read-only
    MsTrGInputTextFlags Flags;          // What user passed to InputText()      // Read-only
    void*               UserData;       // What user passed to InputText()      // Read-only

    // Arguments for the different callback events
    // - To modify the text buffer in a callback, prefer using the InsertChars() / DeleteChars() function. InsertChars() will take care of calling the resize callback if necessary.
    // - If you know your edits are not going to resize the underlying buffer allocation, you may modify the contents of 'Buf[]' directly. You need to update 'BufTextLen' accordingly (0 <= BufTextLen < BufSize) and set 'BufDirty'' to true so InputText can update its internal state.
    ImWchar             EventChar;      // Character input                      // Read-write   // [CharFilter] Replace character with another one, or set to zero to drop. return 1 is equivalent to setting EventChar=0;
    MsTrGKey            EventKey;       // Key pressed (Up/Down/TAB)            // Read-only    // [Completion,History]
    char*               Buf;            // Text buffer                          // Read-write   // [Resize] Can replace pointer / [Completion,History,Always] Only write to pointed data, don't replace the actual pointer!
    int                 BufTextLen;     // Text length (in bytes)               // Read-write   // [Resize,Completion,History,Always] Exclude zero-terminator storage. In C land: == strlen(some_text), in C++ land: string.length()
    int                 BufSize;        // Buffer size (in bytes) = capacity+1  // Read-only    // [Resize,Completion,History,Always] Include zero-terminator storage. In C land == ARRAYSIZE(my_char_array), in C++ land: string.capacity()+1
    bool                BufDirty;       // Set if you modify Buf/BufTextLen!    // Write        // [Completion,History,Always]
    int                 CursorPos;      //                                      // Read-write   // [Completion,History,Always]
    int                 SelectionStart; //                                      // Read-write   // [Completion,History,Always] == to SelectionEnd when no selection)
    int                 SelectionEnd;   //                                      // Read-write   // [Completion,History,Always]

    // Helper functions for text manipulation.
    // Use those function to benefit from the CallbackResize behaviors. Calling those function reset the selection.
    MsTrG_API MsTrGInputTextCallbackData();
    MsTrG_API void      DeleteChars(int pos, int bytes_count);
    MsTrG_API void      InsertChars(int pos, const char* text, const char* text_end = NULL);
    void                SelectAll()             { SelectionStart = 0; SelectionEnd = BufTextLen; }
    void                ClearSelection()        { SelectionStart = SelectionEnd = BufTextLen; }
    bool                HasSelection() const    { return SelectionStart != SelectionEnd; }
};

// Resizing callback data to apply custom constraint. As enabled by SetNextWindowSizeConstraints(). Callback is called during the next Begin().
// NB: For basic min/max size constraint on each axis you don't need to use the callback! The SetNextWindowSizeConstraints() parameters are enough.
struct MsTrGSizeCallbackData
{
    void*   UserData;       // Read-only.   What user passed to SetNextWindowSizeConstraints()
    ImVec2  Pos;            // Read-only.   Window position, for reference.
    ImVec2  CurrentSize;    // Read-only.   Current window size.
    ImVec2  DesiredSize;    // Read-write.  Desired size, based on user's mouse position. Write to this field to restrain resizing.
};

// Data payload for Drag and Drop operations: AcceptDragDropPayload(), GetDragDropPayload()
struct MsTrGPayload
{
    // Members
    void*           Data;               // Data (copied and owned by dear MsTrG)
    int             DataSize;           // Data size

    // [Internal]
    MsTrGID         SourceId;           // Source item id
    MsTrGID         SourceParentId;     // Source parent id (if available)
    int             DataFrameCount;     // Data timestamp
    char            DataType[32 + 1];   // Data type tag (short user-supplied string, 32 characters max)
    bool            Preview;            // Set when AcceptDragDropPayload() was called and mouse has been hovering the target item (nb: handle overlapping drag targets)
    bool            Delivery;           // Set when AcceptDragDropPayload() was called and mouse button is released over the target item.

    MsTrGPayload()  { Clear(); }
    void Clear()    { SourceId = SourceParentId = 0; Data = NULL; DataSize = 0; memset(DataType, 0, sizeof(DataType)); DataFrameCount = -1; Preview = Delivery = false; }
    bool IsDataType(const char* type) const { return DataFrameCount != -1 && strcmp(type, DataType) == 0; }
    bool IsPreview() const                  { return Preview; }
    bool IsDelivery() const                 { return Delivery; }
};

// Sorting specification for one column of a table (sizeof == 12 bytes)
struct MsTrGTableColumnSortSpecs
{
    MsTrGID                     ColumnUserID;       // User id of the column (if specified by a TableSetupColumn() call)
    ImS16                       ColumnIndex;        // Index of the column
    ImS16                       SortOrder;          // Index within parent MsTrGTableSortSpecs (always stored in order starting from 0, tables sorted on a single criteria will always have a 0 here)
    MsTrGSortDirection          SortDirection : 8;  // MsTrGSortDirection_Ascending or MsTrGSortDirection_Descending (you can use this or SortSign, whichever is more convenient for your sort function)

    MsTrGTableColumnSortSpecs() { memset(this, 0, sizeof(*this)); }
};

// Sorting specifications for a table (often handling sort specs for a single column, occasionally more)
// Obtained by calling TableGetSortSpecs().
// When 'SpecsDirty == true' you can sort your data. It will be true with sorting specs have changed since last call, or the first time.
// Make sure to set 'SpecsDirty = false' after sorting, else you may wastefully sort your data every frame!
struct MsTrGTableSortSpecs
{
    const MsTrGTableColumnSortSpecs* Specs;     // Pointer to sort spec array.
    int                         SpecsCount;     // Sort spec count. Most often 1. May be > 1 when MsTrGTableFlags_SortMulti is enabled. May be == 0 when MsTrGTableFlags_SortTristate is enabled.
    bool                        SpecsDirty;     // Set to true when specs have changed since last time! Use this to sort again, then clear the flag.

    MsTrGTableSortSpecs()       { memset(this, 0, sizeof(*this)); }
};

//-----------------------------------------------------------------------------
// [SECTION] Helpers (MsTrGOnceUponAFrame, MsTrGTextFilter, MsTrGTextBuffer, MsTrGStorage, MsTrGListClipper, ImColor)
//-----------------------------------------------------------------------------

// Helper: Unicode defines
#define IM_UNICODE_CODEPOINT_INVALID 0xFFFD     // Invalid Unicode code point (standard value).
#ifdef MsTrG_USE_WCHAR32
#define IM_UNICODE_CODEPOINT_MAX     0x10FFFF   // Maximum Unicode code point supported by this build.
#else
#define IM_UNICODE_CODEPOINT_MAX     0xFFFF     // Maximum Unicode code point supported by this build.
#endif

// Helper: Execute a block of code at maximum once a frame. Convenient if you want to quickly create an UI within deep-nested code that runs multiple times every frame.
// Usage: static MsTrGOnceUponAFrame oaf; if (oaf) MsTrG::Text("This will be called only once per frame");
struct MsTrGOnceUponAFrame
{
    MsTrGOnceUponAFrame() { RefFrame = -1; }
    mutable int RefFrame;
    operator bool() const { int current_frame = MsTrG::GetFrameCount(); if (RefFrame == current_frame) return false; RefFrame = current_frame; return true; }
};

// Helper: Parse and apply text filters. In format "aaaaa[,bbbb][,ccccc]"
struct MsTrGTextFilter
{
    MsTrG_API           MsTrGTextFilter(const char* default_filter = "");
    MsTrG_API bool      Draw(const char* label = "Filter (inc,-exc)", float width = 0.0f);  // Helper calling InputText+Build
    MsTrG_API bool      PassFilter(const char* text, const char* text_end = NULL) const;
    MsTrG_API void      Build();
    void                Clear()          { InputBuf[0] = 0; Build(); }
    bool                IsActive() const { return !Filters.empty(); }

    // [Internal]
    struct MsTrGTextRange
    {
        const char*     b;
        const char*     e;

        MsTrGTextRange()                                { b = e = NULL; }
        MsTrGTextRange(const char* _b, const char* _e)  { b = _b; e = _e; }
        bool            empty() const                   { return b == e; }
        MsTrG_API void  split(char separator, ImVector<MsTrGTextRange>* out) const;
    };
    char                    InputBuf[256];
    ImVector<MsTrGTextRange>Filters;
    int                     CountGrep;
};

// Helper: Growable text buffer for logging/accumulating text
// (this could be called 'MsTrGTextBuilder' / 'MsTrGStringBuilder')
struct MsTrGTextBuffer
{
    ImVector<char>      Buf;
    MsTrG_API static char EmptyString[1];

    MsTrGTextBuffer()   { }
    inline char         operator[](int i) const { IM_ASSERT(Buf.Data != NULL); return Buf.Data[i]; }
    const char*         begin() const           { return Buf.Data ? &Buf.front() : EmptyString; }
    const char*         end() const             { return Buf.Data ? &Buf.back() : EmptyString; }   // Buf is zero-terminated, so end() will point on the zero-terminator
    int                 size() const            { return Buf.Size ? Buf.Size - 1 : 0; }
    bool                empty() const           { return Buf.Size <= 1; }
    void                clear()                 { Buf.clear(); }
    void                reserve(int capacity)   { Buf.reserve(capacity); }
    const char*         c_str() const           { return Buf.Data ? Buf.Data : EmptyString; }
    MsTrG_API void      append(const char* str, const char* str_end = NULL);
    MsTrG_API void      appendf(const char* fmt, ...) IM_FMTARGS(2);
    MsTrG_API void      appendfv(const char* fmt, va_list args) IM_FMTLIST(2);
};

// Helper: Key->Value storage
// Typically you don't have to worry about this since a storage is held within each Window.
// We use it to e.g. store collapse state for a tree (Int 0/1)
// This is optimized for efficient lookup (dichotomy into a contiguous buffer) and rare insertion (typically tied to user interactions aka max once a frame)
// You can use it as custom user storage for temporary values. Declare your own storage if, for example:
// - You want to manipulate the open/close state of a particular sub-tree in your interface (tree node uses Int 0/1 to store their state).
// - You want to store custom debug data easily without adding or editing structures in your code (probably not efficient, but convenient)
// Types are NOT stored, so it is up to you to make sure your Key don't collide with different types.
struct MsTrGStorage
{
    // [Internal]
    struct MsTrGStoragePair
    {
        MsTrGID key;
        union { int val_i; float val_f; void* val_p; };
        MsTrGStoragePair(MsTrGID _key, int _val_i)      { key = _key; val_i = _val_i; }
        MsTrGStoragePair(MsTrGID _key, float _val_f)    { key = _key; val_f = _val_f; }
        MsTrGStoragePair(MsTrGID _key, void* _val_p)    { key = _key; val_p = _val_p; }
    };

    ImVector<MsTrGStoragePair>      Data;

    // - Get***() functions find pair, never add/allocate. Pairs are sorted so a query is O(log N)
    // - Set***() functions find pair, insertion on demand if missing.
    // - Sorted insertion is costly, paid once. A typical frame shouldn't need to insert any new pair.
    void                Clear() { Data.clear(); }
    MsTrG_API int       GetInt(MsTrGID key, int default_val = 0) const;
    MsTrG_API void      SetInt(MsTrGID key, int val);
    MsTrG_API bool      GetBool(MsTrGID key, bool default_val = false) const;
    MsTrG_API void      SetBool(MsTrGID key, bool val);
    MsTrG_API float     GetFloat(MsTrGID key, float default_val = 0.0f) const;
    MsTrG_API void      SetFloat(MsTrGID key, float val);
    MsTrG_API void*     GetVoidPtr(MsTrGID key) const; // default_val is NULL
    MsTrG_API void      SetVoidPtr(MsTrGID key, void* val);

    // - Get***Ref() functions finds pair, insert on demand if missing, return pointer. Useful if you intend to do Get+Set.
    // - References are only valid until a new value is added to the storage. Calling a Set***() function or a Get***Ref() function invalidates the pointer.
    // - A typical use case where this is convenient for quick hacking (e.g. add storage during a live Edit&Continue session if you can't modify existing struct)
    //      float* pvar = MsTrG::GetFloatRef(key); MsTrG::SliderFloat("var", pvar, 0, 100.0f); some_var += *pvar;
    MsTrG_API int*      GetIntRef(MsTrGID key, int default_val = 0);
    MsTrG_API bool*     GetBoolRef(MsTrGID key, bool default_val = false);
    MsTrG_API float*    GetFloatRef(MsTrGID key, float default_val = 0.0f);
    MsTrG_API void**    GetVoidPtrRef(MsTrGID key, void* default_val = NULL);

    // Use on your own storage if you know only integer are being stored (open/close all tree nodes)
    MsTrG_API void      SetAllInt(int val);

    // For quicker full rebuild of a storage (instead of an incremental one), you may add all your contents and then sort once.
    MsTrG_API void      BuildSortByKey();
};

// Helper: Manually clip large list of items.
// If you have lots evenly spaced items and you have a random access to the list, you can perform coarse
// clipping based on visibility to only submit items that are in view.
// The clipper calculates the range of visible items and advance the cursor to compensate for the non-visible items we have skipped.
// (Dear MsTrG already clip items based on their bounds but: it needs to first layout the item to do so, and generally
//  fetching/submitting your own data incurs additional cost. Coarse clipping using MsTrGListClipper allows you to easily
//  scale using lists with tens of thousands of items without a problem)
// Usage:
//   MsTrGListClipper clipper;
//   clipper.Begin(1000);         // We have 1000 elements, evenly spaced.
//   while (clipper.Step())
//       for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
//           MsTrG::Text("line number %d", i);
// Generally what happens is:
// - Clipper lets you process the first element (DisplayStart = 0, DisplayEnd = 1) regardless of it being visible or not.
// - User code submit that one element.
// - Clipper can measure the height of the first element
// - Clipper calculate the actual range of elements to display based on the current clipping rectangle, position the cursor before the first visible element.
// - User code submit visible elements.
// - The clipper also handles various subtleties related to keyboard/gamepad navigation, wrapping etc.
struct MsTrGListClipper
{
    int             DisplayStart;       // First item to display, updated by each call to Step()
    int             DisplayEnd;         // End of items to display (exclusive)
    int             ItemsCount;         // [Internal] Number of items
    float           ItemsHeight;        // [Internal] Height of item after a first step and item submission can calculate it
    float           StartPosY;          // [Internal] Cursor position at the time of Begin() or after table frozen rows are all processed
    void*           TempData;           // [Internal] Internal data

    // items_count: Use INT_MAX if you don't know how many items you have (in which case the cursor won't be advanced in the final step)
    // items_height: Use -1.0f to be calculated automatically on first step. Otherwise pass in the distance between your items, typically GetTextLineHeightWithSpacing() or GetFrameHeightWithSpacing().
    MsTrG_API MsTrGListClipper();
    MsTrG_API ~MsTrGListClipper();
    MsTrG_API void  Begin(int items_count, float items_height = -1.0f);
    MsTrG_API void  End();             // Automatically called on the last call of Step() that returns false.
    MsTrG_API bool  Step();            // Call until it returns false. The DisplayStart/DisplayEnd fields will be set and you can process/draw those items.

    // Call ForceDisplayRangeByIndices() before first call to Step() if you need a range of items to be displayed regardless of visibility.
    MsTrG_API void  ForceDisplayRangeByIndices(int item_min, int item_max); // item_max is exclusive e.g. use (42, 42+1) to make item 42 always visible BUT due to alignment/padding of certain items it is likely that an extra item may be included on either end of the display range.

#ifndef MsTrG_DISABLE_OBSOLETE_FUNCTIONS
    inline MsTrGListClipper(int items_count, float items_height = -1.0f) { memset(this, 0, sizeof(*this)); ItemsCount = -1; Begin(items_count, items_height); } // [removed in 1.79]
#endif
};

// Helpers macros to generate 32-bit encoded colors
// User can declare their own format by #defining the 5 _SHIFT/_MASK macros in their imconfig file.
#ifndef IM_COL32_R_SHIFT
#ifdef MsTrG_USE_BGRA_PACKED_COLOR
#define IM_COL32_R_SHIFT    16
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    0
#define IM_COL32_A_SHIFT    24
#define IM_COL32_A_MASK     0xFF000000
#else
#define IM_COL32_R_SHIFT    0
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    16
#define IM_COL32_A_SHIFT    24
#define IM_COL32_A_MASK     0xFF000000
#endif
#endif
#define IM_COL32(R,G,B,A)    (((ImU32)(A)<<IM_COL32_A_SHIFT) | ((ImU32)(B)<<IM_COL32_B_SHIFT) | ((ImU32)(G)<<IM_COL32_G_SHIFT) | ((ImU32)(R)<<IM_COL32_R_SHIFT))
#define IM_COL32_WHITE       IM_COL32(255,255,255,255)  // Opaque white = 0xFFFFFFFF
#define IM_COL32_BLACK       IM_COL32(0,0,0,255)        // Opaque black
#define IM_COL32_BLACK_TRANS IM_COL32(0,0,0,0)          // Transparent black = 0x00000000

// Helper: ImColor() implicitly converts colors to either ImU32 (packed 4x1 byte) or ImVec4 (4x1 float)
// Prefer using IM_COL32() macros if you want a guaranteed compile-time ImU32 for usage with ImDrawList API.
// **Avoid storing ImColor! Store either u32 of ImVec4. This is not a full-featured color class. MAY OBSOLETE.
// **None of the MsTrG API are using ImColor directly but you can use it as a convenience to pass colors in either ImU32 or ImVec4 formats. Explicitly cast to ImU32 or ImVec4 if needed.
struct ImColor
{
    ImVec4          Value;

    constexpr ImColor()                                             { }
    constexpr ImColor(float r, float g, float b, float a = 1.0f)    : Value(r, g, b, a) { }
    constexpr ImColor(const ImVec4& col)                            : Value(col) {}
    ImColor(int r, int g, int b, int a = 255)                       { float sc = 1.0f / 255.0f; Value.x = (float)r * sc; Value.y = (float)g * sc; Value.z = (float)b * sc; Value.w = (float)a * sc; }
    ImColor(ImU32 rgba)                                             { float sc = 1.0f / 255.0f; Value.x = (float)((rgba >> IM_COL32_R_SHIFT) & 0xFF) * sc; Value.y = (float)((rgba >> IM_COL32_G_SHIFT) & 0xFF) * sc; Value.z = (float)((rgba >> IM_COL32_B_SHIFT) & 0xFF) * sc; Value.w = (float)((rgba >> IM_COL32_A_SHIFT) & 0xFF) * sc; }
    inline operator ImU32() const                                   { return MsTrG::ColorConvertFloat4ToU32(Value); }
    inline operator ImVec4() const                                  { return Value; }

    // FIXME-OBSOLETE: May need to obsolete/cleanup those helpers.
    inline void    SetHSV(float h, float s, float v, float a = 1.0f){ MsTrG::ColorConvertHSVtoRGB(h, s, v, Value.x, Value.y, Value.z); Value.w = a; }
    static ImColor HSV(float h, float s, float v, float a = 1.0f)   { float r, g, b; MsTrG::ColorConvertHSVtoRGB(h, s, v, r, g, b); return ImColor(r, g, b, a); }
};

//-----------------------------------------------------------------------------
// [SECTION] Drawing API (ImDrawCmd, ImDrawIdx, ImDrawVert, ImDrawChannel, ImDrawListSplitter, ImDrawListFlags, ImDrawList, ImDrawData)
// Hold a series of drawing commands. The user provides a renderer for ImDrawData which essentially contains an array of ImDrawList.
//-----------------------------------------------------------------------------

// The maximum line width to bake anti-aliased textures for. Build atlas with ImFontAtlasFlags_NoBakedLines to disable baking.
#ifndef IM_DRAWLIST_TEX_LINES_WIDTH_MAX
#define IM_DRAWLIST_TEX_LINES_WIDTH_MAX     (63)
#endif

// ImDrawCallback: Draw callbacks for advanced uses [configurable type: override in imconfig.h]
// NB: You most likely do NOT need to use draw callbacks just to create your own widget or customized UI rendering,
// you can poke into the draw list for that! Draw callback may be useful for example to:
//  A) Change your GPU render state,
//  B) render a complex 3D scene inside a UI element without an intermediate texture/render target, etc.
// The expected behavior from your rendering function is 'if (cmd.UserCallback != NULL) { cmd.UserCallback(parent_list, cmd); } else { RenderTriangles() }'
// If you want to override the signature of ImDrawCallback, you can simply use e.g. '#define ImDrawCallback MyDrawCallback' (in imconfig.h) + update rendering backend accordingly.
#ifndef ImDrawCallback
typedef void (*ImDrawCallback)(const ImDrawList* parent_list, const ImDrawCmd* cmd);
#endif

// Special Draw callback value to request renderer backend to reset the graphics/render state.
// The renderer backend needs to handle this special value, otherwise it will crash trying to call a function at this address.
// This is useful for example if you submitted callbacks which you know have altered the render state and you want it to be restored.
// It is not done by default because they are many perfectly useful way of altering render state for MsTrG contents (e.g. changing shader/blending settings before an Image call).
#define ImDrawCallback_ResetRenderState     (ImDrawCallback)(-1)

// Typically, 1 command = 1 GPU draw call (unless command is a callback)
// - VtxOffset: When 'io.BackendFlags & MsTrGBackendFlags_RendererHasVtxOffset' is enabled,
//   this fields allow us to render meshes larger than 64K vertices while keeping 16-bit indices.
//   Backends made for <1.71. will typically ignore the VtxOffset fields.
// - The ClipRect/TextureId/VtxOffset fields must be contiguous as we memcmp() them together (this is asserted for).
struct ImDrawCmd
{
    ImVec4          ClipRect;           // 4*4  // Clipping rectangle (x1, y1, x2, y2). Subtract ImDrawData->DisplayPos to get clipping rectangle in "viewport" coordinates
    ImTextureID     TextureId;          // 4-8  // User-provided texture ID. Set by user in ImfontAtlas::SetTexID() for fonts or passed to Image*() functions. Ignore if never using images or multiple fonts atlas.
    unsigned int    VtxOffset;          // 4    // Start offset in vertex buffer. MsTrGBackendFlags_RendererHasVtxOffset: always 0, otherwise may be >0 to support meshes larger than 64K vertices with 16-bit indices.
    unsigned int    IdxOffset;          // 4    // Start offset in index buffer.
    unsigned int    ElemCount;          // 4    // Number of indices (multiple of 3) to be rendered as triangles. Vertices are stored in the callee ImDrawList's vtx_buffer[] array, indices in idx_buffer[].
    ImDrawCallback  UserCallback;       // 4-8  // If != NULL, call the function instead of rendering the vertices. clip_rect and texture_id will be set normally.
    void*           UserCallbackData;   // 4-8  // The draw callback code can access this.

    ImDrawCmd() { memset(this, 0, sizeof(*this)); } // Also ensure our padding fields are zeroed

    // Since 1.83: returns ImTextureID associated with this draw call. Warning: DO NOT assume this is always same as 'TextureId' (we will change this function for an upcoming feature)
    inline ImTextureID GetTexID() const { return TextureId; }
};

// Vertex layout
#ifndef MsTrG_OVERRIDE_DRAWVERT_STRUCT_LAYOUT
struct ImDrawVert
{
    ImVec2  pos;
    ImVec2  uv;
    ImU32   col;
};
#else
// You can override the vertex format layout by defining MsTrG_OVERRIDE_DRAWVERT_STRUCT_LAYOUT in imconfig.h
// The code expect ImVec2 pos (8 bytes), ImVec2 uv (8 bytes), ImU32 col (4 bytes), but you can re-order them or add other fields as needed to simplify integration in your engine.
// The type has to be described within the macro (you can either declare the struct or use a typedef). This is because ImVec2/ImU32 are likely not declared a the time you'd want to set your type up.
// NOTE: MsTrG DOESN'T CLEAR THE STRUCTURE AND DOESN'T CALL A CONSTRUCTOR SO ANY CUSTOM FIELD WILL BE UNINITIALIZED. IF YOU ADD EXTRA FIELDS (SUCH AS A 'Z' COORDINATES) YOU WILL NEED TO CLEAR THEM DURING RENDER OR TO IGNORE THEM.
MsTrG_OVERRIDE_DRAWVERT_STRUCT_LAYOUT;
#endif

// [Internal] For use by ImDrawList
struct ImDrawCmdHeader
{
    ImVec4          ClipRect;
    ImTextureID     TextureId;
    unsigned int    VtxOffset;
};

// [Internal] For use by ImDrawListSplitter
struct ImDrawChannel
{
    ImVector<ImDrawCmd>         _CmdBuffer;
    ImVector<ImDrawIdx>         _IdxBuffer;
};


// Split/Merge functions are used to split the draw list into different layers which can be drawn into out of order.
// This is used by the Columns/Tables API, so items of each column can be batched together in a same draw call.
struct ImDrawListSplitter
{
    int                         _Current;    // Current channel number (0)
    int                         _Count;      // Number of active channels (1+)
    ImVector<ImDrawChannel>     _Channels;   // Draw channels (not resized down so _Count might be < Channels.Size)

    inline ImDrawListSplitter()  { memset(this, 0, sizeof(*this)); }
    inline ~ImDrawListSplitter() { ClearFreeMemory(); }
    inline void                 Clear() { _Current = 0; _Count = 1; } // Do not clear Channels[] so our allocations are reused next frame
    MsTrG_API void              ClearFreeMemory();
    MsTrG_API void              Split(ImDrawList* draw_list, int count);
    MsTrG_API void              Merge(ImDrawList* draw_list);
    MsTrG_API void              SetCurrentChannel(ImDrawList* draw_list, int channel_idx);
};

// Flags for ImDrawList functions
// (Legacy: bit 0 must always correspond to ImDrawFlags_Closed to be backward compatible with old API using a bool. Bits 1..3 must be unused)
enum ImDrawFlags_
{
    ImDrawFlags_None                        = 0,
    ImDrawFlags_Closed                      = 1 << 0, // PathStroke(), AddPolyline(): specify that shape should be closed (Important: this is always == 1 for legacy reason)
    ImDrawFlags_RoundCornersTopLeft         = 1 << 4, // AddRect(), AddRectFilled(), PathRect(): enable rounding top-left corner only (when rounding > 0.0f, we default to all corners). Was 0x01.
    ImDrawFlags_RoundCornersTopRight        = 1 << 5, // AddRect(), AddRectFilled(), PathRect(): enable rounding top-right corner only (when rounding > 0.0f, we default to all corners). Was 0x02.
    ImDrawFlags_RoundCornersBottomLeft      = 1 << 6, // AddRect(), AddRectFilled(), PathRect(): enable rounding bottom-left corner only (when rounding > 0.0f, we default to all corners). Was 0x04.
    ImDrawFlags_RoundCornersBottomRight     = 1 << 7, // AddRect(), AddRectFilled(), PathRect(): enable rounding bottom-right corner only (when rounding > 0.0f, we default to all corners). Wax 0x08.
    ImDrawFlags_RoundCornersNone            = 1 << 8, // AddRect(), AddRectFilled(), PathRect(): disable rounding on all corners (when rounding > 0.0f). This is NOT zero, NOT an implicit flag!
    ImDrawFlags_RoundCornersTop             = ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight,
    ImDrawFlags_RoundCornersBottom          = ImDrawFlags_RoundCornersBottomLeft | ImDrawFlags_RoundCornersBottomRight,
    ImDrawFlags_RoundCornersLeft            = ImDrawFlags_RoundCornersBottomLeft | ImDrawFlags_RoundCornersTopLeft,
    ImDrawFlags_RoundCornersRight           = ImDrawFlags_RoundCornersBottomRight | ImDrawFlags_RoundCornersTopRight,
    ImDrawFlags_RoundCornersAll             = ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight | ImDrawFlags_RoundCornersBottomLeft | ImDrawFlags_RoundCornersBottomRight,
    ImDrawFlags_RoundCornersDefault_        = ImDrawFlags_RoundCornersAll, // Default to ALL corners if none of the _RoundCornersXX flags are specified.
    ImDrawFlags_RoundCornersMask_           = ImDrawFlags_RoundCornersAll | ImDrawFlags_RoundCornersNone
};

// Flags for ImDrawList instance. Those are set automatically by MsTrG:: functions from MsTrGIO settings, and generally not manipulated directly.
// It is however possible to temporarily alter flags between calls to ImDrawList:: functions.
enum ImDrawListFlags_
{
    ImDrawListFlags_None                    = 0,
    ImDrawListFlags_AntiAliasedLines        = 1 << 0,  // Enable anti-aliased lines/borders (*2 the number of triangles for 1.0f wide line or lines thin enough to be drawn using textures, otherwise *3 the number of triangles)
    ImDrawListFlags_AntiAliasedLinesUseTex  = 1 << 1,  // Enable anti-aliased lines/borders using textures when possible. Require backend to render with bilinear filtering (NOT point/nearest filtering).
    ImDrawListFlags_AntiAliasedFill         = 1 << 2,  // Enable anti-aliased edge around filled shapes (rounded rectangles, circles).
    ImDrawListFlags_AllowVtxOffset          = 1 << 3   // Can emit 'VtxOffset > 0' to allow large meshes. Set when 'MsTrGBackendFlags_RendererHasVtxOffset' is enabled.
};

// Draw command list
// This is the low-level list of polygons that MsTrG:: functions are filling. At the end of the frame,
// all command lists are passed to your MsTrGIO::RenderDrawListFn function for rendering.
// Each dear MsTrG window contains its own ImDrawList. You can use MsTrG::GetWindowDrawList() to
// access the current window draw list and draw custom primitives.
// You can interleave normal MsTrG:: calls and adding primitives to the current draw list.
// In single viewport mode, top-left is == GetMainViewport()->Pos (generally 0,0), bottom-right is == GetMainViewport()->Pos+Size (generally io.DisplaySize).
// You are totally free to apply whatever transformation matrix to want to the data (depending on the use of the transformation you may want to apply it to ClipRect as well!)
// Important: Primitives are always added to the list and not culled (culling is done at higher-level by MsTrG:: functions), if you use this API a lot consider coarse culling your drawn objects.
struct ImDrawList
{
    // This is what you have to render
    ImVector<ImDrawCmd>     CmdBuffer;          // Draw commands. Typically 1 command = 1 GPU draw call, unless the command is a callback.
    ImVector<ImDrawIdx>     IdxBuffer;          // Index buffer. Each command consume ImDrawCmd::ElemCount of those
    ImVector<ImDrawVert>    VtxBuffer;          // Vertex buffer.
    ImDrawListFlags         Flags;              // Flags, you may poke into these to adjust anti-aliasing settings per-primitive.

    // [Internal, used while building lists]
    unsigned int            _VtxCurrentIdx;     // [Internal] generally == VtxBuffer.Size unless we are past 64K vertices, in which case this gets reset to 0.
    const ImDrawListSharedData* _Data;          // Pointer to shared draw data (you can use MsTrG::GetDrawListSharedData() to get the one from current MsTrG context)
    const char*             _OwnerName;         // Pointer to owner window's name for debugging
    ImDrawVert*             _VtxWritePtr;       // [Internal] point within VtxBuffer.Data after each add command (to avoid using the ImVector<> operators too much)
    ImDrawIdx*              _IdxWritePtr;       // [Internal] point within IdxBuffer.Data after each add command (to avoid using the ImVector<> operators too much)
    ImVector<ImVec4>        _ClipRectStack;     // [Internal]
    ImVector<ImTextureID>   _TextureIdStack;    // [Internal]
    ImVector<ImVec2>        _Path;              // [Internal] current path building
    ImDrawCmdHeader         _CmdHeader;         // [Internal] template of active commands. Fields should match those of CmdBuffer.back().
    ImDrawListSplitter      _Splitter;          // [Internal] for channels api (note: prefer using your own persistent instance of ImDrawListSplitter!)
    float                   _FringeScale;       // [Internal] anti-alias fringe is scaled by this value, this helps to keep things sharp while zooming at vertex buffer content

    // If you want to create ImDrawList instances, pass them MsTrG::GetDrawListSharedData() or create and use your own ImDrawListSharedData (so you can use ImDrawList without MsTrG)
    ImDrawList(const ImDrawListSharedData* shared_data) { memset(this, 0, sizeof(*this)); _Data = shared_data; }

    ~ImDrawList() { _ClearFreeMemory(); }
    MsTrG_API void  PushClipRect(const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect = false);  // Render-level scissoring. This is passed down to your render function but not used for CPU-side coarse clipping. Prefer using higher-level MsTrG::PushClipRect() to affect logic (hit-testing and widget culling)
    MsTrG_API void  PushClipRectFullScreen();
    MsTrG_API void  PopClipRect();
    MsTrG_API void  PushTextureID(ImTextureID texture_id);
    MsTrG_API void  PopTextureID();
    inline ImVec2   GetClipRectMin() const { const ImVec4& cr = _ClipRectStack.back(); return ImVec2(cr.x, cr.y); }
    inline ImVec2   GetClipRectMax() const { const ImVec4& cr = _ClipRectStack.back(); return ImVec2(cr.z, cr.w); }

    // Primitives
    // - Filled shapes must always use clockwise winding order. The anti-aliasing fringe depends on it. Counter-clockwise shapes will have "inward" anti-aliasing.
    // - For rectangular primitives, "p_min" and "p_max" represent the upper-left and lower-right corners.
    // - For circle primitives, use "num_segments == 0" to automatically calculate tessellation (preferred).
    //   In older versions (until Dear MsTrG 1.77) the AddCircle functions defaulted to num_segments == 12.
    //   In future versions we will use textures to provide cheaper and higher-quality circles.
    //   Use AddNgon() and AddNgonFilled() functions if you need to guaranteed a specific number of sides.
    MsTrG_API void  AddLine(const ImVec2& p1, const ImVec2& p2, ImU32 col, float thickness = 1.0f);
    MsTrG_API void  AddRect(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0, float thickness = 1.0f);   // a: upper-left, b: lower-right (== upper-left + size)
    MsTrG_API void  AddRectFilled(const ImVec2& p_min, const ImVec2& p_max, ImU32 col, float rounding = 0.0f, ImDrawFlags flags = 0);                     // a: upper-left, b: lower-right (== upper-left + size)
    MsTrG_API void  AddRectFilledMultiColor(const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);
    MsTrG_API void  AddQuad(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness = 1.0f);
    MsTrG_API void  AddQuadFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col);
    MsTrG_API void  AddTriangle(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness = 1.0f);
    MsTrG_API void  AddTriangleFilled(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col);
    MsTrG_API void  AddCircle(const ImVec2& center, float radius, ImU32 col, int num_segments = 0, float thickness = 1.0f);
    MsTrG_API void  AddCircleFilled(const ImVec2& center, float radius, ImU32 col, int num_segments = 0);
    MsTrG_API void  AddNgon(const ImVec2& center, float radius, ImU32 col, int num_segments, float thickness = 1.0f);
    MsTrG_API void  AddNgonFilled(const ImVec2& center, float radius, ImU32 col, int num_segments);
    MsTrG_API void  AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL);
    MsTrG_API void  AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL, float wrap_width = 0.0f, const ImVec4* cpu_fine_clip_rect = NULL);
    MsTrG_API void  AddPolyline(const ImVec2* points, int num_points, ImU32 col, ImDrawFlags flags, float thickness);
    MsTrG_API void  AddConvexPolyFilled(const ImVec2* points, int num_points, ImU32 col);
    MsTrG_API void  AddBezierCubic(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness, int num_segments = 0); // Cubic Bezier (4 control points)
    MsTrG_API void  AddBezierQuadratic(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, ImU32 col, float thickness, int num_segments = 0);               // Quadratic Bezier (3 control points)

    // Image primitives
    // - Read FAQ to understand what ImTextureID is.
    // - "p_min" and "p_max" represent the upper-left and lower-right corners of the rectangle.
    // - "uv_min" and "uv_max" represent the normalized texture coordinates to use for those corners. Using (0,0)->(1,1) texture coordinates will generally display the entire texture.
    MsTrG_API void  AddImage(ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min = ImVec2(0, 0), const ImVec2& uv_max = ImVec2(1, 1), ImU32 col = IM_COL32_WHITE);
    MsTrG_API void  AddImageQuad(ImTextureID user_texture_id, const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& uv1 = ImVec2(0, 0), const ImVec2& uv2 = ImVec2(1, 0), const ImVec2& uv3 = ImVec2(1, 1), const ImVec2& uv4 = ImVec2(0, 1), ImU32 col = IM_COL32_WHITE);
    MsTrG_API void  AddImageRounded(ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max, const ImVec2& uv_min, const ImVec2& uv_max, ImU32 col, float rounding, ImDrawFlags flags = 0);

    // Stateful path API, add points then finish with PathFillConvex() or PathStroke()
    // - Filled shapes must always use clockwise winding order. The anti-aliasing fringe depends on it. Counter-clockwise shapes will have "inward" anti-aliasing.
    inline    void  PathClear()                                                 { _Path.Size = 0; }
    inline    void  PathLineTo(const ImVec2& pos)                               { _Path.push_back(pos); }
    inline    void  PathLineToMergeDuplicate(const ImVec2& pos)                 { if (_Path.Size == 0 || memcmp(&_Path.Data[_Path.Size - 1], &pos, 8) != 0) _Path.push_back(pos); }
    inline    void  PathFillConvex(ImU32 col)                                   { AddConvexPolyFilled(_Path.Data, _Path.Size, col); _Path.Size = 0; }
    inline    void  PathStroke(ImU32 col, ImDrawFlags flags = 0, float thickness = 1.0f) { AddPolyline(_Path.Data, _Path.Size, col, flags, thickness); _Path.Size = 0; }
    MsTrG_API void  PathArcTo(const ImVec2& center, float radius, float a_min, float a_max, int num_segments = 0);
    MsTrG_API void  PathArcToFast(const ImVec2& center, float radius, int a_min_of_12, int a_max_of_12);                // Use precomputed angles for a 12 steps circle
    MsTrG_API void  PathBezierCubicCurveTo(const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments = 0); // Cubic Bezier (4 control points)
    MsTrG_API void  PathBezierQuadraticCurveTo(const ImVec2& p2, const ImVec2& p3, int num_segments = 0);               // Quadratic Bezier (3 control points)
    MsTrG_API void  PathRect(const ImVec2& rect_min, const ImVec2& rect_max, float rounding = 0.0f, ImDrawFlags flags = 0);

    // Advanced
    MsTrG_API void  AddCallback(ImDrawCallback callback, void* callback_data);  // Your rendering function must check for 'UserCallback' in ImDrawCmd and call the function instead of rendering triangles.
    MsTrG_API void  AddDrawCmd();                                               // This is useful if you need to forcefully create a new draw call (to allow for dependent rendering / blending). Otherwise primitives are merged into the same draw-call as much as possible
    MsTrG_API ImDrawList* CloneOutput() const;                                  // Create a clone of the CmdBuffer/IdxBuffer/VtxBuffer.

    // Advanced: Channels
    // - Use to split render into layers. By switching channels to can render out-of-order (e.g. submit FG primitives before BG primitives)
    // - Use to minimize draw calls (e.g. if going back-and-forth between multiple clipping rectangles, prefer to append into separate channels then merge at the end)
    // - FIXME-OBSOLETE: This API shouldn't have been in ImDrawList in the first place!
    //   Prefer using your own persistent instance of ImDrawListSplitter as you can stack them.
    //   Using the ImDrawList::ChannelsXXXX you cannot stack a split over another.
    inline void     ChannelsSplit(int count)    { _Splitter.Split(this, count); }
    inline void     ChannelsMerge()             { _Splitter.Merge(this); }
    inline void     ChannelsSetCurrent(int n)   { _Splitter.SetCurrentChannel(this, n); }

    // Advanced: Primitives allocations
    // - We render triangles (three vertices)
    // - All primitives needs to be reserved via PrimReserve() beforehand.
    MsTrG_API void  PrimReserve(int idx_count, int vtx_count);
    MsTrG_API void  PrimUnreserve(int idx_count, int vtx_count);
    MsTrG_API void  PrimRect(const ImVec2& a, const ImVec2& b, ImU32 col);      // Axis aligned rectangle (composed of two triangles)
    MsTrG_API void  PrimRectUV(const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, ImU32 col);
    MsTrG_API void  PrimQuadUV(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a, const ImVec2& uv_b, const ImVec2& uv_c, const ImVec2& uv_d, ImU32 col);
    inline    void  PrimWriteVtx(const ImVec2& pos, const ImVec2& uv, ImU32 col)    { _VtxWritePtr->pos = pos; _VtxWritePtr->uv = uv; _VtxWritePtr->col = col; _VtxWritePtr++; _VtxCurrentIdx++; }
    inline    void  PrimWriteIdx(ImDrawIdx idx)                                     { *_IdxWritePtr = idx; _IdxWritePtr++; }
    inline    void  PrimVtx(const ImVec2& pos, const ImVec2& uv, ImU32 col)         { PrimWriteIdx((ImDrawIdx)_VtxCurrentIdx); PrimWriteVtx(pos, uv, col); } // Write vertex with unique index

#ifndef MsTrG_DISABLE_OBSOLETE_FUNCTIONS
    inline    void  AddBezierCurve(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, ImU32 col, float thickness, int num_segments = 0) { AddBezierCubic(p1, p2, p3, p4, col, thickness, num_segments); } // OBSOLETED in 1.80 (Jan 2021)
    inline    void  PathBezierCurveTo(const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, int num_segments = 0) { PathBezierCubicCurveTo(p2, p3, p4, num_segments); } // OBSOLETED in 1.80 (Jan 2021)
#endif

    // [Internal helpers]
    MsTrG_API void  _ResetForNewFrame();
    MsTrG_API void  _ClearFreeMemory();
    MsTrG_API void  _PopUnusedDrawCmd();
    MsTrG_API void  _TryMergeDrawCmds();
    MsTrG_API void  _OnChangedClipRect();
    MsTrG_API void  _OnChangedTextureID();
    MsTrG_API void  _OnChangedVtxOffset();
    MsTrG_API int   _CalcCircleAutoSegmentCount(float radius) const;
    MsTrG_API void  _PathArcToFastEx(const ImVec2& center, float radius, int a_min_sample, int a_max_sample, int a_step);
    MsTrG_API void  _PathArcToN(const ImVec2& center, float radius, float a_min, float a_max, int num_segments);
};

// All draw data to render a Dear MsTrG frame
// (NB: the style and the naming convention here is a little inconsistent, we currently preserve them for backward compatibility purpose,
// as this is one of the oldest structure exposed by the library! Basically, ImDrawList == CmdList)
struct ImDrawData
{
    bool            Valid;                  // Only valid after Render() is called and before the next NewFrame() is called.
    int             CmdListsCount;          // Number of ImDrawList* to render
    int             TotalIdxCount;          // For convenience, sum of all ImDrawList's IdxBuffer.Size
    int             TotalVtxCount;          // For convenience, sum of all ImDrawList's VtxBuffer.Size
    ImDrawList**    CmdLists;               // Array of ImDrawList* to render. The ImDrawList are owned by MsTrGContext and only pointed to from here.
    ImVec2          DisplayPos;             // Top-left position of the viewport to render (== top-left of the orthogonal projection matrix to use) (== GetMainViewport()->Pos for the main viewport, == (0.0) in most single-viewport applications)
    ImVec2          DisplaySize;            // Size of the viewport to render (== GetMainViewport()->Size for the main viewport, == io.DisplaySize in most single-viewport applications)
    ImVec2          FramebufferScale;       // Amount of pixels for each unit of DisplaySize. Based on io.DisplayFramebufferScale. Generally (1,1) on normal display, (2,2) on OSX with Retina display.

    // Functions
    ImDrawData()    { Clear(); }
    void Clear()    { memset(this, 0, sizeof(*this)); }     // The ImDrawList are owned by MsTrGContext!
    MsTrG_API void  DeIndexAllBuffers();                    // Helper to convert all buffers from indexed to non-indexed, in case you cannot render indexed. Note: this is slow and most likely a waste of resources. Always prefer indexed rendering!
    MsTrG_API void  ScaleClipRects(const ImVec2& fb_scale); // Helper to scale the ClipRect field of each ImDrawCmd. Use if your final output buffer is at a different scale than Dear MsTrG expects, or if there is a difference between your window resolution and framebuffer resolution.
};

//-----------------------------------------------------------------------------
// [SECTION] Font API (ImFontConfig, ImFontGlyph, ImFontAtlasFlags, ImFontAtlas, ImFontGlyphRangesBuilder, ImFont)
//-----------------------------------------------------------------------------

struct ImFontConfig
{
    void*           FontData;               //          // TTF/OTF data
    int             FontDataSize;           //          // TTF/OTF data size
    bool            FontDataOwnedByAtlas;   // true     // TTF/OTF data ownership taken by the container ImFontAtlas (will delete memory itself).
    int             FontNo;                 // 0        // Index of font within TTF/OTF file
    float           SizePixels;             //          // Size in pixels for rasterizer (more or less maps to the resulting font height).
    int             OversampleH;            // 3        // Rasterize at higher quality for sub-pixel positioning. Note the difference between 2 and 3 is minimal so you can reduce this to 2 to save memory. Read https://github.com/nothings/stb/blob/master/tests/oversample/README.md for details.
    int             OversampleV;            // 1        // Rasterize at higher quality for sub-pixel positioning. This is not really useful as we don't use sub-pixel positions on the Y axis.
    bool            PixelSnapH;             // false    // Align every glyph to pixel boundary. Useful e.g. if you are merging a non-pixel aligned font with the default font. If enabled, you can set OversampleH/V to 1.
    ImVec2          GlyphExtraSpacing;      // 0, 0     // Extra spacing (in pixels) between glyphs. Only X axis is supported for now.
    ImVec2          GlyphOffset;            // 0, 0     // Offset all glyphs from this font input.
    const ImWchar*  GlyphRanges;            // NULL     // Pointer to a user-provided list of Unicode range (2 value per range, values are inclusive, zero-terminated list). THE ARRAY DATA NEEDS TO PERSIST AS LONG AS THE FONT IS ALIVE.
    float           GlyphMinAdvanceX;       // 0        // Minimum AdvanceX for glyphs, set Min to align font icons, set both Min/Max to enforce mono-space font
    float           GlyphMaxAdvanceX;       // FLT_MAX  // Maximum AdvanceX for glyphs
    bool            MergeMode;              // false    // Merge into previous ImFont, so you can combine multiple inputs font into one ImFont (e.g. ASCII font + icons + Japanese glyphs). You may want to use GlyphOffset.y when merge font of different heights.
    unsigned int    FontBuilderFlags;       // 0        // Settings for custom font builder. THIS IS BUILDER IMPLEMENTATION DEPENDENT. Leave as zero if unsure.
    float           RasterizerMultiply;     // 1.0f     // Brighten (>1.0f) or darken (<1.0f) font output. Brightening small fonts may be a good workaround to make them more readable.
    ImWchar         EllipsisChar;           // -1       // Explicitly specify unicode codepoint of ellipsis character. When fonts are being merged first specified ellipsis will be used.

    // [Internal]
    char            Name[40];               // Name (strictly to ease debugging)
    ImFont*         DstFont;

    MsTrG_API ImFontConfig();
};

// Hold rendering data for one glyph.
// (Note: some language parsers may fail to convert the 31+1 bitfield members, in this case maybe drop store a single u32 or we can rework this)
struct ImFontGlyph
{
    unsigned int    Colored : 1;        // Flag to indicate glyph is colored and should generally ignore tinting (make it usable with no shift on little-endian as this is used in loops)
    unsigned int    Visible : 1;        // Flag to indicate glyph has no visible pixels (e.g. space). Allow early out when rendering.
    unsigned int    Codepoint : 30;     // 0x0000..0x10FFFF
    float           AdvanceX;           // Distance to next character (= data from font + ImFontConfig::GlyphExtraSpacing.x baked in)
    float           X0, Y0, X1, Y1;     // Glyph corners
    float           U0, V0, U1, V1;     // Texture coordinates
};

// Helper to build glyph ranges from text/string data. Feed your application strings/characters to it then call BuildRanges().
// This is essentially a tightly packed of vector of 64k booleans = 8KB storage.
struct ImFontGlyphRangesBuilder
{
    ImVector<ImU32> UsedChars;            // Store 1-bit per Unicode code point (0=unused, 1=used)

    ImFontGlyphRangesBuilder()              { Clear(); }
    inline void     Clear()                 { int size_in_bytes = (IM_UNICODE_CODEPOINT_MAX + 1) / 8; UsedChars.resize(size_in_bytes / (int)sizeof(ImU32)); memset(UsedChars.Data, 0, (size_t)size_in_bytes); }
    inline bool     GetBit(size_t n) const  { int off = (int)(n >> 5); ImU32 mask = 1u << (n & 31); return (UsedChars[off] & mask) != 0; }  // Get bit n in the array
    inline void     SetBit(size_t n)        { int off = (int)(n >> 5); ImU32 mask = 1u << (n & 31); UsedChars[off] |= mask; }               // Set bit n in the array
    inline void     AddChar(ImWchar c)      { SetBit(c); }                      // Add character
    MsTrG_API void  AddText(const char* text, const char* text_end = NULL);     // Add string (each character of the UTF-8 string are added)
    MsTrG_API void  AddRanges(const ImWchar* ranges);                           // Add ranges, e.g. builder.AddRanges(ImFontAtlas::GetGlyphRangesDefault()) to force add all of ASCII/Latin+Ext
    MsTrG_API void  BuildRanges(ImVector<ImWchar>* out_ranges);                 // Output new ranges
};

// See ImFontAtlas::AddCustomRectXXX functions.
struct ImFontAtlasCustomRect
{
    unsigned short  Width, Height;  // Input    // Desired rectangle dimension
    unsigned short  X, Y;           // Output   // Packed position in Atlas
    unsigned int    GlyphID;        // Input    // For custom font glyphs only (ID < 0x110000)
    float           GlyphAdvanceX;  // Input    // For custom font glyphs only: glyph xadvance
    ImVec2          GlyphOffset;    // Input    // For custom font glyphs only: glyph display offset
    ImFont*         Font;           // Input    // For custom font glyphs only: target font
    ImFontAtlasCustomRect()         { Width = Height = 0; X = Y = 0xFFFF; GlyphID = 0; GlyphAdvanceX = 0.0f; GlyphOffset = ImVec2(0, 0); Font = NULL; }
    bool IsPacked() const           { return X != 0xFFFF; }
};

// Flags for ImFontAtlas build
enum ImFontAtlasFlags_
{
    ImFontAtlasFlags_None               = 0,
    ImFontAtlasFlags_NoPowerOfTwoHeight = 1 << 0,   // Don't round the height to next power of two
    ImFontAtlasFlags_NoMouseCursors     = 1 << 1,   // Don't build software mouse cursors into the atlas (save a little texture memory)
    ImFontAtlasFlags_NoBakedLines       = 1 << 2    // Don't build thick line textures into the atlas (save a little texture memory, allow support for point/nearest filtering). The AntiAliasedLinesUseTex features uses them, otherwise they will be rendered using polygons (more expensive for CPU/GPU).
};

// Load and rasterize multiple TTF/OTF fonts into a same texture. The font atlas will build a single texture holding:
//  - One or more fonts.
//  - Custom graphics data needed to render the shapes needed by Dear MsTrG.
//  - Mouse cursor shapes for software cursor rendering (unless setting 'Flags |= ImFontAtlasFlags_NoMouseCursors' in the font atlas).
// It is the user-code responsibility to setup/build the atlas, then upload the pixel data into a texture accessible by your graphics api.
//  - Optionally, call any of the AddFont*** functions. If you don't call any, the default font embedded in the code will be loaded for you.
//  - Call GetTexDataAsAlpha8() or GetTexDataAsRGBA32() to build and retrieve pixels data.
//  - Upload the pixels data into a texture within your graphics system (see MsTrG_impl_xxxx.cpp examples)
//  - Call SetTexID(my_tex_id); and pass the pointer/identifier to your texture in a format natural to your graphics API.
//    This value will be passed back to you during rendering to identify the texture. Read FAQ entry about ImTextureID for more details.
// Common pitfalls:
// - If you pass a 'glyph_ranges' array to AddFont*** functions, you need to make sure that your array persist up until the
//   atlas is build (when calling GetTexData*** or Build()). We only copy the pointer, not the data.
// - Important: By default, AddFontFromMemoryTTF() takes ownership of the data. Even though we are not writing to it, we will free the pointer on destruction.
//   You can set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed,
// - Even though many functions are suffixed with "TTF", OTF data is supported just as well.
// - This is an old API and it is currently awkward for those and and various other reasons! We will address them in the future!
struct ImFontAtlas
{
    MsTrG_API ImFontAtlas();
    MsTrG_API ~ImFontAtlas();
    MsTrG_API ImFont*           AddFont(const ImFontConfig* font_cfg);
    MsTrG_API ImFont*           AddFontDefault(const ImFontConfig* font_cfg = NULL);
    MsTrG_API ImFont*           AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);
    MsTrG_API ImFont*           AddFontFromMemoryTTF(void* font_data, int font_size, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL); // Note: Transfer ownership of 'ttf_data' to ImFontAtlas! Will be deleted after destruction of the atlas. Set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed.
    MsTrG_API ImFont*           AddFontFromMemoryCompressedTTF(const void* compressed_font_data, int compressed_font_size, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL); // 'compressed_font_data' still owned by caller. Compress with binary_to_compressed_c.cpp.
    MsTrG_API ImFont*           AddFontFromMemoryCompressedBase85TTF(const char* compressed_font_data_base85, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);              // 'compressed_font_data_base85' still owned by caller. Compress with binary_to_compressed_c.cpp with -base85 parameter.
    MsTrG_API void              ClearInputData();           // Clear input data (all ImFontConfig structures including sizes, TTF data, glyph ranges, etc.) = all the data used to build the texture and fonts.
    MsTrG_API void              ClearTexData();             // Clear output texture data (CPU side). Saves RAM once the texture has been copied to graphics memory.
    MsTrG_API void              ClearFonts();               // Clear output font data (glyphs storage, UV coordinates).
    MsTrG_API void              Clear();                    // Clear all input and output.

    // Build atlas, retrieve pixel data.
    // User is in charge of copying the pixels into graphics memory (e.g. create a texture with your engine). Then store your texture handle with SetTexID().
    // The pitch is always = Width * BytesPerPixels (1 or 4)
    // Building in RGBA32 format is provided for convenience and compatibility, but note that unless you manually manipulate or copy color data into
    // the texture (e.g. when using the AddCustomRect*** api), then the RGB pixels emitted will always be white (~75% of memory/bandwidth waste.
    MsTrG_API bool              Build();                    // Build pixels data. This is called automatically for you by the GetTexData*** functions.
    MsTrG_API void              GetTexDataAsAlpha8(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel = NULL);  // 1 byte per-pixel
    MsTrG_API void              GetTexDataAsRGBA32(unsigned char** out_pixels, int* out_width, int* out_height, int* out_bytes_per_pixel = NULL);  // 4 bytes-per-pixel
    bool                        IsBuilt() const             { return Fonts.Size > 0 && TexReady; } // Bit ambiguous: used to detect when user didn't built texture but effectively we should check TexID != 0 except that would be backend dependent...
    void                        SetTexID(ImTextureID id)    { TexID = id; }

    //-------------------------------------------
    // Glyph Ranges
    //-------------------------------------------

    // Helpers to retrieve list of common Unicode ranges (2 value per range, values are inclusive, zero-terminated list)
    // NB: Make sure that your string are UTF-8 and NOT in your local code page. In C++11, you can create UTF-8 string literal using the u8"Hello world" syntax. See FAQ for details.
    // NB: Consider using ImFontGlyphRangesBuilder to build glyph ranges from textual data.
    MsTrG_API const ImWchar*    GetGlyphRangesDefault();                // Basic Latin, Extended Latin
    MsTrG_API const ImWchar*    GetGlyphRangesKorean();                 // Default + Korean characters
    MsTrG_API const ImWchar*    GetGlyphRangesJapanese();               // Default + Hiragana, Katakana, Half-Width, Selection of 2999 Ideographs
    MsTrG_API const ImWchar*    GetGlyphRangesChineseFull();            // Default + Half-Width + Japanese Hiragana/Katakana + full set of about 21000 CJK Unified Ideographs
    MsTrG_API const ImWchar*    GetGlyphRangesChineseSimplifiedCommon();// Default + Half-Width + Japanese Hiragana/Katakana + set of 2500 CJK Unified Ideographs for common simplified Chinese
    MsTrG_API const ImWchar*    GetGlyphRangesCyrillic();               // Default + about 400 Cyrillic characters
    MsTrG_API const ImWchar*    GetGlyphRangesThai();                   // Default + Thai characters
    MsTrG_API const ImWchar*    GetGlyphRangesVietnamese();             // Default + Vietnamese characters

    //-------------------------------------------
    // [BETA] Custom Rectangles/Glyphs API
    //-------------------------------------------

    // You can request arbitrary rectangles to be packed into the atlas, for your own purposes.
    // - After calling Build(), you can query the rectangle position and render your pixels.
    // - If you render colored output, set 'atlas->TexPixelsUseColors = true' as this may help some backends decide of prefered texture format.
    // - You can also request your rectangles to be mapped as font glyph (given a font + Unicode point),
    //   so you can render e.g. custom colorful icons and use them as regular glyphs.
    // - Read docs/FONTS.md for more details about using colorful icons.
    // - Note: this API may be redesigned later in order to support multi-monitor varying DPI settings.
    MsTrG_API int               AddCustomRectRegular(int width, int height);
    MsTrG_API int               AddCustomRectFontGlyph(ImFont* font, ImWchar id, int width, int height, float advance_x, const ImVec2& offset = ImVec2(0, 0));
    ImFontAtlasCustomRect*      GetCustomRectByIndex(int index) { IM_ASSERT(index >= 0); return &CustomRects[index]; }

    // [Internal]
    MsTrG_API void              CalcCustomRectUV(const ImFontAtlasCustomRect* rect, ImVec2* out_uv_min, ImVec2* out_uv_max) const;
    MsTrG_API bool              GetMouseCursorTexData(MsTrGMouseCursor cursor, ImVec2* out_offset, ImVec2* out_size, ImVec2 out_uv_border[2], ImVec2 out_uv_fill[2]);

    //-------------------------------------------
    // Members
    //-------------------------------------------

    ImFontAtlasFlags            Flags;              // Build flags (see ImFontAtlasFlags_)
    ImTextureID                 TexID;              // User data to refer to the texture once it has been uploaded to user's graphic systems. It is passed back to you during rendering via the ImDrawCmd structure.
    int                         TexDesiredWidth;    // Texture width desired by user before Build(). Must be a power-of-two. If have many glyphs your graphics API have texture size restrictions you may want to increase texture width to decrease height.
    int                         TexGlyphPadding;    // Padding between glyphs within texture in pixels. Defaults to 1. If your rendering method doesn't rely on bilinear filtering you may set this to 0 (will also need to set AntiAliasedLinesUseTex = false).
    bool                        Locked;             // Marked as Locked by MsTrG::NewFrame() so attempt to modify the atlas will assert.

    // [Internal]
    // NB: Access texture data via GetTexData*() calls! Which will setup a default font for you.
    bool                        TexReady;           // Set when texture was built matching current font input
    bool                        TexPixelsUseColors; // Tell whether our texture data is known to use colors (rather than just alpha channel), in order to help backend select a format.
    unsigned char*              TexPixelsAlpha8;    // 1 component per pixel, each component is unsigned 8-bit. Total size = TexWidth * TexHeight
    unsigned int*               TexPixelsRGBA32;    // 4 component per pixel, each component is unsigned 8-bit. Total size = TexWidth * TexHeight * 4
    int                         TexWidth;           // Texture width calculated during Build().
    int                         TexHeight;          // Texture height calculated during Build().
    ImVec2                      TexUvScale;         // = (1.0f/TexWidth, 1.0f/TexHeight)
    ImVec2                      TexUvWhitePixel;    // Texture coordinates to a white pixel
    ImVector<ImFont*>           Fonts;              // Hold all the fonts returned by AddFont*. Fonts[0] is the default font upon calling MsTrG::NewFrame(), use MsTrG::PushFont()/PopFont() to change the current font.
    ImVector<ImFontAtlasCustomRect> CustomRects;    // Rectangles for packing custom texture data into the atlas.
    ImVector<ImFontConfig>      ConfigData;         // Configuration data
    ImVec4                      TexUvLines[IM_DRAWLIST_TEX_LINES_WIDTH_MAX + 1];  // UVs for baked anti-aliased lines

    // [Internal] Font builder
    const ImFontBuilderIO*      FontBuilderIO;      // Opaque interface to a font builder (default to stb_truetype, can be changed to use FreeType by defining MsTrG_ENABLE_FREETYPE).
    unsigned int                FontBuilderFlags;   // Shared flags (for all fonts) for custom font builder. THIS IS BUILD IMPLEMENTATION DEPENDENT. Per-font override is also available in ImFontConfig.

    // [Internal] Packing data
    int                         PackIdMouseCursors; // Custom texture rectangle ID for white pixel and mouse cursors
    int                         PackIdLines;        // Custom texture rectangle ID for baked anti-aliased lines

    // [Obsolete]
    //typedef ImFontAtlasCustomRect    CustomRect;         // OBSOLETED in 1.72+
    //typedef ImFontGlyphRangesBuilder GlyphRangesBuilder; // OBSOLETED in 1.67+
};

// Font runtime data and rendering
// ImFontAtlas automatically loads a default embedded font for you when you call GetTexDataAsAlpha8() or GetTexDataAsRGBA32().
struct ImFont
{
    // Members: Hot ~20/24 bytes (for CalcTextSize)
    ImVector<float>             IndexAdvanceX;      // 12-16 // out //            // Sparse. Glyphs->AdvanceX in a directly indexable way (cache-friendly for CalcTextSize functions which only this this info, and are often bottleneck in large UI).
    float                       FallbackAdvanceX;   // 4     // out // = FallbackGlyph->AdvanceX
    float                       FontSize;           // 4     // in  //            // Height of characters/line, set during loading (don't change after loading)

    // Members: Hot ~28/40 bytes (for CalcTextSize + render loop)
    ImVector<ImWchar>           IndexLookup;        // 12-16 // out //            // Sparse. Index glyphs by Unicode code-point.
    ImVector<ImFontGlyph>       Glyphs;             // 12-16 // out //            // All glyphs.
    const ImFontGlyph*          FallbackGlyph;      // 4-8   // out // = FindGlyph(FontFallbackChar)

    // Members: Cold ~32/40 bytes
    ImFontAtlas*                ContainerAtlas;     // 4-8   // out //            // What we has been loaded into
    const ImFontConfig*         ConfigData;         // 4-8   // in  //            // Pointer within ContainerAtlas->ConfigData
    short                       ConfigDataCount;    // 2     // in  // ~ 1        // Number of ImFontConfig involved in creating this font. Bigger than 1 when merging multiple font sources into one ImFont.
    ImWchar                     FallbackChar;       // 2     // out // = FFFD/'?' // Character used if a glyph isn't found.
    ImWchar                     EllipsisChar;       // 2     // out // = '...'    // Character used for ellipsis rendering.
    ImWchar                     DotChar;            // 2     // out // = '.'      // Character used for ellipsis rendering (if a single '...' character isn't found)
    bool                        DirtyLookupTables;  // 1     // out //
    float                       Scale;              // 4     // in  // = 1.f      // Base font scale, multiplied by the per-window font scale which you can adjust with SetWindowFontScale()
    float                       Ascent, Descent;    // 4+4   // out //            // Ascent: distance from top to bottom of e.g. 'A' [0..FontSize]
    int                         MetricsTotalSurface;// 4     // out //            // Total surface in pixels to get an idea of the font rasterization/texture cost (not exact, we approximate the cost of padding between glyphs)
    ImU8                        Used4kPagesMap[(IM_UNICODE_CODEPOINT_MAX+1)/4096/8]; // 2 bytes if ImWchar=ImWchar16, 34 bytes if ImWchar==ImWchar32. Store 1-bit for each block of 4K codepoints that has one active glyph. This is mainly used to facilitate iterations across all used codepoints.

    // Methods
    MsTrG_API ImFont();
    MsTrG_API ~ImFont();
    MsTrG_API const ImFontGlyph*FindGlyph(ImWchar c) const;
    MsTrG_API const ImFontGlyph*FindGlyphNoFallback(ImWchar c) const;
    float                       GetCharAdvance(ImWchar c) const     { return ((int)c < IndexAdvanceX.Size) ? IndexAdvanceX[(int)c] : FallbackAdvanceX; }
    bool                        IsLoaded() const                    { return ContainerAtlas != NULL; }
    const char*                 GetDebugName() const                { return ConfigData ? ConfigData->Name : "<unknown>"; }

    // 'max_width' stops rendering after a certain width (could be turned into a 2d size). FLT_MAX to disable.
    // 'wrap_width' enable automatic word-wrapping across multiple lines to fit into given width. 0.0f to disable.
    MsTrG_API ImVec2            CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end = NULL, const char** remaining = NULL) const; // utf8
    MsTrG_API const char*       CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width) const;
    MsTrG_API void              RenderChar(ImDrawList* draw_list, float size, const ImVec2& pos, ImU32 col, ImWchar c) const;
    MsTrG_API void              RenderText(ImDrawList* draw_list, float size, const ImVec2& pos, ImU32 col, const ImVec4& clip_rect, const char* text_begin, const char* text_end, float wrap_width = 0.0f, bool cpu_fine_clip = false) const;

    // [Internal] Don't use!
    MsTrG_API void              BuildLookupTable();
    MsTrG_API void              ClearOutputData();
    MsTrG_API void              GrowIndex(int new_size);
    MsTrG_API void              AddGlyph(const ImFontConfig* src_cfg, ImWchar c, float x0, float y0, float x1, float y1, float u0, float v0, float u1, float v1, float advance_x);
    MsTrG_API void              AddRemapChar(ImWchar dst, ImWchar src, bool overwrite_dst = true); // Makes 'dst' character/glyph points to 'src' character/glyph. Currently needs to be called AFTER fonts have been built.
    MsTrG_API void              SetGlyphVisible(ImWchar c, bool visible);
    MsTrG_API bool              IsGlyphRangeUnused(unsigned int c_begin, unsigned int c_last);
};

//-----------------------------------------------------------------------------
// [SECTION] Viewports
//-----------------------------------------------------------------------------

// Flags stored in MsTrGViewport::Flags, giving indications to the platform backends.
enum MsTrGViewportFlags_
{
    MsTrGViewportFlags_None                     = 0,
    MsTrGViewportFlags_IsPlatformWindow         = 1 << 0,   // Represent a Platform Window
    MsTrGViewportFlags_IsPlatformMonitor        = 1 << 1,   // Represent a Platform Monitor (unused yet)
    MsTrGViewportFlags_OwnedByApp               = 1 << 2    // Platform Window: is created/managed by the application (rather than a dear MsTrG backend)
};

// - Currently represents the Platform Window created by the application which is hosting our Dear MsTrG windows.
// - In 'docking' branch with multi-viewport enabled, we extend this concept to have multiple active viewports.
// - In the future we will extend this concept further to also represent Platform Monitor and support a "no main platform window" operation mode.
// - About Main Area vs Work Area:
//   - Main Area = entire viewport.
//   - Work Area = entire viewport minus sections used by main menu bars (for platform windows), or by task bar (for platform monitor).
//   - Windows are generally trying to stay within the Work Area of their host viewport.
struct MsTrGViewport
{
    MsTrGViewportFlags  Flags;                  // See MsTrGViewportFlags_
    ImVec2              Pos;                    // Main Area: Position of the viewport (Dear MsTrG coordinates are the same as OS desktop/native coordinates)
    ImVec2              Size;                   // Main Area: Size of the viewport.
    ImVec2              WorkPos;                // Work Area: Position of the viewport minus task bars, menus bars, status bars (>= Pos)
    ImVec2              WorkSize;               // Work Area: Size of the viewport minus task bars, menu bars, status bars (<= Size)

    // Platform/Backend Dependent Data
    void*               PlatformHandleRaw;      // void* to hold lower-level, platform-native window handle (under Win32 this is expected to be a HWND, unused for other platforms)

    MsTrGViewport()     { memset(this, 0, sizeof(*this)); }

    // Helpers
    ImVec2              GetCenter() const       { return ImVec2(Pos.x + Size.x * 0.5f, Pos.y + Size.y * 0.5f); }
    ImVec2              GetWorkCenter() const   { return ImVec2(WorkPos.x + WorkSize.x * 0.5f, WorkPos.y + WorkSize.y * 0.5f); }
};

//-----------------------------------------------------------------------------
// [SECTION] Platform Dependent Interfaces
//-----------------------------------------------------------------------------

// (Optional) Support for IME (Input Method Editor) via the io.SetPlatformImeDataFn() function.
struct MsTrGPlatformImeData
{
    bool    WantVisible;        // A widget wants the IME to be visible
    ImVec2  InputPos;           // Position of the input cursor
    float   InputLineHeight;    // Line height

    MsTrGPlatformImeData() { memset(this, 0, sizeof(*this)); }
};

//-----------------------------------------------------------------------------
// [SECTION] Obsolete functions and types
// (Will be removed! Read 'API BREAKING CHANGES' section in MsTrG.cpp for details)
// Please keep your copy of dear MsTrG up to date! Occasionally set '#define MsTrG_DISABLE_OBSOLETE_FUNCTIONS' in imconfig.h to stay ahead.
//-----------------------------------------------------------------------------

namespace MsTrG
{
#ifndef MsTrG_DISABLE_OBSOLETE_KEYIO
    MsTrG_API int       GetKeyIndex(MsTrGKey key);  // map MsTrGKey_* values into legacy native key index. == io.KeyMap[key]
#else
    static inline int   GetKeyIndex(MsTrGKey key)   { IM_ASSERT(key >= MsTrGKey_NamedKey_BEGIN && key < MsTrGKey_NamedKey_END && "MsTrGKey and native_index was merged together and native_index is disabled by MsTrG_DISABLE_OBSOLETE_KEYIO. Please switch to MsTrGKey."); return key; }
#endif
}

#ifndef MsTrG_DISABLE_OBSOLETE_FUNCTIONS
namespace MsTrG
{
    // OBSOLETED in 1.88 (from May 2022)
    static inline void  CaptureKeyboardFromApp(bool want_capture_keyboard = true)   { SetNextFrameWantCaptureKeyboard(want_capture_keyboard); } // Renamed as name was misleading + removed default value.
    static inline void  CaptureMouseFromApp(bool want_capture_mouse = true)         { SetNextFrameWantCaptureMouse(want_capture_mouse); }       // Renamed as name was misleading + removed default value.
    // OBSOLETED in 1.86 (from November 2021)
    MsTrG_API void      CalcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end); // Calculate coarse clipping for large list of evenly sized items. Prefer using MsTrGListClipper.
    // OBSOLETED in 1.85 (from August 2021)
    static inline float GetWindowContentRegionWidth()                               { return GetWindowContentRegionMax().x - GetWindowContentRegionMin().x; }
    // OBSOLETED in 1.81 (from February 2021)
    MsTrG_API bool      ListBoxHeader(const char* label, int items_count, int height_in_items = -1); // Helper to calculate size from items_count and height_in_items
    static inline bool  ListBoxHeader(const char* label, const ImVec2& size = ImVec2(0, 0))         { return BeginListBox(label, size); }
    static inline void  ListBoxFooter() { EndListBox(); }
    // OBSOLETED in 1.79 (from August 2020)
    static inline void  OpenPopupContextItem(const char* str_id = NULL, MsTrGMouseButton mb = 1)    { OpenPopupOnItemClick(str_id, mb); } // Bool return value removed. Use IsWindowAppearing() in BeginPopup() instead. Renamed in 1.77, renamed back in 1.79. Sorry!
    // OBSOLETED in 1.78 (from June 2020)
    // Old drag/sliders functions that took a 'float power = 1.0' argument instead of flags.
    // For shared code, you can version check at compile-time with `#if MsTrG_VERSION_NUM >= 17704`.
    MsTrG_API bool      DragScalar(const char* label, MsTrGDataType data_type, void* p_data, float v_speed, const void* p_min, const void* p_max, const char* format, float power);
    MsTrG_API bool      DragScalarN(const char* label, MsTrGDataType data_type, void* p_data, int components, float v_speed, const void* p_min, const void* p_max, const char* format, float power);
    static inline bool  DragFloat(const char* label, float* v, float v_speed, float v_min, float v_max, const char* format, float power)    { return DragScalar(label, MsTrGDataType_Float, v, v_speed, &v_min, &v_max, format, power); }
    static inline bool  DragFloat2(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* format, float power) { return DragScalarN(label, MsTrGDataType_Float, v, 2, v_speed, &v_min, &v_max, format, power); }
    static inline bool  DragFloat3(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* format, float power) { return DragScalarN(label, MsTrGDataType_Float, v, 3, v_speed, &v_min, &v_max, format, power); }
    static inline bool  DragFloat4(const char* label, float v[4], float v_speed, float v_min, float v_max, const char* format, float power) { return DragScalarN(label, MsTrGDataType_Float, v, 4, v_speed, &v_min, &v_max, format, power); }
    MsTrG_API bool      SliderScalar(const char* label, MsTrGDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, float power);
    MsTrG_API bool      SliderScalarN(const char* label, MsTrGDataType data_type, void* p_data, int components, const void* p_min, const void* p_max, const char* format, float power);
    static inline bool  SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, float power)                 { return SliderScalar(label, MsTrGDataType_Float, v, &v_min, &v_max, format, power); }
    static inline bool  SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format, float power)              { return SliderScalarN(label, MsTrGDataType_Float, v, 2, &v_min, &v_max, format, power); }
    static inline bool  SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format, float power)              { return SliderScalarN(label, MsTrGDataType_Float, v, 3, &v_min, &v_max, format, power); }
    static inline bool  SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format, float power)              { return SliderScalarN(label, MsTrGDataType_Float, v, 4, &v_min, &v_max, format, power); }
    // OBSOLETED in 1.77 (from June 2020)
    static inline bool  BeginPopupContextWindow(const char* str_id, MsTrGMouseButton mb, bool over_items) { return BeginPopupContextWindow(str_id, mb | (over_items ? 0 : MsTrGPopupFlags_NoOpenOverItems)); }

    // Some of the older obsolete names along with their replacement (commented out so they are not reported in IDE)
    //static inline void  TreeAdvanceToLabelPos()               { SetCursorPosX(GetCursorPosX() + GetTreeNodeToLabelSpacing()); }   // OBSOLETED in 1.72 (from July 2019)
    //static inline void  SetNextTreeNodeOpen(bool open, MsTrGCond cond = 0) { SetNextItemOpen(open, cond); }                       // OBSOLETED in 1.71 (from June 2019)
    //static inline float GetContentRegionAvailWidth()          { return GetContentRegionAvail().x; }                               // OBSOLETED in 1.70 (from May 2019)
    static inline ImDrawList* GetOverlayDrawList()            { return GetForegroundDrawList(); }                                 // OBSOLETED in 1.69 (from Mar 2019)
    //static inline void  SetScrollHere(float ratio = 0.5f)     { SetScrollHereY(ratio); }                                          // OBSOLETED in 1.66 (from Nov 2018)
    //static inline bool  IsItemDeactivatedAfterChange()        { return IsItemDeactivatedAfterEdit(); }                            // OBSOLETED in 1.63 (from Aug 2018)
    //static inline bool  IsAnyWindowFocused()                  { return IsWindowFocused(MsTrGFocusedFlags_AnyWindow); }            // OBSOLETED in 1.60 (from Apr 2018)
    //static inline bool  IsAnyWindowHovered()                  { return IsWindowHovered(MsTrGHoveredFlags_AnyWindow); }            // OBSOLETED in 1.60 (between Dec 2017 and Apr 2018)
    //static inline void  ShowTestWindow()                      { return ShowDemoWindow(); }                                        // OBSOLETED in 1.53 (between Oct 2017 and Dec 2017)
    //static inline bool  IsRootWindowFocused()                 { return IsWindowFocused(MsTrGFocusedFlags_RootWindow); }           // OBSOLETED in 1.53 (between Oct 2017 and Dec 2017)
    //static inline bool  IsRootWindowOrAnyChildFocused()       { return IsWindowFocused(MsTrGFocusedFlags_RootAndChildWindows); }  // OBSOLETED in 1.53 (between Oct 2017 and Dec 2017)
    //static inline void  SetNextWindowContentWidth(float w)    { SetNextWindowContentSize(ImVec2(w, 0.0f)); }                      // OBSOLETED in 1.53 (between Oct 2017 and Dec 2017)
    //static inline float GetItemsLineHeightWithSpacing()       { return GetFrameHeightWithSpacing(); }                             // OBSOLETED in 1.53 (between Oct 2017 and Dec 2017)
}

// OBSOLETED in 1.82 (from Mars 2021): flags for AddRect(), AddRectFilled(), AddImageRounded(), PathRect()
typedef ImDrawFlags ImDrawCornerFlags;
enum ImDrawCornerFlags_
{
    ImDrawCornerFlags_None      = ImDrawFlags_RoundCornersNone,         // Was == 0 prior to 1.82, this is now == ImDrawFlags_RoundCornersNone which is != 0 and not implicit
    ImDrawCornerFlags_TopLeft   = ImDrawFlags_RoundCornersTopLeft,      // Was == 0x01 (1 << 0) prior to 1.82. Order matches ImDrawFlags_NoRoundCorner* flag (we exploit this internally).
    ImDrawCornerFlags_TopRight  = ImDrawFlags_RoundCornersTopRight,     // Was == 0x02 (1 << 1) prior to 1.82.
    ImDrawCornerFlags_BotLeft   = ImDrawFlags_RoundCornersBottomLeft,   // Was == 0x04 (1 << 2) prior to 1.82.
    ImDrawCornerFlags_BotRight  = ImDrawFlags_RoundCornersBottomRight,  // Was == 0x08 (1 << 3) prior to 1.82.
    ImDrawCornerFlags_All       = ImDrawFlags_RoundCornersAll,          // Was == 0x0F prior to 1.82
    ImDrawCornerFlags_Top       = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight,
    ImDrawCornerFlags_Bot       = ImDrawCornerFlags_BotLeft | ImDrawCornerFlags_BotRight,
    ImDrawCornerFlags_Left      = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotLeft,
    ImDrawCornerFlags_Right     = ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotRight
};

// RENAMED MsTrGKeyModFlags -> MsTrGModFlags in 1.88 (from April 2022)
typedef int MsTrGKeyModFlags;
enum MsTrGKeyModFlags_ { MsTrGKeyModFlags_None = MsTrGModFlags_None, MsTrGKeyModFlags_Ctrl = MsTrGModFlags_Ctrl, MsTrGKeyModFlags_Shift = MsTrGModFlags_Shift, MsTrGKeyModFlags_Alt = MsTrGModFlags_Alt, MsTrGKeyModFlags_Super = MsTrGModFlags_Super };

#endif // #ifndef MsTrG_DISABLE_OBSOLETE_FUNCTIONS

// RENAMED MsTrG_DISABLE_METRICS_WINDOW > MsTrG_DISABLE_DEBUG_TOOLS in 1.88 (from June 2022)
#if defined(MsTrG_DISABLE_METRICS_WINDOW) && !defined(MsTrG_DISABLE_OBSOLETE_FUNCTIONS) && !defined(MsTrG_DISABLE_DEBUG_TOOLS)
#define MsTrG_DISABLE_DEBUG_TOOLS
#endif
#if defined(MsTrG_DISABLE_METRICS_WINDOW) && defined(MsTrG_DISABLE_OBSOLETE_FUNCTIONS)
#error MsTrG_DISABLE_METRICS_WINDOW was renamed to MsTrG_DISABLE_DEBUG_TOOLS, please use new name.
#endif

//-----------------------------------------------------------------------------

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning (pop)
#endif

// Include MsTrG_user.h at the end of MsTrG.h (convenient for user to only explicitly include vanilla MsTrG.h)
#ifdef MsTrG_INCLUDE_MsTrG_USER_H
#include "MsTrG_user.h"
#endif

#endif // #ifndef MsTrG_DISABLE
