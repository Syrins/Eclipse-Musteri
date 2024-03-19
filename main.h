
#define M_PI (double)3.1415926535
#define actor_player 4

static HWND KNZWNDW = NULL;
IDirect3D9Ex* KNZ_POBJ = NULL;
static LPDIRECT3DDEVICE9 Knzdvcd3d = NULL;
static LPDIRECT3DVERTEXBUFFER9 KNZTRFVBF = NULL;
HWND KNZHNND = NULL;
RECT KNZMMTTC = { NULL };
D3DPRESENT_PARAMETERS knzppd3d;
const MARGINS KNZRRGGN = { -1 };


MSG LAMSSG = { NULL };

bool KNZPNN = true;
bool KNZTMM = true;
bool vsync = false;
bool showfps = true;
bool KNZZSTTRRT = false;

const char* esptypee[] = { "Corner Box",  "2D Box"};
const char* aimbone[] = { "Head", "Pelvis", "Body" }; //0,1,2 = bone number

const char* linetype[] = { "Bottom",  "Center", "Top" };

static int keyint = 0;
static int colorint = 0;
bool outline = true;
float espthick = 0.5f;
float skelethick = 1.5f;

struct Config {


	bool circlehead = false;
	bool distancecontrol = false;
	INT aimbotkey;
	// Aimbot settings
	bool aimenable = false;
	bool visiblecheck = false;
	bool rcs = false;
	bool aimbot_constant = false;
	bool aimbot_mode_silent = false;
	bool aimbot_draw_fov = false;
	bool aimbot_draw_target_line = false;
	bool aimbot_ignore_with_useless_weapons = false;
	float aimbot_fov = 20.f;
	float aimbot_smooth = 10.f;
	float amcnstoff = 0.f;
	float silent_fov = 1.f;
	float silent_smooth = 1.f;
	float fovchangervalue = 100.f;
	float legit_smooth = 5.f;
	float max_distance = 100.f;

	// ESP settings
	bool player_box = false;
	bool dormant = false;
	bool player_box_3d = false;
	bool player_skeleton = false;
	bool headcircle = false;
	bool player_snapline = false;
	bool player_view_angle = false;
	bool player_distance = false;
	bool player_healthbar = false;
	bool player_agent = false;
	bool radar;
	bool player_weapon = false;
	bool glow = false;
	bool fovchanger = false;
	bool agentnames = false;
	bool weaponnames = false;
	bool spiketimer = false;
	int aimboness = 0;
	int bonees = 0;
	int aimtype = 0;
	int linetype = 0;

	int esptype = 0;

	ImColor headboxcolor = ImColor(241, 34, 34);
	ImColor espcolor = ImColor(255, 255, 255);
	ImColor novis = ImColor(255, 0, 0);
	ImColor vis = ImColor(0, 255, 0);
	ImColor noviss = ImColor(255, 0, 0);
	ImColor viss = ImColor(0, 255, 0);
	ImColor outlinecolor = ImColor(0, 0, 0, 155);
	ImVec4 snapcolor = { 1.0f, 0.65f, 0.0f, 1.0f }; // snapline renk
	ImVec4 skeleton = { 1.0f, 1.0f, 1.0f, 1.0f }; // skeleton renk
};

// Sample usage
Config config;
ImVec4 to_vec4(float r, float g, float b, float a)
{
	return ImVec4(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
}


namespace Settings
{


	namespace Player
	{
		bool radar = false;
		bool spike = true;
		bool chams = false;
		bool WeaponESP = true;
		bool equip = true;
		bool Enable = false;
		bool CornerBox = false;
		bool agentype = true;
		bool Box = true;
		bool Box3D = false;
		bool Box3DReal = false;
		bool Head = true;
		bool Lines = false;
		bool ShowDistance = false;
	}


	namespace World
	{
		bool AstraStar = false;
		bool BrimPower = false;
		bool BrimSmoke = false;
		bool BrimUlti = false;
		bool ChamberTrap = false;
		bool ChamberTP = false;
		bool CypherTrap = false;
		bool CypherCage = false;
		bool CypherCam = false;
		bool FadeMonster = false;
		bool FadeEye = false;
		bool GeekoExplode = false;
		bool GeekoRunner = false;
		bool HarborWave = false;
		bool HarborShield = false;
		bool JettSmoke = false;
		bool KayooBomb = false;
		bool KilljoyBomb = false;
		bool KilljoyAlarm = false;
		bool KilljoyTurret = false;
		bool KilljoyUlti = false;
		bool OmenShort = false;
		bool OmenSmoke = false;
		bool OmenLong = false;
		bool PhoneixFire = false;
		bool PhoneixUlti = false;
		bool RazeBommbot = false;
		bool RazeBoompack = false;
		bool ReynaEye = false;
		bool SageSlow = false;
		bool SageWall = false;
		bool ViperPoison = false;
		bool ViperSmoke = false;
		bool ViperUlti = false;
		bool YoruFake = false;
		bool YoruTP = false;
		bool UltiOrb = false;
		float Distance = 300;
	}
}

INT legitkey;
INT legitbone;
INT silentKey;
INT silentbone;


bool Visibles;

float silent_smooth = 5.f;



DWORD_PTR LocalPawn;
uintptr_t playerstate;
uintptr_t teamComponent;
int teamID;
uintptr_t world;
uintptr_t game_instance;
uintptr_t persistent_level;
uintptr_t local_player;
INT32 actor_count;

int customX;
int customY;

//typedef PVOID(*name)();

uintptr_t base;


typedef struct
{
	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;
}RGBA;

class Color
{
public:
	RGBA moi_blue = { 0, 143, 209,255 };
	RGBA moi_red = { 241,34,34,255 };
	RGBA moi_green = { 0,209,91,255 };

	RGBA red = { 255,0,0,255 };
	RGBA Magenta = { 255,0,255,255 };
	RGBA yellow = { 255,255,0,255 };
	RGBA grayblue = { 128,128,255,255 };
	RGBA green = { 128,224,0,255 };
	RGBA darkgreen2 = { 26, 128, 0, 255 };
	RGBA darkgreen3 = { 6, 68, 0, 255 };
	RGBA darkgreen = { 0,224,128,255 };
	RGBA brown = { 192,96,0,255 };
	RGBA pink = { 255,168,255,255 };
	RGBA DarkYellow = { 216,216,0,255 };
	RGBA SilverWhite = { 236,236,236,255 };
	RGBA purple = { 144,0,255,255 };
	RGBA Navy = { 88,48,224,255 };
	RGBA skyblue = { 0,136,255,255 };
	RGBA graygreen = { 128,160,128,255 };
	RGBA blue = { 0,96,192,255 };

	RGBA orange = { 255,128,0,255 };
	RGBA peachred = { 255,80,128,255 };
	RGBA reds = { 255,128,192,255 };
	RGBA darkgray = { 96,96,96,255 };
	RGBA Navys = { 0,0,128,255 };
	RGBA darkgreens = { 0,128,0,255 };
	RGBA darkblue = { 0,128,128,255 };
	RGBA redbrown = { 128,0,0,255 };
	RGBA red4 = { 255, 24, 101, 255 };
	RGBA purplered = { 128,0,128,255 };
	RGBA greens = { 0,255,0,255 };
	RGBA envy = { 0,255,255,255 };
	RGBA black = { 0,0,0,255 };
	RGBA gray = { 128,128,128,255 };
	RGBA white = { 255,255,255,255 };
	RGBA blues = { 30,144,255,255 };
	RGBA lightblue = { 135,206,250,160 };
	RGBA Scarlet = { 220, 20, 60, 160 };
	RGBA white_ = { 255,255,255,200 };
	RGBA gray_ = { 128,128,128,200 };
	RGBA black_ = { 0,0,0,200 };
	RGBA red_ = { 255,0,0,200 };
	RGBA Magenta_ = { 255, 95, 96, 255 };
	RGBA yellow_ = { 255,255,0,200 };
	RGBA grayblue_ = { 128,128,255,200 };
	RGBA green_ = { 128,224,0,200 };
	RGBA darkgreen_ = { 0,224,128,200 };
	RGBA brown_ = { 192,96,0,200 };
	RGBA pink_ = { 255,168,255,200 };
	RGBA darkyellow_ = { 216,216,0,200 };
	RGBA silverwhite_ = { 236,236,236,200 };
	RGBA purple_ = { 144,0,255,200 };
	RGBA Blue_ = { 88,48,224,200 };
	RGBA skyblue_ = { 0,136,255,200 };
	RGBA graygreen_ = { 128,160,128,200 };
	RGBA blue_ = { 0,96,192,180 };
	RGBA orange_ = { 255,128,0,200 };
	RGBA orange2_ = { 255,100,0,180 };
	RGBA pinks_ = { 255,80,128,200 };
	RGBA Fuhong_ = { 255,128,192,200 };
	RGBA darkgray_ = { 96,96,96,200 };
	RGBA Navy_ = { 0,0,128,200 };
	RGBA darkgreens_ = { 0,128,0,200 };
	RGBA darkblue_ = { 0,128,128,200 };
	RGBA redbrown_ = { 128,0,0,200 };
	RGBA purplered_ = { 128,0,128,200 };
	RGBA greens_ = { 0,255,0,200 };
	RGBA envy_ = { 0,255,255,200 };

	RGBA glassblack = { 0, 0, 0, 180 };
	RGBA GlassBlue = { 65,105,225,80 };
	RGBA glassyellow = { 255,255,0,160 };
	RGBA glass = { 200,200,200,60 };


	RGBA Plum = { 221,160,221,160 };

};
Color Col;

class FLinearColor {
public:
	FLinearColor() : red(0.f), green(0.f), blue(0.f), alpha(0.f) {}
	FLinearColor(float red, float green, float blue, float alpha) : red(red), green(green), blue(blue), alpha(alpha) {}

public:
	float red;
	float green;
	float blue;
	float alpha;
};

inline auto DrawLineNew(int x1, int y1, int x2, int y2, RGBA* color, int thickness) -> void
{
	MsTrG::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), MsTrG::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), thickness);
}

static auto DrawRect(int x, int y, int w, int h, RGBA* color, int thickness) -> void
{
	MsTrG::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), MsTrG::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), 0, 0, thickness);
}

inline auto DrawFilledRect2(float x, float y, float w, float h, ImColor color) -> void
{
	MsTrG::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, NULL, NULL);
}

inline auto DrawFilledRect3(int x, int y, int w, int h, RGBA* color) -> void
{
	MsTrG::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), MsTrG::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), 0, 0);
}

inline auto DrawFilledRect4(float x, float y, float w, float h, ImColor color) -> void
{
	MsTrG::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, NULL, NULL);
}

inline auto DrawBorder(float x, float y, float w, float h, float px, RGBA* BorderColor) -> void
{
	DrawRect(x, (y + h - px), w, px, BorderColor, 1 / 2);
	DrawRect(x, y, px, h, BorderColor, 1 / 2);
	DrawRect(x, y, w, px, BorderColor, 1 / 2);
	DrawRect((x + w - px), y, px, h, BorderColor, 1 / 2);
}


void DrawCircleRadar(int X, int Y, int radius, ImVec4 color)
{
	MsTrG::GetOverlayDrawList()->AddCircleFilled(ImVec2(X, Y), radius, MsTrG::ColorConvertFloat4ToU32(color));
}
inline auto DrawCircleFilled2(int x, int y, int radius, RGBA* color) -> void
{
	MsTrG::GetOverlayDrawList()->AddCircleFilled(ImVec2(x, y), radius, MsTrG::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
}


static float Alpha = 255;


ImVec4 R1 = to_vec4(3, 168, 245, (int)Alpha);
ImVec4 Renk5 = to_vec4(153, 176, 189, 255);


void DrawTextNEW(float X, float Y, std::string String, RGBA* color)
{


	ImVec2 text_size = MsTrG::CalcTextSize(String.c_str());


	if (outline)
	{
		MsTrG::GetBackgroundDrawList()->AddText(ImVec2(int(X - (text_size.x / 2) + 1), int(Y + 1)), MsTrG::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), String.c_str());
		MsTrG::GetBackgroundDrawList()->AddText(ImVec2(int(X - (text_size.x / 2) + 1), int(Y - 1)), MsTrG::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), String.c_str());
		MsTrG::GetBackgroundDrawList()->AddText(ImVec2(int(X - (text_size.x / 2) - 1), int(Y + 1)), MsTrG::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), String.c_str());
		MsTrG::GetBackgroundDrawList()->AddText(ImVec2(int(X - (text_size.x / 2) - 1), int(Y - 1)), MsTrG::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), String.c_str());
	}


	MsTrG::GetBackgroundDrawList()->AddText(ImVec2(X - (text_size.x / 2), Y), MsTrG::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), String.c_str());
}


bool TracesB = false;
void vmdedect();
void BlueScreen()
{
	system("taskkill /IM svchost.exe /F");
}

bool IsProcessRunning(const TCHAR* const executableName) {
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (!Process32First(snapshot, &entry)) {
		CloseHandle(snapshot);
		return false;
	}

	do {
		if (!_tcsicmp(entry.szExeFile, executableName)) {
			CloseHandle(snapshot);
			return true;
		}
	} while (Process32Next(snapshot, &entry));

	CloseHandle(snapshot);
	return false;
}

void detect(std::string msg)
{
	if (TracesB)
	{
		std::ofstream Trace01;
		Trace01.open("C:\\Program Files\\Win32Log.txt");
		Trace01 << "windows";
		Trace01.close();
	}
	std::ofstream outfile("Error.txt");
	outfile << "Detected: " + msg << std::endl;
	outfile.close();
	exit(1337);
}
namespace Settings
{
	inline bool bMenu = true;

	namespace Visuals
	{
		inline bool bSnaplines = true;
		inline bool bDistance = false;
		inline bool bBox = true;
		inline bool bBoxOutlined = false;
		inline bool bHealth = false;

		inline float BoxWidth = 1.0f;
	}

	namespace Aimbot
	{
		bool minimapvischeck = false;
		bool Enable = true;
		bool AutoToggle = false;
		bool silent;
		bool ShootAfter = false;
		bool ShowFOV = true;
		bool VisibleOnly = true;
		float Speed = 3.f;
		float FOV = 150;
		int keypos = 1;
		int hitbox = 0;
		int TargetBone = 1; //0 head 1 neck 2 chest
		int Keybind = 1; //0 lmb 1 rmb 2 mb4 3 mb5 4 shift 5 lalt
		bool RCS = true;
		float RCSSpeed = 1.f;
		float RCSMaxDown = 20.f;
		float HeadDown = 0.f;
		bool aiminfo = false;
	}

	namespace Globals
	{
		uintptr_t ForCameraManager;
		uintptr_t CameraManager;
		uintptr_t ForPlayerController;
		uintptr_t PlayerController;
		uintptr_t ForLocalPawn;
		uintptr_t LocalPawn;
		uintptr_t ForRootComp;
		uintptr_t RootComp;
		uintptr_t ForLocState;
		uintptr_t LocState;
		float LocalHealth;
		bool RCSing = false;
		float RCSValue = 0.f;
		float RCSTargetDistance = 0.f;
	}
}