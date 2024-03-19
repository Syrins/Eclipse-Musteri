#include "structs_funcs.h"

ImDrawList* draw;
uintptr_t root_comp;
uintptr_t apawn;
uintptr_t actor;
DWORD ScreenCenterX;
DWORD ScreenCenterY;


extern LRESULT MsTrG_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct State {
	uintptr_t LApkys[7];
};


void CacheCh34t()
{
	MsTrGIO& io = MsTrG::GetIO();
	ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\tahoma.ttf ", 14);
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
	ImFont* FontAwesome = io.Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 14.f, &icons_config, icons_ranges);
	ImFont* FontAwesomeBig = io.Fonts->AddFontFromMemoryCompressedTTF(fa6_solid_compressed_data, fa6_solid_compressed_size, 19.f, nullptr, icons_ranges);
	ImFont* TitleFont = io.Fonts->AddFontFromMemoryCompressedTTF(trebucbd_compressed_data, trebucbd_compressed_size, 30, nullptr, io.Fonts->GetGlyphRangesDefault());
	while (true)
	{
		//Sleep(250);
		world = read<uintptr_t>(virtualaddy + 0x60);
		world = check::validate_pointer(world);
		if (!world) continue;

		game_instance = read2<uintptr_t>(world + 0x1A0);//offset 0x1a0
		if (!game_instance) continue;

		persistent_level = read2<uintptr_t>(world + 0x38);
		persistent_level = check::validate_pointer(persistent_level);
		if (!persistent_level) continue;


		uintptr_t local_players = read2<uintptr_t>(game_instance + 0x40);//offset 0x40
		if (!local_players) continue;

		local_player = read2<uintptr_t>(local_players);
		if (!local_player) continue;
		Sleep(500);
		LAPNT::player_controller = read2<uintptr_t>(local_player + 0x38);

		cntrlrttn = read<fvector>(LAPNT::player_controller + 0x448);

		apawn = read2<DWORD_PTR>(LAPNT::player_controller + 0x468);
		playerstate = read2<uintptr_t>(apawn + 0x3f8);//offset
		teamComponent = read2<uintptr_t>(playerstate + 0x630);//offset
		teamID = read2<int>(teamComponent + 0xf8);


		uintptr_t local_pawn = read2<uintptr_t>(LAPNT::player_controller + 0x468);

		LAPNT::local_pawn = local_pawn;

		LAPNT::camera_manager = read2<uintptr_t>(LAPNT::player_controller + 0x480);//offsets player_camera 0x480


		uintptr_t actor_array = read2<uintptr_t>(persistent_level + 0xa0);//offset 0xa0
		if (!actor_array) continue;

		actor_count = read2<INT32>(persistent_level + 0xa8);
		if (!actor_count) continue;


		for (int x = 0; x < actor_count; x++)
		{
			bool is_damage_handler_guarded = false;
			bool is_mesh_guarded = false;

			uintptr_t for_mesh = 0;
			uintptr_t for_actor = 0;

			uintptr_t actor = read2<uintptr_t>(actor_array + (x * 8));
			if (!actor) continue;

			INT32 unique_id = read2<INT32>( actor + 0x38);
			if (unique_id != 18743553) continue;

			uintptr_t mesh = read2<uintptr_t>(actor + 0x438);//offset 0x438
			if (!mesh) continue;


			root_comp = read2<uintptr_t>( actor + 0x238);// offsets 0x238
			if (!root_comp) continue;

			uintptr_t damage_handler = read2<uintptr_t>( actor + 0xa08);//offset 0xa10
			if (!damage_handler) continue;
			float health = 0.f;

			health = read2<float>(damage_handler + 0x1B0);
			uintptr_t playerstate = read2<uintptr_t>( actor + 0x3f8);
			uintptr_t team_component = read2<uintptr_t>(playerstate + 0x630);
			int team_id = read2<int>(team_component + 0xF8);
			if (team_id == teamID) continue;

			if (health <= 0) continue;

			uintptr_t bone_array = read2<uintptr_t>( mesh + 0x5D8);
			if (!bone_array) continue;

			INT32 bone_count = read2<INT32>( mesh + 0x5E0);
			if (!bone_count) continue;



			Enemy Entities
			{
				for_actor, //guarded region ptr
				for_mesh, //guarded region ptr
				actor,
				mesh,
				bone_array,
				root_comp,
				damage_handler,
				bone_count,
				0, //player_ammo count
				"", //weapon name
				"", //agent name
				"", //Enemy name
				0.f, //distance
				health, //health
				0.f, //shleid
				true,
				is_damage_handler_guarded,
				is_mesh_guarded
			};
			if (!player_pawns.empty()) {
				auto found_player = std::find(player_pawns.begin(), player_pawns.end(), Entities);
				if (found_player == player_pawns.end())
				{
					player_pawns.push_back(Entities);
				}
			}
			else
			{
				player_pawns.push_back(Entities);
			}
		}
		Sleep(500);
	}
}

#define DegreeToRadian( Degree )	( ( Degree ) * ( M_PI / 180.0f ) )



static fvector pRadar;
static fvector asd;

Vector2 WorldRadar(fvector srcPos, fvector distPos, float yaw, float radarX, float radarY, float size)
{
	auto cosYaw = cos(DegreeToRadian(yaw));
	auto sinYaw = sin(DegreeToRadian(yaw));

	auto deltaX = srcPos.X - distPos.X;
	auto deltaY = srcPos.Y - distPos.Y;

	auto locationX = (float)(deltaY * cosYaw - deltaX * sinYaw) / 45.f;
	auto locationY = (float)(deltaX * cosYaw + deltaY * sinYaw) / 45.f;

	if (locationX > (size - 2.f))
		locationX = (size - 2.f);
	else if (locationX < -(size - 2.f))
		locationX = -(size - 2.f);

	if (locationY > (size - 6.f))
		locationY = (size - 6.f);
	else if (locationY < -(size - 6.f))
		locationY = -(size - 6.f);

	return Vector2((int)(-locationX + radarX), (int)(locationY + radarY));
}

void DrawRadar(fvector EntityPos)
{
	auto radar_posX = pRadar.X + 105;
	auto radar_posY = pRadar.Y + 95;
	uint64_t LocalRootComp = read<uint64_t>(LAPNT::local_pawn + 0x238);
	fvector LocalPos = read<fvector>(LocalRootComp + 0x164);
	auto Radar2D = WorldRadar(LocalPos, EntityPos, LACAMRA::rotation.Y, radar_posX, radar_posY, 95.f);
	ImVec4 S4 = ImVec4(0.682f, 0.0f, 0.204f, 1.0f); // enemy color
	DrawCircleRadar(Radar2D.X, Radar2D.Y, 4, S4); //R1);
}

void DrawNormalBox(int x, int y, int w, int h, float borderLevel, ImColor color, ImColor color2, ImColor outlineColor)
{
	float clampedBorderLevel = min(max(borderLevel, 0.5f), 2.0f);

	float borderPx = 0.5f + clampedBorderLevel * 0.5f;

	int roundedX = static_cast<int>(std::round(x + 0.5f));
	int roundedY = static_cast<int>(std::round(y + 0.5f));
	int roundedBorderPx = static_cast<int>(std::round(borderPx));

	// Draw Outline
	DrawFilledRect3(roundedX - roundedBorderPx, roundedY - roundedBorderPx, w + 2 * roundedBorderPx, roundedBorderPx, outlineColor);

	// Draw left outline
	DrawFilledRect3(roundedX - roundedBorderPx, roundedY, roundedBorderPx, h, outlineColor);

	// Draw bottom outline
	DrawFilledRect3(roundedX - roundedBorderPx, roundedY + h, w + 2 * roundedBorderPx, roundedBorderPx, outlineColor);

	// Draw right outline
	DrawFilledRect3(roundedX + w, roundedY, roundedBorderPx, h, outlineColor);

	// Top
	DrawFilledRect3(roundedX + roundedBorderPx, roundedY, w - 2 * roundedBorderPx, roundedBorderPx, color);

	// Left
	DrawFilledRect3(roundedX, roundedY + roundedBorderPx, roundedBorderPx, h - 2 * roundedBorderPx, color);

	// Bottom
	DrawFilledRect3(roundedX + roundedBorderPx, roundedY + h - roundedBorderPx, w - 2 * roundedBorderPx, roundedBorderPx, color);

	// Right
	DrawFilledRect3(roundedX + w - roundedBorderPx, roundedY + roundedBorderPx, roundedBorderPx, h - 2 * roundedBorderPx, color);

	// Corners
	DrawFilledRect3(roundedX, roundedY, roundedBorderPx, roundedBorderPx, color2); // top-left
	DrawFilledRect3(roundedX + w - roundedBorderPx, roundedY, roundedBorderPx, roundedBorderPx, color2); // top-right
	DrawFilledRect3(roundedX, roundedY + h - roundedBorderPx, roundedBorderPx, roundedBorderPx, color2); // bottom-left
	DrawFilledRect3(roundedX + w - roundedBorderPx, roundedY + h - roundedBorderPx, roundedBorderPx, roundedBorderPx, color2); // bottom-right
}

auto Draw2DBox(fvector RootPosition, float Width, float Height, ImColor Color, ImColor Color2, ImColor outlineColor) -> void
{
	DrawNormalBox(RootPosition.X - Width / 2, RootPosition.Y - Height / 2, Width, Height, Settings::Visuals::BoxWidth, Color, Color2, outlineColor);
}

void DrawFilledRect(int x, int y, int w, int h, RGBA* color)
{
	MsTrG::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), MsTrG::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 153.0, color->B / 51.0, color->A / 255.0)), 0, 0);
}

auto DrawHealthBar(fvector RootPosition, float Width, float Height, float Health, float RelativeDistance) -> void
{
	auto HPBoxWidth = 1 / RelativeDistance;

	auto HPBox_X = RootPosition.X - Width / 2;
	auto HPBox_Y = RootPosition.Y - Height / 2;

	// Determine color based on health range
	RGBA healthColor;

	if (Health == 100.0f)
		healthColor = { 0, 255, 0, 255 }; // Green
	else if (Health >= 70.0f)
		healthColor = { 252, 197, 0, 255 }; // Yellow
	else
		healthColor = { 239, 59, 0, 255 }; // Red

	// Hesaplanan genişlik ve yüksekliği kullanmadan sadece renk değişikliği
	DrawFilledRect(HPBox_X, HPBox_Y, HPBoxWidth * (Health / 100.0f), Height, &healthColor);
	DrawRect(HPBox_X - 1, HPBox_Y - 1, HPBoxWidth + 2, Height + 2, &Col.black, 1);
}
static ULONG Width2 = GetSystemMetrics(SM_CXSCREEN);

static ULONG Height2 = GetSystemMetrics(SM_CYSCREEN);

auto DrawTracers(fvector Target, ImColor Color) -> void
{
	MsTrG::GetForegroundDrawList()->AddLine(
		ImVec2(ScreenCenterX, Height2),
		ImVec2(Target.X, Target.Y),
		Color,
		0.1f
	);
}

auto RelativeLocation(uintptr_t APawn) -> fvector {
	auto RootComponent = read<uintptr_t>(APawn + 0x238);
	return read<fvector>(RootComponent + 0x164);
}


auto bisVisible(Enemy APawn) -> bool {
	float LastRenderTime = read<float>(APawn.mesh + 0x384);
	float LastSubmitTime = read<float>(APawn.mesh + 0x380);
	bool IsVisible = LastRenderTime + 0.06F >= LastSubmitTime;
	return IsVisible;
};

bool bIsDormant(Enemy APawn) {
	bool dormant = read<bool>(APawn.actor + 0x100);
	if (!dormant) { return false; }
	return true;
};

void camera()//offsets camera
{
	auto ViewInfo = read<FMinimalViewInfo>(LAPNT::camera_manager + 0x1FB0);//cameracache offsets.
	LACAMRA::location = ViewInfo.Location;
	LACAMRA::rotation = ViewInfo.Rotation;
	LACAMRA::fov = ViewInfo.FOV;
}

void Render_Players()
{

	camera();

	int closest_player = 1338;
	float closest_distance = FLT_MAX;

	for (int x = 0; x < player_pawns.size(); x++)
	{
		Enemy Entity = player_pawns[x];


		float health = 0;
		health = read<float>(Entity.damage_handler + 0x1B0);

		if (health <= 0.f || health > 999.f)
		{
			player_pawns[x].is_valid = false;
		}

		if (!Entity.is_valid)
		{
			auto erase_player = std::find(player_pawns.begin(), player_pawns.end(), Entity);
			player_pawns.erase(erase_player);
			continue;
		}
		uintptr_t playerstate = read2<uintptr_t>(Entity.for_actor + Entity.actor + 0x3f8);
		uintptr_t team_component = read2<uintptr_t>(playerstate + 0x630);
		int team_id = read2<int>(team_component + 0xF8);
		if (team_id == teamID) continue;
		int actors_int = read2<int>(Entity.actor + 0x38);

		if (actors_int != 18743553) continue;

		auto active = read<bool>(Entity.actor + 0x100);
		bool visible = bisVisible(Entity) && active;



		fvector zero = bone_matrix(0, Entity);
		fvector head = bone_matrix(8, Entity);


		fvector vBaseBoneOut = projw2s(fvector(zero.X, zero.Y, zero.Z));

		fvector vHeadBoneOut = projw2s(fvector(head.X, head.Y, head.Z));
		fvector vHeadBoneOut2 = projw2s(fvector(head.X, head.Y, head.Z + 40));


		float BoxHeight = 1.0 * abs(vHeadBoneOut2.Y - vBaseBoneOut.Y);
		float BoxWidth = BoxHeight * 0.40;
		float BoxWidth2 = BoxHeight * 0.47;
		fvector head_r = fvector(head.X, head.Y, head.Z + 15);
		fvector zero_r = fvector(zero.X, zero.Y, zero.Z - 5);

		fvector zero_projw2s = projw2s(zero);
		fvector head_projw2s = projw2s(head);

		fvector zero_projw2s2 = projw2s(zero_r);
		fvector head_projw2s2 = projw2s(head_r);
		static ULONG Width = GetSystemMetrics(SM_CXSCREEN);
		static ULONG Height = GetSystemMetrics(SM_CYSCREEN);

		pRadar.X = (Width / 2) + 550;
		pRadar.Y = Width / 2 - Height / 2 - 300;

		auto RelativeLocationn = RelativeLocation(Entity.actor);
		auto RelativePosition = RelativeLocationn - LACAMRA::location;
		auto RelativeDistance = RelativePosition.Length() / 10000 * 2;
		auto RelativeLocationProjected = projw2s(RelativeLocationn);

		float distance = LACAMRA::location.Distance(zero) / 100.f;

		ImVec4 S4 = to_vec4(0, 153, 191, (int)Alpha);
		if (config.radar)
		{
			ImVec4 bgColor = ImVec4(0.5f, 0.5f, 0.5f, 0.1f);
			ImVec4 lineColor = to_vec4(208, 162, 0, 255);
			ImVec4 edgeColor = to_vec4(197, 197, 197, 225);

			// Arka planı gri ve saydam çiz
			MsTrG::GetOverlayDrawList()->AddRectFilled(ImVec2(pRadar.X, pRadar.Y), ImVec2(pRadar.X + 210, pRadar.Y + 190), MsTrG::ColorConvertFloat4ToU32(bgColor));

			// Kenar çizgilerini belirt
			MsTrG::GetOverlayDrawList()->AddRect(ImVec2(pRadar.X, pRadar.Y), ImVec2(pRadar.X + 210, pRadar.Y + 190), MsTrG::ColorConvertFloat4ToU32(lineColor), 2);

			auto radar_posX = pRadar.X + 105;
			auto radar_posY = pRadar.Y + 95;

			// Kenar çizgilerini belirt (çizgi renkleri edgeColor olarak ayarlandı)
			DrawLine(ImVec2(radar_posX, radar_posY), ImVec2(radar_posX, radar_posY + 95), MsTrG::ColorConvertFloat4ToU32(edgeColor), 2);
			DrawLine(ImVec2(radar_posX, radar_posY), ImVec2(radar_posX, radar_posY - 95), MsTrG::ColorConvertFloat4ToU32(edgeColor), 2);
			DrawLine(ImVec2(radar_posX, radar_posY), ImVec2(radar_posX + 105, radar_posY), MsTrG::ColorConvertFloat4ToU32(edgeColor), 2);
			DrawLine(ImVec2(radar_posX, radar_posY), ImVec2(radar_posX - 105, radar_posY), MsTrG::ColorConvertFloat4ToU32(edgeColor), 2);

			// Daireyi çiz
			DrawCircleFilled(radar_posX + 1, radar_posY + 1, 3, S4, 10);

			// Rakip oyuncunun konumunu radar üzerinde belirt
			fvector pos = read<fvector>(Entity.root_component + 0x164);
			DrawRadar(pos);
		}
		if (config.dormant) if (!bIsDormant(Entity)) continue;
		int bottom_text_offset = 2;

		if (config.player_box)
		{
			if (config.visiblecheck)
			{
				if (visible)
				{
					Draw2DBox(RelativeLocationProjected, BoxWidth, BoxHeight, config.vis, config.vis, config.outlinecolor);
				}
				else
				{
					Draw2DBox(RelativeLocationProjected, BoxWidth, BoxHeight, config.novis, config.novis, config.outlinecolor);
				}
			}
			else
			{
				Draw2DBox(RelativeLocationProjected, BoxWidth, BoxHeight, config.espcolor, config.espcolor, config.outlinecolor);
			}


		}
		if (config.headcircle)
		{
			MsTrG::GetBackgroundDrawList()->AddCircle(ImVec2(vHeadBoneOut.X, vHeadBoneOut.Y), BoxWidth / 7, ImColor(config.headboxcolor), 64, 1.5f);
		}
		if (config.player_healthbar)
		{
			DrawHealthBar(RelativeLocationProjected, BoxWidth2, BoxHeight, health, RelativeDistance);
		}
		if (config.player_skeleton)
		{
			//MsTrG::GetBackgroundDrawList()->AddCircle(ImVec2(vHeadBoneOut.X, vHeadBoneOut.Y), BoxWidth / 7, ImColor(config.skeleton), 64, 1.5f);
			if (config.visiblecheck)
			{
				if (visible)
				{
					skeleton_box(Entity, ImVec4(config.viss), skelethick);
				}
				else
				{
					skeleton_box(Entity, ImVec4(config.noviss), skelethick);
				}

			}
			else
			{
				skeleton_box(Entity, ImVec4(config.skeleton), skelethick);
			}

		}
		if (config.player_snapline)
		{
			DrawTracers(RelativeLocationProjected, config.espcolor);
		}
		if (config.player_distance)
		{


			char distance_text[256];
			sprintf_s(distance_text, ("[ %.fm ]"), distance);
			ImVec2 text_size = MsTrG::CalcTextSize(distance_text);
			bottom_text_offset += 14;
			DrawTextNEW(zero_projw2s.X, zero_projw2s.Y + RelativeLocationProjected, distance_text, &Col.DarkYellow);

		}
		int keyya = read<int>(Entity.actor + 0x18);
		std::string nameea = get_fname(keyya);
		if (config.agentnames)
		{
			MapAgentName(nameea);
			ImVec2 TextSize2 = MsTrG::CalcTextSize(nameea.c_str());
			DrawTextNEW(zero_projw2s.X, zero_projw2s.Y + RelativeLocationProjected, nameea.c_str(), &Col.SilverWhite);				bottom_text_offset += 14;
		}


	}

}

IDirect3DSwapChain9* D3D9SwapChain;

void LARNNDDDRR() {
	MsTrG_ImplDX9_NewFrame();
	MsTrG_ImplWin32_NewFrame();
	MsTrG::NewFrame();
	static bool menuopen = true;

	if (GetAsyncKeyState(VK_INSERT) & 1)
	{
		menuopen = !menuopen;
	}

	stylemenu();
	if (menuopen)
	{
		MsTrG::GetIO().MouseDrawCursor = 1;
		RenderUI();
	}
	else
		MsTrG::GetIO().MouseDrawCursor = 0;

	Render_Players();
	if (showfps)
	{
		float fps = MsTrG::GetIO().Framerate;
		std::string asd = "Overlay Fps " + std::to_string(static_cast<int>(fps));
		ImVec2 textSize = MsTrG::CalcTextSize(asd.c_str());
		DrawTextNEW(75, 20, asd.c_str(), &Col.SilverWhite);
	}


	Knzdvcd3d->SetRenderState(D3DRS_ZENABLE, false);
	Knzdvcd3d->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Knzdvcd3d->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	Knzdvcd3d->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	if (Knzdvcd3d->BeginScene() >= 0) {
		MsTrG::Render();
		MsTrG_ImplDX9_RenderDrawData(MsTrG::GetDrawData());
		Knzdvcd3d->EndScene();
	}
	HRESULT LARSLT = Knzdvcd3d->Present(NULL, NULL, NULL, NULL);

	if (LARSLT == D3DERR_DEVICELOST && Knzdvcd3d->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
		MsTrG_ImplDX9_InvalidateDeviceObjects();
		Knzdvcd3d->Reset(&knzppd3d);
		MsTrG_ImplDX9_CreateDeviceObjects();
	}
}



	bool KNZDRTCTX() {
		if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &KNZ_POBJ)))
			return false;

		ZeroMemory(&knzppd3d, sizeof(knzppd3d));
		knzppd3d.BackBufferWidth = Width;
		knzppd3d.BackBufferHeight = Height;
		knzppd3d.BackBufferFormat = D3DFMT_A8R8G8B8;
		knzppd3d.MultiSampleQuality = D3DMULTISAMPLE_NONE;
		knzppd3d.AutoDepthStencilFormat = D3DFMT_D16;
		knzppd3d.SwapEffect = D3DSWAPEFFECT_DISCARD;
		knzppd3d.EnableAutoDepthStencil = TRUE;
		knzppd3d.hDeviceWindow = KNZWNDW;
		knzppd3d.Windowed = TRUE;

		KNZ_POBJ->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, KNZWNDW, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &knzppd3d, &Knzdvcd3d);

		MsTrG_CHECKVERSION();

		MsTrG::CreateContext();
		MsTrGIO& io = MsTrG::GetIO();


		MsTrG_ImplWin32_Init(KNZWNDW);
		MsTrG_ImplDX9_Init(Knzdvcd3d);
		KNZ_POBJ->Release();
		return true;
	}

	void KNZWNDWR() {
		while (true) {
			HWND LAFRGRNDWNDDW = GetForegroundWindow();
			HWND LATRGTWNDW = FindWindowA(0, "VALORANT  ");

			if (LAFRGRNDWNDDW == LATRGTWNDW)
				break;

			Sleep(200);
		}
	}

	void KNKPPKKL() {
		static RECT old_rc;
		ZeroMemory(&LAMSSG, sizeof(MSG));

		while (LAMSSG.message != WM_QUIT)
		{
			if (PeekMessage(&LAMSSG, KNZWNDW, 0, 0, 0x0001))
			{
				TranslateMessage(&LAMSSG);
				DispatchMessage(&LAMSSG);
			}

			HWND hwnd_active = GetForegroundWindow();

			if (hwnd_active == KNZHNND) {
				HWND hwndtest = GetWindow(hwnd_active, 3);
				SetWindowPos(KNZWNDW, hwndtest, 2, 2, -3, -3, 0x0002 | 0x0001);
			}

			RECT rc;
			POINT xy;
			ZeroMemory(&rc, sizeof(RECT));
			ZeroMemory(&xy, sizeof(POINT));
			GetClientRect(KNZHNND, &rc);
			ClientToScreen(KNZHNND, &xy);
			rc.left = xy.x;
			rc.top = xy.y;

			MsTrGIO& io = MsTrG::GetIO();
			io.ImeWindowHandle = KNZHNND;
			io.DeltaTime = 1.0f / 60.0f;

			POINT p;
			GetCursorPos(&p);
			io.MousePos.x = p.x - xy.x;
			io.MousePos.y = p.y - xy.y;

			if (GetAsyncKeyState(VK_LBUTTON)) {
				io.MouseDown[0] = true;
				io.MouseClicked[0] = true;
				io.MouseClickedPos[0].x = io.MousePos.x;
				io.MouseClickedPos[0].x = io.MousePos.y;
			}
			else
				io.MouseDown[0] = false;

			if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
			{
				old_rc = rc;

				Width = rc.right;
				Height = rc.bottom;

				knzppd3d.BackBufferWidth = Width;
				knzppd3d.BackBufferHeight = Height;
				SetWindowPos(KNZWNDW, (HWND)0, xy.x + 2, xy.y + 2, Width - 3, Height - 3, 0x0008);
				Knzdvcd3d->Reset(&knzppd3d);
			}



			LARNNDDDRR();

		}
		if (vsync)
		{

			D3D9SwapChain->Present(NULL, NULL, NULL, NULL, D3DPRESENT_INTERVAL_DEFAULT);

		}
		else
		{
			D3D9SwapChain->Present(NULL, NULL, NULL, NULL, 0);

		}

		MsTrG_ImplDX9_Shutdown();
		MsTrG_ImplWin32_Shutdown();
		MsTrG::DestroyContext();

		DestroyWindow(KNZWNDW);
	}

	void KNZPPWERWW() {
		KNZTRFVBF->Release();
		Knzdvcd3d->Release();
		KNZ_POBJ->Release();

		DestroyWindow(KNZWNDW);
		UnregisterClassW(_(L"qwewqpepqwepqwpeqpwepqwepwqpqwepqepwpqwepewqpewqpweqpweqpwqeppewqpweq"), NULL);
	}

	LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
		// MsTrG kütüphanesinin ileti işleme işlevini çağırır.
		if (MsTrG_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
			return true;

		switch (Message)
		{
		case WM_DESTROY:
			// KNZPPWERWW fonksiyonunu çağırır (bu fonksiyon kodunuzda tanımlı olmalıdır).
			KNZPPWERWW();

			// Pencereyi kapatma işlemini başlatır.
			PostQuitMessage(0);
			exit(4); // Programı sonlandırır (bu kısmı gözden geçirmeniz gerekebilir).
			break;
		case WM_SIZE:
			// Pencere boyutu değiştiğinde çalışır.
			if (Knzdvcd3d != NULL && wParam != SIZE_MINIMIZED)
			{
				// Direct3D cihazının geçerli boyutunu ayarlar.
				MsTrG_ImplDX9_InvalidateDeviceObjects();
				knzppd3d.BackBufferWidth = LOWORD(lParam);
				knzppd3d.BackBufferHeight = HIWORD(lParam);
				HRESULT hr = Knzdvcd3d->Reset(&knzppd3d);

				// Hata durumu kontrolü yapabilirsiniz.
				if (hr == D3DERR_INVALIDCALL)
					IM_ASSERT(0);

				// MsTrG için Direct3D cihazı nesnelerini oluşturur.
				MsTrG_ImplDX9_CreateDeviceObjects();
			}
			break;
		default:
			// Bilinmeyen iletiler için varsayılan Windows ileti işleme işlevini çağırır.
			return DefWindowProc(hWnd, Message, wParam, lParam);
			break;
		}

		return 0;
	}
	void SetWindowToTarget()
	{
		while (true)
		{
			if (KNZHNND)
			{
				ZeroMemory(&KNZMMTTC, sizeof(KNZMMTTC));
				GetWindowRect(KNZHNND, &KNZMMTTC);
				Width = KNZMMTTC.right - KNZMMTTC.left;
				Height = KNZMMTTC.bottom - KNZMMTTC.top;
				DWORD dwStyle = GetWindowLong(KNZHNND, GWL_STYLE);

				if (dwStyle & WS_BORDER)
				{
					KNZMMTTC.top += 32;
					Height -= 39;
				}
				ScreenCenterX = Width / 2;
				ScreenCenterY = Height / 2;
				MoveWindow(KNZWNDW, KNZMMTTC.left, KNZMMTTC.top, Width, Height, true);
			}
			else
			{
				exit(0);
			}
		}
	}

	void KNZOVRLLY() {
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetWindowToTarget, 0, 0, 0);

		WNDCLASSEX wc;
		ZeroMemory(&wc, sizeof(wc));
		wc.cbSize = sizeof(wc);
		wc.lpszClassName = _(L"qwewqpepqwepqwpeqpwepqwepwqpqwepqepwpqwepewqpewqpweqpweqpwqeppewqpweq");
		wc.lpfnWndProc = WinProc;
		RegisterClassEx(&wc);

		if (KNZHNND)
		{
			GetClientRect(KNZHNND, &KNZMMTTC);
			POINT xy;
			ClientToScreen(KNZHNND, &xy);
			KNZMMTTC.left = xy.x;
			KNZMMTTC.top = xy.y;

			Width = KNZMMTTC.right;
			Height = KNZMMTTC.bottom;
		}
		else
			exit(2);

		KNZWNDW = CreateWindowEx(NULL, _(L"qwewqpepqwepqwpeqpwepqwepwqpqwepqepwpqwepewqpewqpweqpweqpwqeppewqpweq"), _(L"qwewqpepqwepqwpeqpwepqwepwqpqwepqepwpqwepewqpewqpweqpweqpwqeppewqpwe"), WS_POPUP | WS_VISIBLE, 0, 0, Width, Height, 0, 0, 0, 0);

		DwmExtendFrameIntoClientArea(KNZWNDW, &KNZRRGGN);
		SetWindowLong(KNZWNDW, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
		ShowWindow(KNZWNDW, SW_SHOW);
		UpdateWindow(KNZWNDW);
	}

	void Knzstrtcnnmw() {
		KNZHNND = FindWindowA(0, _("VALORANT  "));

		KNZOVRLLY();

		KNZDRTCTX();

		KNZWNDWR();

		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)CacheCh34t, NULL, NULL, NULL);

		KNKPPKKL();
		KNZPPWERWW();
	}