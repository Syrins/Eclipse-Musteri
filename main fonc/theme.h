#include "../main.h"
#include "../blabla/icons.h"
#include "../blabla/fonthelper.h"
#include "../sdk/user_interface.h"

int AimbotButon = 0x01;
int keystatus;
void ChangeKey()
{
	keystatus = 1;
	while (true)
	{
		for (int i = 0x01; i < 0x87; i++)
		{
			if (GetKeyState(i) & 0x8000)
			{
				AimbotButon = i;
				keystatus = 0;
				return;
			}
		}
	}
}
static const char* keyNames[] = {
	(("")),
	(("Left Mouse")),
	(("Right Mouse")),
	(("Cancel")),
	(("Middle Mouse")),
	(("Mouse 5")),
	(("Mouse 4")),
	(("")),
	(("Backspace")),
	(("Tab")),
	(("")),
	(("")),
	(("Clear")),
	(("Enter")),
	(("")),
	(("")),
	(("Shift")),
	(("Control")),
	(("Alt")),
	(("Pause")),
	(("Caps")),
	(("")),
	(("")),
	(("")),
	(("")),
	(("")),
	(("")),
	(("Escape")),
	(("")),
	(("")),
	(("")),
	(("")),
	(("Space")),
	(("Page Up")),
	(("Page Down")),
	(("End")),
	(("Home")),
	(("Arrow Left")),
	(("Arrow Up")),
	(("Arrow Right")),
	(("Arrow Down")),
	(("")),
	(("")),
	(("")),
	(("Print")),
	(("Insert")),
	(("Delete")),
	(("")),
	(("0")),
	(("1")),
	(("2")),
	(("3")),
	(("4")),
	(("5")),
	(("6")),
	(("7")),
	(("8")),
	(("9")),
	(("")),
	(("")),
	(("")),
	(("")),
	(("")),
	(("")),
	(("")),
	(("A")),
	(("B")),
	(("C")),
	(("D")),
	(("E")),
	(("F")),
	(("G")),
	(("H")),
	(("I")),
	(("J")),
	(("K")),
	(("L")),
	(("M")),
	(("N")),
	(("O")),
	(("P")),
	(("Q")),
	(("R")),
	(("S")),
	(("T")),
	(("U")),
	(("V")),
	(("W")),
	(("X")),
	(("Y")),
	(("Z")),
	(("")),
	(("")),
	(("")),
	(("")),
	(("")),
	(("Numpad 0")),
	(("Numpad 1")),
	(("Numpad 2")),
	(("Numpad 3")),
	(("Numpad 4")),
	(("Numpad 5")),
	(("Numpad 6")),
	(("Numpad 7")),
	(("Numpad 8")),
	(("Numpad 9")),
	(("Multiply")),
	(("Add")),
	(("")),
	(("Subtract")),
	(("Decimal")),
	(("Divide")),
	(("F1")),
	(("F2")),
	(("F3")),
	(("F4")),
	(("F5")),
	(("F6")),
	(("F7")),
	(("F8")),
	(("F9")),
	(("F10")),
	(("F11")),
	(("F12")),
};
static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
	const char* const* items = (const char* const*)data;
	if (out_text)
		*out_text = items[idx];
	return true;
}
void HotkeyButton(int aimkey, void* changekey, int status)
{
	const char* preview_value = NULL;
	if (aimkey >= 0 && aimkey < IM_ARRAYSIZE(keyNames))
		Items_ArrayGetter(keyNames, aimkey, &preview_value);

	std::string aimkeys;
	if (preview_value == NULL)
		aimkeys = _("bind");
	else
		aimkeys = preview_value;

	if (status == 1)
	{
		aimkeys = _("Press key to bind");
	}
	if (MsTrG::Button(aimkeys.c_str(), ImVec2(85, 25)))
	{
		if (status == 0)
		{
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)changekey, nullptr, 0, nullptr);
		}
	}
}
void stylemenu()
{
	ImVec4* colors = MsTrG::GetStyle().Colors;
	MsTrGIO& io = MsTrG::GetIO();

	// Arka planı siyah, renkleri turuncu tonlarına ayarla
	colors[MsTrGCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);  // Beyaz metin
	colors[MsTrGCol_TextDisabled] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);  // Gri metin
	colors[MsTrGCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);  // Siyah pencere arka plan
	colors[MsTrGCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.10f);  // Siyah child pencere arka plan
	colors[MsTrGCol_PopupBg] = ImVec4(1.00f, 0.50f, 0.00f, 1.00f);  // Turuncu popup arka plan
	colors[MsTrGCol_Border] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);  // Kırmızı kenarlık
	colors[MsTrGCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);  // Siyah kenarlık gölgesi
	colors[MsTrGCol_FrameBg] = ImVec4(0.90f, 0.50f, 0.00f, 0.54f);  // Turuncu çerçeve arka plan
	colors[MsTrGCol_FrameBgHovered] = ImVec4(1.00f, 0.00f, 0.00f, 0.54f);  // Kırmızı çerçeve arka planı
	colors[MsTrGCol_FrameBgActive] = ImVec4(1.00f, 0.22f, 0.20f, 1.00f);  // Kırmızı çerçeve arka planı (etkileşimli)
	colors[MsTrGCol_TitleBg] = ImVec4(0.90f, 0.50f, 0.00f, 1.00f);  // Turuncu başlık arka plan
	colors[MsTrGCol_TitleBgActive] = ImVec4(1.00f, 0.22f, 0.20f, 1.00f);  // Kırmızı başlık arka plan (etkileşimli)
	colors[MsTrGCol_TitleBgCollapsed] = ImVec4(0.90f, 0.50f, 0.00f, 1.00f);  // Turuncu başlık arka planı (daraltıldığında)
	colors[MsTrGCol_MenuBarBg] = ImVec4(0.90f, 0.50f, 0.00f, 1.00f);  // Turuncu menü bar arka plan
	colors[MsTrGCol_ScrollbarBg] = ImVec4(0.90f, 0.50f, 0.00f, 0.54f);  // Turuncu kaydırma çubuğu arka plan
	colors[MsTrGCol_ScrollbarGrab] = ImVec4(1.00f, 0.22f, 0.20f, 0.54f);  // Kırmızı kaydırma çubuğu
	colors[MsTrGCol_ScrollbarGrabHovered] = ImVec4(1.00f, 0.40f, 0.35f, 0.54f);  // Kırmızı kaydırma çubuğu (üzerine gelindiğinde)
	colors[MsTrGCol_ScrollbarGrabActive] = ImVec4(1.00f, 0.20f, 0.15f, 1.00f);  // Kırmızı kaydırma çubuğu (etkileşimli)
	colors[MsTrGCol_CheckMark] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);  // Kırmızı işaretleme işareti
	colors[MsTrGCol_SliderGrab] = ImVec4(1.00f, 0.00f, 0.00f, 0.54f);  // Kırmızı kaydırma çubuğu tutma alanı
	colors[MsTrGCol_SliderGrabActive] = ImVec4(1.00f, 0.22f, 0.20f, 1.00f);  // Kırmızı kaydırma çubuğu tutma alanı (etkileşimli)
	colors[MsTrGCol_Button] = ImVec4(1.00f, 0.00f, 0.00f, 0.54f);  // Kırmızı düğme
	colors[MsTrGCol_ButtonHovered] = ImVec4(1.00f, 0.00f, 0.00f, 0.74f);  // Kırmızı düğme (üzerine gelindiğinde)
	colors[MsTrGCol_ButtonActive] = ImVec4(1.00f, 0.22f, 0.20f, 1.00f);  // Kırmızı düğme (etkileşimli)
	colors[MsTrGCol_Header] = ImVec4(1.00f, 0.22f, 0.20f, 0.33f);  // Kırmızı başlık
	colors[MsTrGCol_HeaderHovered] = ImVec4(1.00f, 0.00f, 0.00f, 0.33f);  // Kırmızı başlık (üzerine gelindiğinde)
	colors[MsTrGCol_HeaderActive] = ImVec4(1.00f, 0.22f, 0.20f, 0.44f);  // Kırmızı başlık (etkileşimli)
	colors[MsTrGCol_Separator] = ImVec4(1.00f, 0.00f, 0.00f, 0.54f);  // Kırmızı ayırıcı çizgi
	colors[MsTrGCol_SeparatorHovered] = ImVec4(1.00f, 0.00f, 0.00f, 0.74f);  // Kırmızı ayırıcı çizgi (üzerine gelindiğinde)
	colors[MsTrGCol_SeparatorActive] = ImVec4(1.00f, 0.22f, 0.20f, 1.00f);  // Kırmızı ayırıcı çizgi (etkileşimli)
	colors[MsTrGCol_ResizeGrip] = ImVec4(1.00f, 0.22f, 0.20f, 0.44f);  // Kırmızı boyutlandırma tutamağı
	colors[MsTrGCol_ResizeGripHovered] = ImVec4(1.00f, 0.00f, 0.00f, 0.44f);  // Kırmızı boyutlandırma tutamağı (üzerine gelindiğinde)
	colors[MsTrGCol_ResizeGripActive] = ImVec4(1.00f, 0.20f, 0.15f, 1.00f);  // Kırmızı boyutlandırma tutamağı (etkileşimli)
	colors[MsTrGCol_Tab] = ImVec4(1.00f, 0.00f, 0.00f, 0.54f);  // Kırmızı sekme
	colors[MsTrGCol_TabHovered] = ImVec4(1.00f, 0.00f, 0.00f, 0.74f);  // Kırmızı sekme (üzerine gelindiğinde)
	colors[MsTrGCol_TabActive] = ImVec4(1.00f, 0.22f, 0.20f, 1.00f);  // Kırmızı sekme (etkileşimli)
	colors[MsTrGCol_TabUnfocused] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);  // Gri seçilmemiş sekme
	colors[MsTrGCol_TabUnfocusedActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);  // Gri seçilmemiş sekme (etkileşimli)
	colors[MsTrGCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);  // Kırmızı grafik çizgileri
	colors[MsTrGCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);  // Kırmızı grafik çizgileri (üzerine gelindiğinde)
	colors[MsTrGCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);  // Kırmızı grafik histogramı
	colors[MsTrGCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);  // Kırmızı grafik histogramı (üzerine gelindiğinde)
	colors[MsTrGCol_TextSelectedBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);  // Kırmızı metin seçim arka planı
	colors[MsTrGCol_DragDropTarget] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);  // Kırmızı sürükle ve bırak hedefi
	colors[MsTrGCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);  // Kırmızı gezinme vurgusu
	colors[MsTrGCol_NavWindowingHighlight] = ImVec4(1.00f, 0.50f, 0.00f, 0.70f);  // Turuncu pencereleme vurgusu
	colors[MsTrGCol_NavWindowingDimBg] = ImVec4(1.00f, 0.50f, 0.00f, 0.20f);  // Turuncu pencereleme arka planı
	colors[MsTrGCol_ModalWindowDimBg] = ImVec4(1.00f, 0.50f, 0.00f, 0.35f);  // Turuncu modal pencere arka planı

	MsTrGStyle& style = MsTrG::GetStyle();
	style.WindowPadding = ImVec2(8.00f, 8.00f);
	style.FramePadding = ImVec2(5.00f, 2.00f);
	style.ItemSpacing = ImVec2(6.00f, 6.00f);
	style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
	style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
	style.IndentSpacing = 25;
	style.ScrollbarSize = 15;
	style.GrabMinSize = 10;
	style.WindowBorderSize = 1;
	style.ChildBorderSize = 1;
	style.PopupBorderSize = 1;
	style.TabBorderSize = 1;
	style.WindowRounding = 7;
}


int sayfa = 1;

void RenderUI()
{
	ImVec2 windowSize = ImVec2{ 580, 330 };
	MsTrG::SetNextWindowSize(windowSize);

	POINT mousePos;
	GetCursorPos(&mousePos);

	static int selected = 0;
	static int subSelected = 0;

	MsTrG::Begin(_("#karibayani.store"), 0, MsTrGWindowFlags_NoResize | MsTrGWindowFlags_NoScrollbar | MsTrGWindowFlags_NoTitleBar);
	{
		MsTrG::AlignTextToFramePadding();
		auto windowPos = MsTrG::GetWindowPos();
		auto windowSize = MsTrG::GetWindowSize();
		auto isHovering = mousePos.x >= windowPos.x && mousePos.y >= windowPos.y &&
			mousePos.x <= (windowPos.x + windowSize.x) && mousePos.y <= (windowPos.y + windowSize.y);

		auto drawList = MsTrG::GetWindowDrawList();

		drawList->AddRectFilled(ImVec2(windowPos.x, windowPos.y), ImVec2(windowPos.x + windowSize.x, windowPos.y + 35), ImColor(20, 20, 20, 255)); // upper rect
		drawList->AddLine(ImVec2(windowPos.x, windowPos.y + 35), ImVec2(windowPos.x + windowSize.x, windowPos.y + 35), ImColor(255, 255, 255, 15)); // upper line

		MsTrG::SetCursorPosY(MsTrG::GetCursorPosY() + 290);
		MsTrG::SetCursorPosX(MsTrG::GetCursorPosX() - 20);
		MsTrG::BeginGroup();

		if (selected == 0) {
			MsTrG::BeginGroup();
			MsTrG::SetCursorPos(ImVec2(15, 14));
			if (custom_interface::subtab(_("VISUALS"), 0 == subSelected)) {
				subSelected = 0;
			}

			MsTrG::EndGroup();
			MsTrG::BeginGroup();
			MsTrG::SetCursorPos(ImVec2(350, 14));
			MsTrG::Text(_("#Version 1.1.1"));
			MsTrG::SameLine();
			MsTrG::SetCursorPosX(MsTrG::GetCursorPosX() - 7);
			MsTrG::EndGroup();

			MsTrG::SetCursorPos(ImVec2(15, 44));
			MsTrG::BeginChild(_("#karibayani"), ImVec2(240, 282));
			{
				if (subSelected == 0) {
					MsTrG::Checkbox(_("2D Box"), &config.player_box);
					MsTrG::Checkbox(_("Skeleton"), &config.player_skeleton);
					MsTrG::Checkbox(_("Head"), &config.headcircle);
					MsTrG::Checkbox(_("Distance"), &config.player_distance);
					MsTrG::Checkbox(_("Snapline"), &config.player_snapline);
					MsTrG::Checkbox(_("Agent Name"), &config.agentnames);
					MsTrG::Checkbox(_("Healthbar"), &config.player_healthbar);
					//MsTrG::Checkbox(_("Spike Timer"), &config.spiketimer);
					MsTrG::Checkbox(_("Radar"), &config.radar);
					MsTrG::Checkbox(_("Visible Check"), &config.visiblecheck);
					MsTrG::Checkbox(_("vSync Mode"), &vsync);
					MsTrG::Checkbox(_("No Lag"), &config.dormant);
				}
			}
			MsTrG::EndChild();

			MsTrG::SetCursorPos(ImVec2(262, 44));
			MsTrG::BeginChild(_("esp color"), ImVec2(240, 282));
			{
				if (subSelected == 0) {

					if (config.visiblecheck == true)
					{
						MsTrG::ColorEdit4(_("2D Box No Visible Color"), (float*)&config.novis, MsTrGColorEditFlags_NoDragDrop | MsTrGWindowFlags_NoScrollbar | MsTrGColorEditFlags_AlphaBar | MsTrGColorEditFlags_NoInputs);
						MsTrG::ColorEdit4(_("2D Box Visible Color"), (float*)&config.vis, MsTrGColorEditFlags_NoDragDrop | MsTrGWindowFlags_NoScrollbar | MsTrGColorEditFlags_AlphaBar | MsTrGColorEditFlags_NoInputs);
						MsTrG::ColorEdit4(_("Skeleton No Visible Color"), (float*)&config.noviss, MsTrGColorEditFlags_NoDragDrop | MsTrGWindowFlags_NoScrollbar | MsTrGColorEditFlags_AlphaBar | MsTrGColorEditFlags_NoInputs);
						MsTrG::ColorEdit4(_("Skeleton Visible Color"), (float*)&config.viss, MsTrGColorEditFlags_NoDragDrop | MsTrGWindowFlags_NoScrollbar | MsTrGColorEditFlags_AlphaBar | MsTrGColorEditFlags_NoInputs);
					}
					else
					{
						MsTrG::ColorEdit4(_("2D Box Color"), (float*)&config.espcolor, MsTrGColorEditFlags_NoDragDrop | MsTrGWindowFlags_NoScrollbar | MsTrGColorEditFlags_AlphaBar | MsTrGColorEditFlags_NoInputs);
						MsTrG::ColorEdit4(_("Skeleton Color"), (float*)&config.skeleton, MsTrGColorEditFlags_NoDragDrop | MsTrGWindowFlags_NoScrollbar | MsTrGColorEditFlags_AlphaBar | MsTrGColorEditFlags_NoInputs);
					}
				}
			}
			MsTrG::EndChild();
		}
	}
	MsTrG::End();
}