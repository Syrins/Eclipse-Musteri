// dear MsTrG, v1.88
// (internal structures/api)

// You may use this file to debug, understand or extend MsTrG features but we don't provide any guarantee of forward compatibility!
// Set:
//   #define MsTrG_DEFINE_MATH_OPERATORS
// To implement maths operators for ImVec2 (disabled by default to not collide with using IM_VEC2_CLASS_EXTRA along with your own math types+operators)

/*

Index of this file:

// [SECTION] Header mess
// [SECTION] Forward declarations
// [SECTION] Context pointer
// [SECTION] STB libraries includes
// [SECTION] Macros
// [SECTION] Generic helpers
// [SECTION] ImDrawList support
// [SECTION] Widgets support: flags, enums, data structures
// [SECTION] Inputs support
// [SECTION] Clipper support
// [SECTION] Navigation support
// [SECTION] Columns support
// [SECTION] Multi-select support
// [SECTION] Docking support
// [SECTION] Viewport support
// [SECTION] Settings support
// [SECTION] Metrics, Debug tools
// [SECTION] Generic context hooks
// [SECTION] MsTrGContext (main MsTrG context)
// [SECTION] MsTrGWindowTempData, MsTrGWindow
// [SECTION] Tab bar, Tab item support
// [SECTION] Table support
// [SECTION] MsTrG internal API
// [SECTION] ImFontAtlas internal API
// [SECTION] Test Engine specific hooks (MsTrG_test_engine)

*/

#pragma once
#ifndef MsTrG_DISABLE

//-----------------------------------------------------------------------------
// [SECTION] Header mess
//-----------------------------------------------------------------------------

#ifndef MsTrG_VERSION
#include "MsTrG.h"
#endif

#include <stdio.h>      // FILE*, sscanf
#include <stdlib.h>     // NULL, malloc, free, qsort, atoi, atof
#include <math.h>       // sqrtf, fabsf, fmodf, powf, floorf, ceilf, cosf, sinf
#include <limits.h>     // INT_MIN, INT_MAX

// Enable SSE intrinsics if available
#if (defined __SSE__ || defined __x86_64__ || defined _M_X64) && !defined(MsTrG_DISABLE_SSE)
#define MsTrG_ENABLE_SSE
#include <immintrin.h>
#endif

// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable: 4251)     // class 'xxx' needs to have dll-interface to be used by clients of struct 'xxx' // when MsTrG_API is set to__declspec(dllexport)
#pragma warning (disable: 26812)    // The enum type 'xxx' is unscoped. Prefer 'enum class' over 'enum' (Enum.3). [MSVC Static Analyzer)
#pragma warning (disable: 26495)    // [Static Analyzer] Variable 'XXX' is uninitialized. Always initialize a member variable (type.6).
#if defined(_MSC_VER) && _MSC_VER >= 1922 // MSVC 2019 16.2 or later
#pragma warning (disable: 5054)     // operator '|': deprecated between enumerations of different types
#endif
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic push
#if __has_warning("-Wunknown-warning-option")
#pragma clang diagnostic ignored "-Wunknown-warning-option"         // warning: unknown warning group 'xxx'
#endif
#pragma clang diagnostic ignored "-Wunknown-pragmas"                // warning: unknown warning group 'xxx'
#pragma clang diagnostic ignored "-Wfloat-equal"                    // warning: comparing floating point with == or != is unsafe // storing and comparing against same constants ok, for ImFloorSigned()
#pragma clang diagnostic ignored "-Wunused-function"                // for stb_textedit.h
#pragma clang diagnostic ignored "-Wmissing-prototypes"             // for stb_textedit.h
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#pragma clang diagnostic ignored "-Wdouble-promotion"
#pragma clang diagnostic ignored "-Wimplicit-int-float-conversion"  // warning: implicit conversion from 'xxx' to 'float' may lose precision
#pragma clang diagnostic ignored "-Wmissing-noreturn"               // warning: function 'xxx' could be declared with attribute 'noreturn'
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"              // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wclass-memaccess"      // [__GNUC__ >= 8] warning: 'memset/memcpy' clearing/writing an object of type 'xxxx' with no trivial copy-assignment; use assignment or value-initialization instead
#endif

// Legacy defines
#ifdef MsTrG_DISABLE_FORMAT_STRING_FUNCTIONS            // Renamed in 1.74
#error Use MsTrG_DISABLE_DEFAULT_FORMAT_FUNCTIONS
#endif
#ifdef MsTrG_DISABLE_MATH_FUNCTIONS                     // Renamed in 1.74
#error Use MsTrG_DISABLE_DEFAULT_MATH_FUNCTIONS
#endif

// Enable stb_truetype by default unless FreeType is enabled.
// You can compile with both by defining both MsTrG_ENABLE_FREETYPE and MsTrG_ENABLE_STB_TRUETYPE together.
#ifndef MsTrG_ENABLE_FREETYPE
#define MsTrG_ENABLE_STB_TRUETYPE
#endif

//-----------------------------------------------------------------------------
// [SECTION] Forward declarations
//-----------------------------------------------------------------------------

struct ImBitVector;                 // Store 1-bit per value
struct ImRect;                      // An axis-aligned rectangle (2 points)
struct ImDrawDataBuilder;           // Helper to build a ImDrawData instance
struct ImDrawListSharedData;        // Data shared between all ImDrawList instances
struct MsTrGColorMod;               // Stacked color modifier, backup of modified data so we can restore it
struct MsTrGContext;                // Main Dear MsTrG context
struct MsTrGContextHook;            // Hook for extensions like MsTrGTestEngine
struct MsTrGDataTypeInfo;           // Type information associated to a MsTrGDataType enum
struct MsTrGGroupData;              // Stacked storage data for BeginGroup()/EndGroup()
struct MsTrGInputTextState;         // Internal state of the currently focused/edited text input box
struct MsTrGLastItemData;           // Status storage for last submitted items
struct MsTrGMenuColumns;            // Simple column measurement, currently used for MenuItem() only
struct MsTrGNavItemData;            // Result of a gamepad/keyboard directional navigation move query result
struct MsTrGMetricsConfig;          // Storage for ShowMetricsWindow() and DebugNodeXXX() functions
struct MsTrGNextWindowData;         // Storage for SetNextWindow** functions
struct MsTrGNextItemData;           // Storage for SetNextItem** functions
struct MsTrGOldColumnData;          // Storage data for a single column for legacy Columns() api
struct MsTrGOldColumns;             // Storage data for a columns set for legacy Columns() api
struct MsTrGPopupData;              // Storage for current popup stack
struct MsTrGSettingsHandler;        // Storage for one type registered in the .ini file
struct MsTrGStackSizes;             // Storage of stack sizes for debugging/asserting
struct MsTrGStyleMod;               // Stacked style modifier, backup of modified data so we can restore it
struct MsTrGTabBar;                 // Storage for a tab bar
struct MsTrGTabItem;                // Storage for a tab item (within a tab bar)
struct MsTrGTable;                  // Storage for a table
struct MsTrGTableColumn;            // Storage for one column of a table
struct MsTrGTableInstanceData;      // Storage for one instance of a same table
struct MsTrGTableTempData;          // Temporary storage for one table (one per table in the stack), shared between tables.
struct MsTrGTableSettings;          // Storage for a table .ini settings
struct MsTrGTableColumnsSettings;   // Storage for a column .ini settings
struct MsTrGWindow;                 // Storage for one window
struct MsTrGWindowTempData;         // Temporary storage for one window (that's the data which in theory we could ditch at the end of the frame, in practice we currently keep it for each window)
struct MsTrGWindowSettings;         // Storage for a window .ini settings (we keep one of those even if the actual window wasn't instanced during this session)

// Use your programming IDE "Go to definition" facility on the names of the center columns to find the actual flags/enum lists.
typedef int MsTrGLayoutType;            // -> enum MsTrGLayoutType_         // Enum: Horizontal or vertical
typedef int MsTrGActivateFlags;         // -> enum MsTrGActivateFlags_      // Flags: for navigation/focus function (will be for ActivateItem() later)
typedef int MsTrGDebugLogFlags;         // -> enum MsTrGDebugLogFlags_      // Flags: for ShowDebugLogWindow(), g.DebugLogFlags
typedef int MsTrGItemFlags;             // -> enum MsTrGItemFlags_          // Flags: for PushItemFlag()
typedef int MsTrGItemStatusFlags;       // -> enum MsTrGItemStatusFlags_    // Flags: for DC.LastItemStatusFlags
typedef int MsTrGOldColumnFlags;        // -> enum MsTrGOldColumnFlags_     // Flags: for BeginColumns()
typedef int MsTrGNavHighlightFlags;     // -> enum MsTrGNavHighlightFlags_  // Flags: for RenderNavHighlight()
typedef int MsTrGNavDirSourceFlags;     // -> enum MsTrGNavDirSourceFlags_  // Flags: for GetNavInputAmount2d()
typedef int MsTrGNavMoveFlags;          // -> enum MsTrGNavMoveFlags_       // Flags: for navigation requests
typedef int MsTrGNextItemDataFlags;     // -> enum MsTrGNextItemDataFlags_  // Flags: for SetNextItemXXX() functions
typedef int MsTrGNextWindowDataFlags;   // -> enum MsTrGNextWindowDataFlags_// Flags: for SetNextWindowXXX() functions
typedef int MsTrGScrollFlags;           // -> enum MsTrGScrollFlags_        // Flags: for ScrollToItem() and navigation requests
typedef int MsTrGSeparatorFlags;        // -> enum MsTrGSeparatorFlags_     // Flags: for SeparatorEx()
typedef int MsTrGTextFlags;             // -> enum MsTrGTextFlags_          // Flags: for TextEx()
typedef int MsTrGTooltipFlags;          // -> enum MsTrGTooltipFlags_       // Flags: for BeginTooltipEx()

typedef void (*MsTrGErrorLogCallback)(void* user_data, const char* fmt, ...);

//-----------------------------------------------------------------------------
// [SECTION] Context pointer
// See implementation of this variable in MsTrG.cpp for comments and details.
//-----------------------------------------------------------------------------

#ifndef GMsTrG
extern MsTrG_API MsTrGContext* GMsTrG;  // Current implicit context pointer
#endif

//-------------------------------------------------------------------------
// [SECTION] STB libraries includes
//-------------------------------------------------------------------------

namespace ImStb
{

#undef STB_TEXTEDIT_STRING
#undef STB_TEXTEDIT_CHARTYPE
#define STB_TEXTEDIT_STRING             MsTrGInputTextState
#define STB_TEXTEDIT_CHARTYPE           ImWchar
#define STB_TEXTEDIT_GETWIDTH_NEWLINE   (-1.0f)
#define STB_TEXTEDIT_UNDOSTATECOUNT     99
#define STB_TEXTEDIT_UNDOCHARCOUNT      999
#include "imstb_textedit.h"

} // namespace ImStb

//-----------------------------------------------------------------------------
// [SECTION] Macros
//-----------------------------------------------------------------------------

// Debug Printing Into TTY
// (since MsTrG_VERSION_NUM >= 18729: MsTrG_DEBUG_LOG was reworked into MsTrG_DEBUG_PRINTF (and removed framecount from it). If you were using a #define MsTrG_DEBUG_LOG please rename)
#ifndef MsTrG_DEBUG_PRINTF
#ifndef MsTrG_DISABLE_DEFAULT_FORMAT_FUNCTIONS
#define MsTrG_DEBUG_PRINTF(_FMT,...)    printf(_FMT, __VA_ARGS__)
#else
#define MsTrG_DEBUG_PRINTF(_FMT,...)
#endif
#endif

// Debug Logging for ShowDebugLogWindow(). This is designed for relatively rare events so please don't spam.
#define MsTrG_DEBUG_LOG(...)            MsTrG::DebugLog(__VA_ARGS__);
#define MsTrG_DEBUG_LOG_ACTIVEID(...)   do { if (g.DebugLogFlags & MsTrGDebugLogFlags_EventActiveId) MsTrG_DEBUG_LOG(__VA_ARGS__); } while (0)
#define MsTrG_DEBUG_LOG_FOCUS(...)      do { if (g.DebugLogFlags & MsTrGDebugLogFlags_EventFocus)    MsTrG_DEBUG_LOG(__VA_ARGS__); } while (0)
#define MsTrG_DEBUG_LOG_POPUP(...)      do { if (g.DebugLogFlags & MsTrGDebugLogFlags_EventPopup)    MsTrG_DEBUG_LOG(__VA_ARGS__); } while (0)
#define MsTrG_DEBUG_LOG_NAV(...)        do { if (g.DebugLogFlags & MsTrGDebugLogFlags_EventNav)      MsTrG_DEBUG_LOG(__VA_ARGS__); } while (0)
#define MsTrG_DEBUG_LOG_IO(...)         do { if (g.DebugLogFlags & MsTrGDebugLogFlags_EventIO)       MsTrG_DEBUG_LOG(__VA_ARGS__); } while (0)

// Static Asserts
#define IM_STATIC_ASSERT(_COND)         static_assert(_COND, "")

// "Paranoid" Debug Asserts are meant to only be enabled during specific debugging/work, otherwise would slow down the code too much.
// We currently don't have many of those so the effect is currently negligible, but onward intent to add more aggressive ones in the code.
//#define MsTrG_DEBUG_PARANOID
#ifdef MsTrG_DEBUG_PARANOID
#define IM_ASSERT_PARANOID(_EXPR)       IM_ASSERT(_EXPR)
#else
#define IM_ASSERT_PARANOID(_EXPR)
#endif

// Error handling
// Down the line in some frameworks/languages we would like to have a way to redirect those to the programmer and recover from more faults.
#ifndef IM_ASSERT_USER_ERROR
#define IM_ASSERT_USER_ERROR(_EXP,_MSG) IM_ASSERT((_EXP) && _MSG)   // Recoverable User Error
#endif

// Misc Macros
#define IM_PI                           3.14159265358979323846f
#ifdef _WIN32
#define IM_NEWLINE                      "\r\n"   // Play it nice with Windows users (Update: since 2018-05, Notepad finally appears to support Unix-style carriage returns!)
#else
#define IM_NEWLINE                      "\n"
#endif
#define IM_TABSIZE                      (4)
#define IM_MEMALIGN(_OFF,_ALIGN)        (((_OFF) + ((_ALIGN) - 1)) & ~((_ALIGN) - 1))           // Memory align e.g. IM_ALIGN(0,4)=0, IM_ALIGN(1,4)=4, IM_ALIGN(4,4)=4, IM_ALIGN(5,4)=8
#define IM_F32_TO_INT8_UNBOUND(_VAL)    ((int)((_VAL) * 255.0f + ((_VAL)>=0 ? 0.5f : -0.5f)))   // Unsaturated, for display purpose
#define IM_F32_TO_INT8_SAT(_VAL)        ((int)(ImSaturate(_VAL) * 255.0f + 0.5f))               // Saturated, always output 0..255
#define IM_FLOOR(_VAL)                  ((float)(int)(_VAL))                                    // ImFloor() is not inlined in MSVC debug builds
#define IM_ROUND(_VAL)                  ((float)(int)((_VAL) + 0.5f))                           //

// Enforce cdecl calling convention for functions called by the standard library, in case compilation settings changed the default to e.g. __vectorcall
#ifdef _MSC_VER
#define MsTrG_CDECL __cdecl
#else
#define MsTrG_CDECL
#endif

// Warnings
#if defined(_MSC_VER) && !defined(__clang__)
#define IM_MSVC_WARNING_SUPPRESS(XXXX)  __pragma(warning(suppress: XXXX))
#else
#define IM_MSVC_WARNING_SUPPRESS(XXXX)
#endif

// Debug Tools
// Use 'Metrics/Debugger->Tools->Item Picker' to break into the call-stack of a specific item.
// This will call IM_DEBUG_BREAK() which you may redefine yourself. See https://github.com/scottt/debugbreak for more reference.
#ifndef IM_DEBUG_BREAK
#if defined (_MSC_VER)
#define IM_DEBUG_BREAK()    __debugbreak()
#elif defined(__clang__)
#define IM_DEBUG_BREAK()    __builtin_debugtrap()
#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
#define IM_DEBUG_BREAK()    __asm__ volatile("int $0x03")
#elif defined(__GNUC__) && defined(__thumb__)
#define IM_DEBUG_BREAK()    __asm__ volatile(".inst 0xde01")
#elif defined(__GNUC__) && defined(__arm__) && !defined(__thumb__)
#define IM_DEBUG_BREAK()    __asm__ volatile(".inst 0xe7f001f0");
#else
#define IM_DEBUG_BREAK()    IM_ASSERT(0)    // It is expected that you define IM_DEBUG_BREAK() into something that will break nicely in a debugger!
#endif
#endif // #ifndef IM_DEBUG_BREAK

//-----------------------------------------------------------------------------
// [SECTION] Generic helpers
// Note that the ImXXX helpers functions are lower-level than MsTrG functions.
// MsTrG functions or the MsTrG context are never called/used from other ImXXX functions.
//-----------------------------------------------------------------------------
// - Helpers: Hashing
// - Helpers: Sorting
// - Helpers: Bit manipulation
// - Helpers: String
// - Helpers: Formatting
// - Helpers: UTF-8 <> wchar conversions
// - Helpers: ImVec2/ImVec4 operators
// - Helpers: Maths
// - Helpers: Geometry
// - Helper: ImVec1
// - Helper: ImVec2ih
// - Helper: ImRect
// - Helper: ImBitArray
// - Helper: ImBitVector
// - Helper: ImSpan<>, ImSpanAllocator<>
// - Helper: ImPool<>
// - Helper: ImChunkStream<>
//-----------------------------------------------------------------------------

// Helpers: Hashing
MsTrG_API MsTrGID       ImHashData(const void* data, size_t data_size, ImU32 seed = 0);
MsTrG_API MsTrGID       ImHashStr(const char* data, size_t data_size = 0, ImU32 seed = 0);

// Helpers: Sorting
#ifndef ImQsort
static inline void      ImQsort(void* base, size_t count, size_t size_of_element, int(MsTrG_CDECL *compare_func)(void const*, void const*)) { if (count > 1) qsort(base, count, size_of_element, compare_func); }
#endif

// Helpers: Color Blending
MsTrG_API ImU32         ImAlphaBlendColors(ImU32 col_a, ImU32 col_b);

// Helpers: Bit manipulation
static inline bool      ImIsPowerOfTwo(int v)           { return v != 0 && (v & (v - 1)) == 0; }
static inline bool      ImIsPowerOfTwo(ImU64 v)         { return v != 0 && (v & (v - 1)) == 0; }
static inline int       ImUpperPowerOfTwo(int v)        { v--; v |= v >> 1; v |= v >> 2; v |= v >> 4; v |= v >> 8; v |= v >> 16; v++; return v; }

// Helpers: String
MsTrG_API int           ImStricmp(const char* str1, const char* str2);
MsTrG_API int           ImStrnicmp(const char* str1, const char* str2, size_t count);
MsTrG_API void          ImStrncpy(char* dst, const char* src, size_t count);
MsTrG_API char*         ImStrdup(const char* str);
MsTrG_API char*         ImStrdupcpy(char* dst, size_t* p_dst_size, const char* str);
MsTrG_API const char*   ImStrchrRange(const char* str_begin, const char* str_end, char c);
MsTrG_API int           ImStrlenW(const ImWchar* str);
MsTrG_API const char*   ImStreolRange(const char* str, const char* str_end);                // End end-of-line
MsTrG_API const ImWchar*ImStrbolW(const ImWchar* buf_mid_line, const ImWchar* buf_begin);   // Find beginning-of-line
MsTrG_API const char*   ImStristr(const char* haystack, const char* haystack_end, const char* needle, const char* needle_end);
MsTrG_API void          ImStrTrimBlanks(char* str);
MsTrG_API const char*   ImStrSkipBlank(const char* str);
static inline bool      ImCharIsBlankA(char c)          { return c == ' ' || c == '\t'; }
static inline bool      ImCharIsBlankW(unsigned int c)  { return c == ' ' || c == '\t' || c == 0x3000; }

// Helpers: Formatting
MsTrG_API int           ImFormatString(char* buf, size_t buf_size, const char* fmt, ...) IM_FMTARGS(3);
MsTrG_API int           ImFormatStringV(char* buf, size_t buf_size, const char* fmt, va_list args) IM_FMTLIST(3);
MsTrG_API void          ImFormatStringToTempBuffer(const char** out_buf, const char** out_buf_end, const char* fmt, ...) IM_FMTARGS(3);
MsTrG_API void          ImFormatStringToTempBufferV(const char** out_buf, const char** out_buf_end, const char* fmt, va_list args) IM_FMTLIST(3);
MsTrG_API const char*   ImParseFormatFindStart(const char* format);
MsTrG_API const char*   ImParseFormatFindEnd(const char* format);
MsTrG_API const char*   ImParseFormatTrimDecorations(const char* format, char* buf, size_t buf_size);
MsTrG_API void          ImParseFormatSanitizeForPrinting(const char* fmt_in, char* fmt_out, size_t fmt_out_size);
MsTrG_API const char*   ImParseFormatSanitizeForScanning(const char* fmt_in, char* fmt_out, size_t fmt_out_size);
MsTrG_API int           ImParseFormatPrecision(const char* format, int default_value);

// Helpers: UTF-8 <> wchar conversions
MsTrG_API const char*   ImTextCharToUtf8(char out_buf[5], unsigned int c);                                                      // return out_buf
MsTrG_API int           ImTextStrToUtf8(char* out_buf, int out_buf_size, const ImWchar* in_text, const ImWchar* in_text_end);   // return output UTF-8 bytes count
MsTrG_API int           ImTextCharFromUtf8(unsigned int* out_char, const char* in_text, const char* in_text_end);               // read one character. return input UTF-8 bytes count
MsTrG_API int           ImTextStrFromUtf8(ImWchar* out_buf, int out_buf_size, const char* in_text, const char* in_text_end, const char** in_remaining = NULL);   // return input UTF-8 bytes count
MsTrG_API int           ImTextCountCharsFromUtf8(const char* in_text, const char* in_text_end);                                 // return number of UTF-8 code-points (NOT bytes count)
MsTrG_API int           ImTextCountUtf8BytesFromChar(const char* in_text, const char* in_text_end);                             // return number of bytes to express one char in UTF-8
MsTrG_API int           ImTextCountUtf8BytesFromStr(const ImWchar* in_text, const ImWchar* in_text_end);                        // return number of bytes to express string in UTF-8

// Helpers: ImVec2/ImVec4 operators
// We are keeping those disabled by default so they don't leak in user space, to allow user enabling implicit cast operators between ImVec2 and their own types (using IM_VEC2_CLASS_EXTRA etc.)
// We unfortunately don't have a unary- operator for ImVec2 because this would needs to be defined inside the class itself.
#ifdef MsTrG_DEFINE_MATH_OPERATORS
IM_MSVC_RUNTIME_CHECKS_OFF
static inline ImVec2 operator*(const ImVec2& lhs, const float rhs)              { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
static inline ImVec2 operator/(const ImVec2& lhs, const float rhs)              { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs)            { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs)                  { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const float rhs)                  { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs)                { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }
static inline ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs)            { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
static inline ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs)            { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
static inline ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs)            { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
IM_MSVC_RUNTIME_CHECKS_RESTORE
#endif

// Helpers: File System
#ifdef MsTrG_DISABLE_FILE_FUNCTIONS
#define MsTrG_DISABLE_DEFAULT_FILE_FUNCTIONS
typedef void* ImFileHandle;
static inline ImFileHandle  ImFileOpen(const char*, const char*)                    { return NULL; }
static inline bool          ImFileClose(ImFileHandle)                               { return false; }
static inline ImU64         ImFileGetSize(ImFileHandle)                             { return (ImU64)-1; }
static inline ImU64         ImFileRead(void*, ImU64, ImU64, ImFileHandle)           { return 0; }
static inline ImU64         ImFileWrite(const void*, ImU64, ImU64, ImFileHandle)    { return 0; }
#endif
#ifndef MsTrG_DISABLE_DEFAULT_FILE_FUNCTIONS
typedef FILE* ImFileHandle;
MsTrG_API ImFileHandle      ImFileOpen(const char* filename, const char* mode);
MsTrG_API bool              ImFileClose(ImFileHandle file);
MsTrG_API ImU64             ImFileGetSize(ImFileHandle file);
MsTrG_API ImU64             ImFileRead(void* data, ImU64 size, ImU64 count, ImFileHandle file);
MsTrG_API ImU64             ImFileWrite(const void* data, ImU64 size, ImU64 count, ImFileHandle file);
#else
#define MsTrG_DISABLE_TTY_FUNCTIONS // Can't use stdout, fflush if we are not using default file functions
#endif
MsTrG_API void*             ImFileLoadToMemory(const char* filename, const char* mode, size_t* out_file_size = NULL, int padding_bytes = 0);

// Helpers: Maths
IM_MSVC_RUNTIME_CHECKS_OFF
// - Wrapper for standard libs functions. (Note that MsTrG_demo.cpp does _not_ use them to keep the code easy to copy)
#ifndef MsTrG_DISABLE_DEFAULT_MATH_FUNCTIONS
#define ImFabs(X)           fabsf(X)
#define ImSqrt(X)           sqrtf(X)
#define ImFmod(X, Y)        fmodf((X), (Y))
#define ImCos(X)            cosf(X)
#define ImSin(X)            sinf(X)
#define ImAcos(X)           acosf(X)
#define ImAtan2(Y, X)       atan2f((Y), (X))
#define ImAtof(STR)         atof(STR)
//#define ImFloorStd(X)     floorf(X)           // We use our own, see ImFloor() and ImFloorSigned()
#define ImCeil(X)           ceilf(X)
static inline float  ImPow(float x, float y)    { return powf(x, y); }          // DragBehaviorT/SliderBehaviorT uses ImPow with either float/double and need the precision
static inline double ImPow(double x, double y)  { return pow(x, y); }
static inline float  ImLog(float x)             { return logf(x); }             // DragBehaviorT/SliderBehaviorT uses ImLog with either float/double and need the precision
static inline double ImLog(double x)            { return log(x); }
static inline int    ImAbs(int x)               { return x < 0 ? -x : x; }
static inline float  ImAbs(float x)             { return fabsf(x); }
static inline double ImAbs(double x)            { return fabs(x); }
static inline float  ImSign(float x)            { return (x < 0.0f) ? -1.0f : (x > 0.0f) ? 1.0f : 0.0f; } // Sign operator - returns -1, 0 or 1 based on sign of argument
static inline double ImSign(double x)           { return (x < 0.0) ? -1.0 : (x > 0.0) ? 1.0 : 0.0; }
#ifdef MsTrG_ENABLE_SSE
static inline float  ImRsqrt(float x)           { return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x))); }
#else
static inline float  ImRsqrt(float x)           { return 1.0f / sqrtf(x); }
#endif
static inline double ImRsqrt(double x)          { return 1.0 / sqrt(x); }
#endif
// - ImMin/ImMax/ImClamp/ImLerp/ImSwap are used by widgets which support variety of types: signed/unsigned int/long long float/double
// (Exceptionally using templates here but we could also redefine them for those types)
template<typename T> static inline T ImMin(T lhs, T rhs)                        { return lhs < rhs ? lhs : rhs; }
template<typename T> static inline T ImMax(T lhs, T rhs)                        { return lhs >= rhs ? lhs : rhs; }
template<typename T> static inline T ImClamp(T v, T mn, T mx)                   { return (v < mn) ? mn : (v > mx) ? mx : v; }
template<typename T> static inline T ImLerp(T a, T b, float t)                  { return (T)(a + (b - a) * t); }
template<typename T> static inline void ImSwap(T& a, T& b)                      { T tmp = a; a = b; b = tmp; }
template<typename T> static inline T ImAddClampOverflow(T a, T b, T mn, T mx)   { if (b < 0 && (a < mn - b)) return mn; if (b > 0 && (a > mx - b)) return mx; return a + b; }
template<typename T> static inline T ImSubClampOverflow(T a, T b, T mn, T mx)   { if (b > 0 && (a < mn + b)) return mn; if (b < 0 && (a > mx + b)) return mx; return a - b; }
// - Misc maths helpers
static inline ImVec2 ImMin(const ImVec2& lhs, const ImVec2& rhs)                { return ImVec2(lhs.x < rhs.x ? lhs.x : rhs.x, lhs.y < rhs.y ? lhs.y : rhs.y); }
static inline ImVec2 ImMax(const ImVec2& lhs, const ImVec2& rhs)                { return ImVec2(lhs.x >= rhs.x ? lhs.x : rhs.x, lhs.y >= rhs.y ? lhs.y : rhs.y); }
static inline ImVec2 ImClamp(const ImVec2& v, const ImVec2& mn, ImVec2 mx)      { return ImVec2((v.x < mn.x) ? mn.x : (v.x > mx.x) ? mx.x : v.x, (v.y < mn.y) ? mn.y : (v.y > mx.y) ? mx.y : v.y); }
static inline ImVec2 ImLerp(const ImVec2& a, const ImVec2& b, float t)          { return ImVec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t); }
static inline ImVec2 ImLerp(const ImVec2& a, const ImVec2& b, const ImVec2& t)  { return ImVec2(a.x + (b.x - a.x) * t.x, a.y + (b.y - a.y) * t.y); }
static inline ImVec4 ImLerp(const ImVec4& a, const ImVec4& b, float t)          { return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t); }
static inline float  ImSaturate(float f)                                        { return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f; }
static inline float  ImLengthSqr(const ImVec2& lhs)                             { return (lhs.x * lhs.x) + (lhs.y * lhs.y); }
static inline float  ImLengthSqr(const ImVec4& lhs)                             { return (lhs.x * lhs.x) + (lhs.y * lhs.y) + (lhs.z * lhs.z) + (lhs.w * lhs.w); }
static inline float  ImInvLength(const ImVec2& lhs, float fail_value)           { float d = (lhs.x * lhs.x) + (lhs.y * lhs.y); if (d > 0.0f) return ImRsqrt(d); return fail_value; }
static inline float  ImFloor(float f)                                           { return (float)(int)(f); }
static inline float  ImFloorSigned(float f)                                     { return (float)((f >= 0 || (float)(int)f == f) ? (int)f : (int)f - 1); } // Decent replacement for floorf()
static inline ImVec2 ImFloor(const ImVec2& v)                                   { return ImVec2((float)(int)(v.x), (float)(int)(v.y)); }
static inline ImVec2 ImFloorSigned(const ImVec2& v)                             { return ImVec2(ImFloorSigned(v.x), ImFloorSigned(v.y)); }
static inline int    ImModPositive(int a, int b)                                { return (a + b) % b; }
static inline float  ImDot(const ImVec2& a, const ImVec2& b)                    { return a.x * b.x + a.y * b.y; }
static inline ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a)        { return ImVec2(v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a); }
static inline float  ImLinearSweep(float current, float target, float speed)    { if (current < target) return ImMin(current + speed, target); if (current > target) return ImMax(current - speed, target); return current; }
static inline ImVec2 ImMul(const ImVec2& lhs, const ImVec2& rhs)                { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline bool   ImIsFloatAboveGuaranteedIntegerPrecision(float f)          { return f <= -16777216 || f >= 16777216; }
IM_MSVC_RUNTIME_CHECKS_RESTORE

// Helpers: Geometry
MsTrG_API ImVec2     ImBezierCubicCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, float t);
MsTrG_API ImVec2     ImBezierCubicClosestPoint(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p, int num_segments);       // For curves with explicit number of segments
MsTrG_API ImVec2     ImBezierCubicClosestPointCasteljau(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, const ImVec2& p, float tess_tol);// For auto-tessellated curves you can use tess_tol = style.CurveTessellationTol
MsTrG_API ImVec2     ImBezierQuadraticCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, float t);
MsTrG_API ImVec2     ImLineClosestPoint(const ImVec2& a, const ImVec2& b, const ImVec2& p);
MsTrG_API bool       ImTriangleContainsPoint(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& p);
MsTrG_API ImVec2     ImTriangleClosestPoint(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& p);
MsTrG_API void       ImTriangleBarycentricCoords(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& p, float& out_u, float& out_v, float& out_w);
inline float         ImTriangleArea(const ImVec2& a, const ImVec2& b, const ImVec2& c) { return ImFabs((a.x * (b.y - c.y)) + (b.x * (c.y - a.y)) + (c.x * (a.y - b.y))) * 0.5f; }
MsTrG_API MsTrGDir   ImGetDirQuadrantFromDelta(float dx, float dy);

// Helper: ImVec1 (1D vector)
// (this odd construct is used to facilitate the transition between 1D and 2D, and the maintenance of some branches/patches)
IM_MSVC_RUNTIME_CHECKS_OFF
struct ImVec1
{
    float   x;
    constexpr ImVec1()         : x(0.0f) { }
    constexpr ImVec1(float _x) : x(_x) { }
};

// Helper: ImVec2ih (2D vector, half-size integer, for long-term packed storage)
struct ImVec2ih
{
    short   x, y;
    constexpr ImVec2ih()                           : x(0), y(0) {}
    constexpr ImVec2ih(short _x, short _y)         : x(_x), y(_y) {}
    constexpr explicit ImVec2ih(const ImVec2& rhs) : x((short)rhs.x), y((short)rhs.y) {}
};

// Helper: ImRect (2D axis aligned bounding-box)
// NB: we can't rely on ImVec2 math operators being available here!
struct MsTrG_API ImRect
{
    ImVec2      Min;    // Upper-left
    ImVec2      Max;    // Lower-right

    constexpr ImRect()                                        : Min(0.0f, 0.0f), Max(0.0f, 0.0f)  {}
    constexpr ImRect(const ImVec2& min, const ImVec2& max)    : Min(min), Max(max)                {}
    constexpr ImRect(const ImVec4& v)                         : Min(v.x, v.y), Max(v.z, v.w)      {}
    constexpr ImRect(float x1, float y1, float x2, float y2)  : Min(x1, y1), Max(x2, y2)          {}

    ImVec2      GetCenter() const                   { return ImVec2((Min.x + Max.x) * 0.5f, (Min.y + Max.y) * 0.5f); }
    ImVec2      GetSize() const                     { return ImVec2(Max.x - Min.x, Max.y - Min.y); }
    float       GetWidth() const                    { return Max.x - Min.x; }
    float       GetHeight() const                   { return Max.y - Min.y; }
    float       GetArea() const                     { return (Max.x - Min.x) * (Max.y - Min.y); }
    ImVec2      GetTL() const                       { return Min; }                   // Top-left
    ImVec2      GetTR() const                       { return ImVec2(Max.x, Min.y); }  // Top-right
    ImVec2      GetBL() const                       { return ImVec2(Min.x, Max.y); }  // Bottom-left
    ImVec2      GetBR() const                       { return Max; }                   // Bottom-right
    bool        Contains(const ImVec2& p) const     { return p.x     >= Min.x && p.y     >= Min.y && p.x     <  Max.x && p.y     <  Max.y; }
    bool        Contains(const ImRect& r) const     { return r.Min.x >= Min.x && r.Min.y >= Min.y && r.Max.x <= Max.x && r.Max.y <= Max.y; }
    bool        Overlaps(const ImRect& r) const     { return r.Min.y <  Max.y && r.Max.y >  Min.y && r.Min.x <  Max.x && r.Max.x >  Min.x; }
    void        Add(const ImVec2& p)                { if (Min.x > p.x)     Min.x = p.x;     if (Min.y > p.y)     Min.y = p.y;     if (Max.x < p.x)     Max.x = p.x;     if (Max.y < p.y)     Max.y = p.y; }
    void        Add(const ImRect& r)                { if (Min.x > r.Min.x) Min.x = r.Min.x; if (Min.y > r.Min.y) Min.y = r.Min.y; if (Max.x < r.Max.x) Max.x = r.Max.x; if (Max.y < r.Max.y) Max.y = r.Max.y; }
    void        Expand(const float amount)          { Min.x -= amount;   Min.y -= amount;   Max.x += amount;   Max.y += amount; }
    void        Expand(const ImVec2& amount)        { Min.x -= amount.x; Min.y -= amount.y; Max.x += amount.x; Max.y += amount.y; }
    void        Translate(const ImVec2& d)          { Min.x += d.x; Min.y += d.y; Max.x += d.x; Max.y += d.y; }
    void        TranslateX(float dx)                { Min.x += dx; Max.x += dx; }
    void        TranslateY(float dy)                { Min.y += dy; Max.y += dy; }
    void        ClipWith(const ImRect& r)           { Min = ImMax(Min, r.Min); Max = ImMin(Max, r.Max); }                   // Simple version, may lead to an inverted rectangle, which is fine for Contains/Overlaps test but not for display.
    void        ClipWithFull(const ImRect& r)       { Min = ImClamp(Min, r.Min, r.Max); Max = ImClamp(Max, r.Min, r.Max); } // Full version, ensure both points are fully clipped.
    void        Floor()                             { Min.x = IM_FLOOR(Min.x); Min.y = IM_FLOOR(Min.y); Max.x = IM_FLOOR(Max.x); Max.y = IM_FLOOR(Max.y); }
    bool        IsInverted() const                  { return Min.x > Max.x || Min.y > Max.y; }
    ImVec4      ToVec4() const                      { return ImVec4(Min.x, Min.y, Max.x, Max.y); }
};
IM_MSVC_RUNTIME_CHECKS_RESTORE

// Helper: ImBitArray
inline bool     ImBitArrayTestBit(const ImU32* arr, int n)      { ImU32 mask = (ImU32)1 << (n & 31); return (arr[n >> 5] & mask) != 0; }
inline void     ImBitArrayClearBit(ImU32* arr, int n)           { ImU32 mask = (ImU32)1 << (n & 31); arr[n >> 5] &= ~mask; }
inline void     ImBitArraySetBit(ImU32* arr, int n)             { ImU32 mask = (ImU32)1 << (n & 31); arr[n >> 5] |= mask; }
inline void     ImBitArraySetBitRange(ImU32* arr, int n, int n2) // Works on range [n..n2)
{
    n2--;
    while (n <= n2)
    {
        int a_mod = (n & 31);
        int b_mod = (n2 > (n | 31) ? 31 : (n2 & 31)) + 1;
        ImU32 mask = (ImU32)(((ImU64)1 << b_mod) - 1) & ~(ImU32)(((ImU64)1 << a_mod) - 1);
        arr[n >> 5] |= mask;
        n = (n + 32) & ~31;
    }
}

// Helper: ImBitArray class (wrapper over ImBitArray functions)
// Store 1-bit per value.
template<int BITCOUNT, int OFFSET = 0>
struct ImBitArray
{
    ImU32           Storage[(BITCOUNT + 31) >> 5];
    ImBitArray()                                { ClearAllBits(); }
    void            ClearAllBits()              { memset(Storage, 0, sizeof(Storage)); }
    void            SetAllBits()                { memset(Storage, 255, sizeof(Storage)); }
    bool            TestBit(int n) const        { n += OFFSET; IM_ASSERT(n >= 0 && n < BITCOUNT); return ImBitArrayTestBit(Storage, n); }
    void            SetBit(int n)               { n += OFFSET; IM_ASSERT(n >= 0 && n < BITCOUNT); ImBitArraySetBit(Storage, n); }
    void            ClearBit(int n)             { n += OFFSET; IM_ASSERT(n >= 0 && n < BITCOUNT); ImBitArrayClearBit(Storage, n); }
    void            SetBitRange(int n, int n2)  { n += OFFSET; n2 += OFFSET; IM_ASSERT(n >= 0 && n < BITCOUNT && n2 > n && n2 <= BITCOUNT); ImBitArraySetBitRange(Storage, n, n2); } // Works on range [n..n2)
    bool            operator[](int n) const     { n += OFFSET; IM_ASSERT(n >= 0 && n < BITCOUNT); return ImBitArrayTestBit(Storage, n); }
};

// Helper: ImBitVector
// Store 1-bit per value.
struct MsTrG_API ImBitVector
{
    ImVector<ImU32> Storage;
    void            Create(int sz)              { Storage.resize((sz + 31) >> 5); memset(Storage.Data, 0, (size_t)Storage.Size * sizeof(Storage.Data[0])); }
    void            Clear()                     { Storage.clear(); }
    bool            TestBit(int n) const        { IM_ASSERT(n < (Storage.Size << 5)); return ImBitArrayTestBit(Storage.Data, n); }
    void            SetBit(int n)               { IM_ASSERT(n < (Storage.Size << 5)); ImBitArraySetBit(Storage.Data, n); }
    void            ClearBit(int n)             { IM_ASSERT(n < (Storage.Size << 5)); ImBitArrayClearBit(Storage.Data, n); }
};

// Helper: ImSpan<>
// Pointing to a span of data we don't own.
template<typename T>
struct ImSpan
{
    T*                  Data;
    T*                  DataEnd;

    // Constructors, destructor
    inline ImSpan()                                 { Data = DataEnd = NULL; }
    inline ImSpan(T* data, int size)                { Data = data; DataEnd = data + size; }
    inline ImSpan(T* data, T* data_end)             { Data = data; DataEnd = data_end; }

    inline void         set(T* data, int size)      { Data = data; DataEnd = data + size; }
    inline void         set(T* data, T* data_end)   { Data = data; DataEnd = data_end; }
    inline int          size() const                { return (int)(ptrdiff_t)(DataEnd - Data); }
    inline int          size_in_bytes() const       { return (int)(ptrdiff_t)(DataEnd - Data) * (int)sizeof(T); }
    inline T&           operator[](int i)           { T* p = Data + i; IM_ASSERT(p >= Data && p < DataEnd); return *p; }
    inline const T&     operator[](int i) const     { const T* p = Data + i; IM_ASSERT(p >= Data && p < DataEnd); return *p; }

    inline T*           begin()                     { return Data; }
    inline const T*     begin() const               { return Data; }
    inline T*           end()                       { return DataEnd; }
    inline const T*     end() const                 { return DataEnd; }

    // Utilities
    inline int  index_from_ptr(const T* it) const   { IM_ASSERT(it >= Data && it < DataEnd); const ptrdiff_t off = it - Data; return (int)off; }
};

// Helper: ImSpanAllocator<>
// Facilitate storing multiple chunks into a single large block (the "arena")
// - Usage: call Reserve() N times, allocate GetArenaSizeInBytes() worth, pass it to SetArenaBasePtr(), call GetSpan() N times to retrieve the aligned ranges.
template<int CHUNKS>
struct ImSpanAllocator
{
    char*   BasePtr;
    int     CurrOff;
    int     CurrIdx;
    int     Offsets[CHUNKS];
    int     Sizes[CHUNKS];

    ImSpanAllocator()                               { memset(this, 0, sizeof(*this)); }
    inline void  Reserve(int n, size_t sz, int a=4) { IM_ASSERT(n == CurrIdx && n < CHUNKS); CurrOff = IM_MEMALIGN(CurrOff, a); Offsets[n] = CurrOff; Sizes[n] = (int)sz; CurrIdx++; CurrOff += (int)sz; }
    inline int   GetArenaSizeInBytes()              { return CurrOff; }
    inline void  SetArenaBasePtr(void* base_ptr)    { BasePtr = (char*)base_ptr; }
    inline void* GetSpanPtrBegin(int n)             { IM_ASSERT(n >= 0 && n < CHUNKS && CurrIdx == CHUNKS); return (void*)(BasePtr + Offsets[n]); }
    inline void* GetSpanPtrEnd(int n)               { IM_ASSERT(n >= 0 && n < CHUNKS && CurrIdx == CHUNKS); return (void*)(BasePtr + Offsets[n] + Sizes[n]); }
    template<typename T>
    inline void  GetSpan(int n, ImSpan<T>* span)    { span->set((T*)GetSpanPtrBegin(n), (T*)GetSpanPtrEnd(n)); }
};

// Helper: ImPool<>
// Basic keyed storage for contiguous instances, slow/amortized insertion, O(1) indexable, O(Log N) queries by ID over a dense/hot buffer,
// Honor constructor/destructor. Add/remove invalidate all pointers. Indexes have the same lifetime as the associated object.
typedef int ImPoolIdx;
template<typename T>
struct ImPool
{
    ImVector<T>     Buf;        // Contiguous data
    MsTrGStorage    Map;        // ID->Index
    ImPoolIdx       FreeIdx;    // Next free idx to use
    ImPoolIdx       AliveCount; // Number of active/alive items (for display purpose)

    ImPool()    { FreeIdx = AliveCount = 0; }
    ~ImPool()   { Clear(); }
    T*          GetByKey(MsTrGID key)               { int idx = Map.GetInt(key, -1); return (idx != -1) ? &Buf[idx] : NULL; }
    T*          GetByIndex(ImPoolIdx n)             { return &Buf[n]; }
    ImPoolIdx   GetIndex(const T* p) const          { IM_ASSERT(p >= Buf.Data && p < Buf.Data + Buf.Size); return (ImPoolIdx)(p - Buf.Data); }
    T*          GetOrAddByKey(MsTrGID key)          { int* p_idx = Map.GetIntRef(key, -1); if (*p_idx != -1) return &Buf[*p_idx]; *p_idx = FreeIdx; return Add(); }
    bool        Contains(const T* p) const          { return (p >= Buf.Data && p < Buf.Data + Buf.Size); }
    void        Clear()                             { for (int n = 0; n < Map.Data.Size; n++) { int idx = Map.Data[n].val_i; if (idx != -1) Buf[idx].~T(); } Map.Clear(); Buf.clear(); FreeIdx = AliveCount = 0; }
    T*          Add()                               { int idx = FreeIdx; if (idx == Buf.Size) { Buf.resize(Buf.Size + 1); FreeIdx++; } else { FreeIdx = *(int*)&Buf[idx]; } IM_PLACEMENT_NEW(&Buf[idx]) T(); AliveCount++; return &Buf[idx]; }
    void        Remove(MsTrGID key, const T* p)     { Remove(key, GetIndex(p)); }
    void        Remove(MsTrGID key, ImPoolIdx idx)  { Buf[idx].~T(); *(int*)&Buf[idx] = FreeIdx; FreeIdx = idx; Map.SetInt(key, -1); AliveCount--; }
    void        Reserve(int capacity)               { Buf.reserve(capacity); Map.Data.reserve(capacity); }

    // To iterate a ImPool: for (int n = 0; n < pool.GetMapSize(); n++) if (T* t = pool.TryGetMapData(n)) { ... }
    // Can be avoided if you know .Remove() has never been called on the pool, or AliveCount == GetMapSize()
    int         GetAliveCount() const               { return AliveCount; }      // Number of active/alive items in the pool (for display purpose)
    int         GetBufSize() const                  { return Buf.Size; }
    int         GetMapSize() const                  { return Map.Data.Size; }   // It is the map we need iterate to find valid items, since we don't have "alive" storage anywhere
    T*          TryGetMapData(ImPoolIdx n)          { int idx = Map.Data[n].val_i; if (idx == -1) return NULL; return GetByIndex(idx); }
#ifndef MsTrG_DISABLE_OBSOLETE_FUNCTIONS
    int         GetSize()                           { return GetMapSize(); } // For ImPlot: should use GetMapSize() from (MsTrG_VERSION_NUM >= 18304)
#endif
};

// Helper: ImChunkStream<>
// Build and iterate a contiguous stream of variable-sized structures.
// This is used by Settings to store persistent data while reducing allocation count.
// We store the chunk size first, and align the final size on 4 bytes boundaries.
// The tedious/zealous amount of casting is to avoid -Wcast-align warnings.
template<typename T>
struct ImChunkStream
{
    ImVector<char>  Buf;

    void    clear()                     { Buf.clear(); }
    bool    empty() const               { return Buf.Size == 0; }
    int     size() const                { return Buf.Size; }
    T*      alloc_chunk(size_t sz)      { size_t HDR_SZ = 4; sz = IM_MEMALIGN(HDR_SZ + sz, 4u); int off = Buf.Size; Buf.resize(off + (int)sz); ((int*)(void*)(Buf.Data + off))[0] = (int)sz; return (T*)(void*)(Buf.Data + off + (int)HDR_SZ); }
    T*      begin()                     { size_t HDR_SZ = 4; if (!Buf.Data) return NULL; return (T*)(void*)(Buf.Data + HDR_SZ); }
    T*      next_chunk(T* p)            { size_t HDR_SZ = 4; IM_ASSERT(p >= begin() && p < end()); p = (T*)(void*)((char*)(void*)p + chunk_size(p)); if (p == (T*)(void*)((char*)end() + HDR_SZ)) return (T*)0; IM_ASSERT(p < end()); return p; }
    int     chunk_size(const T* p)      { return ((const int*)p)[-1]; }
    T*      end()                       { return (T*)(void*)(Buf.Data + Buf.Size); }
    int     offset_from_ptr(const T* p) { IM_ASSERT(p >= begin() && p < end()); const ptrdiff_t off = (const char*)p - Buf.Data; return (int)off; }
    T*      ptr_from_offset(int off)    { IM_ASSERT(off >= 4 && off < Buf.Size); return (T*)(void*)(Buf.Data + off); }
    void    swap(ImChunkStream<T>& rhs) { rhs.Buf.swap(Buf); }

};

//-----------------------------------------------------------------------------
// [SECTION] ImDrawList support
//-----------------------------------------------------------------------------

// ImDrawList: Helper function to calculate a circle's segment count given its radius and a "maximum error" value.
// Estimation of number of circle segment based on error is derived using method described in https://stackoverflow.com/a/2244088/15194693
// Number of segments (N) is calculated using equation:
//   N = ceil ( pi / acos(1 - error / r) )     where r > 0, error <= r
// Our equation is significantly simpler that one in the post thanks for choosing segment that is
// perpendicular to X axis. Follow steps in the article from this starting condition and you will
// will get this result.
//
// Rendering circles with an odd number of segments, while mathematically correct will produce
// asymmetrical results on the raster grid. Therefore we're rounding N to next even number (7->8, 8->8, 9->10 etc.)
#define IM_ROUNDUP_TO_EVEN(_V)                                  ((((_V) + 1) / 2) * 2)
#define IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MIN                     4
#define IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX                     512
#define IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC(_RAD,_MAXERROR)    ImClamp(IM_ROUNDUP_TO_EVEN((int)ImCeil(IM_PI / ImAcos(1 - ImMin((_MAXERROR), (_RAD)) / (_RAD)))), IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MIN, IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_MAX)

// Raw equation from IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC rewritten for 'r' and 'error'.
#define IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC_R(_N,_MAXERROR)    ((_MAXERROR) / (1 - ImCos(IM_PI / ImMax((float)(_N), IM_PI))))
#define IM_DRAWLIST_CIRCLE_AUTO_SEGMENT_CALC_ERROR(_N,_RAD)     ((1 - ImCos(IM_PI / ImMax((float)(_N), IM_PI))) / (_RAD))

// ImDrawList: Lookup table size for adaptive arc drawing, cover full circle.
#ifndef IM_DRAWLIST_ARCFAST_TABLE_SIZE
#define IM_DRAWLIST_ARCFAST_TABLE_SIZE                          48 // Number of samples in lookup table.
#endif
#define IM_DRAWLIST_ARCFAST_SAMPLE_MAX                          IM_DRAWLIST_ARCFAST_TABLE_SIZE // Sample index _PathArcToFastEx() for 360 angle.

// Data shared between all ImDrawList instances
// You may want to create your own instance of this if you want to use ImDrawList completely without MsTrG. In that case, watch out for future changes to this structure.
struct MsTrG_API ImDrawListSharedData
{
    ImVec2          TexUvWhitePixel;            // UV of white pixel in the atlas
    ImFont*         Font;                       // Current/default font (optional, for simplified AddText overload)
    float           FontSize;                   // Current/default font size (optional, for simplified AddText overload)
    float           CurveTessellationTol;       // Tessellation tolerance when using PathBezierCurveTo()
    float           CircleSegmentMaxError;      // Number of circle segments to use per pixel of radius for AddCircle() etc
    ImVec4          ClipRectFullscreen;         // Value for PushClipRectFullscreen()
    ImDrawListFlags InitialFlags;               // Initial flags at the beginning of the frame (it is possible to alter flags on a per-drawlist basis afterwards)

    // [Internal] Lookup tables
    ImVec2          ArcFastVtx[IM_DRAWLIST_ARCFAST_TABLE_SIZE]; // Sample points on the quarter of the circle.
    float           ArcFastRadiusCutoff;                        // Cutoff radius after which arc drawing will fallback to slower PathArcTo()
    ImU8            CircleSegmentCounts[64];    // Precomputed segment count for given radius before we calculate it dynamically (to avoid calculation overhead)
    const ImVec4*   TexUvLines;                 // UV of anti-aliased lines in the atlas

    ImDrawListSharedData();
    void SetCircleTessellationMaxError(float max_error);
};

struct ImDrawDataBuilder
{
    ImVector<ImDrawList*>   Layers[2];           // Global layers for: regular, tooltip

    void Clear()                    { for (int n = 0; n < IM_ARRAYSIZE(Layers); n++) Layers[n].resize(0); }
    void ClearFreeMemory()          { for (int n = 0; n < IM_ARRAYSIZE(Layers); n++) Layers[n].clear(); }
    int  GetDrawListCount() const   { int count = 0; for (int n = 0; n < IM_ARRAYSIZE(Layers); n++) count += Layers[n].Size; return count; }
    MsTrG_API void FlattenIntoSingleLayer();
};

//-----------------------------------------------------------------------------
// [SECTION] Widgets support: flags, enums, data structures
//-----------------------------------------------------------------------------

// Transient per-window flags, reset at the beginning of the frame. For child window, inherited from parent on first Begin().
// This is going to be exposed in MsTrG.h when stabilized enough.
enum MsTrGItemFlags_
{
    MsTrGItemFlags_None                     = 0,
    MsTrGItemFlags_NoTabStop                = 1 << 0,  // false     // Disable keyboard tabbing (FIXME: should merge with _NoNav)
    MsTrGItemFlags_ButtonRepeat             = 1 << 1,  // false     // Button() will return true multiple times based on io.KeyRepeatDelay and io.KeyRepeatRate settings.
    MsTrGItemFlags_Disabled                 = 1 << 2,  // false     // Disable interactions but doesn't affect visuals. See BeginDisabled()/EndDisabled(). See github.com/ocornut/MsTrG/issues/211
    MsTrGItemFlags_NoNav                    = 1 << 3,  // false     // Disable keyboard/gamepad directional navigation (FIXME: should merge with _NoTabStop)
    MsTrGItemFlags_NoNavDefaultFocus        = 1 << 4,  // false     // Disable item being a candidate for default focus (e.g. used by title bar items)
    MsTrGItemFlags_SelectableDontClosePopup = 1 << 5,  // false     // Disable MenuItem/Selectable() automatically closing their popup window
    MsTrGItemFlags_MixedValue               = 1 << 6,  // false     // [BETA] Represent a mixed/indeterminate value, generally multi-selection where values differ. Currently only supported by Checkbox() (later should support all sorts of widgets)
    MsTrGItemFlags_ReadOnly                 = 1 << 7,  // false     // [ALPHA] Allow hovering interactions but underlying value is not changed.
    MsTrGItemFlags_Inputable                = 1 << 8   // false     // [WIP] Auto-activate input mode when tab focused. Currently only used and supported by a few items before it becomes a generic feature.
};

// Storage for LastItem data
enum MsTrGItemStatusFlags_
{
    MsTrGItemStatusFlags_None               = 0,
    MsTrGItemStatusFlags_HoveredRect        = 1 << 0,   // Mouse position is within item rectangle (does NOT mean that the window is in correct z-order and can be hovered!, this is only one part of the most-common IsItemHovered test)
    MsTrGItemStatusFlags_HasDisplayRect     = 1 << 1,   // g.LastItemData.DisplayRect is valid
    MsTrGItemStatusFlags_Edited             = 1 << 2,   // Value exposed by item was edited in the current frame (should match the bool return value of most widgets)
    MsTrGItemStatusFlags_ToggledSelection   = 1 << 3,   // Set when Selectable(), TreeNode() reports toggling a selection. We can't report "Selected", only state changes, in order to easily handle clipping with less issues.
    MsTrGItemStatusFlags_ToggledOpen        = 1 << 4,   // Set when TreeNode() reports toggling their open state.
    MsTrGItemStatusFlags_HasDeactivated     = 1 << 5,   // Set if the widget/group is able to provide data for the MsTrGItemStatusFlags_Deactivated flag.
    MsTrGItemStatusFlags_Deactivated        = 1 << 6,   // Only valid if MsTrGItemStatusFlags_HasDeactivated is set.
    MsTrGItemStatusFlags_HoveredWindow      = 1 << 7,   // Override the HoveredWindow test to allow cross-window hover testing.
    MsTrGItemStatusFlags_FocusedByTabbing   = 1 << 8    // Set when the Focusable item just got focused by Tabbing (FIXME: to be removed soon)

#ifdef MsTrG_ENABLE_TEST_ENGINE
    , // [MsTrG_tests only]
    MsTrGItemStatusFlags_Openable           = 1 << 20,  // Item is an openable (e.g. TreeNode)
    MsTrGItemStatusFlags_Opened             = 1 << 21,  //
    MsTrGItemStatusFlags_Checkable          = 1 << 22,  // Item is a checkable (e.g. CheckBox, MenuItem)
    MsTrGItemStatusFlags_Checked            = 1 << 23   //
#endif
};

// Extend MsTrGInputTextFlags_
enum MsTrGInputTextFlagsPrivate_
{
    // [Internal]
    MsTrGInputTextFlags_Multiline           = 1 << 26,  // For internal use by InputTextMultiline()
    MsTrGInputTextFlags_NoMarkEdited        = 1 << 27,  // For internal use by functions using InputText() before reformatting data
    MsTrGInputTextFlags_MergedItem          = 1 << 28   // For internal use by TempInputText(), will skip calling ItemAdd(). Require bounding-box to strictly match.
};

// Extend MsTrGButtonFlags_
enum MsTrGButtonFlagsPrivate_
{
    MsTrGButtonFlags_PressedOnClick         = 1 << 4,   // return true on click (mouse down event)
    MsTrGButtonFlags_PressedOnClickRelease  = 1 << 5,   // [Default] return true on click + release on same item <-- this is what the majority of Button are using
    MsTrGButtonFlags_PressedOnClickReleaseAnywhere = 1 << 6, // return true on click + release even if the release event is not done while hovering the item
    MsTrGButtonFlags_PressedOnRelease       = 1 << 7,   // return true on release (default requires click+release)
    MsTrGButtonFlags_PressedOnDoubleClick   = 1 << 8,   // return true on double-click (default requires click+release)
    MsTrGButtonFlags_PressedOnDragDropHold  = 1 << 9,   // return true when held into while we are drag and dropping another item (used by e.g. tree nodes, collapsing headers)
    MsTrGButtonFlags_Repeat                 = 1 << 10,  // hold to repeat
    MsTrGButtonFlags_FlattenChildren        = 1 << 11,  // allow interactions even if a child window is overlapping
    MsTrGButtonFlags_AllowItemOverlap       = 1 << 12,  // require previous frame HoveredId to either match id or be null before being usable, use along with SetItemAllowOverlap()
    MsTrGButtonFlags_DontClosePopups        = 1 << 13,  // disable automatically closing parent popup on press // [UNUSED]
    //MsTrGButtonFlags_Disabled             = 1 << 14,  // disable interactions -> use BeginDisabled() or MsTrGItemFlags_Disabled
    MsTrGButtonFlags_AlignTextBaseLine      = 1 << 15,  // vertically align button to match text baseline - ButtonEx() only // FIXME: Should be removed and handled by SmallButton(), not possible currently because of DC.CursorPosPrevLine
    MsTrGButtonFlags_NoKeyModifiers         = 1 << 16,  // disable mouse interaction if a key modifier is held
    MsTrGButtonFlags_NoHoldingActiveId      = 1 << 17,  // don't set ActiveId while holding the mouse (MsTrGButtonFlags_PressedOnClick only)
    MsTrGButtonFlags_NoNavFocus             = 1 << 18,  // don't override navigation focus when activated
    MsTrGButtonFlags_NoHoveredOnFocus       = 1 << 19,  // don't report as hovered when nav focus is on this item
    MsTrGButtonFlags_PressedOnMask_         = MsTrGButtonFlags_PressedOnClick | MsTrGButtonFlags_PressedOnClickRelease | MsTrGButtonFlags_PressedOnClickReleaseAnywhere | MsTrGButtonFlags_PressedOnRelease | MsTrGButtonFlags_PressedOnDoubleClick | MsTrGButtonFlags_PressedOnDragDropHold,
    MsTrGButtonFlags_PressedOnDefault_      = MsTrGButtonFlags_PressedOnClickRelease
};

// Extend MsTrGComboFlags_
enum MsTrGComboFlagsPrivate_
{
    MsTrGComboFlags_CustomPreview           = 1 << 20   // enable BeginComboPreview()
};

// Extend MsTrGSliderFlags_
enum MsTrGSliderFlagsPrivate_
{
    MsTrGSliderFlags_Vertical               = 1 << 20,  // Should this slider be orientated vertically?
    MsTrGSliderFlags_ReadOnly               = 1 << 21
};

// Extend MsTrGSelectableFlags_
enum MsTrGSelectableFlagsPrivate_
{
    // NB: need to be in sync with last value of MsTrGSelectableFlags_
    MsTrGSelectableFlags_NoHoldingActiveID      = 1 << 20,
    MsTrGSelectableFlags_SelectOnNav            = 1 << 21,  // (WIP) Auto-select when moved into. This is not exposed in public API as to handle multi-select and modifiers we will need user to explicitly control focus scope. May be replaced with a BeginSelection() API.
    MsTrGSelectableFlags_SelectOnClick          = 1 << 22,  // Override button behavior to react on Click (default is Click+Release)
    MsTrGSelectableFlags_SelectOnRelease        = 1 << 23,  // Override button behavior to react on Release (default is Click+Release)
    MsTrGSelectableFlags_SpanAvailWidth         = 1 << 24,  // Span all avail width even if we declared less for layout purpose. FIXME: We may be able to remove this (added in 6251d379, 2bcafc86 for menus)
    MsTrGSelectableFlags_DrawHoveredWhenHeld    = 1 << 25,  // Always show active when held, even is not hovered. This concept could probably be renamed/formalized somehow.
    MsTrGSelectableFlags_SetNavIdOnHover        = 1 << 26,  // Set Nav/Focus ID on mouse hover (used by MenuItem)
    MsTrGSelectableFlags_NoPadWithHalfSpacing   = 1 << 27   // Disable padding each side with ItemSpacing * 0.5f
};

// Extend MsTrGTreeNodeFlags_
enum MsTrGTreeNodeFlagsPrivate_
{
    MsTrGTreeNodeFlags_ClipLabelForTrailingButton = 1 << 20
};

enum MsTrGSeparatorFlags_
{
    MsTrGSeparatorFlags_None                = 0,
    MsTrGSeparatorFlags_Horizontal          = 1 << 0,   // Axis default to current layout type, so generally Horizontal unless e.g. in a menu bar
    MsTrGSeparatorFlags_Vertical            = 1 << 1,
    MsTrGSeparatorFlags_SpanAllColumns      = 1 << 2
};

enum MsTrGTextFlags_
{
    MsTrGTextFlags_None = 0,
    MsTrGTextFlags_NoWidthForLargeClippedText = 1 << 0
};

enum MsTrGTooltipFlags_
{
    MsTrGTooltipFlags_None = 0,
    MsTrGTooltipFlags_OverridePreviousTooltip = 1 << 0      // Override will clear/ignore previously submitted tooltip (defaults to append)
};

// FIXME: this is in development, not exposed/functional as a generic feature yet.
// Horizontal/Vertical enums are fixed to 0/1 so they may be used to index ImVec2
enum MsTrGLayoutType_
{
    MsTrGLayoutType_Horizontal = 0,
    MsTrGLayoutType_Vertical = 1
};

enum MsTrGLogType
{
    MsTrGLogType_None = 0,
    MsTrGLogType_TTY,
    MsTrGLogType_File,
    MsTrGLogType_Buffer,
    MsTrGLogType_Clipboard
};

// X/Y enums are fixed to 0/1 so they may be used to index ImVec2
enum MsTrGAxis
{
    MsTrGAxis_None = -1,
    MsTrGAxis_X = 0,
    MsTrGAxis_Y = 1
};

enum MsTrGPlotType
{
    MsTrGPlotType_Lines,
    MsTrGPlotType_Histogram
};

enum MsTrGPopupPositionPolicy
{
    MsTrGPopupPositionPolicy_Default,
    MsTrGPopupPositionPolicy_ComboBox,
    MsTrGPopupPositionPolicy_Tooltip
};

struct MsTrGDataTypeTempStorage
{
    ImU8        Data[8];        // Can fit any data up to MsTrGDataType_COUNT
};

// Type information associated to one MsTrGDataType. Retrieve with DataTypeGetInfo().
struct MsTrGDataTypeInfo
{
    size_t      Size;           // Size in bytes
    const char* Name;           // Short descriptive name for the type, for debugging
    const char* PrintFmt;       // Default printf format for the type
    const char* ScanFmt;        // Default scanf format for the type
};

// Extend MsTrGDataType_
enum MsTrGDataTypePrivate_
{
    MsTrGDataType_String = MsTrGDataType_COUNT + 1,
    MsTrGDataType_Pointer,
    MsTrGDataType_ID
};

// Stacked color modifier, backup of modified data so we can restore it
struct MsTrGColorMod
{
    MsTrGCol        Col;
    ImVec4          BackupValue;
};

// Stacked style modifier, backup of modified data so we can restore it. Data type inferred from the variable.
struct MsTrGStyleMod
{
    MsTrGStyleVar   VarIdx;
    union           { int BackupInt[2]; float BackupFloat[2]; };
    MsTrGStyleMod(MsTrGStyleVar idx, int v)     { VarIdx = idx; BackupInt[0] = v; }
    MsTrGStyleMod(MsTrGStyleVar idx, float v)   { VarIdx = idx; BackupFloat[0] = v; }
    MsTrGStyleMod(MsTrGStyleVar idx, ImVec2 v)  { VarIdx = idx; BackupFloat[0] = v.x; BackupFloat[1] = v.y; }
};

// Storage data for BeginComboPreview()/EndComboPreview()
struct MsTrG_API MsTrGComboPreviewData
{
    ImRect          PreviewRect;
    ImVec2          BackupCursorPos;
    ImVec2          BackupCursorMaxPos;
    ImVec2          BackupCursorPosPrevLine;
    float           BackupPrevLineTextBaseOffset;
    MsTrGLayoutType BackupLayout;

    MsTrGComboPreviewData() { memset(this, 0, sizeof(*this)); }
};

// Stacked storage data for BeginGroup()/EndGroup()
struct MsTrG_API MsTrGGroupData
{
    MsTrGID     WindowID;
    ImVec2      BackupCursorPos;
    ImVec2      BackupCursorMaxPos;
    ImVec1      BackupIndent;
    ImVec1      BackupGroupOffset;
    ImVec2      BackupCurrLineSize;
    float       BackupCurrLineTextBaseOffset;
    MsTrGID     BackupActiveIdIsAlive;
    bool        BackupActiveIdPreviousFrameIsAlive;
    bool        BackupHoveredIdIsAlive;
    bool        EmitItem;
};

// Simple column measurement, currently used for MenuItem() only.. This is very short-sighted/throw-away code and NOT a generic helper.
struct MsTrG_API MsTrGMenuColumns
{
    ImU32       TotalWidth;
    ImU32       NextTotalWidth;
    ImU16       Spacing;
    ImU16       OffsetIcon;         // Always zero for now
    ImU16       OffsetLabel;        // Offsets are locked in Update()
    ImU16       OffsetShortcut;
    ImU16       OffsetMark;
    ImU16       Widths[4];          // Width of:   Icon, Label, Shortcut, Mark  (accumulators for current frame)

    MsTrGMenuColumns() { memset(this, 0, sizeof(*this)); }
    void        Update(float spacing, bool window_reappearing);
    float       DeclColumns(float w_icon, float w_label, float w_shortcut, float w_mark);
    void        CalcNextTotalWidth(bool update_offsets);
};

// Internal state of the currently focused/edited text input box
// For a given item ID, access with MsTrG::GetInputTextState()
struct MsTrG_API MsTrGInputTextState
{
    MsTrGID                 ID;                     // widget id owning the text state
    int                     CurLenW, CurLenA;       // we need to maintain our buffer length in both UTF-8 and wchar format. UTF-8 length is valid even if TextA is not.
    ImVector<ImWchar>       TextW;                  // edit buffer, we need to persist but can't guarantee the persistence of the user-provided buffer. so we copy into own buffer.
    ImVector<char>          TextA;                  // temporary UTF8 buffer for callbacks and other operations. this is not updated in every code-path! size=capacity.
    ImVector<char>          InitialTextA;           // backup of end-user buffer at the time of focus (in UTF-8, unaltered)
    bool                    TextAIsValid;           // temporary UTF8 buffer is not initially valid before we make the widget active (until then we pull the data from user argument)
    int                     BufCapacityA;           // end-user buffer capacity
    float                   ScrollX;                // horizontal scrolling/offset
    ImStb::STB_TexteditState Stb;                   // state for stb_textedit.h
    float                   CursorAnim;             // timer for cursor blink, reset on every user action so the cursor reappears immediately
    bool                    CursorFollow;           // set when we want scrolling to follow the current cursor position (not always!)
    bool                    SelectedAllMouseLock;   // after a double-click to select all, we ignore further mouse drags to update selection
    bool                    Edited;                 // edited this frame
    MsTrGInputTextFlags     Flags;                  // copy of InputText() flags

    MsTrGInputTextState()                   { memset(this, 0, sizeof(*this)); }
    void        ClearText()                 { CurLenW = CurLenA = 0; TextW[0] = 0; TextA[0] = 0; CursorClamp(); }
    void        ClearFreeMemory()           { TextW.clear(); TextA.clear(); InitialTextA.clear(); }
    int         GetUndoAvailCount() const   { return Stb.undostate.undo_point; }
    int         GetRedoAvailCount() const   { return STB_TEXTEDIT_UNDOSTATECOUNT - Stb.undostate.redo_point; }
    void        OnKeyPressed(int key);      // Cannot be inline because we call in code in stb_textedit.h implementation

    // Cursor & Selection
    void        CursorAnimReset()           { CursorAnim = -0.30f; }                                   // After a user-input the cursor stays on for a while without blinking
    void        CursorClamp()               { Stb.cursor = ImMin(Stb.cursor, CurLenW); Stb.select_start = ImMin(Stb.select_start, CurLenW); Stb.select_end = ImMin(Stb.select_end, CurLenW); }
    bool        HasSelection() const        { return Stb.select_start != Stb.select_end; }
    void        ClearSelection()            { Stb.select_start = Stb.select_end = Stb.cursor; }
    int         GetCursorPos() const        { return Stb.cursor; }
    int         GetSelectionStart() const   { return Stb.select_start; }
    int         GetSelectionEnd() const     { return Stb.select_end; }
    void        SelectAll()                 { Stb.select_start = 0; Stb.cursor = Stb.select_end = CurLenW; Stb.has_preferred_x = 0; }
};

// Storage for current popup stack
struct MsTrGPopupData
{
    MsTrGID             PopupId;        // Set on OpenPopup()
    MsTrGWindow*        Window;         // Resolved on BeginPopup() - may stay unresolved if user never calls OpenPopup()
    MsTrGWindow*        SourceWindow;   // Set on OpenPopup() copy of NavWindow at the time of opening the popup
    int                 ParentNavLayer; // Resolved on BeginPopup(). Actually a MsTrGNavLayer type (declared down below), initialized to -1 which is not part of an enum, but serves well-enough as "not any of layers" value
    int                 OpenFrameCount; // Set on OpenPopup()
    MsTrGID             OpenParentId;   // Set on OpenPopup(), we need this to differentiate multiple menu sets from each others (e.g. inside menu bar vs loose menu items)
    ImVec2              OpenPopupPos;   // Set on OpenPopup(), preferred popup position (typically == OpenMousePos when using mouse)
    ImVec2              OpenMousePos;   // Set on OpenPopup(), copy of mouse position at the time of opening popup

    MsTrGPopupData()    { memset(this, 0, sizeof(*this)); ParentNavLayer = OpenFrameCount = -1; }
};

enum MsTrGNextWindowDataFlags_
{
    MsTrGNextWindowDataFlags_None               = 0,
    MsTrGNextWindowDataFlags_HasPos             = 1 << 0,
    MsTrGNextWindowDataFlags_HasSize            = 1 << 1,
    MsTrGNextWindowDataFlags_HasContentSize     = 1 << 2,
    MsTrGNextWindowDataFlags_HasCollapsed       = 1 << 3,
    MsTrGNextWindowDataFlags_HasSizeConstraint  = 1 << 4,
    MsTrGNextWindowDataFlags_HasFocus           = 1 << 5,
    MsTrGNextWindowDataFlags_HasBgAlpha         = 1 << 6,
    MsTrGNextWindowDataFlags_HasScroll          = 1 << 7
};

// Storage for SetNexWindow** functions
struct MsTrGNextWindowData
{
    MsTrGNextWindowDataFlags    Flags;
    MsTrGCond                   PosCond;
    MsTrGCond                   SizeCond;
    MsTrGCond                   CollapsedCond;
    ImVec2                      PosVal;
    ImVec2                      PosPivotVal;
    ImVec2                      SizeVal;
    ImVec2                      ContentSizeVal;
    ImVec2                      ScrollVal;
    bool                        CollapsedVal;
    ImRect                      SizeConstraintRect;
    MsTrGSizeCallback           SizeCallback;
    void*                       SizeCallbackUserData;
    float                       BgAlphaVal;             // Override background alpha
    ImVec2                      MenuBarOffsetMinVal;    // (Always on) This is not exposed publicly, so we don't clear it and it doesn't have a corresponding flag (could we? for consistency?)

    MsTrGNextWindowData()       { memset(this, 0, sizeof(*this)); }
    inline void ClearFlags()    { Flags = MsTrGNextWindowDataFlags_None; }
};

enum MsTrGNextItemDataFlags_
{
    MsTrGNextItemDataFlags_None     = 0,
    MsTrGNextItemDataFlags_HasWidth = 1 << 0,
    MsTrGNextItemDataFlags_HasOpen  = 1 << 1
};

struct MsTrGNextItemData
{
    MsTrGNextItemDataFlags      Flags;
    float                       Width;          // Set by SetNextItemWidth()
    MsTrGID                     FocusScopeId;   // Set by SetNextItemMultiSelectData() (!= 0 signify value has been set, so it's an alternate version of HasSelectionData, we don't use Flags for this because they are cleared too early. This is mostly used for debugging)
    MsTrGCond                   OpenCond;
    bool                        OpenVal;        // Set by SetNextItemOpen()

    MsTrGNextItemData()         { memset(this, 0, sizeof(*this)); }
    inline void ClearFlags()    { Flags = MsTrGNextItemDataFlags_None; } // Also cleared manually by ItemAdd()!
};

// Status storage for the last submitted item
struct MsTrGLastItemData
{
    MsTrGID                 ID;
    MsTrGItemFlags          InFlags;            // See MsTrGItemFlags_
    MsTrGItemStatusFlags    StatusFlags;        // See MsTrGItemStatusFlags_
    ImRect                  Rect;               // Full rectangle
    ImRect                  NavRect;            // Navigation scoring rectangle (not displayed)
    ImRect                  DisplayRect;        // Display rectangle (only if MsTrGItemStatusFlags_HasDisplayRect is set)

    MsTrGLastItemData()     { memset(this, 0, sizeof(*this)); }
};

struct MsTrG_API MsTrGStackSizes
{
    short   SizeOfIDStack;
    short   SizeOfColorStack;
    short   SizeOfStyleVarStack;
    short   SizeOfFontStack;
    short   SizeOfFocusScopeStack;
    short   SizeOfGroupStack;
    short   SizeOfItemFlagsStack;
    short   SizeOfBeginPopupStack;
    short   SizeOfDisabledStack;

    MsTrGStackSizes() { memset(this, 0, sizeof(*this)); }
    void SetToCurrentState();
    void CompareWithCurrentState();
};

// Data saved for each window pushed into the stack
struct MsTrGWindowStackData
{
    MsTrGWindow*            Window;
    MsTrGLastItemData       ParentLastItemDataBackup;
    MsTrGStackSizes         StackSizesOnBegin;      // Store size of various stacks for asserting
};

struct MsTrGShrinkWidthItem
{
    int         Index;
    float       Width;
    float       InitialWidth;
};

struct MsTrGPtrOrIndex
{
    void*       Ptr;            // Either field can be set, not both. e.g. Dock node tab bars are loose while BeginTabBar() ones are in a pool.
    int         Index;          // Usually index in a main pool.

    MsTrGPtrOrIndex(void* ptr)  { Ptr = ptr; Index = -1; }
    MsTrGPtrOrIndex(int index)  { Ptr = NULL; Index = index; }
};

//-----------------------------------------------------------------------------
// [SECTION] Inputs support
//-----------------------------------------------------------------------------

typedef ImBitArray<MsTrGKey_NamedKey_COUNT, -MsTrGKey_NamedKey_BEGIN>    ImBitArrayForNamedKeys;

enum MsTrGKeyPrivate_
{
    MsTrGKey_LegacyNativeKey_BEGIN  = 0,
    MsTrGKey_LegacyNativeKey_END    = 512,
    MsTrGKey_Gamepad_BEGIN          = MsTrGKey_GamepadStart,
    MsTrGKey_Gamepad_END            = MsTrGKey_GamepadRStickRight + 1
};

enum MsTrGInputEventType
{
    MsTrGInputEventType_None = 0,
    MsTrGInputEventType_MousePos,
    MsTrGInputEventType_MouseWheel,
    MsTrGInputEventType_MouseButton,
    MsTrGInputEventType_Key,
    MsTrGInputEventType_Text,
    MsTrGInputEventType_Focus,
    MsTrGInputEventType_COUNT
};

enum MsTrGInputSource
{
    MsTrGInputSource_None = 0,
    MsTrGInputSource_Mouse,
    MsTrGInputSource_Keyboard,
    MsTrGInputSource_Gamepad,
    MsTrGInputSource_Clipboard,     // Currently only used by InputText()
    MsTrGInputSource_Nav,           // Stored in g.ActiveIdSource only
    MsTrGInputSource_COUNT
};

// FIXME: Structures in the union below need to be declared as anonymous unions appears to be an extension?
// Using ImVec2() would fail on Clang 'union member 'MousePos' has a non-trivial default constructor'
struct MsTrGInputEventMousePos      { float PosX, PosY; };
struct MsTrGInputEventMouseWheel    { float WheelX, WheelY; };
struct MsTrGInputEventMouseButton   { int Button; bool Down; };
struct MsTrGInputEventKey           { MsTrGKey Key; bool Down; float AnalogValue; };
struct MsTrGInputEventText          { unsigned int Char; };
struct MsTrGInputEventAppFocused    { bool Focused; };

struct MsTrGInputEvent
{
    MsTrGInputEventType             Type;
    MsTrGInputSource                Source;
    union
    {
        MsTrGInputEventMousePos     MousePos;       // if Type == MsTrGInputEventType_MousePos
        MsTrGInputEventMouseWheel   MouseWheel;     // if Type == MsTrGInputEventType_MouseWheel
        MsTrGInputEventMouseButton  MouseButton;    // if Type == MsTrGInputEventType_MouseButton
        MsTrGInputEventKey          Key;            // if Type == MsTrGInputEventType_Key
        MsTrGInputEventText         Text;           // if Type == MsTrGInputEventType_Text
        MsTrGInputEventAppFocused   AppFocused;     // if Type == MsTrGInputEventType_Focus
    };
    bool                            AddedByTestEngine;

    MsTrGInputEvent() { memset(this, 0, sizeof(*this)); }
};

// FIXME-NAV: Clarify/expose various repeat delay/rate
enum MsTrGNavReadMode
{
    MsTrGNavReadMode_Down,
    MsTrGNavReadMode_Pressed,
    MsTrGNavReadMode_Released,
    MsTrGNavReadMode_Repeat,
    MsTrGNavReadMode_RepeatSlow,
    MsTrGNavReadMode_RepeatFast
};

//-----------------------------------------------------------------------------
// [SECTION] Clipper support
//-----------------------------------------------------------------------------

struct MsTrGListClipperRange
{
    int     Min;
    int     Max;
    bool    PosToIndexConvert;      // Begin/End are absolute position (will be converted to indices later)
    ImS8    PosToIndexOffsetMin;    // Add to Min after converting to indices
    ImS8    PosToIndexOffsetMax;    // Add to Min after converting to indices

    static MsTrGListClipperRange    FromIndices(int min, int max)                               { MsTrGListClipperRange r = { min, max, false, 0, 0 }; return r; }
    static MsTrGListClipperRange    FromPositions(float y1, float y2, int off_min, int off_max) { MsTrGListClipperRange r = { (int)y1, (int)y2, true, (ImS8)off_min, (ImS8)off_max }; return r; }
};

// Temporary clipper data, buffers shared/reused between instances
struct MsTrGListClipperData
{
    MsTrGListClipper*               ListClipper;
    float                           LossynessOffset;
    int                             StepNo;
    int                             ItemsFrozen;
    ImVector<MsTrGListClipperRange> Ranges;

    MsTrGListClipperData()          { memset(this, 0, sizeof(*this)); }
    void                            Reset(MsTrGListClipper* clipper) { ListClipper = clipper; StepNo = ItemsFrozen = 0; Ranges.resize(0); }
};

//-----------------------------------------------------------------------------
// [SECTION] Navigation support
//-----------------------------------------------------------------------------

enum MsTrGActivateFlags_
{
    MsTrGActivateFlags_None                 = 0,
    MsTrGActivateFlags_PreferInput          = 1 << 0,       // Favor activation that requires keyboard text input (e.g. for Slider/Drag). Default if keyboard is available.
    MsTrGActivateFlags_PreferTweak          = 1 << 1,       // Favor activation for tweaking with arrows or gamepad (e.g. for Slider/Drag). Default if keyboard is not available.
    MsTrGActivateFlags_TryToPreserveState   = 1 << 2        // Request widget to preserve state if it can (e.g. InputText will try to preserve cursor/selection)
};

// Early work-in-progress API for ScrollToItem()
enum MsTrGScrollFlags_
{
    MsTrGScrollFlags_None                   = 0,
    MsTrGScrollFlags_KeepVisibleEdgeX       = 1 << 0,       // If item is not visible: scroll as little as possible on X axis to bring item back into view [default for X axis]
    MsTrGScrollFlags_KeepVisibleEdgeY       = 1 << 1,       // If item is not visible: scroll as little as possible on Y axis to bring item back into view [default for Y axis for windows that are already visible]
    MsTrGScrollFlags_KeepVisibleCenterX     = 1 << 2,       // If item is not visible: scroll to make the item centered on X axis [rarely used]
    MsTrGScrollFlags_KeepVisibleCenterY     = 1 << 3,       // If item is not visible: scroll to make the item centered on Y axis
    MsTrGScrollFlags_AlwaysCenterX          = 1 << 4,       // Always center the result item on X axis [rarely used]
    MsTrGScrollFlags_AlwaysCenterY          = 1 << 5,       // Always center the result item on Y axis [default for Y axis for appearing window)
    MsTrGScrollFlags_NoScrollParent         = 1 << 6,       // Disable forwarding scrolling to parent window if required to keep item/rect visible (only scroll window the function was applied to).
    MsTrGScrollFlags_MaskX_                 = MsTrGScrollFlags_KeepVisibleEdgeX | MsTrGScrollFlags_KeepVisibleCenterX | MsTrGScrollFlags_AlwaysCenterX,
    MsTrGScrollFlags_MaskY_                 = MsTrGScrollFlags_KeepVisibleEdgeY | MsTrGScrollFlags_KeepVisibleCenterY | MsTrGScrollFlags_AlwaysCenterY
};

enum MsTrGNavHighlightFlags_
{
    MsTrGNavHighlightFlags_None             = 0,
    MsTrGNavHighlightFlags_TypeDefault      = 1 << 0,
    MsTrGNavHighlightFlags_TypeThin         = 1 << 1,
    MsTrGNavHighlightFlags_AlwaysDraw       = 1 << 2,       // Draw rectangular highlight if (g.NavId == id) _even_ when using the mouse.
    MsTrGNavHighlightFlags_NoRounding       = 1 << 3
};

enum MsTrGNavDirSourceFlags_
{
    MsTrGNavDirSourceFlags_None             = 0,
    MsTrGNavDirSourceFlags_RawKeyboard      = 1 << 0,   // Raw keyboard (not pulled from nav), facilitate use of some functions before we can unify nav and keys
    MsTrGNavDirSourceFlags_Keyboard         = 1 << 1,
    MsTrGNavDirSourceFlags_PadDPad          = 1 << 2,
    MsTrGNavDirSourceFlags_PadLStick        = 1 << 3
};

enum MsTrGNavMoveFlags_
{
    MsTrGNavMoveFlags_None                  = 0,
    MsTrGNavMoveFlags_LoopX                 = 1 << 0,   // On failed request, restart from opposite side
    MsTrGNavMoveFlags_LoopY                 = 1 << 1,
    MsTrGNavMoveFlags_WrapX                 = 1 << 2,   // On failed request, request from opposite side one line down (when NavDir==right) or one line up (when NavDir==left)
    MsTrGNavMoveFlags_WrapY                 = 1 << 3,   // This is not super useful but provided for completeness
    MsTrGNavMoveFlags_AllowCurrentNavId     = 1 << 4,   // Allow scoring and considering the current NavId as a move target candidate. This is used when the move source is offset (e.g. pressing PageDown actually needs to send a Up move request, if we are pressing PageDown from the bottom-most item we need to stay in place)
    MsTrGNavMoveFlags_AlsoScoreVisibleSet   = 1 << 5,   // Store alternate result in NavMoveResultLocalVisible that only comprise elements that are already fully visible (used by PageUp/PageDown)
    MsTrGNavMoveFlags_ScrollToEdgeY         = 1 << 6,   // Force scrolling to min/max (used by Home/End) // FIXME-NAV: Aim to remove or reword, probably unnecessary
    MsTrGNavMoveFlags_Forwarded             = 1 << 7,
    MsTrGNavMoveFlags_DebugNoResult         = 1 << 8,   // Dummy scoring for debug purpose, don't apply result
    MsTrGNavMoveFlags_FocusApi              = 1 << 9,
    MsTrGNavMoveFlags_Tabbing               = 1 << 10,  // == Focus + Activate if item is Inputable + DontChangeNavHighlight
    MsTrGNavMoveFlags_Activate              = 1 << 11,
    MsTrGNavMoveFlags_DontSetNavHighlight   = 1 << 12   // Do not alter the visible state of keyboard vs mouse nav highlight
};

enum MsTrGNavLayer
{
    MsTrGNavLayer_Main  = 0,    // Main scrolling layer
    MsTrGNavLayer_Menu  = 1,    // Menu layer (access with Alt/MsTrGNavInput_Menu)
    MsTrGNavLayer_COUNT
};

struct MsTrGNavItemData
{
    MsTrGWindow*        Window;         // Init,Move    // Best candidate window (result->ItemWindow->RootWindowForNav == request->Window)
    MsTrGID             ID;             // Init,Move    // Best candidate item ID
    MsTrGID             FocusScopeId;   // Init,Move    // Best candidate focus scope ID
    ImRect              RectRel;        // Init,Move    // Best candidate bounding box in window relative space
    MsTrGItemFlags      InFlags;        // ????,Move    // Best candidate item flags
    float               DistBox;        //      Move    // Best candidate box distance to current NavId
    float               DistCenter;     //      Move    // Best candidate center distance to current NavId
    float               DistAxial;      //      Move    // Best candidate axial distance to current NavId

    MsTrGNavItemData()  { Clear(); }
    void Clear()        { Window = NULL; ID = FocusScopeId = 0; InFlags = 0; DistBox = DistCenter = DistAxial = FLT_MAX; }
};

//-----------------------------------------------------------------------------
// [SECTION] Columns support
//-----------------------------------------------------------------------------

// Flags for internal's BeginColumns(). Prefix using BeginTable() nowadays!
enum MsTrGOldColumnFlags_
{
    MsTrGOldColumnFlags_None                    = 0,
    MsTrGOldColumnFlags_NoBorder                = 1 << 0,   // Disable column dividers
    MsTrGOldColumnFlags_NoResize                = 1 << 1,   // Disable resizing columns when clicking on the dividers
    MsTrGOldColumnFlags_NoPreserveWidths        = 1 << 2,   // Disable column width preservation when adjusting columns
    MsTrGOldColumnFlags_NoForceWithinWindow     = 1 << 3,   // Disable forcing columns to fit within window
    MsTrGOldColumnFlags_GrowParentContentsSize  = 1 << 4    // (WIP) Restore pre-1.51 behavior of extending the parent window contents size but _without affecting the columns width at all_. Will eventually remove.

    // Obsolete names (will be removed)
#ifndef MsTrG_DISABLE_OBSOLETE_FUNCTIONS
    , MsTrGColumnsFlags_None                    = MsTrGOldColumnFlags_None,
    MsTrGColumnsFlags_NoBorder                  = MsTrGOldColumnFlags_NoBorder,
    MsTrGColumnsFlags_NoResize                  = MsTrGOldColumnFlags_NoResize,
    MsTrGColumnsFlags_NoPreserveWidths          = MsTrGOldColumnFlags_NoPreserveWidths,
    MsTrGColumnsFlags_NoForceWithinWindow       = MsTrGOldColumnFlags_NoForceWithinWindow,
    MsTrGColumnsFlags_GrowParentContentsSize    = MsTrGOldColumnFlags_GrowParentContentsSize
#endif
};

struct MsTrGOldColumnData
{
    float               OffsetNorm;         // Column start offset, normalized 0.0 (far left) -> 1.0 (far right)
    float               OffsetNormBeforeResize;
    MsTrGOldColumnFlags Flags;              // Not exposed
    ImRect              ClipRect;

    MsTrGOldColumnData() { memset(this, 0, sizeof(*this)); }
};

struct MsTrGOldColumns
{
    MsTrGID             ID;
    MsTrGOldColumnFlags Flags;
    bool                IsFirstFrame;
    bool                IsBeingResized;
    int                 Current;
    int                 Count;
    float               OffMinX, OffMaxX;       // Offsets from HostWorkRect.Min.x
    float               LineMinY, LineMaxY;
    float               HostCursorPosY;         // Backup of CursorPos at the time of BeginColumns()
    float               HostCursorMaxPosX;      // Backup of CursorMaxPos at the time of BeginColumns()
    ImRect              HostInitialClipRect;    // Backup of ClipRect at the time of BeginColumns()
    ImRect              HostBackupClipRect;     // Backup of ClipRect during PushColumnsBackground()/PopColumnsBackground()
    ImRect              HostBackupParentWorkRect;//Backup of WorkRect at the time of BeginColumns()
    ImVector<MsTrGOldColumnData> Columns;
    ImDrawListSplitter  Splitter;

    MsTrGOldColumns()   { memset(this, 0, sizeof(*this)); }
};

//-----------------------------------------------------------------------------
// [SECTION] Multi-select support
//-----------------------------------------------------------------------------

#ifdef MsTrG_HAS_MULTI_SELECT
// <this is filled in 'range_select' branch>
#endif // #ifdef MsTrG_HAS_MULTI_SELECT

//-----------------------------------------------------------------------------
// [SECTION] Docking support
//-----------------------------------------------------------------------------

#ifdef MsTrG_HAS_DOCK
// <this is filled in 'docking' branch>
#endif // #ifdef MsTrG_HAS_DOCK

//-----------------------------------------------------------------------------
// [SECTION] Viewport support
//-----------------------------------------------------------------------------

// MsTrGViewport Private/Internals fields (cardinal sin: we are using inheritance!)
// Every instance of MsTrGViewport is in fact a MsTrGViewportP.
struct MsTrGViewportP : public MsTrGViewport
{
    int                 DrawListsLastFrame[2];  // Last frame number the background (0) and foreground (1) draw lists were used
    ImDrawList*         DrawLists[2];           // Convenience background (0) and foreground (1) draw lists. We use them to draw software mouser cursor when io.MouseDrawCursor is set and to draw most debug overlays.
    ImDrawData          DrawDataP;
    ImDrawDataBuilder   DrawDataBuilder;

    ImVec2              WorkOffsetMin;          // Work Area: Offset from Pos to top-left corner of Work Area. Generally (0,0) or (0,+main_menu_bar_height). Work Area is Full Area but without menu-bars/status-bars (so WorkArea always fit inside Pos/Size!)
    ImVec2              WorkOffsetMax;          // Work Area: Offset from Pos+Size to bottom-right corner of Work Area. Generally (0,0) or (0,-status_bar_height).
    ImVec2              BuildWorkOffsetMin;     // Work Area: Offset being built during current frame. Generally >= 0.0f.
    ImVec2              BuildWorkOffsetMax;     // Work Area: Offset being built during current frame. Generally <= 0.0f.

    MsTrGViewportP()    { DrawListsLastFrame[0] = DrawListsLastFrame[1] = -1; DrawLists[0] = DrawLists[1] = NULL; }
    ~MsTrGViewportP()   { if (DrawLists[0]) IM_DELETE(DrawLists[0]); if (DrawLists[1]) IM_DELETE(DrawLists[1]); }

    // Calculate work rect pos/size given a set of offset (we have 1 pair of offset for rect locked from last frame data, and 1 pair for currently building rect)
    ImVec2  CalcWorkRectPos(const ImVec2& off_min) const                            { return ImVec2(Pos.x + off_min.x, Pos.y + off_min.y); }
    ImVec2  CalcWorkRectSize(const ImVec2& off_min, const ImVec2& off_max) const    { return ImVec2(ImMax(0.0f, Size.x - off_min.x + off_max.x), ImMax(0.0f, Size.y - off_min.y + off_max.y)); }
    void    UpdateWorkRect()            { WorkPos = CalcWorkRectPos(WorkOffsetMin); WorkSize = CalcWorkRectSize(WorkOffsetMin, WorkOffsetMax); } // Update public fields

    // Helpers to retrieve ImRect (we don't need to store BuildWorkRect as every access tend to change it, hence the code asymmetry)
    ImRect  GetMainRect() const         { return ImRect(Pos.x, Pos.y, Pos.x + Size.x, Pos.y + Size.y); }
    ImRect  GetWorkRect() const         { return ImRect(WorkPos.x, WorkPos.y, WorkPos.x + WorkSize.x, WorkPos.y + WorkSize.y); }
    ImRect  GetBuildWorkRect() const    { ImVec2 pos = CalcWorkRectPos(BuildWorkOffsetMin); ImVec2 size = CalcWorkRectSize(BuildWorkOffsetMin, BuildWorkOffsetMax); return ImRect(pos.x, pos.y, pos.x + size.x, pos.y + size.y); }
};

//-----------------------------------------------------------------------------
// [SECTION] Settings support
//-----------------------------------------------------------------------------

// Windows data saved in MsTrG.ini file
// Because we never destroy or rename MsTrGWindowSettings, we can store the names in a separate buffer easily.
// (this is designed to be stored in a ImChunkStream buffer, with the variable-length Name following our structure)
struct MsTrGWindowSettings
{
    MsTrGID     ID;
    ImVec2ih    Pos;
    ImVec2ih    Size;
    bool        Collapsed;
    bool        WantApply;      // Set when loaded from .ini data (to enable merging/loading .ini data into an already running context)

    MsTrGWindowSettings()       { memset(this, 0, sizeof(*this)); }
    char* GetName()             { return (char*)(this + 1); }
};

struct MsTrGSettingsHandler
{
    const char* TypeName;       // Short description stored in .ini file. Disallowed characters: '[' ']'
    MsTrGID     TypeHash;       // == ImHashStr(TypeName)
    void        (*ClearAllFn)(MsTrGContext* ctx, MsTrGSettingsHandler* handler);                                // Clear all settings data
    void        (*ReadInitFn)(MsTrGContext* ctx, MsTrGSettingsHandler* handler);                                // Read: Called before reading (in registration order)
    void*       (*ReadOpenFn)(MsTrGContext* ctx, MsTrGSettingsHandler* handler, const char* name);              // Read: Called when entering into a new ini entry e.g. "[Window][Name]"
    void        (*ReadLineFn)(MsTrGContext* ctx, MsTrGSettingsHandler* handler, void* entry, const char* line); // Read: Called for every line of text within an ini entry
    void        (*ApplyAllFn)(MsTrGContext* ctx, MsTrGSettingsHandler* handler);                                // Read: Called after reading (in registration order)
    void        (*WriteAllFn)(MsTrGContext* ctx, MsTrGSettingsHandler* handler, MsTrGTextBuffer* out_buf);      // Write: Output every entries into 'out_buf'
    void*       UserData;

    MsTrGSettingsHandler() { memset(this, 0, sizeof(*this)); }
};

//-----------------------------------------------------------------------------
// [SECTION] Metrics, Debug Tools
//-----------------------------------------------------------------------------

enum MsTrGDebugLogFlags_
{
    // Event types
    MsTrGDebugLogFlags_None             = 0,
    MsTrGDebugLogFlags_EventActiveId    = 1 << 0,
    MsTrGDebugLogFlags_EventFocus       = 1 << 1,
    MsTrGDebugLogFlags_EventPopup       = 1 << 2,
    MsTrGDebugLogFlags_EventNav         = 1 << 3,
    MsTrGDebugLogFlags_EventIO          = 1 << 4,
    MsTrGDebugLogFlags_EventMask_       = MsTrGDebugLogFlags_EventActiveId  | MsTrGDebugLogFlags_EventFocus | MsTrGDebugLogFlags_EventPopup | MsTrGDebugLogFlags_EventNav | MsTrGDebugLogFlags_EventIO,
    MsTrGDebugLogFlags_OutputToTTY      = 1 << 10   // Also send output to TTY
};

struct MsTrGMetricsConfig
{
    bool        ShowDebugLog;
    bool        ShowStackTool;
    bool        ShowWindowsRects;
    bool        ShowWindowsBeginOrder;
    bool        ShowTablesRects;
    bool        ShowDrawCmdMesh;
    bool        ShowDrawCmdBoundingBoxes;
    int         ShowWindowsRectsType;
    int         ShowTablesRectsType;

    MsTrGMetricsConfig()
    {
        ShowDebugLog = ShowStackTool = ShowWindowsRects = ShowWindowsBeginOrder = ShowTablesRects = false;
        ShowDrawCmdMesh = true;
        ShowDrawCmdBoundingBoxes = true;
        ShowWindowsRectsType = ShowTablesRectsType = -1;
    }
};

struct MsTrGStackLevelInfo
{
    MsTrGID                 ID;
    ImS8                    QueryFrameCount;            // >= 1: Query in progress
    bool                    QuerySuccess;               // Obtained result from DebugHookIdInfo()
    MsTrGDataType           DataType : 8;
    char                    Desc[57];                   // Arbitrarily sized buffer to hold a result (FIXME: could replace Results[] with a chunk stream?) FIXME: Now that we added CTRL+C this should be fixed.

    MsTrGStackLevelInfo()   { memset(this, 0, sizeof(*this)); }
};

// State for Stack tool queries
struct MsTrGStackTool
{
    int                     LastActiveFrame;
    int                     StackLevel;                 // -1: query stack and resize Results, >= 0: individual stack level
    MsTrGID                 QueryId;                    // ID to query details for
    ImVector<MsTrGStackLevelInfo> Results;
    bool                    CopyToClipboardOnCtrlC;
    float                   CopyToClipboardLastTime;

    MsTrGStackTool()        { memset(this, 0, sizeof(*this)); CopyToClipboardLastTime = -FLT_MAX; }
};

//-----------------------------------------------------------------------------
// [SECTION] Generic context hooks
//-----------------------------------------------------------------------------

typedef void (*MsTrGContextHookCallback)(MsTrGContext* ctx, MsTrGContextHook* hook);
enum MsTrGContextHookType { MsTrGContextHookType_NewFramePre, MsTrGContextHookType_NewFramePost, MsTrGContextHookType_EndFramePre, MsTrGContextHookType_EndFramePost, MsTrGContextHookType_RenderPre, MsTrGContextHookType_RenderPost, MsTrGContextHookType_Shutdown, MsTrGContextHookType_PendingRemoval_ };

struct MsTrGContextHook
{
    MsTrGID                     HookId;     // A unique ID assigned by AddContextHook()
    MsTrGContextHookType        Type;
    MsTrGID                     Owner;
    MsTrGContextHookCallback    Callback;
    void*                       UserData;

    MsTrGContextHook()          { memset(this, 0, sizeof(*this)); }
};

//-----------------------------------------------------------------------------
// [SECTION] MsTrGContext (main Dear MsTrG context)
//-----------------------------------------------------------------------------

struct MsTrGContext
{
    bool                    Initialized;
    bool                    FontAtlasOwnedByContext;            // IO.Fonts-> is owned by the MsTrGContext and will be destructed along with it.
    MsTrGIO                 IO;
    ImVector<MsTrGInputEvent> InputEventsQueue;                 // Input events which will be tricked/written into IO structure.
    ImVector<MsTrGInputEvent> InputEventsTrail;                 // Past input events processed in NewFrame(). This is to allow domain-specific application to access e.g mouse/pen trail.
    MsTrGStyle              Style;
    ImFont*                 Font;                               // (Shortcut) == FontStack.empty() ? IO.Font : FontStack.back()
    float                   FontSize;                           // (Shortcut) == FontBaseSize * g.CurrentWindow->FontWindowScale == window->FontSize(). Text height for current window.
    float                   FontBaseSize;                       // (Shortcut) == IO.FontGlobalScale * Font->Scale * Font->FontSize. Base text height.
    ImDrawListSharedData    DrawListSharedData;
    double                  Time;
    int                     FrameCount;
    int                     FrameCountEnded;
    int                     FrameCountRendered;
    bool                    WithinFrameScope;                   // Set by NewFrame(), cleared by EndFrame()
    bool                    WithinFrameScopeWithImplicitWindow; // Set by NewFrame(), cleared by EndFrame() when the implicit debug window has been pushed
    bool                    WithinEndChild;                     // Set within EndChild()
    bool                    GcCompactAll;                       // Request full GC
    bool                    TestEngineHookItems;                // Will call test engine hooks: MsTrGTestEngineHook_ItemAdd(), MsTrGTestEngineHook_ItemInfo(), MsTrGTestEngineHook_Log()
    void*                   TestEngine;                         // Test engine user data

    // Windows state
    ImVector<MsTrGWindow*>  Windows;                            // Windows, sorted in display order, back to front
    ImVector<MsTrGWindow*>  WindowsFocusOrder;                  // Root windows, sorted in focus order, back to front.
    ImVector<MsTrGWindow*>  WindowsTempSortBuffer;              // Temporary buffer used in EndFrame() to reorder windows so parents are kept before their child
    ImVector<MsTrGWindowStackData> CurrentWindowStack;
    MsTrGStorage            WindowsById;                        // Map window's MsTrGID to MsTrGWindow*
    int                     WindowsActiveCount;                 // Number of unique windows submitted by frame
    ImVec2                  WindowsHoverPadding;                // Padding around resizable windows for which hovering on counts as hovering the window == ImMax(style.TouchExtraPadding, WINDOWS_HOVER_PADDING)
    MsTrGWindow*            CurrentWindow;                      // Window being drawn into
    MsTrGWindow*            HoveredWindow;                      // Window the mouse is hovering. Will typically catch mouse inputs.
    MsTrGWindow*            HoveredWindowUnderMovingWindow;     // Hovered window ignoring MovingWindow. Only set if MovingWindow is set.
    MsTrGWindow*            MovingWindow;                       // Track the window we clicked on (in order to preserve focus). The actual window that is moved is generally MovingWindow->RootWindow.
    MsTrGWindow*            WheelingWindow;                     // Track the window we started mouse-wheeling on. Until a timer elapse or mouse has moved, generally keep scrolling the same window even if during the course of scrolling the mouse ends up hovering a child window.
    ImVec2                  WheelingWindowRefMousePos;
    float                   WheelingWindowTimer;

    // Item/widgets state and tracking information
    MsTrGID                 DebugHookIdInfo;                    // Will call core hooks: DebugHookIdInfo() from GetID functions, used by Stack Tool [next HoveredId/ActiveId to not pull in an extra cache-line]
    MsTrGID                 HoveredId;                          // Hovered widget, filled during the frame
    MsTrGID                 HoveredIdPreviousFrame;
    bool                    HoveredIdAllowOverlap;
    bool                    HoveredIdUsingMouseWheel;           // Hovered widget will use mouse wheel. Blocks scrolling the underlying window.
    bool                    HoveredIdPreviousFrameUsingMouseWheel;
    bool                    HoveredIdDisabled;                  // At least one widget passed the rect test, but has been discarded by disabled flag or popup inhibit. May be true even if HoveredId == 0.
    float                   HoveredIdTimer;                     // Measure contiguous hovering time
    float                   HoveredIdNotActiveTimer;            // Measure contiguous hovering time where the item has not been active
    MsTrGID                 ActiveId;                           // Active widget
    MsTrGID                 ActiveIdIsAlive;                    // Active widget has been seen this frame (we can't use a bool as the ActiveId may change within the frame)
    float                   ActiveIdTimer;
    bool                    ActiveIdIsJustActivated;            // Set at the time of activation for one frame
    bool                    ActiveIdAllowOverlap;               // Active widget allows another widget to steal active id (generally for overlapping widgets, but not always)
    bool                    ActiveIdNoClearOnFocusLoss;         // Disable losing active id if the active id window gets unfocused.
    bool                    ActiveIdHasBeenPressedBefore;       // Track whether the active id led to a press (this is to allow changing between PressOnClick and PressOnRelease without pressing twice). Used by range_select branch.
    bool                    ActiveIdHasBeenEditedBefore;        // Was the value associated to the widget Edited over the course of the Active state.
    bool                    ActiveIdHasBeenEditedThisFrame;
    ImVec2                  ActiveIdClickOffset;                // Clicked offset from upper-left corner, if applicable (currently only set by ButtonBehavior)
    MsTrGWindow*            ActiveIdWindow;
    MsTrGInputSource        ActiveIdSource;                     // Activating with mouse or nav (gamepad/keyboard)
    int                     ActiveIdMouseButton;
    MsTrGID                 ActiveIdPreviousFrame;
    bool                    ActiveIdPreviousFrameIsAlive;
    bool                    ActiveIdPreviousFrameHasBeenEditedBefore;
    MsTrGWindow*            ActiveIdPreviousFrameWindow;
    MsTrGID                 LastActiveId;                       // Store the last non-zero ActiveId, useful for animation.
    float                   LastActiveIdTimer;                  // Store the last non-zero ActiveId timer since the beginning of activation, useful for animation.

    // Input Ownership
    bool                    ActiveIdUsingMouseWheel;            // Active widget will want to read mouse wheel. Blocks scrolling the underlying window.
    ImU32                   ActiveIdUsingNavDirMask;            // Active widget will want to read those nav move requests (e.g. can activate a button and move away from it)
    ImU32                   ActiveIdUsingNavInputMask;          // Active widget will want to read those nav inputs.
    ImBitArrayForNamedKeys  ActiveIdUsingKeyInputMask;          // Active widget will want to read those key inputs. When we grow the MsTrGKey enum we'll need to either to order the enum to make useful keys come first, either redesign this into e.g. a small array.

    // Next window/item data
    MsTrGItemFlags          CurrentItemFlags;                      // == g.ItemFlagsStack.back()
    MsTrGNextItemData       NextItemData;                       // Storage for SetNextItem** functions
    MsTrGLastItemData       LastItemData;                       // Storage for last submitted item (setup by ItemAdd)
    MsTrGNextWindowData     NextWindowData;                     // Storage for SetNextWindow** functions

    // Shared stacks
    ImVector<MsTrGColorMod> ColorStack;                         // Stack for PushStyleColor()/PopStyleColor() - inherited by Begin()
    ImVector<MsTrGStyleMod> StyleVarStack;                      // Stack for PushStyleVar()/PopStyleVar() - inherited by Begin()
    ImVector<ImFont*>       FontStack;                          // Stack for PushFont()/PopFont() - inherited by Begin()
    ImVector<MsTrGID>       FocusScopeStack;                    // Stack for PushFocusScope()/PopFocusScope() - not inherited by Begin(), unless child window
    ImVector<MsTrGItemFlags>ItemFlagsStack;                     // Stack for PushItemFlag()/PopItemFlag() - inherited by Begin()
    ImVector<MsTrGGroupData>GroupStack;                         // Stack for BeginGroup()/EndGroup() - not inherited by Begin()
    ImVector<MsTrGPopupData>OpenPopupStack;                     // Which popups are open (persistent)
    ImVector<MsTrGPopupData>BeginPopupStack;                    // Which level of BeginPopup() we are in (reset every frame)
    int                     BeginMenuCount;

    // Viewports
    ImVector<MsTrGViewportP*> Viewports;                        // Active viewports (Size==1 in 'master' branch). Each viewports hold their copy of ImDrawData.

    // Gamepad/keyboard Navigation
    MsTrGWindow*            NavWindow;                          // Focused window for navigation. Could be called 'FocusedWindow'
    MsTrGID                 NavId;                              // Focused item for navigation
    MsTrGID                 NavFocusScopeId;                    // Identify a selection scope (selection code often wants to "clear other items" when landing on an item of the selection set)
    MsTrGID                 NavActivateId;                      // ~~ (g.ActiveId == 0) && IsNavInputPressed(MsTrGNavInput_Activate) ? NavId : 0, also set when calling ActivateItem()
    MsTrGID                 NavActivateDownId;                  // ~~ IsNavInputDown(MsTrGNavInput_Activate) ? NavId : 0
    MsTrGID                 NavActivatePressedId;               // ~~ IsNavInputPressed(MsTrGNavInput_Activate) ? NavId : 0
    MsTrGID                 NavActivateInputId;                 // ~~ IsNavInputPressed(MsTrGNavInput_Input) ? NavId : 0; MsTrGActivateFlags_PreferInput will be set and NavActivateId will be 0.
    MsTrGActivateFlags      NavActivateFlags;
    MsTrGID                 NavJustMovedToId;                   // Just navigated to this id (result of a successfully MoveRequest).
    MsTrGID                 NavJustMovedToFocusScopeId;         // Just navigated to this focus scope id (result of a successfully MoveRequest).
    MsTrGModFlags           NavJustMovedToKeyMods;
    MsTrGID                 NavNextActivateId;                  // Set by ActivateItem(), queued until next frame.
    MsTrGActivateFlags      NavNextActivateFlags;
    MsTrGInputSource        NavInputSource;                     // Keyboard or Gamepad mode? THIS WILL ONLY BE None or NavGamepad or NavKeyboard.
    MsTrGNavLayer           NavLayer;                           // Layer we are navigating on. For now the system is hard-coded for 0=main contents and 1=menu/title bar, may expose layers later.
    bool                    NavIdIsAlive;                       // Nav widget has been seen this frame ~~ NavRectRel is valid
    bool                    NavMousePosDirty;                   // When set we will update mouse position if (io.ConfigFlags & MsTrGConfigFlags_NavEnableSetMousePos) if set (NB: this not enabled by default)
    bool                    NavDisableHighlight;                // When user starts using mouse, we hide gamepad/keyboard highlight (NB: but they are still available, which is why NavDisableHighlight isn't always != NavDisableMouseHover)
    bool                    NavDisableMouseHover;               // When user starts using gamepad/keyboard, we hide mouse hovering highlight until mouse is touched again.

    // Navigation: Init & Move Requests
    bool                    NavAnyRequest;                      // ~~ NavMoveRequest || NavInitRequest this is to perform early out in ItemAdd()
    bool                    NavInitRequest;                     // Init request for appearing window to select first item
    bool                    NavInitRequestFromMove;
    MsTrGID                 NavInitResultId;                    // Init request result (first item of the window, or one for which SetItemDefaultFocus() was called)
    ImRect                  NavInitResultRectRel;               // Init request result rectangle (relative to parent window)
    bool                    NavMoveSubmitted;                   // Move request submitted, will process result on next NewFrame()
    bool                    NavMoveScoringItems;                // Move request submitted, still scoring incoming items
    bool                    NavMoveForwardToNextFrame;
    MsTrGNavMoveFlags       NavMoveFlags;
    MsTrGScrollFlags        NavMoveScrollFlags;
    MsTrGModFlags           NavMoveKeyMods;
    MsTrGDir                NavMoveDir;                         // Direction of the move request (left/right/up/down)
    MsTrGDir                NavMoveDirForDebug;
    MsTrGDir                NavMoveClipDir;                     // FIXME-NAV: Describe the purpose of this better. Might want to rename?
    ImRect                  NavScoringRect;                     // Rectangle used for scoring, in screen space. Based of window->NavRectRel[], modified for directional navigation scoring.
    ImRect                  NavScoringNoClipRect;               // Some nav operations (such as PageUp/PageDown) enforce a region which clipper will attempt to always keep submitted
    int                     NavScoringDebugCount;               // Metrics for debugging
    int                     NavTabbingDir;                      // Generally -1 or +1, 0 when tabbing without a nav id
    int                     NavTabbingCounter;                  // >0 when counting items for tabbing
    MsTrGNavItemData        NavMoveResultLocal;                 // Best move request candidate within NavWindow
    MsTrGNavItemData        NavMoveResultLocalVisible;          // Best move request candidate within NavWindow that are mostly visible (when using MsTrGNavMoveFlags_AlsoScoreVisibleSet flag)
    MsTrGNavItemData        NavMoveResultOther;                 // Best move request candidate within NavWindow's flattened hierarchy (when using MsTrGWindowFlags_NavFlattened flag)
    MsTrGNavItemData        NavTabbingResultFirst;              // First tabbing request candidate within NavWindow and flattened hierarchy

    // Navigation: Windowing (CTRL+TAB for list, or Menu button + keys or directional pads to move/resize)
    MsTrGWindow*            NavWindowingTarget;                 // Target window when doing CTRL+Tab (or Pad Menu + FocusPrev/Next), this window is temporarily displayed top-most!
    MsTrGWindow*            NavWindowingTargetAnim;             // Record of last valid NavWindowingTarget until DimBgRatio and NavWindowingHighlightAlpha becomes 0.0f, so the fade-out can stay on it.
    MsTrGWindow*            NavWindowingListWindow;             // Internal window actually listing the CTRL+Tab contents
    float                   NavWindowingTimer;
    float                   NavWindowingHighlightAlpha;
    bool                    NavWindowingToggleLayer;

    // Render
    float                   DimBgRatio;                         // 0.0..1.0 animation when fading in a dimming background (for modal window and CTRL+TAB list)
    MsTrGMouseCursor        MouseCursor;

    // Drag and Drop
    bool                    DragDropActive;
    bool                    DragDropWithinSource;               // Set when within a BeginDragDropXXX/EndDragDropXXX block for a drag source.
    bool                    DragDropWithinTarget;               // Set when within a BeginDragDropXXX/EndDragDropXXX block for a drag target.
    MsTrGDragDropFlags      DragDropSourceFlags;
    int                     DragDropSourceFrameCount;
    int                     DragDropMouseButton;
    MsTrGPayload            DragDropPayload;
    ImRect                  DragDropTargetRect;                 // Store rectangle of current target candidate (we favor small targets when overlapping)
    MsTrGID                 DragDropTargetId;
    MsTrGDragDropFlags      DragDropAcceptFlags;
    float                   DragDropAcceptIdCurrRectSurface;    // Target item surface (we resolve overlapping targets by prioritizing the smaller surface)
    MsTrGID                 DragDropAcceptIdCurr;               // Target item id (set at the time of accepting the payload)
    MsTrGID                 DragDropAcceptIdPrev;               // Target item id from previous frame (we need to store this to allow for overlapping drag and drop targets)
    int                     DragDropAcceptFrameCount;           // Last time a target expressed a desire to accept the source
    MsTrGID                 DragDropHoldJustPressedId;          // Set when holding a payload just made ButtonBehavior() return a press.
    ImVector<unsigned char> DragDropPayloadBufHeap;             // We don't expose the ImVector<> directly, MsTrGPayload only holds pointer+size
    unsigned char           DragDropPayloadBufLocal[16];        // Local buffer for small payloads

    // Clipper
    int                             ClipperTempDataStacked;
    ImVector<MsTrGListClipperData>  ClipperTempData;

    // Tables
    MsTrGTable*                     CurrentTable;
    int                             TablesTempDataStacked;      // Temporary table data size (because we leave previous instances undestructed, we generally don't use TablesTempData.Size)
    ImVector<MsTrGTableTempData>    TablesTempData;             // Temporary table data (buffers reused/shared across instances, support nesting)
    ImPool<MsTrGTable>              Tables;                     // Persistent table data
    ImVector<float>                 TablesLastTimeActive;       // Last used timestamp of each tables (SOA, for efficient GC)
    ImVector<ImDrawChannel>         DrawChannelsTempMergeBuffer;

    // Tab bars
    MsTrGTabBar*                    CurrentTabBar;
    ImPool<MsTrGTabBar>             TabBars;
    ImVector<MsTrGPtrOrIndex>       CurrentTabBarStack;
    ImVector<MsTrGShrinkWidthItem>  ShrinkWidthBuffer;

    // Widget state
    ImVec2                  MouseLastValidPos;
    MsTrGInputTextState     InputTextState;
    ImFont                  InputTextPasswordFont;
    MsTrGID                 TempInputId;                        // Temporary text input when CTRL+clicking on a slider, etc.
    MsTrGColorEditFlags     ColorEditOptions;                   // Store user options for color edit widgets
    float                   ColorEditLastHue;                   // Backup of last Hue associated to LastColor, so we can restore Hue in lossy RGB<>HSV round trips
    float                   ColorEditLastSat;                   // Backup of last Saturation associated to LastColor, so we can restore Saturation in lossy RGB<>HSV round trips
    ImU32                   ColorEditLastColor;                 // RGB value with alpha set to 0.
    ImVec4                  ColorPickerRef;                     // Initial/reference color at the time of opening the color picker.
    MsTrGComboPreviewData   ComboPreviewData;
    float                   SliderGrabClickOffset;
    float                   SliderCurrentAccum;                 // Accumulated slider delta when using navigation controls.
    bool                    SliderCurrentAccumDirty;            // Has the accumulated slider delta changed since last time we tried to apply it?
    bool                    DragCurrentAccumDirty;
    float                   DragCurrentAccum;                   // Accumulator for dragging modification. Always high-precision, not rounded by end-user precision settings
    float                   DragSpeedDefaultRatio;              // If speed == 0.0f, uses (max-min) * DragSpeedDefaultRatio
    float                   ScrollbarClickDeltaToGrabCenter;    // Distance between mouse and center of grab box, normalized in parent space. Use storage?
    float                   DisabledAlphaBackup;                // Backup for style.Alpha for BeginDisabled()
    short                   DisabledStackSize;
    short                   TooltipOverrideCount;
    float                   TooltipSlowDelay;                   // Time before slow tooltips appears (FIXME: This is temporary until we merge in tooltip timer+priority work)
    ImVector<char>          ClipboardHandlerData;               // If no custom clipboard handler is defined
    ImVector<MsTrGID>       MenusIdSubmittedThisFrame;          // A list of menu IDs that were rendered at least once

    // Platform support
    MsTrGPlatformImeData    PlatformImeData;                    // Data updated by current frame
    MsTrGPlatformImeData    PlatformImeDataPrev;                // Previous frame data (when changing we will call io.SetPlatformImeDataFn
    char                    PlatformLocaleDecimalPoint;         // '.' or *localeconv()->decimal_point

    // Settings
    bool                    SettingsLoaded;
    float                   SettingsDirtyTimer;                 // Save .ini Settings to memory when time reaches zero
    MsTrGTextBuffer         SettingsIniData;                    // In memory .ini settings
    ImVector<MsTrGSettingsHandler>      SettingsHandlers;       // List of .ini settings handlers
    ImChunkStream<MsTrGWindowSettings>  SettingsWindows;        // MsTrGWindow .ini settings entries
    ImChunkStream<MsTrGTableSettings>   SettingsTables;         // MsTrGTable .ini settings entries
    ImVector<MsTrGContextHook>          Hooks;                  // Hooks for extensions (e.g. test engine)
    MsTrGID                             HookIdNext;             // Next available HookId

    // Capture/Logging
    bool                    LogEnabled;                         // Currently capturing
    MsTrGLogType            LogType;                            // Capture target
    ImFileHandle            LogFile;                            // If != NULL log to stdout/ file
    MsTrGTextBuffer         LogBuffer;                          // Accumulation buffer when log to clipboard. This is pointer so our GMsTrG static constructor doesn't call heap allocators.
    const char*             LogNextPrefix;
    const char*             LogNextSuffix;
    float                   LogLinePosY;
    bool                    LogLineFirstItem;
    int                     LogDepthRef;
    int                     LogDepthToExpand;
    int                     LogDepthToExpandDefault;            // Default/stored value for LogDepthMaxExpand if not specified in the LogXXX function call.

    // Debug Tools
    MsTrGDebugLogFlags      DebugLogFlags;
    MsTrGTextBuffer         DebugLogBuf;
    bool                    DebugItemPickerActive;              // Item picker is active (started with DebugStartItemPicker())
    MsTrGID                 DebugItemPickerBreakId;             // Will call IM_DEBUG_BREAK() when encountering this ID
    MsTrGMetricsConfig      DebugMetricsConfig;
    MsTrGStackTool          DebugStackTool;

    // Misc
    float                   FramerateSecPerFrame[120];          // Calculate estimate of framerate for user over the last 2 seconds.
    int                     FramerateSecPerFrameIdx;
    int                     FramerateSecPerFrameCount;
    float                   FramerateSecPerFrameAccum;
    int                     WantCaptureMouseNextFrame;          // Explicit capture override via SetNextFrameWantCaptureMouse()/SetNextFrameWantCaptureKeyboard(). Default to -1.
    int                     WantCaptureKeyboardNextFrame;       // "
    int                     WantTextInputNextFrame;
    ImVector<char>          TempBuffer;                         // Temporary text buffer

    MsTrGContext(ImFontAtlas* shared_font_atlas)
    {
        Initialized = false;
        FontAtlasOwnedByContext = shared_font_atlas ? false : true;
        Font = NULL;
        FontSize = FontBaseSize = 0.0f;
        IO.Fonts = shared_font_atlas ? shared_font_atlas : IM_NEW(ImFontAtlas)();
        Time = 0.0f;
        FrameCount = 0;
        FrameCountEnded = FrameCountRendered = -1;
        WithinFrameScope = WithinFrameScopeWithImplicitWindow = WithinEndChild = false;
        GcCompactAll = false;
        TestEngineHookItems = false;
        TestEngine = NULL;

        WindowsActiveCount = 0;
        CurrentWindow = NULL;
        HoveredWindow = NULL;
        HoveredWindowUnderMovingWindow = NULL;
        MovingWindow = NULL;
        WheelingWindow = NULL;
        WheelingWindowTimer = 0.0f;

        DebugHookIdInfo = 0;
        HoveredId = HoveredIdPreviousFrame = 0;
        HoveredIdAllowOverlap = false;
        HoveredIdUsingMouseWheel = HoveredIdPreviousFrameUsingMouseWheel = false;
        HoveredIdDisabled = false;
        HoveredIdTimer = HoveredIdNotActiveTimer = 0.0f;
        ActiveId = 0;
        ActiveIdIsAlive = 0;
        ActiveIdTimer = 0.0f;
        ActiveIdIsJustActivated = false;
        ActiveIdAllowOverlap = false;
        ActiveIdNoClearOnFocusLoss = false;
        ActiveIdHasBeenPressedBefore = false;
        ActiveIdHasBeenEditedBefore = false;
        ActiveIdHasBeenEditedThisFrame = false;
        ActiveIdClickOffset = ImVec2(-1, -1);
        ActiveIdWindow = NULL;
        ActiveIdSource = MsTrGInputSource_None;
        ActiveIdMouseButton = -1;
        ActiveIdPreviousFrame = 0;
        ActiveIdPreviousFrameIsAlive = false;
        ActiveIdPreviousFrameHasBeenEditedBefore = false;
        ActiveIdPreviousFrameWindow = NULL;
        LastActiveId = 0;
        LastActiveIdTimer = 0.0f;

        ActiveIdUsingMouseWheel = false;
        ActiveIdUsingNavDirMask = 0x00;
        ActiveIdUsingNavInputMask = 0x00;
        ActiveIdUsingKeyInputMask.ClearAllBits();

        CurrentItemFlags = MsTrGItemFlags_None;
        BeginMenuCount = 0;

        NavWindow = NULL;
        NavId = NavFocusScopeId = NavActivateId = NavActivateDownId = NavActivatePressedId = NavActivateInputId = 0;
        NavJustMovedToId = NavJustMovedToFocusScopeId = NavNextActivateId = 0;
        NavActivateFlags = NavNextActivateFlags = MsTrGActivateFlags_None;
        NavJustMovedToKeyMods = MsTrGModFlags_None;
        NavInputSource = MsTrGInputSource_None;
        NavLayer = MsTrGNavLayer_Main;
        NavIdIsAlive = false;
        NavMousePosDirty = false;
        NavDisableHighlight = true;
        NavDisableMouseHover = false;
        NavAnyRequest = false;
        NavInitRequest = false;
        NavInitRequestFromMove = false;
        NavInitResultId = 0;
        NavMoveSubmitted = false;
        NavMoveScoringItems = false;
        NavMoveForwardToNextFrame = false;
        NavMoveFlags = MsTrGNavMoveFlags_None;
        NavMoveScrollFlags = MsTrGScrollFlags_None;
        NavMoveKeyMods = MsTrGModFlags_None;
        NavMoveDir = NavMoveDirForDebug = NavMoveClipDir = MsTrGDir_None;
        NavScoringDebugCount = 0;
        NavTabbingDir = 0;
        NavTabbingCounter = 0;

        NavWindowingTarget = NavWindowingTargetAnim = NavWindowingListWindow = NULL;
        NavWindowingTimer = NavWindowingHighlightAlpha = 0.0f;
        NavWindowingToggleLayer = false;

        DimBgRatio = 0.0f;
        MouseCursor = MsTrGMouseCursor_Arrow;

        DragDropActive = DragDropWithinSource = DragDropWithinTarget = false;
        DragDropSourceFlags = MsTrGDragDropFlags_None;
        DragDropSourceFrameCount = -1;
        DragDropMouseButton = -1;
        DragDropTargetId = 0;
        DragDropAcceptFlags = MsTrGDragDropFlags_None;
        DragDropAcceptIdCurrRectSurface = 0.0f;
        DragDropAcceptIdPrev = DragDropAcceptIdCurr = 0;
        DragDropAcceptFrameCount = -1;
        DragDropHoldJustPressedId = 0;
        memset(DragDropPayloadBufLocal, 0, sizeof(DragDropPayloadBufLocal));

        ClipperTempDataStacked = 0;

        CurrentTable = NULL;
        TablesTempDataStacked = 0;
        CurrentTabBar = NULL;

        TempInputId = 0;
        ColorEditOptions = MsTrGColorEditFlags_DefaultOptions_;
        ColorEditLastHue = ColorEditLastSat = 0.0f;
        ColorEditLastColor = 0;
        SliderGrabClickOffset = 0.0f;
        SliderCurrentAccum = 0.0f;
        SliderCurrentAccumDirty = false;
        DragCurrentAccumDirty = false;
        DragCurrentAccum = 0.0f;
        DragSpeedDefaultRatio = 1.0f / 100.0f;
        DisabledAlphaBackup = 0.0f;
        DisabledStackSize = 0;
        ScrollbarClickDeltaToGrabCenter = 0.0f;
        TooltipOverrideCount = 0;
        TooltipSlowDelay = 0.50f;

        PlatformImeData.InputPos = ImVec2(0.0f, 0.0f);
        PlatformImeDataPrev.InputPos = ImVec2(-1.0f, -1.0f); // Different to ensure initial submission
        PlatformLocaleDecimalPoint = '.';

        SettingsLoaded = false;
        SettingsDirtyTimer = 0.0f;
        HookIdNext = 0;

        LogEnabled = false;
        LogType = MsTrGLogType_None;
        LogNextPrefix = LogNextSuffix = NULL;
        LogFile = NULL;
        LogLinePosY = FLT_MAX;
        LogLineFirstItem = false;
        LogDepthRef = 0;
        LogDepthToExpand = LogDepthToExpandDefault = 2;

        DebugLogFlags = MsTrGDebugLogFlags_OutputToTTY;
        DebugItemPickerActive = false;
        DebugItemPickerBreakId = 0;

        memset(FramerateSecPerFrame, 0, sizeof(FramerateSecPerFrame));
        FramerateSecPerFrameIdx = FramerateSecPerFrameCount = 0;
        FramerateSecPerFrameAccum = 0.0f;
        WantCaptureMouseNextFrame = WantCaptureKeyboardNextFrame = WantTextInputNextFrame = -1;
    }
};

//-----------------------------------------------------------------------------
// [SECTION] MsTrGWindowTempData, MsTrGWindow
//-----------------------------------------------------------------------------

// Transient per-window data, reset at the beginning of the frame. This used to be called MsTrGDrawContext, hence the DC variable name in MsTrGWindow.
// (That's theory, in practice the delimitation between MsTrGWindow and MsTrGWindowTempData is quite tenuous and could be reconsidered..)
// (This doesn't need a constructor because we zero-clear it as part of MsTrGWindow and all frame-temporary data are setup on Begin)
struct MsTrG_API MsTrGWindowTempData
{
    // Layout
    ImVec2                  CursorPos;              // Current emitting position, in absolute coordinates.
    ImVec2                  CursorPosPrevLine;
    ImVec2                  CursorStartPos;         // Initial position after Begin(), generally ~ window position + WindowPadding.
    ImVec2                  CursorMaxPos;           // Used to implicitly calculate ContentSize at the beginning of next frame, for scrolling range and auto-resize. Always growing during the frame.
    ImVec2                  IdealMaxPos;            // Used to implicitly calculate ContentSizeIdeal at the beginning of next frame, for auto-resize only. Always growing during the frame.
    ImVec2                  CurrLineSize;
    ImVec2                  PrevLineSize;
    float                   CurrLineTextBaseOffset; // Baseline offset (0.0f by default on a new line, generally == style.FramePadding.y when a framed item has been added).
    float                   PrevLineTextBaseOffset;
    bool                    IsSameLine;
    ImVec1                  Indent;                 // Indentation / start position from left of window (increased by TreePush/TreePop, etc.)
    ImVec1                  ColumnsOffset;          // Offset to the current column (if ColumnsCurrent > 0). FIXME: This and the above should be a stack to allow use cases like Tree->Column->Tree. Need revamp columns API.
    ImVec1                  GroupOffset;
    ImVec2                  CursorStartPosLossyness;// Record the loss of precision of CursorStartPos due to really large scrolling amount. This is used by clipper to compensentate and fix the most common use case of large scroll area.

       // Last item status
    MsTrGID                 LastItemId;             // ID for last item
    MsTrGItemStatusFlags    LastItemStatusFlags;    // Status flags for last item (see MsTrGItemStatusFlags_)
    ImRect                  LastItemRect;           // Interaction rect for last item
    ImRect                  LastItemDisplayRect;    // End-user display rect for last item (only valid if LastItemStatusFlags & MsTrGItemStatusFlags_HasDisplayRect)

    // Keyboard/Gamepad navigation
    MsTrGNavLayer           NavLayerCurrent;        // Current layer, 0..31 (we currently only use 0..1)
    short                   NavLayersActiveMask;    // Which layers have been written to (result from previous frame)
    short                   NavLayersActiveMaskNext;// Which layers have been written to (accumulator for current frame)
    MsTrGID                 NavFocusScopeIdCurrent; // Current focus scope ID while appending
    bool                    NavHideHighlightOneFrame;
    bool                    NavHasScroll;           // Set when scrolling can be used (ScrollMax > 0.0f)

    // Miscellaneous
    bool                    MenuBarAppending;       // FIXME: Remove this
    ImVec2                  MenuBarOffset;          // MenuBarOffset.x is sort of equivalent of a per-layer CursorPos.x, saved/restored as we switch to the menu bar. The only situation when MenuBarOffset.y is > 0 if when (SafeAreaPadding.y > FramePadding.y), often used on TVs.
    MsTrGMenuColumns        MenuColumns;            // Simplified columns storage for menu items measurement
    int                     TreeDepth;              // Current tree depth.
    ImU32                   TreeJumpToParentOnPopMask; // Store a copy of !g.NavIdIsAlive for TreeDepth 0..31.. Could be turned into a ImU64 if necessary.
    ImVector<MsTrGWindow*>  ChildWindows;
    MsTrGStorage*           StateStorage;           // Current persistent per-window storage (store e.g. tree node open/close state)
    MsTrGOldColumns*        CurrentColumns;         // Current columns set
    int                     CurrentTableIdx;        // Current table index (into g.Tables)
    MsTrGLayoutType         LayoutType;
    MsTrGLayoutType         ParentLayoutType;       // Layout type of parent window at the time of Begin()

    // Local parameters stacks
    // We store the current settings outside of the vectors to increase memory locality (reduce cache misses). The vectors are rarely modified. Also it allows us to not heap allocate for short-lived windows which are not using those settings.
    float                   ItemWidth;              // Current item width (>0.0: width in pixels, <0.0: align xx pixels to the right of window).
    float                   TextWrapPos;            // Current text wrap pos.
    ImVector<float>         ItemWidthStack;         // Store item widths to restore (attention: .back() is not == ItemWidth)
    ImVector<float>         TextWrapPosStack;       // Store text wrap pos to restore (attention: .back() is not == TextWrapPos)
};

// Storage for one window
struct MsTrG_API MsTrGWindow
{
    char*                   Name;                               // Window name, owned by the window.
    MsTrGID                 ID;                                 // == ImHashStr(Name)
    MsTrGWindowFlags        Flags;                              // See enum MsTrGWindowFlags_
    MsTrGViewportP*         Viewport;                           // Always set in Begin(). Inactive windows may have a NULL value here if their viewport was discarded.
    ImVec2                  Pos;                                // Position (always rounded-up to nearest pixel)
    ImVec2                  Size;                               // Current size (==SizeFull or collapsed title bar size)
    ImVec2                  SizeFull;                           // Size when non collapsed
    ImVec2                  ContentSize;                        // Size of contents/scrollable client area (calculated from the extents reach of the cursor) from previous frame. Does not include window decoration or window padding.
    ImVec2                  ContentSizeIdeal;
    ImVec2                  ContentSizeExplicit;                // Size of contents/scrollable client area explicitly request by the user via SetNextWindowContentSize().
    ImVec2                  WindowPadding;                      // Window padding at the time of Begin().
    float                   WindowRounding;                     // Window rounding at the time of Begin(). May be clamped lower to avoid rendering artifacts with title bar, menu bar etc.
    float                   WindowBorderSize;                   // Window border size at the time of Begin().
    int                     NameBufLen;                         // Size of buffer storing Name. May be larger than strlen(Name)!
    MsTrGID                 MoveId;                             // == window->GetID("#MOVE")
    MsTrGID                 ChildId;                            // ID of corresponding item in parent window (for navigation to return from child window to parent window)
    ImVec2                  Scroll;
    ImVec2                  ScrollMax;
    ImVec2                  ScrollTarget;                       // target scroll position. stored as cursor position with scrolling canceled out, so the highest point is always 0.0f. (FLT_MAX for no change)
    ImVec2                  ScrollTargetCenterRatio;            // 0.0f = scroll so that target position is at top, 0.5f = scroll so that target position is centered
    ImVec2                  ScrollTargetEdgeSnapDist;           // 0.0f = no snapping, >0.0f snapping threshold
    ImVec2                  ScrollbarSizes;                     // Size taken by each scrollbars on their smaller axis. Pay attention! ScrollbarSizes.x == width of the vertical scrollbar, ScrollbarSizes.y = height of the horizontal scrollbar.
    bool                    ScrollbarX, ScrollbarY;             // Are scrollbars visible?
    bool                    Active;                             // Set to true on Begin(), unless Collapsed
    bool                    WasActive;
    bool                    WriteAccessed;                      // Set to true when any widget access the current window
    bool                    Collapsed;                          // Set when collapsing window to become only title-bar
    bool                    WantCollapseToggle;
    bool                    SkipItems;                          // Set when items can safely be all clipped (e.g. window not visible or collapsed)
    bool                    Appearing;                          // Set during the frame where the window is appearing (or re-appearing)
    bool                    Hidden;                             // Do not display (== HiddenFrames*** > 0)
    bool                    IsFallbackWindow;                   // Set on the "Debug##Default" window.
    bool                    IsExplicitChild;                    // Set when passed _ChildWindow, left to false by BeginDocked()
    bool                    HasCloseButton;                     // Set when the window has a close button (p_open != NULL)
    signed char             ResizeBorderHeld;                   // Current border being held for resize (-1: none, otherwise 0-3)
    short                   BeginCount;                         // Number of Begin() during the current frame (generally 0 or 1, 1+ if appending via multiple Begin/End pairs)
    short                   BeginOrderWithinParent;             // Begin() order within immediate parent window, if we are a child window. Otherwise 0.
    short                   BeginOrderWithinContext;            // Begin() order within entire MsTrG context. This is mostly used for debugging submission order related issues.
    short                   FocusOrder;                         // Order within WindowsFocusOrder[], altered when windows are focused.
    MsTrGID                 PopupId;                            // ID in the popup stack when this window is used as a popup/menu (because we use generic Name/ID for recycling)
    ImS8                    AutoFitFramesX, AutoFitFramesY;
    ImS8                    AutoFitChildAxises;
    bool                    AutoFitOnlyGrows;
    MsTrGDir                AutoPosLastDirection;
    ImS8                    HiddenFramesCanSkipItems;           // Hide the window for N frames
    ImS8                    HiddenFramesCannotSkipItems;        // Hide the window for N frames while allowing items to be submitted so we can measure their size
    ImS8                    HiddenFramesForRenderOnly;          // Hide the window until frame N at Render() time only
    ImS8                    DisableInputsFrames;                // Disable window interactions for N frames
    MsTrGCond               SetWindowPosAllowFlags : 8;         // store acceptable condition flags for SetNextWindowPos() use.
    MsTrGCond               SetWindowSizeAllowFlags : 8;        // store acceptable condition flags for SetNextWindowSize() use.
    MsTrGCond               SetWindowCollapsedAllowFlags : 8;   // store acceptable condition flags for SetNextWindowCollapsed() use.
    ImVec2                  SetWindowPosVal;                    // store window position when using a non-zero Pivot (position set needs to be processed when we know the window size)
    ImVec2                  SetWindowPosPivot;                  // store window pivot for positioning. ImVec2(0, 0) when positioning from top-left corner; ImVec2(0.5f, 0.5f) for centering; ImVec2(1, 1) for bottom right.

    ImVector<MsTrGID>       IDStack;                            // ID stack. ID are hashes seeded with the value at the top of the stack. (In theory this should be in the TempData structure)
    MsTrGWindowTempData     DC;                                 // Temporary per-window data, reset at the beginning of the frame. This used to be called MsTrGDrawContext, hence the "DC" variable name.

    // The best way to understand what those rectangles are is to use the 'Metrics->Tools->Show Windows Rectangles' viewer.
    // The main 'OuterRect', omitted as a field, is window->Rect().
    ImRect                  OuterRectClipped;                   // == Window->Rect() just after setup in Begin(). == window->Rect() for root window.
    ImRect                  InnerRect;                          // Inner rectangle (omit title bar, menu bar, scroll bar)
    ImRect                  InnerClipRect;                      // == InnerRect shrunk by WindowPadding*0.5f on each side, clipped within viewport or parent clip rect.
    ImRect                  WorkRect;                           // Initially covers the whole scrolling region. Reduced by containers e.g columns/tables when active. Shrunk by WindowPadding*1.0f on each side. This is meant to replace ContentRegionRect over time (from 1.71+ onward).
    ImRect                  ParentWorkRect;                     // Backup of WorkRect before entering a container such as columns/tables. Used by e.g. SpanAllColumns functions to easily access. Stacked containers are responsible for maintaining this. // FIXME-WORKRECT: Could be a stack?
    ImRect                  ClipRect;                           // Current clipping/scissoring rectangle, evolve as we are using PushClipRect(), etc. == DrawList->clip_rect_stack.back().
    ImRect                  ContentRegionRect;                  // FIXME: This is currently confusing/misleading. It is essentially WorkRect but not handling of scrolling. We currently rely on it as right/bottom aligned sizing operation need some size to rely on.
    ImVec2ih                HitTestHoleSize;                    // Define an optional rectangular hole where mouse will pass-through the window.
    ImVec2ih                HitTestHoleOffset;

    int                     LastFrameActive;                    // Last frame number the window was Active.
    float                   LastTimeActive;                     // Last timestamp the window was Active (using float as we don't need high precision there)
    float                   ItemWidthDefault;
    MsTrGStorage            StateStorage;
    ImVector<MsTrGOldColumns> ColumnsStorage;
    float                   FontWindowScale;                    // User scale multiplier per-window, via SetWindowFontScale()
    int                     SettingsOffset;                     // Offset into SettingsWindows[] (offsets are always valid as we only grow the array from the back)

    ImDrawList*             DrawList;                           // == &DrawListInst (for backward compatibility reason with code using MsTrG_internal.h we keep this a pointer)
    ImDrawList              DrawListInst;
    MsTrGWindow*            ParentWindow;                       // If we are a child _or_ popup _or_ docked window, this is pointing to our parent. Otherwise NULL.
    MsTrGWindow*            ParentWindowInBeginStack;
    MsTrGWindow*            RootWindow;                         // Point to ourself or first ancestor that is not a child window. Doesn't cross through popups/dock nodes.
    MsTrGWindow*            RootWindowPopupTree;                // Point to ourself or first ancestor that is not a child window. Cross through popups parent<>child.
    MsTrGWindow*            RootWindowForTitleBarHighlight;     // Point to ourself or first ancestor which will display TitleBgActive color when this window is active.
    MsTrGWindow*            RootWindowForNav;                   // Point to ourself or first ancestor which doesn't have the NavFlattened flag.

    MsTrGWindow*            NavLastChildNavWindow;              // When going to the menu bar, we remember the child window we came from. (This could probably be made implicit if we kept g.Windows sorted by last focused including child window.)
    MsTrGID                 NavLastIds[MsTrGNavLayer_COUNT];    // Last known NavId for this window, per layer (0/1)
    ImRect                  NavRectRel[MsTrGNavLayer_COUNT];    // Reference rectangle, in window relative space

    int                     MemoryDrawListIdxCapacity;          // Backup of last idx/vtx count, so when waking up the window we can preallocate and avoid iterative alloc/copy
    int                     MemoryDrawListVtxCapacity;
    bool                    MemoryCompacted;                    // Set when window extraneous data have been garbage collected

public:
    MsTrGWindow(MsTrGContext* context, const char* name);
    ~MsTrGWindow();

    MsTrGID     GetID(const char* str, const char* str_end = NULL);
    MsTrGID     GetID(const void* ptr);
    MsTrGID     GetID(int n);
    MsTrGID     GetIDFromRectangle(const ImRect& r_abs);

    // We don't use g.FontSize because the window may be != g.CurrentWidow.
    ImRect      Rect() const            { return ImRect(Pos.x, Pos.y, Pos.x + Size.x, Pos.y + Size.y); }
    float       CalcFontSize() const    { MsTrGContext& g = *GMsTrG; float scale = g.FontBaseSize * FontWindowScale; if (ParentWindow) scale *= ParentWindow->FontWindowScale; return scale; }
    float       TitleBarHeight() const  { MsTrGContext& g = *GMsTrG; return (Flags & MsTrGWindowFlags_NoTitleBar) ? 0.0f : CalcFontSize() + g.Style.FramePadding.y * 2.0f; }
    ImRect      TitleBarRect() const    { return ImRect(Pos, ImVec2(Pos.x + SizeFull.x, Pos.y + TitleBarHeight())); }
    float       MenuBarHeight() const   { MsTrGContext& g = *GMsTrG; return (Flags & MsTrGWindowFlags_MenuBar) ? DC.MenuBarOffset.y + CalcFontSize() + g.Style.FramePadding.y * 2.0f : 0.0f; }
    ImRect      MenuBarRect() const     { float y1 = Pos.y + TitleBarHeight(); return ImRect(Pos.x, y1, Pos.x + SizeFull.x, y1 + MenuBarHeight()); }
};

//-----------------------------------------------------------------------------
// [SECTION] Tab bar, Tab item support
//-----------------------------------------------------------------------------

// Extend MsTrGTabBarFlags_
enum MsTrGTabBarFlagsPrivate_
{
    MsTrGTabBarFlags_DockNode                   = 1 << 20,  // Part of a dock node [we don't use this in the master branch but it facilitate branch syncing to keep this around]
    MsTrGTabBarFlags_IsFocused                  = 1 << 21,
    MsTrGTabBarFlags_SaveSettings               = 1 << 22   // FIXME: Settings are handled by the docking system, this only request the tab bar to mark settings dirty when reordering tabs
};

// Extend MsTrGTabItemFlags_
enum MsTrGTabItemFlagsPrivate_
{
    MsTrGTabItemFlags_SectionMask_              = MsTrGTabItemFlags_Leading | MsTrGTabItemFlags_Trailing,
    MsTrGTabItemFlags_NoCloseButton             = 1 << 20,  // Track whether p_open was set or not (we'll need this info on the next frame to recompute ContentWidth during layout)
    MsTrGTabItemFlags_Button                    = 1 << 21   // Used by TabItemButton, change the tab item behavior to mimic a button
};

// Storage for one active tab item (sizeof() 40 bytes)
struct MsTrGTabItem
{
    MsTrGID             ID;
    MsTrGTabItemFlags   Flags;
    int                 LastFrameVisible;
    int                 LastFrameSelected;      // This allows us to infer an ordered list of the last activated tabs with little maintenance
    float               Offset;                 // Position relative to beginning of tab
    float               Width;                  // Width currently displayed
    float               ContentWidth;           // Width of label, stored during BeginTabItem() call
    float               RequestedWidth;         // Width optionally requested by caller, -1.0f is unused
    ImS32               NameOffset;             // When Window==NULL, offset to name within parent MsTrGTabBar::TabsNames
    ImS16               BeginOrder;             // BeginTabItem() order, used to re-order tabs after toggling MsTrGTabBarFlags_Reorderable
    ImS16               IndexDuringLayout;      // Index only used during TabBarLayout()
    bool                WantClose;              // Marked as closed by SetTabItemClosed()

    MsTrGTabItem()      { memset(this, 0, sizeof(*this)); LastFrameVisible = LastFrameSelected = -1; NameOffset = -1; BeginOrder = IndexDuringLayout = -1; }
};

// Storage for a tab bar (sizeof() 152 bytes)
struct MsTrG_API MsTrGTabBar
{
    ImVector<MsTrGTabItem> Tabs;
    MsTrGTabBarFlags    Flags;
    MsTrGID             ID;                     // Zero for tab-bars used by docking
    MsTrGID             SelectedTabId;          // Selected tab/window
    MsTrGID             NextSelectedTabId;      // Next selected tab/window. Will also trigger a scrolling animation
    MsTrGID             VisibleTabId;           // Can occasionally be != SelectedTabId (e.g. when previewing contents for CTRL+TAB preview)
    int                 CurrFrameVisible;
    int                 PrevFrameVisible;
    ImRect              BarRect;
    float               CurrTabsContentsHeight;
    float               PrevTabsContentsHeight; // Record the height of contents submitted below the tab bar
    float               WidthAllTabs;           // Actual width of all tabs (locked during layout)
    float               WidthAllTabsIdeal;      // Ideal width if all tabs were visible and not clipped
    float               ScrollingAnim;
    float               ScrollingTarget;
    float               ScrollingTargetDistToVisibility;
    float               ScrollingSpeed;
    float               ScrollingRectMinX;
    float               ScrollingRectMaxX;
    MsTrGID             ReorderRequestTabId;
    ImS16               ReorderRequestOffset;
    ImS8                BeginCount;
    bool                WantLayout;
    bool                VisibleTabWasSubmitted;
    bool                TabsAddedNew;           // Set to true when a new tab item or button has been added to the tab bar during last frame
    ImS16               TabsActiveCount;        // Number of tabs submitted this frame.
    ImS16               LastTabItemIdx;         // Index of last BeginTabItem() tab for use by EndTabItem()
    float               ItemSpacingY;
    ImVec2              FramePadding;           // style.FramePadding locked at the time of BeginTabBar()
    ImVec2              BackupCursorPos;
    MsTrGTextBuffer     TabsNames;              // For non-docking tab bar we re-append names in a contiguous buffer.

    MsTrGTabBar();
    int                 GetTabOrder(const MsTrGTabItem* tab) const  { return Tabs.index_from_ptr(tab); }
    const char*         GetTabName(const MsTrGTabItem* tab) const
    {
        IM_ASSERT(tab->NameOffset != -1 && tab->NameOffset < TabsNames.Buf.Size);
        return TabsNames.Buf.Data + tab->NameOffset;
    }
};

//-----------------------------------------------------------------------------
// [SECTION] Table support
//-----------------------------------------------------------------------------

#define IM_COL32_DISABLE                IM_COL32(0,0,0,1)   // Special sentinel code which cannot be used as a regular color.
#define MsTrG_TABLE_MAX_COLUMNS         64                  // sizeof(ImU64) * 8. This is solely because we frequently encode columns set in a ImU64.
#define MsTrG_TABLE_MAX_DRAW_CHANNELS   (4 + 64 * 2)        // See TableSetupDrawChannels()

// Our current column maximum is 64 but we may raise that in the future.
typedef ImS8 MsTrGTableColumnIdx;
typedef ImU8 MsTrGTableDrawChannelIdx;

// [Internal] sizeof() ~ 104
// We use the terminology "Enabled" to refer to a column that is not Hidden by user/api.
// We use the terminology "Clipped" to refer to a column that is out of sight because of scrolling/clipping.
// This is in contrast with some user-facing api such as IsItemVisible() / IsRectVisible() which use "Visible" to mean "not clipped".
struct MsTrGTableColumn
{
    MsTrGTableColumnFlags   Flags;                          // Flags after some patching (not directly same as provided by user). See MsTrGTableColumnFlags_
    float                   WidthGiven;                     // Final/actual width visible == (MaxX - MinX), locked in TableUpdateLayout(). May be > WidthRequest to honor minimum width, may be < WidthRequest to honor shrinking columns down in tight space.
    float                   MinX;                           // Absolute positions
    float                   MaxX;
    float                   WidthRequest;                   // Master width absolute value when !(Flags & _WidthStretch). When Stretch this is derived every frame from StretchWeight in TableUpdateLayout()
    float                   WidthAuto;                      // Automatic width
    float                   StretchWeight;                  // Master width weight when (Flags & _WidthStretch). Often around ~1.0f initially.
    float                   InitStretchWeightOrWidth;       // Value passed to TableSetupColumn(). For Width it is a content width (_without padding_).
    ImRect                  ClipRect;                       // Clipping rectangle for the column
    MsTrGID                 UserID;                         // Optional, value passed to TableSetupColumn()
    float                   WorkMinX;                       // Contents region min ~(MinX + CellPaddingX + CellSpacingX1) == cursor start position when entering column
    float                   WorkMaxX;                       // Contents region max ~(MaxX - CellPaddingX - CellSpacingX2)
    float                   ItemWidth;                      // Current item width for the column, preserved across rows
    float                   ContentMaxXFrozen;              // Contents maximum position for frozen rows (apart from headers), from which we can infer content width.
    float                   ContentMaxXUnfrozen;
    float                   ContentMaxXHeadersUsed;         // Contents maximum position for headers rows (regardless of freezing). TableHeader() automatically softclip itself + report ideal desired size, to avoid creating extraneous draw calls
    float                   ContentMaxXHeadersIdeal;
    ImS16                   NameOffset;                     // Offset into parent ColumnsNames[]
    MsTrGTableColumnIdx     DisplayOrder;                   // Index within Table's IndexToDisplayOrder[] (column may be reordered by users)
    MsTrGTableColumnIdx     IndexWithinEnabledSet;          // Index within enabled/visible set (<= IndexToDisplayOrder)
    MsTrGTableColumnIdx     PrevEnabledColumn;              // Index of prev enabled/visible column within Columns[], -1 if first enabled/visible column
    MsTrGTableColumnIdx     NextEnabledColumn;              // Index of next enabled/visible column within Columns[], -1 if last enabled/visible column
    MsTrGTableColumnIdx     SortOrder;                      // Index of this column within sort specs, -1 if not sorting on this column, 0 for single-sort, may be >0 on multi-sort
    MsTrGTableDrawChannelIdx DrawChannelCurrent;            // Index within DrawSplitter.Channels[]
    MsTrGTableDrawChannelIdx DrawChannelFrozen;             // Draw channels for frozen rows (often headers)
    MsTrGTableDrawChannelIdx DrawChannelUnfrozen;           // Draw channels for unfrozen rows
    bool                    IsEnabled;                      // IsUserEnabled && (Flags & MsTrGTableColumnFlags_Disabled) == 0
    bool                    IsUserEnabled;                  // Is the column not marked Hidden by the user? (unrelated to being off view, e.g. clipped by scrolling).
    bool                    IsUserEnabledNextFrame;
    bool                    IsVisibleX;                     // Is actually in view (e.g. overlapping the host window clipping rectangle, not scrolled).
    bool                    IsVisibleY;
    bool                    IsRequestOutput;                // Return value for TableSetColumnIndex() / TableNextColumn(): whether we request user to output contents or not.
    bool                    IsSkipItems;                    // Do we want item submissions to this column to be completely ignored (no layout will happen).
    bool                    IsPreserveWidthAuto;
    ImS8                    NavLayerCurrent;                // MsTrGNavLayer in 1 byte
    ImU8                    AutoFitQueue;                   // Queue of 8 values for the next 8 frames to request auto-fit
    ImU8                    CannotSkipItemsQueue;           // Queue of 8 values for the next 8 frames to disable Clipped/SkipItem
    ImU8                    SortDirection : 2;              // MsTrGSortDirection_Ascending or MsTrGSortDirection_Descending
    ImU8                    SortDirectionsAvailCount : 2;   // Number of available sort directions (0 to 3)
    ImU8                    SortDirectionsAvailMask : 4;    // Mask of available sort directions (1-bit each)
    ImU8                    SortDirectionsAvailList;        // Ordered of available sort directions (2-bits each)

    MsTrGTableColumn()
    {
        memset(this, 0, sizeof(*this));
        StretchWeight = WidthRequest = -1.0f;
        NameOffset = -1;
        DisplayOrder = IndexWithinEnabledSet = -1;
        PrevEnabledColumn = NextEnabledColumn = -1;
        SortOrder = -1;
        SortDirection = MsTrGSortDirection_None;
        DrawChannelCurrent = DrawChannelFrozen = DrawChannelUnfrozen = (ImU8)-1;
    }
};

// Transient cell data stored per row.
// sizeof() ~ 6
struct MsTrGTableCellData
{
    ImU32                       BgColor;    // Actual color
    MsTrGTableColumnIdx         Column;     // Column number
};

// Per-instance data that needs preserving across frames (seemingly most others do not need to be preserved aside from debug needs, does that needs they could be moved to MsTrGTableTempData ?)
struct MsTrGTableInstanceData
{
    float                       LastOuterHeight;            // Outer height from last frame // FIXME: multi-instance issue (#3955)
    float                       LastFirstRowHeight;         // Height of first row from last frame // FIXME: possible multi-instance issue?

    MsTrGTableInstanceData()    { LastOuterHeight = LastFirstRowHeight = 0.0f; }
};

// FIXME-TABLE: more transient data could be stored in a per-stacked table structure: DrawSplitter, SortSpecs, incoming RowData
struct MsTrG_API MsTrGTable
{
    MsTrGID                     ID;
    MsTrGTableFlags             Flags;
    void*                       RawData;                    // Single allocation to hold Columns[], DisplayOrderToIndex[] and RowCellData[]
    MsTrGTableTempData*         TempData;                   // Transient data while table is active. Point within g.CurrentTableStack[]
    ImSpan<MsTrGTableColumn>    Columns;                    // Point within RawData[]
    ImSpan<MsTrGTableColumnIdx> DisplayOrderToIndex;        // Point within RawData[]. Store display order of columns (when not reordered, the values are 0...Count-1)
    ImSpan<MsTrGTableCellData>  RowCellData;                // Point within RawData[]. Store cells background requests for current row.
    ImU64                       EnabledMaskByDisplayOrder;  // Column DisplayOrder -> IsEnabled map
    ImU64                       EnabledMaskByIndex;         // Column Index -> IsEnabled map (== not hidden by user/api) in a format adequate for iterating column without touching cold data
    ImU64                       VisibleMaskByIndex;         // Column Index -> IsVisibleX|IsVisibleY map (== not hidden by user/api && not hidden by scrolling/cliprect)
    ImU64                       RequestOutputMaskByIndex;   // Column Index -> IsVisible || AutoFit (== expect user to submit items)
    MsTrGTableFlags             SettingsLoadedFlags;        // Which data were loaded from the .ini file (e.g. when order is not altered we won't save order)
    int                         SettingsOffset;             // Offset in g.SettingsTables
    int                         LastFrameActive;
    int                         ColumnsCount;               // Number of columns declared in BeginTable()
    int                         CurrentRow;
    int                         CurrentColumn;
    ImS16                       InstanceCurrent;            // Count of BeginTable() calls with same ID in the same frame (generally 0). This is a little bit similar to BeginCount for a window, but multiple table with same ID look are multiple tables, they are just synched.
    ImS16                       InstanceInteracted;         // Mark which instance (generally 0) of the same ID is being interacted with
    float                       RowPosY1;
    float                       RowPosY2;
    float                       RowMinHeight;               // Height submitted to TableNextRow()
    float                       RowTextBaseline;
    float                       RowIndentOffsetX;
    MsTrGTableRowFlags          RowFlags : 16;              // Current row flags, see MsTrGTableRowFlags_
    MsTrGTableRowFlags          LastRowFlags : 16;
    int                         RowBgColorCounter;          // Counter for alternating background colors (can be fast-forwarded by e.g clipper), not same as CurrentRow because header rows typically don't increase this.
    ImU32                       RowBgColor[2];              // Background color override for current row.
    ImU32                       BorderColorStrong;
    ImU32                       BorderColorLight;
    float                       BorderX1;
    float                       BorderX2;
    float                       HostIndentX;
    float                       MinColumnWidth;
    float                       OuterPaddingX;
    float                       CellPaddingX;               // Padding from each borders
    float                       CellPaddingY;
    float                       CellSpacingX1;              // Spacing between non-bordered cells
    float                       CellSpacingX2;
    float                       InnerWidth;                 // User value passed to BeginTable(), see comments at the top of BeginTable() for details.
    float                       ColumnsGivenWidth;          // Sum of current column width
    float                       ColumnsAutoFitWidth;        // Sum of ideal column width in order nothing to be clipped, used for auto-fitting and content width submission in outer window
    float                       ColumnsStretchSumWeights;   // Sum of weight of all enabled stretching columns
    float                       ResizedColumnNextWidth;
    float                       ResizeLockMinContentsX2;    // Lock minimum contents width while resizing down in order to not create feedback loops. But we allow growing the table.
    float                       RefScale;                   // Reference scale to be able to rescale columns on font/dpi changes.
    ImRect                      OuterRect;                  // Note: for non-scrolling table, OuterRect.Max.y is often FLT_MAX until EndTable(), unless a height has been specified in BeginTable().
    ImRect                      InnerRect;                  // InnerRect but without decoration. As with OuterRect, for non-scrolling tables, InnerRect.Max.y is
    ImRect                      WorkRect;
    ImRect                      InnerClipRect;
    ImRect                      BgClipRect;                 // We use this to cpu-clip cell background color fill, evolve during the frame as we cross frozen rows boundaries
    ImRect                      Bg0ClipRectForDrawCmd;      // Actual ImDrawCmd clip rect for BG0/1 channel. This tends to be == OuterWindow->ClipRect at BeginTable() because output in BG0/BG1 is cpu-clipped
    ImRect                      Bg2ClipRectForDrawCmd;      // Actual ImDrawCmd clip rect for BG2 channel. This tends to be a correct, tight-fit, because output to BG2 are done by widgets relying on regular ClipRect.
    ImRect                      HostClipRect;               // This is used to check if we can eventually merge our columns draw calls into the current draw call of the current window.
    ImRect                      HostBackupInnerClipRect;    // Backup of InnerWindow->ClipRect during PushTableBackground()/PopTableBackground()
    MsTrGWindow*                OuterWindow;                // Parent window for the table
    MsTrGWindow*                InnerWindow;                // Window holding the table data (== OuterWindow or a child window)
    MsTrGTextBuffer             ColumnsNames;               // Contiguous buffer holding columns names
    ImDrawListSplitter*         DrawSplitter;               // Shortcut to TempData->DrawSplitter while in table. Isolate draw commands per columns to avoid switching clip rect constantly
    MsTrGTableInstanceData      InstanceDataFirst;
    ImVector<MsTrGTableInstanceData>    InstanceDataExtra;  // FIXME-OPT: Using a small-vector pattern would be good.
    MsTrGTableColumnSortSpecs   SortSpecsSingle;
    ImVector<MsTrGTableColumnSortSpecs> SortSpecsMulti;     // FIXME-OPT: Using a small-vector pattern would be good.
    MsTrGTableSortSpecs         SortSpecs;                  // Public facing sorts specs, this is what we return in TableGetSortSpecs()
    MsTrGTableColumnIdx         SortSpecsCount;
    MsTrGTableColumnIdx         ColumnsEnabledCount;        // Number of enabled columns (<= ColumnsCount)
    MsTrGTableColumnIdx         ColumnsEnabledFixedCount;   // Number of enabled columns (<= ColumnsCount)
    MsTrGTableColumnIdx         DeclColumnsCount;           // Count calls to TableSetupColumn()
    MsTrGTableColumnIdx         HoveredColumnBody;          // Index of column whose visible region is being hovered. Important: == ColumnsCount when hovering empty region after the right-most column!
    MsTrGTableColumnIdx         HoveredColumnBorder;        // Index of column whose right-border is being hovered (for resizing).
    MsTrGTableColumnIdx         AutoFitSingleColumn;        // Index of single column requesting auto-fit.
    MsTrGTableColumnIdx         ResizedColumn;              // Index of column being resized. Reset when InstanceCurrent==0.
    MsTrGTableColumnIdx         LastResizedColumn;          // Index of column being resized from previous frame.
    MsTrGTableColumnIdx         HeldHeaderColumn;           // Index of column header being held.
    MsTrGTableColumnIdx         ReorderColumn;              // Index of column being reordered. (not cleared)
    MsTrGTableColumnIdx         ReorderColumnDir;           // -1 or +1
    MsTrGTableColumnIdx         LeftMostEnabledColumn;      // Index of left-most non-hidden column.
    MsTrGTableColumnIdx         RightMostEnabledColumn;     // Index of right-most non-hidden column.
    MsTrGTableColumnIdx         LeftMostStretchedColumn;    // Index of left-most stretched column.
    MsTrGTableColumnIdx         RightMostStretchedColumn;   // Index of right-most stretched column.
    MsTrGTableColumnIdx         ContextPopupColumn;         // Column right-clicked on, of -1 if opening context menu from a neutral/empty spot
    MsTrGTableColumnIdx         FreezeRowsRequest;          // Requested frozen rows count
    MsTrGTableColumnIdx         FreezeRowsCount;            // Actual frozen row count (== FreezeRowsRequest, or == 0 when no scrolling offset)
    MsTrGTableColumnIdx         FreezeColumnsRequest;       // Requested frozen columns count
    MsTrGTableColumnIdx         FreezeColumnsCount;         // Actual frozen columns count (== FreezeColumnsRequest, or == 0 when no scrolling offset)
    MsTrGTableColumnIdx         RowCellDataCurrent;         // Index of current RowCellData[] entry in current row
    MsTrGTableDrawChannelIdx    DummyDrawChannel;           // Redirect non-visible columns here.
    MsTrGTableDrawChannelIdx    Bg2DrawChannelCurrent;      // For Selectable() and other widgets drawing across columns after the freezing line. Index within DrawSplitter.Channels[]
    MsTrGTableDrawChannelIdx    Bg2DrawChannelUnfrozen;
    bool                        IsLayoutLocked;             // Set by TableUpdateLayout() which is called when beginning the first row.
    bool                        IsInsideRow;                // Set when inside TableBeginRow()/TableEndRow().
    bool                        IsInitializing;
    bool                        IsSortSpecsDirty;
    bool                        IsUsingHeaders;             // Set when the first row had the MsTrGTableRowFlags_Headers flag.
    bool                        IsContextPopupOpen;         // Set when default context menu is open (also see: ContextPopupColumn, InstanceInteracted).
    bool                        IsSettingsRequestLoad;
    bool                        IsSettingsDirty;            // Set when table settings have changed and needs to be reported into MsTrGTableSetttings data.
    bool                        IsDefaultDisplayOrder;      // Set when display order is unchanged from default (DisplayOrder contains 0...Count-1)
    bool                        IsResetAllRequest;
    bool                        IsResetDisplayOrderRequest;
    bool                        IsUnfrozenRows;             // Set when we got past the frozen row.
    bool                        IsDefaultSizingPolicy;      // Set if user didn't explicitly set a sizing policy in BeginTable()
    bool                        MemoryCompacted;
    bool                        HostSkipItems;              // Backup of InnerWindow->SkipItem at the end of BeginTable(), because we will overwrite InnerWindow->SkipItem on a per-column basis

    MsTrGTable()                { memset(this, 0, sizeof(*this)); LastFrameActive = -1; }
    ~MsTrGTable()               { IM_FREE(RawData); }
};

// Transient data that are only needed between BeginTable() and EndTable(), those buffers are shared (1 per level of stacked table).
// - Accessing those requires chasing an extra pointer so for very frequently used data we leave them in the main table structure.
// - We also leave out of this structure data that tend to be particularly useful for debugging/metrics.
struct MsTrG_API MsTrGTableTempData
{
    int                         TableIndex;                 // Index in g.Tables.Buf[] pool
    float                       LastTimeActive;             // Last timestamp this structure was used

    ImVec2                      UserOuterSize;              // outer_size.x passed to BeginTable()
    ImDrawListSplitter          DrawSplitter;

    ImRect                      HostBackupWorkRect;         // Backup of InnerWindow->WorkRect at the end of BeginTable()
    ImRect                      HostBackupParentWorkRect;   // Backup of InnerWindow->ParentWorkRect at the end of BeginTable()
    ImVec2                      HostBackupPrevLineSize;     // Backup of InnerWindow->DC.PrevLineSize at the end of BeginTable()
    ImVec2                      HostBackupCurrLineSize;     // Backup of InnerWindow->DC.CurrLineSize at the end of BeginTable()
    ImVec2                      HostBackupCursorMaxPos;     // Backup of InnerWindow->DC.CursorMaxPos at the end of BeginTable()
    ImVec1                      HostBackupColumnsOffset;    // Backup of OuterWindow->DC.ColumnsOffset at the end of BeginTable()
    float                       HostBackupItemWidth;        // Backup of OuterWindow->DC.ItemWidth at the end of BeginTable()
    int                         HostBackupItemWidthStackSize;//Backup of OuterWindow->DC.ItemWidthStack.Size at the end of BeginTable()

    MsTrGTableTempData()        { memset(this, 0, sizeof(*this)); LastTimeActive = -1.0f; }
};

// sizeof() ~ 12
struct MsTrGTableColumnSettings
{
    float                   WidthOrWeight;
    MsTrGID                 UserID;
    MsTrGTableColumnIdx     Index;
    MsTrGTableColumnIdx     DisplayOrder;
    MsTrGTableColumnIdx     SortOrder;
    ImU8                    SortDirection : 2;
    ImU8                    IsEnabled : 1; // "Visible" in ini file
    ImU8                    IsStretch : 1;

    MsTrGTableColumnSettings()
    {
        WidthOrWeight = 0.0f;
        UserID = 0;
        Index = -1;
        DisplayOrder = SortOrder = -1;
        SortDirection = MsTrGSortDirection_None;
        IsEnabled = 1;
        IsStretch = 0;
    }
};

// This is designed to be stored in a single ImChunkStream (1 header followed by N MsTrGTableColumnSettings, etc.)
struct MsTrGTableSettings
{
    MsTrGID                     ID;                     // Set to 0 to invalidate/delete the setting
    MsTrGTableFlags             SaveFlags;              // Indicate data we want to save using the Resizable/Reorderable/Sortable/Hideable flags (could be using its own flags..)
    float                       RefScale;               // Reference scale to be able to rescale columns on font/dpi changes.
    MsTrGTableColumnIdx         ColumnsCount;
    MsTrGTableColumnIdx         ColumnsCountMax;        // Maximum number of columns this settings instance can store, we can recycle a settings instance with lower number of columns but not higher
    bool                        WantApply;              // Set when loaded from .ini data (to enable merging/loading .ini data into an already running context)

    MsTrGTableSettings()        { memset(this, 0, sizeof(*this)); }
    MsTrGTableColumnSettings*   GetColumnSettings()     { return (MsTrGTableColumnSettings*)(this + 1); }
};

//-----------------------------------------------------------------------------
// [SECTION] MsTrG internal API
// No guarantee of forward compatibility here!
//-----------------------------------------------------------------------------

namespace MsTrG
{
    // Windows
    // We should always have a CurrentWindow in the stack (there is an implicit "Debug" window)
    // If this ever crash because g.CurrentWindow is NULL it means that either
    // - MsTrG::NewFrame() has never been called, which is illegal.
    // - You are calling MsTrG functions after MsTrG::EndFrame()/MsTrG::Render() and before the next MsTrG::NewFrame(), which is also illegal.
    inline    MsTrGWindow*  GetCurrentWindowRead()      { MsTrGContext& g = *GMsTrG; return g.CurrentWindow; }
    inline    MsTrGWindow*  GetCurrentWindow()          { MsTrGContext& g = *GMsTrG; g.CurrentWindow->WriteAccessed = true; return g.CurrentWindow; }
    MsTrG_API MsTrGWindow*  FindWindowByID(MsTrGID id);
    MsTrG_API MsTrGWindow*  FindWindowByName(const char* name);
    MsTrG_API void          UpdateWindowParentAndRootLinks(MsTrGWindow* window, MsTrGWindowFlags flags, MsTrGWindow* parent_window);
    MsTrG_API ImVec2        CalcWindowNextAutoFitSize(MsTrGWindow* window);
    MsTrG_API bool          IsWindowChildOf(MsTrGWindow* window, MsTrGWindow* potential_parent, bool popup_hierarchy);
    MsTrG_API bool          IsWindowWithinBeginStackOf(MsTrGWindow* window, MsTrGWindow* potential_parent);
    MsTrG_API bool          IsWindowAbove(MsTrGWindow* potential_above, MsTrGWindow* potential_below);
    MsTrG_API bool          IsWindowNavFocusable(MsTrGWindow* window);
    MsTrG_API void          SetWindowPos(MsTrGWindow* window, const ImVec2& pos, MsTrGCond cond = 0);
    MsTrG_API void          SetWindowSize(MsTrGWindow* window, const ImVec2& size, MsTrGCond cond = 0);
    MsTrG_API void          SetWindowCollapsed(MsTrGWindow* window, bool collapsed, MsTrGCond cond = 0);
    MsTrG_API void          SetWindowHitTestHole(MsTrGWindow* window, const ImVec2& pos, const ImVec2& size);
    inline ImRect           WindowRectAbsToRel(MsTrGWindow* window, const ImRect& r) { ImVec2 off = window->DC.CursorStartPos; return ImRect(r.Min.x - off.x, r.Min.y - off.y, r.Max.x - off.x, r.Max.y - off.y); }
    inline ImRect           WindowRectRelToAbs(MsTrGWindow* window, const ImRect& r) { ImVec2 off = window->DC.CursorStartPos; return ImRect(r.Min.x + off.x, r.Min.y + off.y, r.Max.x + off.x, r.Max.y + off.y); }

    // Windows: Display Order and Focus Order
    MsTrG_API void          FocusWindow(MsTrGWindow* window);
    MsTrG_API void          FocusTopMostWindowUnderOne(MsTrGWindow* under_this_window, MsTrGWindow* ignore_window);
    MsTrG_API void          BringWindowToFocusFront(MsTrGWindow* window);
    MsTrG_API void          BringWindowToDisplayFront(MsTrGWindow* window);
    MsTrG_API void          BringWindowToDisplayBack(MsTrGWindow* window);
    MsTrG_API void          BringWindowToDisplayBehind(MsTrGWindow* window, MsTrGWindow* above_window);
    MsTrG_API int           FindWindowDisplayIndex(MsTrGWindow* window);
    MsTrG_API MsTrGWindow*  FindBottomMostVisibleWindowWithinBeginStack(MsTrGWindow* window);

    // Fonts, drawing
    MsTrG_API void          SetCurrentFont(ImFont* font);
    inline ImFont*          GetDefaultFont() { MsTrGContext& g = *GMsTrG; return g.IO.FontDefault ? g.IO.FontDefault : g.IO.Fonts->Fonts[0]; }
    inline ImDrawList*      GetForegroundDrawList(MsTrGWindow* window) { IM_UNUSED(window); return GetForegroundDrawList(); } // This seemingly unnecessary wrapper simplifies compatibility between the 'master' and 'docking' branches.
    MsTrG_API ImDrawList*   GetBackgroundDrawList(MsTrGViewport* viewport);                     // get background draw list for the given viewport. this draw list will be the first rendering one. Useful to quickly draw shapes/text behind dear MsTrG contents.
    MsTrG_API ImDrawList*   GetForegroundDrawList(MsTrGViewport* viewport);                     // get foreground draw list for the given viewport. this draw list will be the last rendered one. Useful to quickly draw shapes/text over dear MsTrG contents.

    // Init
    MsTrG_API void          Initialize();
    MsTrG_API void          Shutdown();    // Since 1.60 this is a _private_ function. You can call DestroyContext() to destroy the context created by CreateContext().

    // NewFrame
    MsTrG_API void          UpdateInputEvents(bool trickle_fast_inputs);
    MsTrG_API void          UpdateHoveredWindowAndCaptureFlags();
    MsTrG_API void          StartMouseMovingWindow(MsTrGWindow* window);
    MsTrG_API void          UpdateMouseMovingWindowNewFrame();
    MsTrG_API void          UpdateMouseMovingWindowEndFrame();

    // Generic context hooks
    MsTrG_API MsTrGID       AddContextHook(MsTrGContext* context, const MsTrGContextHook* hook);
    MsTrG_API void          RemoveContextHook(MsTrGContext* context, MsTrGID hook_to_remove);
    MsTrG_API void          CallContextHooks(MsTrGContext* context, MsTrGContextHookType type);

    // Viewports
    MsTrG_API void          SetWindowViewport(MsTrGWindow* window, MsTrGViewportP* viewport);

    // Settings
    MsTrG_API void                  MarkIniSettingsDirty();
    MsTrG_API void                  MarkIniSettingsDirty(MsTrGWindow* window);
    MsTrG_API void                  ClearIniSettings();
    MsTrG_API MsTrGWindowSettings*  CreateNewWindowSettings(const char* name);
    MsTrG_API MsTrGWindowSettings*  FindWindowSettings(MsTrGID id);
    MsTrG_API MsTrGWindowSettings*  FindOrCreateWindowSettings(const char* name);
    MsTrG_API void                  AddSettingsHandler(const MsTrGSettingsHandler* handler);
    MsTrG_API void                  RemoveSettingsHandler(const char* type_name);
    MsTrG_API MsTrGSettingsHandler* FindSettingsHandler(const char* type_name);

    // Scrolling
    MsTrG_API void          SetNextWindowScroll(const ImVec2& scroll); // Use -1.0f on one axis to leave as-is
    MsTrG_API void          SetScrollX(MsTrGWindow* window, float scroll_x);
    MsTrG_API void          SetScrollY(MsTrGWindow* window, float scroll_y);
    MsTrG_API void          SetScrollFromPosX(MsTrGWindow* window, float local_x, float center_x_ratio);
    MsTrG_API void          SetScrollFromPosY(MsTrGWindow* window, float local_y, float center_y_ratio);

    // Early work-in-progress API (ScrollToItem() will become public)
    MsTrG_API void          ScrollToItem(MsTrGScrollFlags flags = 0);
    MsTrG_API void          ScrollToRect(MsTrGWindow* window, const ImRect& rect, MsTrGScrollFlags flags = 0);
    MsTrG_API ImVec2        ScrollToRectEx(MsTrGWindow* window, const ImRect& rect, MsTrGScrollFlags flags = 0);
//#ifndef MsTrG_DISABLE_OBSOLETE_FUNCTIONS
    inline void             ScrollToBringRectIntoView(MsTrGWindow* window, const ImRect& rect) { ScrollToRect(window, rect, MsTrGScrollFlags_KeepVisibleEdgeY); }
//#endif

    // Basic Accessors
    inline MsTrGID          GetItemID()     { MsTrGContext& g = *GMsTrG; return g.LastItemData.ID; }   // Get ID of last item (~~ often same MsTrG::GetID(label) beforehand)
    inline MsTrGItemStatusFlags GetItemStatusFlags(){ MsTrGContext& g = *GMsTrG; return g.LastItemData.StatusFlags; }
    inline MsTrGItemFlags   GetItemFlags()  { MsTrGContext& g = *GMsTrG; return g.LastItemData.InFlags; }
    inline MsTrGID          GetActiveID()   { MsTrGContext& g = *GMsTrG; return g.ActiveId; }
    inline MsTrGID          GetFocusID()    { MsTrGContext& g = *GMsTrG; return g.NavId; }
    MsTrG_API void          SetActiveID(MsTrGID id, MsTrGWindow* window);
    MsTrG_API void          SetFocusID(MsTrGID id, MsTrGWindow* window);
    MsTrG_API void          ClearActiveID();
    MsTrG_API MsTrGID       GetHoveredID();
    MsTrG_API void          SetHoveredID(MsTrGID id);
    MsTrG_API void          KeepAliveID(MsTrGID id);
    MsTrG_API void          MarkItemEdited(MsTrGID id);     // Mark data associated to given item as "edited", used by IsItemDeactivatedAfterEdit() function.
    MsTrG_API void          PushOverrideID(MsTrGID id);     // Push given value as-is at the top of the ID stack (whereas PushID combines old and new hashes)
    MsTrG_API MsTrGID       GetIDWithSeed(const char* str_id_begin, const char* str_id_end, MsTrGID seed);

    // Basic Helpers for widget code
    MsTrG_API void          ItemSize(const ImVec2& size, float text_baseline_y = -1.0f);
    inline void             ItemSize(const ImRect& bb, float text_baseline_y = -1.0f) { ItemSize(bb.GetSize(), text_baseline_y); } // FIXME: This is a misleading API since we expect CursorPos to be bb.Min.
    MsTrG_API bool          ItemAdd(const ImRect& bb, MsTrGID id, const ImRect* nav_bb = NULL, MsTrGItemFlags extra_flags = 0);
    MsTrG_API bool          ItemHoverable(const ImRect& bb, MsTrGID id);
    MsTrG_API bool          IsClippedEx(const ImRect& bb, MsTrGID id);
    MsTrG_API void          SetLastItemData(MsTrGID item_id, MsTrGItemFlags in_flags, MsTrGItemStatusFlags status_flags, const ImRect& item_rect);
    MsTrG_API ImVec2        CalcItemSize(ImVec2 size, float default_w, float default_h);
    MsTrG_API float         CalcWrapWidthForPos(const ImVec2& pos, float wrap_pos_x);
    MsTrG_API void          PushMultiItemsWidths(int components, float width_full);
    MsTrG_API bool          IsItemToggledSelection();                                   // Was the last item selection toggled? (after Selectable(), TreeNode() etc. We only returns toggle _event_ in order to handle clipping correctly)
    MsTrG_API ImVec2        GetContentRegionMaxAbs();
    MsTrG_API void          ShrinkWidths(MsTrGShrinkWidthItem* items, int count, float width_excess);

    // Parameter stacks
    MsTrG_API void          PushItemFlag(MsTrGItemFlags option, bool enabled);
    MsTrG_API void          PopItemFlag();

#ifndef MsTrG_DISABLE_OBSOLETE_FUNCTIONS
    // Currently refactoring focus/nav/tabbing system
    // If you have old/custom copy-and-pasted widgets that used FocusableItemRegister():
    //  (Old) MsTrG_VERSION_NUM  < 18209: using 'ItemAdd(....)'                              and 'bool tab_focused = FocusableItemRegister(...)'
    //  (Old) MsTrG_VERSION_NUM >= 18209: using 'ItemAdd(..., MsTrGItemAddFlags_Focusable)'  and 'bool tab_focused = (GetItemStatusFlags() & MsTrGItemStatusFlags_Focused) != 0'
    //  (New) MsTrG_VERSION_NUM >= 18413: using 'ItemAdd(..., MsTrGItemFlags_Inputable)'     and 'bool tab_focused = (GetItemStatusFlags() & MsTrGItemStatusFlags_FocusedTabbing) != 0 || g.NavActivateInputId == id' (WIP)
    // Widget code are simplified as there's no need to call FocusableItemUnregister() while managing the transition from regular widget to TempInputText()
    inline bool FocusableItemRegister(MsTrGWindow* window, MsTrGID id)  { IM_ASSERT(0); IM_UNUSED(window); IM_UNUSED(id); return false; } // -> pass MsTrGItemAddFlags_Inputable flag to ItemAdd()
    inline void FocusableItemUnregister(MsTrGWindow* window)            { IM_ASSERT(0); IM_UNUSED(window); }                              // -> unnecessary: TempInputText() uses MsTrGInputTextFlags_MergedItem
#endif

    // Logging/Capture
    MsTrG_API void          LogBegin(MsTrGLogType type, int auto_open_depth);           // -> BeginCapture() when we design v2 api, for now stay under the radar by using the old name.
    MsTrG_API void          LogToBuffer(int auto_open_depth = -1);                      // Start logging/capturing to internal buffer
    MsTrG_API void          LogRenderedText(const ImVec2* ref_pos, const char* text, const char* text_end = NULL);
    MsTrG_API void          LogSetNextTextDecoration(const char* prefix, const char* suffix);

    // Popups, Modals, Tooltips
    MsTrG_API bool          BeginChildEx(const char* name, MsTrGID id, const ImVec2& size_arg, bool border, MsTrGWindowFlags flags);
    MsTrG_API void          OpenPopupEx(MsTrGID id, MsTrGPopupFlags popup_flags = MsTrGPopupFlags_None);
    MsTrG_API void          ClosePopupToLevel(int remaining, bool restore_focus_to_window_under_popup);
    MsTrG_API void          ClosePopupsOverWindow(MsTrGWindow* ref_window, bool restore_focus_to_window_under_popup);
    MsTrG_API void          ClosePopupsExceptModals();
    MsTrG_API bool          IsPopupOpen(MsTrGID id, MsTrGPopupFlags popup_flags);
    MsTrG_API bool          BeginPopupEx(MsTrGID id, MsTrGWindowFlags extra_flags);
    MsTrG_API void          BeginTooltipEx(MsTrGTooltipFlags tooltip_flags, MsTrGWindowFlags extra_window_flags);
    MsTrG_API ImRect        GetPopupAllowedExtentRect(MsTrGWindow* window);
    MsTrG_API MsTrGWindow*  GetTopMostPopupModal();
    MsTrG_API MsTrGWindow*  GetTopMostAndVisiblePopupModal();
    MsTrG_API ImVec2        FindBestWindowPosForPopup(MsTrGWindow* window);
    MsTrG_API ImVec2        FindBestWindowPosForPopupEx(const ImVec2& ref_pos, const ImVec2& size, MsTrGDir* last_dir, const ImRect& r_outer, const ImRect& r_avoid, MsTrGPopupPositionPolicy policy);

    // Menus
    MsTrG_API bool          BeginViewportSideBar(const char* name, MsTrGViewport* viewport, MsTrGDir dir, float size, MsTrGWindowFlags window_flags);
    MsTrG_API bool          BeginMenuEx(const char* label, const char* icon, bool enabled = true);
    MsTrG_API bool          MenuItemEx(const char* label, const char* icon, const char* shortcut = NULL, bool selected = false, bool enabled = true);

    // Combos
    MsTrG_API bool          BeginComboPopup(MsTrGID popup_id, const ImRect& bb, MsTrGComboFlags flags);
    MsTrG_API bool          BeginComboPreview();
    MsTrG_API void          EndComboPreview();

    // Gamepad/Keyboard Navigation
    MsTrG_API void          NavInitWindow(MsTrGWindow* window, bool force_reinit);
    MsTrG_API void          NavInitRequestApplyResult();
    MsTrG_API bool          NavMoveRequestButNoResultYet();
    MsTrG_API void          NavMoveRequestSubmit(MsTrGDir move_dir, MsTrGDir clip_dir, MsTrGNavMoveFlags move_flags, MsTrGScrollFlags scroll_flags);
    MsTrG_API void          NavMoveRequestForward(MsTrGDir move_dir, MsTrGDir clip_dir, MsTrGNavMoveFlags move_flags, MsTrGScrollFlags scroll_flags);
    MsTrG_API void          NavMoveRequestResolveWithLastItem(MsTrGNavItemData* result);
    MsTrG_API void          NavMoveRequestCancel();
    MsTrG_API void          NavMoveRequestApplyResult();
    MsTrG_API void          NavMoveRequestTryWrapping(MsTrGWindow* window, MsTrGNavMoveFlags move_flags);
    MsTrG_API const char*   GetNavInputName(MsTrGNavInput n);
    MsTrG_API float         GetNavInputAmount(MsTrGNavInput n, MsTrGNavReadMode mode);
    MsTrG_API ImVec2        GetNavInputAmount2d(MsTrGNavDirSourceFlags dir_sources, MsTrGNavReadMode mode, float slow_factor = 0.0f, float fast_factor = 0.0f);
    MsTrG_API int           CalcTypematicRepeatAmount(float t0, float t1, float repeat_delay, float repeat_rate);
    MsTrG_API void          ActivateItem(MsTrGID id);   // Remotely activate a button, checkbox, tree node etc. given its unique ID. activation is queued and processed on the next frame when the item is encountered again.
    MsTrG_API void          SetNavWindow(MsTrGWindow* window);
    MsTrG_API void          SetNavID(MsTrGID id, MsTrGNavLayer nav_layer, MsTrGID focus_scope_id, const ImRect& rect_rel);

    // Focus Scope (WIP)
    // This is generally used to identify a selection set (multiple of which may be in the same window), as selection
    // patterns generally need to react (e.g. clear selection) when landing on an item of the set.
    MsTrG_API void          PushFocusScope(MsTrGID id);
    MsTrG_API void          PopFocusScope();
    inline MsTrGID          GetFocusedFocusScope()          { MsTrGContext& g = *GMsTrG; return g.NavFocusScopeId; }                            // Focus scope which is actually active
    inline MsTrGID          GetFocusScope()                 { MsTrGContext& g = *GMsTrG; return g.CurrentWindow->DC.NavFocusScopeIdCurrent; }   // Focus scope we are outputting into, set by PushFocusScope()

    // Inputs
    // FIXME: Eventually we should aim to move e.g. IsActiveIdUsingKey() into IsKeyXXX functions.
    inline bool             IsNamedKey(MsTrGKey key)                                    { return key >= MsTrGKey_NamedKey_BEGIN && key < MsTrGKey_NamedKey_END; }
    inline bool             IsLegacyKey(MsTrGKey key)                                   { return key >= MsTrGKey_LegacyNativeKey_BEGIN && key < MsTrGKey_LegacyNativeKey_END; }
    inline bool             IsGamepadKey(MsTrGKey key)                                  { return key >= MsTrGKey_Gamepad_BEGIN && key < MsTrGKey_Gamepad_END; }
    MsTrG_API MsTrGKeyData* GetKeyData(MsTrGKey key);
    MsTrG_API void          SetItemUsingMouseWheel();
    MsTrG_API void          SetActiveIdUsingNavAndKeys();
    inline bool             IsActiveIdUsingNavDir(MsTrGDir dir)                         { MsTrGContext& g = *GMsTrG; return (g.ActiveIdUsingNavDirMask & (1 << dir)) != 0; }
    inline bool             IsActiveIdUsingNavInput(MsTrGNavInput input)                { MsTrGContext& g = *GMsTrG; return (g.ActiveIdUsingNavInputMask & (1 << input)) != 0; }
    inline bool             IsActiveIdUsingKey(MsTrGKey key)                            { MsTrGContext& g = *GMsTrG; return g.ActiveIdUsingKeyInputMask[key]; }
    inline void             SetActiveIdUsingKey(MsTrGKey key)                           { MsTrGContext& g = *GMsTrG; g.ActiveIdUsingKeyInputMask.SetBit(key); }
    MsTrG_API bool          IsMouseDragPastThreshold(MsTrGMouseButton button, float lock_threshold = -1.0f);
    inline bool             IsNavInputDown(MsTrGNavInput n)                             { MsTrGContext& g = *GMsTrG; return g.IO.NavInputs[n] > 0.0f; }
    inline bool             IsNavInputTest(MsTrGNavInput n, MsTrGNavReadMode rm)        { return (GetNavInputAmount(n, rm) > 0.0f); }
    MsTrG_API MsTrGModFlags GetMergedModFlags();
#ifndef MsTrG_DISABLE_OBSOLETE_KEYIO
    inline bool             IsKeyPressedMap(MsTrGKey key, bool repeat = true)           { IM_ASSERT(IsNamedKey(key)); return IsKeyPressed(key, repeat); } // [removed in 1.87]
#endif

    // Drag and Drop
    MsTrG_API bool          IsDragDropActive();
    MsTrG_API bool          BeginDragDropTargetCustom(const ImRect& bb, MsTrGID id);
    MsTrG_API void          ClearDragDrop();
    MsTrG_API bool          IsDragDropPayloadBeingAccepted();

    // Internal Columns API (this is not exposed because we will encourage transitioning to the Tables API)
    MsTrG_API void          SetWindowClipRectBeforeSetChannel(MsTrGWindow* window, const ImRect& clip_rect);
    MsTrG_API void          BeginColumns(const char* str_id, int count, MsTrGOldColumnFlags flags = 0); // setup number of columns. use an identifier to distinguish multiple column sets. close with EndColumns().
    MsTrG_API void          EndColumns();                                                               // close columns
    MsTrG_API void          PushColumnClipRect(int column_index);
    MsTrG_API void          PushColumnsBackground();
    MsTrG_API void          PopColumnsBackground();
    MsTrG_API MsTrGID       GetColumnsID(const char* str_id, int count);
    MsTrG_API MsTrGOldColumns* FindOrCreateColumns(MsTrGWindow* window, MsTrGID id);
    MsTrG_API float         GetColumnOffsetFromNorm(const MsTrGOldColumns* columns, float offset_norm);
    MsTrG_API float         GetColumnNormFromOffset(const MsTrGOldColumns* columns, float offset);

    // Tables: Candidates for public API
    MsTrG_API void          TableOpenContextMenu(int column_n = -1);
    MsTrG_API void          TableSetColumnWidth(int column_n, float width);
    MsTrG_API void          TableSetColumnSortDirection(int column_n, MsTrGSortDirection sort_direction, bool append_to_sort_specs);
    MsTrG_API int           TableGetHoveredColumn(); // May use (TableGetColumnFlags() & MsTrGTableColumnFlags_IsHovered) instead. Return hovered column. return -1 when table is not hovered. return columns_count if the unused space at the right of visible columns is hovered.
    MsTrG_API float         TableGetHeaderRowHeight();
    MsTrG_API void          TablePushBackgroundChannel();
    MsTrG_API void          TablePopBackgroundChannel();

    // Tables: Internals
    inline    MsTrGTable*   GetCurrentTable() { MsTrGContext& g = *GMsTrG; return g.CurrentTable; }
    MsTrG_API MsTrGTable*   TableFindByID(MsTrGID id);
    MsTrG_API bool          BeginTableEx(const char* name, MsTrGID id, int columns_count, MsTrGTableFlags flags = 0, const ImVec2& outer_size = ImVec2(0, 0), float inner_width = 0.0f);
    MsTrG_API void          TableBeginInitMemory(MsTrGTable* table, int columns_count);
    MsTrG_API void          TableBeginApplyRequests(MsTrGTable* table);
    MsTrG_API void          TableSetupDrawChannels(MsTrGTable* table);
    MsTrG_API void          TableUpdateLayout(MsTrGTable* table);
    MsTrG_API void          TableUpdateBorders(MsTrGTable* table);
    MsTrG_API void          TableUpdateColumnsWeightFromWidth(MsTrGTable* table);
    MsTrG_API void          TableDrawBorders(MsTrGTable* table);
    MsTrG_API void          TableDrawContextMenu(MsTrGTable* table);
    MsTrG_API void          TableMergeDrawChannels(MsTrGTable* table);
    inline MsTrGTableInstanceData*   TableGetInstanceData(MsTrGTable* table, int instance_no) { if (instance_no == 0) return &table->InstanceDataFirst; return &table->InstanceDataExtra[instance_no - 1]; }
    MsTrG_API void          TableSortSpecsSanitize(MsTrGTable* table);
    MsTrG_API void          TableSortSpecsBuild(MsTrGTable* table);
    MsTrG_API MsTrGSortDirection TableGetColumnNextSortDirection(MsTrGTableColumn* column);
    MsTrG_API void          TableFixColumnSortDirection(MsTrGTable* table, MsTrGTableColumn* column);
    MsTrG_API float         TableGetColumnWidthAuto(MsTrGTable* table, MsTrGTableColumn* column);
    MsTrG_API void          TableBeginRow(MsTrGTable* table);
    MsTrG_API void          TableEndRow(MsTrGTable* table);
    MsTrG_API void          TableBeginCell(MsTrGTable* table, int column_n);
    MsTrG_API void          TableEndCell(MsTrGTable* table);
    MsTrG_API ImRect        TableGetCellBgRect(const MsTrGTable* table, int column_n);
    MsTrG_API const char*   TableGetColumnName(const MsTrGTable* table, int column_n);
    MsTrG_API MsTrGID       TableGetColumnResizeID(const MsTrGTable* table, int column_n, int instance_no = 0);
    MsTrG_API float         TableGetMaxColumnWidth(const MsTrGTable* table, int column_n);
    MsTrG_API void          TableSetColumnWidthAutoSingle(MsTrGTable* table, int column_n);
    MsTrG_API void          TableSetColumnWidthAutoAll(MsTrGTable* table);
    MsTrG_API void          TableRemove(MsTrGTable* table);
    MsTrG_API void          TableGcCompactTransientBuffers(MsTrGTable* table);
    MsTrG_API void          TableGcCompactTransientBuffers(MsTrGTableTempData* table);
    MsTrG_API void          TableGcCompactSettings();

    // Tables: Settings
    MsTrG_API void                  TableLoadSettings(MsTrGTable* table);
    MsTrG_API void                  TableSaveSettings(MsTrGTable* table);
    MsTrG_API void                  TableResetSettings(MsTrGTable* table);
    MsTrG_API MsTrGTableSettings*   TableGetBoundSettings(MsTrGTable* table);
    MsTrG_API void                  TableSettingsAddSettingsHandler();
    MsTrG_API MsTrGTableSettings*   TableSettingsCreate(MsTrGID id, int columns_count);
    MsTrG_API MsTrGTableSettings*   TableSettingsFindByID(MsTrGID id);

    // Tab Bars
    MsTrG_API bool          BeginTabBarEx(MsTrGTabBar* tab_bar, const ImRect& bb, MsTrGTabBarFlags flags);
    MsTrG_API MsTrGTabItem* TabBarFindTabByID(MsTrGTabBar* tab_bar, MsTrGID tab_id);
    MsTrG_API void          TabBarRemoveTab(MsTrGTabBar* tab_bar, MsTrGID tab_id);
    MsTrG_API void          TabBarCloseTab(MsTrGTabBar* tab_bar, MsTrGTabItem* tab);
    MsTrG_API void          TabBarQueueReorder(MsTrGTabBar* tab_bar, const MsTrGTabItem* tab, int offset);
    MsTrG_API void          TabBarQueueReorderFromMousePos(MsTrGTabBar* tab_bar, const MsTrGTabItem* tab, ImVec2 mouse_pos);
    MsTrG_API bool          TabBarProcessReorder(MsTrGTabBar* tab_bar);
    MsTrG_API bool          TabItemEx(MsTrGTabBar* tab_bar, const char* label, bool* p_open, MsTrGTabItemFlags flags);
    MsTrG_API ImVec2        TabItemCalcSize(const char* label, bool has_close_button);
    MsTrG_API void          TabItemBackground(ImDrawList* draw_list, const ImRect& bb, MsTrGTabItemFlags flags, ImU32 col);
    MsTrG_API void          TabItemLabelAndCloseButton(ImDrawList* draw_list, const ImRect& bb, MsTrGTabItemFlags flags, ImVec2 frame_padding, const char* label, MsTrGID tab_id, MsTrGID close_button_id, bool is_contents_visible, bool* out_just_closed, bool* out_text_clipped);

    // Render helpers
    // AVOID USING OUTSIDE OF MsTrG.CPP! NOT FOR PUBLIC CONSUMPTION. THOSE FUNCTIONS ARE A MESS. THEIR SIGNATURE AND BEHAVIOR WILL CHANGE, THEY NEED TO BE REFACTORED INTO SOMETHING DECENT.
    // NB: All position are in absolute pixels coordinates (we are never using window coordinates internally)
    MsTrG_API void          RenderText(ImVec2 pos, const char* text, const char* text_end = NULL, bool hide_text_after_hash = true);
    MsTrG_API void          RenderTextWrapped(ImVec2 pos, const char* text, const char* text_end, float wrap_width);
    MsTrG_API void          RenderTextClipped(const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_end, const ImVec2* text_size_if_known, const ImVec2& align = ImVec2(0, 0), const ImRect* clip_rect = NULL);
    MsTrG_API void          RenderTextClippedEx(ImDrawList* draw_list, const ImVec2& pos_min, const ImVec2& pos_max, const char* text, const char* text_end, const ImVec2* text_size_if_known, const ImVec2& align = ImVec2(0, 0), const ImRect* clip_rect = NULL);
    MsTrG_API void          RenderTextEllipsis(ImDrawList* draw_list, const ImVec2& pos_min, const ImVec2& pos_max, float clip_max_x, float ellipsis_max_x, const char* text, const char* text_end, const ImVec2* text_size_if_known);
    MsTrG_API void          RenderFrame(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border = true, float rounding = 0.0f);
    MsTrG_API void          RenderFrameBorder(ImVec2 p_min, ImVec2 p_max, float rounding = 0.0f);
    MsTrG_API void          RenderColorRectWithAlphaCheckerboard(ImDrawList* draw_list, ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, float grid_step, ImVec2 grid_off, float rounding = 0.0f, ImDrawFlags flags = 0);
    MsTrG_API void          RenderNavHighlight(const ImRect& bb, MsTrGID id, MsTrGNavHighlightFlags flags = MsTrGNavHighlightFlags_TypeDefault); // Navigation highlight
    MsTrG_API const char*   FindRenderedTextEnd(const char* text, const char* text_end = NULL); // Find the optional ## from which we stop displaying text.
    MsTrG_API void          RenderMouseCursor(ImVec2 pos, float scale, MsTrGMouseCursor mouse_cursor, ImU32 col_fill, ImU32 col_border, ImU32 col_shadow);

    // Render helpers (those functions don't access any MsTrG state!)
    MsTrG_API void          RenderArrow(ImDrawList* draw_list, ImVec2 pos, ImU32 col, MsTrGDir dir, float scale = 1.0f);
    MsTrG_API void          RenderBullet(ImDrawList* draw_list, ImVec2 pos, ImU32 col);
    MsTrG_API void          RenderCheckMark(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float sz);
    MsTrG_API void          RenderArrowPointingAt(ImDrawList* draw_list, ImVec2 pos, ImVec2 half_sz, MsTrGDir direction, ImU32 col);
    MsTrG_API void          RenderRectFilledRangeH(ImDrawList* draw_list, const ImRect& rect, ImU32 col, float x_start_norm, float x_end_norm, float rounding);
    MsTrG_API void          RenderRectFilledWithHole(ImDrawList* draw_list, const ImRect& outer, const ImRect& inner, ImU32 col, float rounding);

    // Widgets
    MsTrG_API void          TextEx(const char* text, const char* text_end = NULL, MsTrGTextFlags flags = 0);
    MsTrG_API bool          ButtonEx(const char* label, const ImVec2& size_arg = ImVec2(0, 0), MsTrGButtonFlags flags = 0);
    MsTrG_API bool          CloseButton(MsTrGID id, const ImVec2& pos);
    MsTrG_API bool          CollapseButton(MsTrGID id, const ImVec2& pos);
    MsTrG_API bool          ArrowButtonEx(const char* str_id, MsTrGDir dir, ImVec2 size_arg, MsTrGButtonFlags flags = 0);
    MsTrG_API void          Scrollbar(MsTrGAxis axis);
    MsTrG_API bool          ScrollbarEx(const ImRect& bb, MsTrGID id, MsTrGAxis axis, ImS64* p_scroll_v, ImS64 avail_v, ImS64 contents_v, ImDrawFlags flags);
    MsTrG_API bool          ImageButtonEx(MsTrGID id, ImTextureID texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec2& padding, const ImVec4& bg_col, const ImVec4& tint_col);
    MsTrG_API ImRect        GetWindowScrollbarRect(MsTrGWindow* window, MsTrGAxis axis);
    MsTrG_API MsTrGID       GetWindowScrollbarID(MsTrGWindow* window, MsTrGAxis axis);
    MsTrG_API MsTrGID       GetWindowResizeCornerID(MsTrGWindow* window, int n); // 0..3: corners
    MsTrG_API MsTrGID       GetWindowResizeBorderID(MsTrGWindow* window, MsTrGDir dir);
    MsTrG_API void          SeparatorEx(MsTrGSeparatorFlags flags);
    MsTrG_API bool          CheckboxFlags(const char* label, ImS64* flags, ImS64 flags_value);
    MsTrG_API bool          CheckboxFlags(const char* label, ImU64* flags, ImU64 flags_value);

    // Widgets low-level behaviors
    MsTrG_API bool          ButtonBehavior(const ImRect& bb, MsTrGID id, bool* out_hovered, bool* out_held, MsTrGButtonFlags flags = 0);
    MsTrG_API bool          DragBehavior(MsTrGID id, MsTrGDataType data_type, void* p_v, float v_speed, const void* p_min, const void* p_max, const char* format, MsTrGSliderFlags flags);
    MsTrG_API bool          SliderBehavior(const ImRect& bb, MsTrGID id, MsTrGDataType data_type, void* p_v, const void* p_min, const void* p_max, const char* format, MsTrGSliderFlags flags, ImRect* out_grab_bb);
    MsTrG_API bool          SplitterBehavior(const ImRect& bb, MsTrGID id, MsTrGAxis axis, float* size1, float* size2, float min_size1, float min_size2, float hover_extend = 0.0f, float hover_visibility_delay = 0.0f);
    MsTrG_API bool          TreeNodeBehavior(MsTrGID id, MsTrGTreeNodeFlags flags, const char* label, const char* label_end = NULL);
    MsTrG_API bool          TreeNodeBehaviorIsOpen(MsTrGID id, MsTrGTreeNodeFlags flags = 0);                     // Consume previous SetNextItemOpen() data, if any. May return true when logging
    MsTrG_API void          TreePushOverrideID(MsTrGID id);

    // Template functions are instantiated in MsTrG_widgets.cpp for a finite number of types.
    // To use them externally (for custom widget) you may need an "extern template" statement in your code in order to link to existing instances and silence Clang warnings (see #2036).
    // e.g. " extern template MsTrG_API float RoundScalarWithFormatT<float, float>(const char* format, MsTrGDataType data_type, float v); "
    template<typename T, typename SIGNED_T, typename FLOAT_T>   MsTrG_API float ScaleRatioFromValueT(MsTrGDataType data_type, T v, T v_min, T v_max, bool is_logarithmic, float logarithmic_zero_epsilon, float zero_deadzone_size);
    template<typename T, typename SIGNED_T, typename FLOAT_T>   MsTrG_API T     ScaleValueFromRatioT(MsTrGDataType data_type, float t, T v_min, T v_max, bool is_logarithmic, float logarithmic_zero_epsilon, float zero_deadzone_size);
    template<typename T, typename SIGNED_T, typename FLOAT_T>   MsTrG_API bool  DragBehaviorT(MsTrGDataType data_type, T* v, float v_speed, T v_min, T v_max, const char* format, MsTrGSliderFlags flags);
    template<typename T, typename SIGNED_T, typename FLOAT_T>   MsTrG_API bool  SliderBehaviorT(const ImRect& bb, MsTrGID id, MsTrGDataType data_type, T* v, T v_min, T v_max, const char* format, MsTrGSliderFlags flags, ImRect* out_grab_bb);
    template<typename T>                                        MsTrG_API T     RoundScalarWithFormatT(const char* format, MsTrGDataType data_type, T v);
    template<typename T>                                        MsTrG_API bool  CheckboxFlagsT(const char* label, T* flags, T flags_value);

    // Data type helpers
    MsTrG_API const MsTrGDataTypeInfo*  DataTypeGetInfo(MsTrGDataType data_type);
    MsTrG_API int           DataTypeFormatString(char* buf, int buf_size, MsTrGDataType data_type, const void* p_data, const char* format);
    MsTrG_API void          DataTypeApplyOp(MsTrGDataType data_type, int op, void* output, const void* arg_1, const void* arg_2);
    MsTrG_API bool          DataTypeApplyFromText(const char* buf, MsTrGDataType data_type, void* p_data, const char* format);
    MsTrG_API int           DataTypeCompare(MsTrGDataType data_type, const void* arg_1, const void* arg_2);
    MsTrG_API bool          DataTypeClamp(MsTrGDataType data_type, void* p_data, const void* p_min, const void* p_max);

    // InputText
    MsTrG_API bool          InputTextEx(const char* label, const char* hint, char* buf, int buf_size, const ImVec2& size_arg, MsTrGInputTextFlags flags, MsTrGInputTextCallback callback = NULL, void* user_data = NULL);
    MsTrG_API bool          TempInputText(const ImRect& bb, MsTrGID id, const char* label, char* buf, int buf_size, MsTrGInputTextFlags flags);
    MsTrG_API bool          TempInputScalar(const ImRect& bb, MsTrGID id, const char* label, MsTrGDataType data_type, void* p_data, const char* format, const void* p_clamp_min = NULL, const void* p_clamp_max = NULL);
    inline bool             TempInputIsActive(MsTrGID id)       { MsTrGContext& g = *GMsTrG; return (g.ActiveId == id && g.TempInputId == id); }
    inline MsTrGInputTextState* GetInputTextState(MsTrGID id)   { MsTrGContext& g = *GMsTrG; return (g.InputTextState.ID == id) ? &g.InputTextState : NULL; } // Get input text state if active

    // Color
    MsTrG_API void          ColorTooltip(const char* text, const float* col, MsTrGColorEditFlags flags);
    MsTrG_API void          ColorEditOptionsPopup(const float* col, MsTrGColorEditFlags flags);
    MsTrG_API void          ColorPickerOptionsPopup(const float* ref_col, MsTrGColorEditFlags flags);

    // Plot
    MsTrG_API int           PlotEx(MsTrGPlotType plot_type, const char* label, float (*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 frame_size);

    // Shade functions (write over already created vertices)
    MsTrG_API void          ShadeVertsLinearColorGradientKeepAlpha(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, ImVec2 gradient_p0, ImVec2 gradient_p1, ImU32 col0, ImU32 col1);
    MsTrG_API void          ShadeVertsLinearUV(ImDrawList* draw_list, int vert_start_idx, int vert_end_idx, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, bool clamp);

    // Garbage collection
    MsTrG_API void          GcCompactTransientMiscBuffers();
    MsTrG_API void          GcCompactTransientWindowBuffers(MsTrGWindow* window);
    MsTrG_API void          GcAwakeTransientWindowBuffers(MsTrGWindow* window);

    // Debug Log
    MsTrG_API void          DebugLog(const char* fmt, ...) IM_FMTARGS(1);
    MsTrG_API void          DebugLogV(const char* fmt, va_list args) IM_FMTLIST(1);
    
    // Debug Tools
    MsTrG_API void          ErrorCheckEndFrameRecover(MsTrGErrorLogCallback log_callback, void* user_data = NULL);
    MsTrG_API void          ErrorCheckEndWindowRecover(MsTrGErrorLogCallback log_callback, void* user_data = NULL);
    inline void             DebugDrawItemRect(ImU32 col = IM_COL32(255,0,0,255))    { MsTrGContext& g = *GMsTrG; MsTrGWindow* window = g.CurrentWindow; GetForegroundDrawList(window)->AddRect(g.LastItemData.Rect.Min, g.LastItemData.Rect.Max, col); }
    inline void             DebugStartItemPicker()                                  { MsTrGContext& g = *GMsTrG; g.DebugItemPickerActive = true; }
    MsTrG_API void          ShowFontAtlas(ImFontAtlas* atlas);
    MsTrG_API void          DebugHookIdInfo(MsTrGID id, MsTrGDataType data_type, const void* data_id, const void* data_id_end);
    MsTrG_API void          DebugNodeColumns(MsTrGOldColumns* columns);
    MsTrG_API void          DebugNodeDrawList(MsTrGWindow* window, const ImDrawList* draw_list, const char* label);
    MsTrG_API void          DebugNodeDrawCmdShowMeshAndBoundingBox(ImDrawList* out_draw_list, const ImDrawList* draw_list, const ImDrawCmd* draw_cmd, bool show_mesh, bool show_aabb);
    MsTrG_API void          DebugNodeFont(ImFont* font);
    MsTrG_API void          DebugNodeFontGlyph(ImFont* font, const ImFontGlyph* glyph);
    MsTrG_API void          DebugNodeStorage(MsTrGStorage* storage, const char* label);
    MsTrG_API void          DebugNodeTabBar(MsTrGTabBar* tab_bar, const char* label);
    MsTrG_API void          DebugNodeTable(MsTrGTable* table);
    MsTrG_API void          DebugNodeTableSettings(MsTrGTableSettings* settings);
    MsTrG_API void          DebugNodeInputTextState(MsTrGInputTextState* state);
    MsTrG_API void          DebugNodeWindow(MsTrGWindow* window, const char* label);
    MsTrG_API void          DebugNodeWindowSettings(MsTrGWindowSettings* settings);
    MsTrG_API void          DebugNodeWindowsList(ImVector<MsTrGWindow*>* windows, const char* label);
    MsTrG_API void          DebugNodeWindowsListByBeginStackParent(MsTrGWindow** windows, int windows_size, MsTrGWindow* parent_in_begin_stack);
    MsTrG_API void          DebugNodeViewport(MsTrGViewportP* viewport);
    MsTrG_API void          DebugRenderViewportThumbnail(ImDrawList* draw_list, MsTrGViewportP* viewport, const ImRect& bb);

} // namespace MsTrG


//-----------------------------------------------------------------------------
// [SECTION] ImFontAtlas internal API
//-----------------------------------------------------------------------------

// This structure is likely to evolve as we add support for incremental atlas updates
struct ImFontBuilderIO
{
    bool    (*FontBuilder_Build)(ImFontAtlas* atlas);
};

// Helper for font builder
#ifdef MsTrG_ENABLE_STB_TRUETYPE
MsTrG_API const ImFontBuilderIO* ImFontAtlasGetBuilderForStbTruetype();
#endif
MsTrG_API void      ImFontAtlasBuildInit(ImFontAtlas* atlas);
MsTrG_API void      ImFontAtlasBuildSetupFont(ImFontAtlas* atlas, ImFont* font, ImFontConfig* font_config, float ascent, float descent);
MsTrG_API void      ImFontAtlasBuildPackCustomRects(ImFontAtlas* atlas, void* stbrp_context_opaque);
MsTrG_API void      ImFontAtlasBuildFinish(ImFontAtlas* atlas);
MsTrG_API void      ImFontAtlasBuildRender8bppRectFromString(ImFontAtlas* atlas, int x, int y, int w, int h, const char* in_str, char in_marker_char, unsigned char in_marker_pixel_value);
MsTrG_API void      ImFontAtlasBuildRender32bppRectFromString(ImFontAtlas* atlas, int x, int y, int w, int h, const char* in_str, char in_marker_char, unsigned int in_marker_pixel_value);
MsTrG_API void      ImFontAtlasBuildMultiplyCalcLookupTable(unsigned char out_table[256], float in_multiply_factor);
MsTrG_API void      ImFontAtlasBuildMultiplyRectAlpha8(const unsigned char table[256], unsigned char* pixels, int x, int y, int w, int h, int stride);

//-----------------------------------------------------------------------------
// [SECTION] Test Engine specific hooks (MsTrG_test_engine)
//-----------------------------------------------------------------------------

#ifdef MsTrG_ENABLE_TEST_ENGINE
extern void         MsTrGTestEngineHook_ItemAdd(MsTrGContext* ctx, const ImRect& bb, MsTrGID id);
extern void         MsTrGTestEngineHook_ItemInfo(MsTrGContext* ctx, MsTrGID id, const char* label, MsTrGItemStatusFlags flags);
extern void         MsTrGTestEngineHook_Log(MsTrGContext* ctx, const char* fmt, ...);
extern const char*  MsTrGTestEngine_FindItemDebugLabel(MsTrGContext* ctx, MsTrGID id);

#define MsTrG_TEST_ENGINE_ITEM_ADD(_BB,_ID)                 if (g.TestEngineHookItems) MsTrGTestEngineHook_ItemAdd(&g, _BB, _ID)               // Register item bounding box
#define MsTrG_TEST_ENGINE_ITEM_INFO(_ID,_LABEL,_FLAGS)      if (g.TestEngineHookItems) MsTrGTestEngineHook_ItemInfo(&g, _ID, _LABEL, _FLAGS)   // Register item label and status flags (optional)
#define MsTrG_TEST_ENGINE_LOG(_FMT,...)                     if (g.TestEngineHookItems) MsTrGTestEngineHook_Log(&g, _FMT, __VA_ARGS__)          // Custom log entry from user land into test log
#else
#define MsTrG_TEST_ENGINE_ITEM_ADD(_BB,_ID)                 ((void)0)
#define MsTrG_TEST_ENGINE_ITEM_INFO(_ID,_LABEL,_FLAGS)      ((void)g)
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

#endif // #ifndef MsTrG_DISABLE
