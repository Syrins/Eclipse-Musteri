// dear MsTrG: Renderer Backend for DirectX9
// This needs to be used along with a Platform Backend (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'LPDIRECT3DTEXTURE9' as ImTextureID. Read the FAQ about ImTextureID!
//  [X] Renderer: Support for large meshes (64k+ vertices) with 16-bit indices.

// You can use unmodified MsTrG_impl_* files in your project. See examples/ folder for examples of using this. 
// Prefer including the entire MsTrG/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// If you are new to Dear MsTrG, read documentation from the docs/ folder + read the top of MsTrG.cpp.
// Read online: https://github.com/ocornut/MsTrG/tree/master/docs

#pragma once
#include "imgui.h"      // MsTrG_IMPL_API

struct IDirect3DDevice9;

MsTrG_IMPL_API bool     MsTrG_ImplDX9_Init(IDirect3DDevice9* device);
MsTrG_IMPL_API void     MsTrG_ImplDX9_Shutdown();
MsTrG_IMPL_API void     MsTrG_ImplDX9_NewFrame();
MsTrG_IMPL_API void     MsTrG_ImplDX9_RenderDrawData(ImDrawData* draw_data);

// Use if you want to reset your rendering device without losing Dear MsTrG state.
MsTrG_IMPL_API bool     MsTrG_ImplDX9_CreateDeviceObjects();
MsTrG_IMPL_API void     MsTrG_ImplDX9_InvalidateDeviceObjects();
