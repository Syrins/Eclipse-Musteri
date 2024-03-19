#pragma once

#include <cstdint>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <map>

#include "../emgui/imgui.h"
#include "../emgui/imgui_internal.h"

namespace custom_interface {
    bool tab(const char* label, bool selected);
    bool subtab(const char* label, bool selected);
}
