//-----------------------------------------------------------------------------
// COMPILE-TIME OPTIONS FOR DEAR MsTrG
// Runtime options (clipboard callbacks, enabling various features, etc.) can generally be set via the MsTrGIO structure.
// You can use MsTrG::SetAllocatorFunctions() before calling MsTrG::CreateContext() to rewire memory allocation functions.
//-----------------------------------------------------------------------------
// A) You may edit imconfig.h (and not overwrite it when updating Dear MsTrG, or maintain a patch/rebased branch with your modifications to it)
// B) or '#define MsTrG_USER_CONFIG "my_MsTrG_config.h"' in your project and then add directives in your own file without touching this template.
//-----------------------------------------------------------------------------
// You need to make sure that configuration settings are defined consistently _everywhere_ Dear MsTrG is used, which include the MsTrG*.cpp
// files but also _any_ of your code that uses Dear MsTrG. This is because some compile-time options have an affect on data structures.
// Defining those options in imconfig.h will ensure every compilation unit gets to see the same data structure layouts.
// Call MsTrG_CHECKVERSION() from your .cpp files to verify that the data structures your files are using are matching the ones MsTrG.cpp is using.
//-----------------------------------------------------------------------------

#pragma once

//---- Define assertion handler. Defaults to calling assert().
// If your macro uses multiple statements, make sure is enclosed in a 'do { .. } while (0)' block so it can be used as a single statement.
//#define IM_ASSERT(_EXPR)  MyAssert(_EXPR)
//#define IM_ASSERT(_EXPR)  ((void)(_EXPR))     // Disable asserts

//---- Define attributes of all API symbols declarations, e.g. for DLL under Windows
// Using Dear MsTrG via a shared library is not recommended, because of function call overhead and because we don't guarantee backward nor forward ABI compatibility.
// DLL users: heaps and globals are not shared across DLL boundaries! You will need to call SetCurrentContext() + SetAllocatorFunctions()
// for each static/DLL boundary you are calling from. Read "Context and Memory Allocators" section of MsTrG.cpp for more details.
//#define MsTrG_API __declspec( dllexport )
//#define MsTrG_API __declspec( dllimport )

//---- Don't define obsolete functions/enums/behaviors. Consider enabling from time to time after updating to avoid using soon-to-be obsolete function/names.
//#define MsTrG_DISABLE_OBSOLETE_FUNCTIONS
//#define MsTrG_DISABLE_OBSOLETE_KEYIO                      // 1.87: disable legacy io.KeyMap[]+io.KeysDown[] in favor io.AddKeyEvent(). This will be folded into MsTrG_DISABLE_OBSOLETE_FUNCTIONS in a few versions.

//---- Disable all of Dear MsTrG or don't implement standard windows/tools.
// It is very strongly recommended to NOT disable the demo windows and debug tool during development. They are extremely useful in day to day work. Please read comments in MsTrG_demo.cpp.
//#define MsTrG_DISABLE                                     // Disable everything: all headers and source files will be empty.
//#define MsTrG_DISABLE_DEMO_WINDOWS                        // Disable demo windows: ShowDemoWindow()/ShowStyleEditor() will be empty.
//#define MsTrG_DISABLE_DEBUG_TOOLS                         // Disable metrics/debugger and other debug tools: ShowMetricsWindow(), ShowDebugLogWindow() and ShowStackToolWindow() will be empty (this was called MsTrG_DISABLE_METRICS_WINDOW before 1.88).

//---- Don't implement some functions to reduce linkage requirements.
//#define MsTrG_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS   // [Win32] Don't implement default clipboard handler. Won't use and link with OpenClipboard/GetClipboardData/CloseClipboard etc. (user32.lib/.a, kernel32.lib/.a)
//#define MsTrG_ENABLE_WIN32_DEFAULT_IME_FUNCTIONS          // [Win32] [Default with Visual Studio] Implement default IME handler (require imm32.lib/.a, auto-link for Visual Studio, -limm32 on command-line for MinGW)
//#define MsTrG_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS         // [Win32] [Default with non-Visual Studio compilers] Don't implement default IME handler (won't require imm32.lib/.a)
//#define MsTrG_DISABLE_WIN32_FUNCTIONS                     // [Win32] Won't use and link with any Win32 function (clipboard, ime).
//#define MsTrG_ENABLE_OSX_DEFAULT_CLIPBOARD_FUNCTIONS      // [OSX] Implement default OSX clipboard handler (need to link with '-framework ApplicationServices', this is why this is not the default).
//#define MsTrG_DISABLE_DEFAULT_FORMAT_FUNCTIONS            // Don't implement ImFormatString/ImFormatStringV so you can implement them yourself (e.g. if you don't want to link with vsnprintf)
//#define MsTrG_DISABLE_DEFAULT_MATH_FUNCTIONS              // Don't implement ImFabs/ImSqrt/ImPow/ImFmod/ImCos/ImSin/ImAcos/ImAtan2 so you can implement them yourself.
//#define MsTrG_DISABLE_FILE_FUNCTIONS                      // Don't implement ImFileOpen/ImFileClose/ImFileRead/ImFileWrite and ImFileHandle at all (replace them with dummies)
//#define MsTrG_DISABLE_DEFAULT_FILE_FUNCTIONS              // Don't implement ImFileOpen/ImFileClose/ImFileRead/ImFileWrite and ImFileHandle so you can implement them yourself if you don't want to link with fopen/fclose/fread/fwrite. This will also disable the LogToTTY() function.
//#define MsTrG_DISABLE_DEFAULT_ALLOCATORS                  // Don't implement default allocators calling malloc()/free() to avoid linking with them. You will need to call MsTrG::SetAllocatorFunctions().
//#define MsTrG_DISABLE_SSE                                 // Disable use of SSE intrinsics even if available

//---- Include MsTrG_user.h at the end of MsTrG.h as a convenience
//#define MsTrG_INCLUDE_MsTrG_USER_H

//---- Pack colors to BGRA8 instead of RGBA8 (to avoid converting from one to another)
//#define MsTrG_USE_BGRA_PACKED_COLOR

//---- Use 32-bit for ImWchar (default is 16-bit) to support unicode planes 1-16. (e.g. point beyond 0xFFFF like emoticons, dingbats, symbols, shapes, ancient languages, etc...)
//#define MsTrG_USE_WCHAR32

//---- Avoid multiple STB libraries implementations, or redefine path/filenames to prioritize another version
// By default the embedded implementations are declared static and not available outside of Dear MsTrG sources files.
//#define MsTrG_STB_TRUETYPE_FILENAME   "my_folder/stb_truetype.h"
//#define MsTrG_STB_RECT_PACK_FILENAME  "my_folder/stb_rect_pack.h"
//#define MsTrG_STB_SPRINTF_FILENAME    "my_folder/stb_sprintf.h"    // only used if enabled
//#define MsTrG_DISABLE_STB_TRUETYPE_IMPLEMENTATION
//#define MsTrG_DISABLE_STB_RECT_PACK_IMPLEMENTATION

//---- Use stb_sprintf.h for a faster implementation of vsnprintf instead of the one from libc (unless MsTrG_DISABLE_DEFAULT_FORMAT_FUNCTIONS is defined)
// Compatibility checks of arguments and formats done by clang and GCC will be disabled in order to support the extra formats provided by stb_sprintf.h.
//#define MsTrG_USE_STB_SPRINTF

//---- Use FreeType to build and rasterize the font atlas (instead of stb_truetype which is embedded by default in Dear MsTrG)
// Requires FreeType headers to be available in the include path. Requires program to be compiled with 'misc/freetype/MsTrG_freetype.cpp' (in this repository) + the FreeType library (not provided).
// On Windows you may use vcpkg with 'vcpkg install freetype --triplet=x64-windows' + 'vcpkg integrate install'.
//#define MsTrG_ENABLE_FREETYPE

//---- Use stb_truetype to build and rasterize the font atlas (default)
// The only purpose of this define is if you want force compilation of the stb_truetype backend ALONG with the FreeType backend.
//#define MsTrG_ENABLE_STB_TRUETYPE

//---- Define constructor and implicit cast operators to convert back<>forth between your math types and ImVec2/ImVec4.
// This will be inlined as part of ImVec2 and ImVec4 class declarations.
/*
#define IM_VEC2_CLASS_EXTRA                                                     \
        constexpr ImVec2(const MyVec2& f) : x(f.x), y(f.y) {}                   \
        operator MyVec2() const { return MyVec2(x,y); }

#define IM_VEC4_CLASS_EXTRA                                                     \
        constexpr ImVec4(const MyVec4& f) : x(f.x), y(f.y), z(f.z), w(f.w) {}   \
        operator MyVec4() const { return MyVec4(x,y,z,w); }
*/

//---- Use 32-bit vertex indices (default is 16-bit) is one way to allow large meshes with more than 64K vertices.
// Your renderer backend will need to support it (most example renderer backends support both 16/32-bit indices).
// Another way to allow large meshes while keeping 16-bit indices is to handle ImDrawCmd::VtxOffset in your renderer.
// Read about MsTrGBackendFlags_RendererHasVtxOffset for details.
//#define ImDrawIdx unsigned int

//---- Override ImDrawCallback signature (will need to modify renderer backends accordingly)
//struct ImDrawList;
//struct ImDrawCmd;
//typedef void (*MyImDrawCallback)(const ImDrawList* draw_list, const ImDrawCmd* cmd, void* my_renderer_user_data);
//#define ImDrawCallback MyImDrawCallback

//---- Debug Tools: Macro to break in Debugger
// (use 'Metrics->Tools->Item Picker' to pick widgets with the mouse and break into them for easy debugging.)
//#define IM_DEBUG_BREAK  IM_ASSERT(0)
//#define IM_DEBUG_BREAK  __debugbreak()

//---- Debug Tools: Have the Item Picker break in the ItemAdd() function instead of ItemHoverable(),
// (which comes earlier in the code, will catch a few extra items, allow picking items other than Hovered one.)
// This adds a small runtime cost which is why it is not enabled by default.
//#define MsTrG_DEBUG_TOOL_ITEM_PICKER_EX

//---- Debug Tools: Enable slower asserts
//#define MsTrG_DEBUG_PARANOID

//---- Tip: You can add extra functions within the MsTrG:: namespace, here or in your own headers files.
/*
namespace MsTrG
{
    void MyFunction(const char* name, const MyMatrix44& v);
}
*/
