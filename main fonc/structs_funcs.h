#include "theme.h"
#include "xor.h"
#include <dxgi.h>
#include <algorithm>
#include "font.h"
#include <random>
struct fvector
{
	fvector() : X(), Y(), Z() { }
	fvector(float X, float Y, float Z) : X(X), Y(Y), Z(Z) { }

	fvector operator + (const fvector& other) const { return { this->X + other.X, this->Y + other.Y, this->Z + other.Z }; }
	fvector operator - (const fvector& other) const { return { this->X - other.X, this->Y - other.Y, this->Z - other.Z }; }
	fvector operator * (float offset) const { return { this->X * offset, this->Y * offset, this->Z * offset }; }
	fvector operator / (float offset) const { return { this->X / offset, this->Y / offset, this->Z / offset }; }

	fvector& operator *= (const float other) { this->X *= other; this->Y *= other; this->Z *= other; return *this; }
	fvector& operator /= (const float other) { this->X /= other; this->Y /= other; this->Z /= other; return *this; }

	fvector& operator = (const fvector& other) { this->X = other.X; this->Y = other.Y; this->Z = other.Z; return *this; }
	fvector& operator += (const fvector& other) { this->X += other.X; this->Y += other.Y; this->Z += other.Z; return *this; }
	fvector& operator -= (const fvector& other) { this->X -= other.X; this->Y -= other.Y; this->Z -= other.Z; return *this; }
	fvector& operator *= (const fvector& other) { this->X *= other.X; this->Y *= other.Y; this->Z *= other.Z; return *this; }
	fvector& operator /= (const fvector& other) { this->X /= other.X; this->Y /= other.Y; this->Z /= other.Z; return *this; }

	operator bool() { return bool(this->X || this->Y || this->Z); }
	friend bool operator == (const fvector& a, const fvector& b) { return a.X == b.X && a.Y == b.Y && a.Z == b.Z; }
	friend bool operator != (const fvector& a, const fvector& b) { return !(a == b); }

	float Dot(const fvector& V) { return X * V.X + Y * V.Y + Z * V.Z; }
	float Distance(fvector V) { return float(sqrtf(powf(V.X - this->X, 2.0) + powf(V.Y - this->Y, 2.0) + powf(V.Z - this->Z, 2.0))); }
	float Length() { return float(sqrtf((X * X) + (Y * Y) + (Z * Z))); }
	float X, Y, Z;
};

struct FVector2D
{
	FVector2D() : X(), Y() { }
	FVector2D(float X, float Y) : X(X), Y(Y) { }

	FVector2D operator + (const FVector2D& other) const { return { this->X + other.X, this->Y + other.Y }; }
	FVector2D operator - (const FVector2D& other) const { return { this->X - other.X, this->Y - other.Y }; }
	FVector2D operator * (float offset) const { return { this->X * offset, this->Y * offset }; }
	FVector2D operator / (float offset) const { return { this->X / offset, this->Y / offset }; }

	FVector2D& operator *= (const float other) { this->X *= other; this->Y *= other; return *this; }
	FVector2D& operator /= (const float other) { this->X /= other; this->Y /= other; return *this; }

	FVector2D& operator = (const FVector2D& other) { this->X = other.X; this->Y = other.Y; return *this; }
	FVector2D& operator += (const FVector2D& other) { this->X += other.X; this->Y += other.Y; return *this; }
	FVector2D& operator -= (const FVector2D& other) { this->X -= other.X; this->Y -= other.Y; return *this; }
	FVector2D& operator *= (const FVector2D& other) { this->X *= other.X; this->Y *= other.Y; return *this; }
	FVector2D& operator /= (const FVector2D& other) { this->X /= other.X; this->Y /= other.Y; return *this; }

	operator bool() { return bool(this->X || this->Y); }
	friend bool operator == (const FVector2D& A, const FVector2D& B) { return A.X == B.X && A.Y == A.Y; }
	friend bool operator != (const FVector2D& A, const FVector2D& B) { return !(A == B); }

	float Dot(const FVector2D& V) { return X * V.X + Y * V.Y; }
	float Distance(FVector2D V) { return float(sqrtf(powf(V.X - this->X, 2.0) + powf(V.Y - this->Y, 2.0))); }

	float X, Y;
};


struct fquat {
	float X;
	float Y;
	float Z;
	float W;
};


struct FRotator
{
	FRotator() : Pitch(), Yaw(), Roll() { }
	FRotator(float Pitch, float Yaw, float Roll) : Pitch(Pitch), Yaw(Yaw), Roll(Roll) { }

	FRotator operator + (const FRotator& other) const { return { this->Pitch + other.Pitch, this->Yaw + other.Yaw, this->Roll + other.Roll }; }
	FRotator operator - (const FRotator& other) const { return { this->Pitch - other.Pitch, this->Yaw - other.Yaw, this->Roll - other.Roll }; }
	FRotator operator * (float offset) const { return { this->Pitch * offset, this->Yaw * offset, this->Roll * offset }; }
	FRotator operator / (float offset) const { return { this->Pitch / offset, this->Yaw / offset, this->Roll / offset }; }

	FRotator& operator *= (const float other) { this->Pitch *= other; this->Yaw *= other; this->Roll *= other; return *this; }
	FRotator& operator /= (const float other) { this->Pitch /= other; this->Yaw /= other; this->Roll /= other; return *this; }

	FRotator& operator = (const FRotator& other) { this->Pitch = other.Pitch; this->Yaw = other.Yaw; this->Roll = other.Roll; return *this; }
	FRotator& operator += (const FRotator& other) { this->Pitch += other.Pitch; this->Yaw += other.Yaw; this->Roll += other.Roll; return *this; }
	FRotator& operator -= (const FRotator& other) { this->Pitch -= other.Pitch; this->Yaw -= other.Yaw; this->Roll -= other.Roll; return *this; }
	FRotator& operator *= (const FRotator& other) { this->Pitch *= other.Pitch; this->Yaw *= other.Yaw; this->Roll *= other.Roll; return *this; }
	FRotator& operator /= (const FRotator& other) { this->Pitch /= other.Pitch; this->Yaw /= other.Yaw; this->Roll /= other.Roll; return *this; }

	operator bool() { return bool(this->Pitch || this->Yaw || this->Roll); }
	friend bool operator == (const FRotator& a, const FRotator& b) { return a.Pitch == b.Pitch && a.Yaw == b.Yaw && a.Roll == b.Roll; }
	friend bool operator != (const FRotator& a, const FRotator& b) { return !(a == b); }

	float Dot(const FRotator& V) { return Pitch * V.Pitch + Yaw * V.Yaw + Roll * V.Roll; }
	float Distance(FRotator V) { return float(sqrtf(powf(V.Pitch - this->Pitch, 2.0) + powf(V.Yaw - this->Yaw, 2.0) + powf(V.Roll - this->Roll, 2.0))); }

	float Pitch, Yaw, Roll;
};

struct FPlane : fvector
{
	FPlane() : W() { }
	FPlane(float W) : W(W) { }

	float W;
};
struct ftransform {
	fquat rot;
	fvector translation;
	char pad[4];
	fvector scale;
	char pad1[4];
	D3DMATRIX ToMatrixWithScale() {
		D3DMATRIX m;
		m._41 = translation.X;
		m._42 = translation.Y;
		m._43 = translation.Z;

		float x2 = rot.X + rot.X;
		float y2 = rot.Y + rot.Y;
		float z2 = rot.Z + rot.Z;

		float xx2 = rot.X * x2;
		float yy2 = rot.Y * y2;
		float zz2 = rot.Z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.X;
		m._22 = (1.0f - (xx2 + zz2)) * scale.Y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.Z;

		float yz2 = rot.Y * z2;
		float wx2 = rot.W * x2;
		m._32 = (yz2 - wx2) * scale.Z;
		m._23 = (yz2 + wx2) * scale.Y;

		float xy2 = rot.X * y2;
		float wz2 = rot.W * z2;
		m._21 = (xy2 - wz2) * scale.Y;
		m._12 = (xy2 + wz2) * scale.X;

		float xz2 = rot.X * z2;
		float wy2 = rot.W * y2;
		m._31 = (xz2 + wy2) * scale.Z;
		m._13 = (xz2 - wy2) * scale.X;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};
fvector cntrlrttn;
namespace LAmth
{
	D3DMATRIX matrix(fvector rot, fvector origin = fvector(0, 0, 0)) {
		float radPitch = (rot.X * float(M_PI) / 180.f);
		float radYaw = (rot.Y * float(M_PI) / 180.f);
		float radRoll = (rot.Z * float(M_PI) / 180.f);

		float SP = sinf(radPitch);
		float CP = cosf(radPitch);
		float SY = sinf(radYaw);
		float CY = cosf(radYaw);
		float SR = sinf(radRoll);
		float CR = cosf(radRoll);

		D3DMATRIX matrix;
		matrix.m[0][0] = CP * CY;
		matrix.m[0][1] = CP * SY;
		matrix.m[0][2] = SP;
		matrix.m[0][3] = 0.f;

		matrix.m[1][0] = SR * SP * CY - CR * SY;
		matrix.m[1][1] = SR * SP * SY + CR * CY;
		matrix.m[1][2] = -SR * CP;
		matrix.m[1][3] = 0.f;

		matrix.m[2][0] = -(CR * SP * CY + SR * SY);
		matrix.m[2][1] = CY * SR - CR * SP * SY;
		matrix.m[2][2] = CR * CP;
		matrix.m[2][3] = 0.f;

		matrix.m[3][0] = origin.X;
		matrix.m[3][1] = origin.Y;
		matrix.m[3][2] = origin.Z;
		matrix.m[3][3] = 1.f;

		return matrix;
	}

	D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
	{
		D3DMATRIX pOut;
		pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
		pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
		pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
		pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
		pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
		pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
		pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
		pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
		pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
		pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
		pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
		pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
		pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
		pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
		pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
		pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

		return pOut;
	}
}

struct FMinimalViewInfo {
	struct fvector Location;
	struct fvector Rotation;
	float FOV;
};


namespace LAGRDPNT
{
	uintptr_t guard_local_pawn = 0x0;
	uintptr_t guard_controller = 0x0;
}

namespace LAPNT
{
	uintptr_t local_pawn;
	uintptr_t local_pawn_old;
	uintptr_t player_controller;
	uintptr_t camera_manager;
	fvector camera_position;
}


namespace LACAMRA
{
	fvector location;
	fvector rotation;
	float fov;
}



struct Enemy
{
	uintptr_t for_actor;
	uintptr_t for_mesh;

	uintptr_t actor;
	uintptr_t mesh;
	uintptr_t bone_array;
	uintptr_t root_component;
	uintptr_t damage_handler;

	INT32 bone_count;
	INT32 ammo_count;

	std::string weapon_name;
	std::string agent_name;
	std::string player_name;

	float distance;
	float health;
	float shield;

	bool is_valid;
	bool is_damage_handler_guarded;
	bool is_mesh_guarded;
};

std::vector<Enemy> player_pawns;

inline bool operator==(const Enemy& a, const Enemy& b) {
	if (a.actor == b.actor)
		return true;

	return false;
}


void LAPLAMP(fvector& Ang) {
	if (Ang.X < 0.f)
		Ang.X += 360.f;

	if (Ang.X > 360.f)
		Ang.X -= 360.f;

	if (Ang.Y < 0.f) Ang.Y += 360.f;
	if (Ang.Y > 360.f) Ang.Y -= 360.f;
	Ang.Z = 0.f;
}


static ULONG Width = GetSystemMetrics(SM_CXSCREEN);
static ULONG Height = GetSystemMetrics(SM_CYSCREEN);



fvector projw2s(fvector world_location) {
	fvector ScreenLocation;
	D3DMATRIX tempMatrix = LAmth::matrix(LACAMRA::rotation, fvector(0, 0, 0));
	fvector vAxisX, vAxisY, vAxisZ;
	vAxisX = fvector(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = fvector(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = fvector(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);
	fvector vDelta = world_location - LACAMRA::location;
	fvector vTransformed = fvector(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));
	if (vTransformed.Z < .1f)
		vTransformed.Z = .1f;


	float ScreenCenterX = Width / 2.0f;
	float ScreenCenterY = Height / 2.0f;
	ScreenLocation.X = ScreenCenterX + vTransformed.X * (ScreenCenterX / tanf(LACAMRA::fov * (float)M_PI / 360.f)) / vTransformed.Z;
	ScreenLocation.Y = ScreenCenterY - vTransformed.Y * (ScreenCenterX / tanf(LACAMRA::fov * (float)M_PI / 360.f)) / vTransformed.Z;
	return ScreenLocation;
}

boolean in_rect(double centerX, double centerY, double radius, double x, double y) {
	return x >= centerX - radius && x <= centerX + radius &&
		y >= centerY - radius && y <= centerY + radius;
}
struct Vector2 {
public:
	float X;
	float Y;

	inline Vector2() : X(0), Y(0) {}
	inline Vector2(float X, float Y) : X(X), Y(Y) {}

	inline float Distance(Vector2 v) {
		return sqrtf(((v.X - X) * (v.X - X) + (v.Y - Y) * (v.Y - Y)));
	}

	inline Vector2 operator+(const Vector2& v) const {
		return Vector2(X + v.X, Y + v.Y);
	}

	inline Vector2 operator-(const Vector2& v) const {
		return Vector2(X - v.X, Y - v.Y);
	}
};
struct FNameEntryHandle {
	uint16_t bIsWide : 1;
	uint16_t Len : 15;
};
struct FNameEntry {

	int32_t ComparisonId;
	FNameEntryHandle Header;
	union
	{
		char    AnsiName[1024]; // ANSICHAR
		wchar_t    WideName[1024]; // WIDECHAR
	};


	wchar_t const* GetWideName() const { return WideName; }
	char const* GetAnsiName() const { return AnsiName; }
	bool IsWide() const { return Header.bIsWide; }
};

std::string get_fname(int key)
{
	UINT chunkOffset = (UINT)((int)(key) >> 16);
	USHORT nameOffset = (USHORT)key;
	std::uint64_t namePoolChunk = read<std::uint64_t>((std::uintptr_t)(mem::base_address() + 0xA4B9B80 + ((chunkOffset + 2) * 8)));
	std::uint64_t entryOffset = namePoolChunk + (ULONG)(4 * nameOffset);
	FNameEntry nameEntry = read<FNameEntry>(entryOffset);

	auto name = nameEntry.AnsiName; //just ansiname fuck widename
	std::uintptr_t nameKey = read2<uintptr_t>(virtualaddy + 0x0);

	for (std::uint16_t i = 0; i < nameEntry.Header.Len; i++)
	{
		BYTE b = i & 3;
		name[i] ^= nameEntry.Header.Len ^ *((LPBYTE)&nameKey + b);
	}

	return name;
}


struct VelorantItem
{
	uintptr_t ForActor, ForMesh, ForDamageHandler, ForRootComp, ForMinimap;

	uintptr_t Actor, BoneArray, Mesh, DamageHandler, RootComp, Minimap;
	float dist;
	std::string actorname;

	friend bool operator == (const VelorantItem& A, const VelorantItem& B) { return bool(A.Actor == B.Actor); }
}; std::vector<VelorantItem> VelorantItems;

float LACALDISTNC(int p1x, int p1y, int p2x, int p2y)
{
	float diffY = p1y - p2y;
	float diffX = p1x - p2x;
	return sqrt((diffY * diffY) + (diffX * diffX));
}

void LAPNRMLZE(fvector& in)
{
	if (in.X > 89.f) in.X -= 360.f;
	else if (in.X < -89.f) in.X += 360.f;

	while (in.Y > 180)in.Y -= 360;
	while (in.Y < -180)in.Y += 360;
	in.Z = 0;
}

void LAPNRMLZEANGL(fvector& angle)
{
	while (angle.X > 89.0f)
		angle.X -= 180.f;

	while (angle.X < -89.0f)
		angle.X += 180.f;

	while (angle.Y > 180.f)
		angle.Y -= 360.f;

	while (angle.Y < -180.f)
		angle.Y += 360.f;
}
float LAFOVGT(const fvector viewAngle, const fvector aimAngle)
{
	fvector Delta = fvector(aimAngle.X - viewAngle.X, aimAngle.Y - viewAngle.Y, aimAngle.Z - viewAngle.Z);
	LAPNRMLZEANGL(Delta);
	return sqrt(pow(Delta.X, 2.0f) + pow(Delta.Y, 2.0f));
}
namespace Internal
{
	ImDrawList* DrawList;
	ImDrawList* ForegroundDrawList;
}
struct FLinearColor2
{
	FLinearColor2() : R(), G(), B(), A() { }
	FLinearColor2(float R, float G, float B, float A) : R(R), G(G), B(B), A(A) { }

	float R, G, B, A;
};
void asdtexxxt(const char* Content, fvector At, FLinearColor2 RenderColor, bool Centered, bool Outlined, bool Foreground = false)
{
	ImDrawList* DrawList = Foreground ? Internal::ForegroundDrawList : Internal::DrawList;

	if (Centered)
	{
		At.X -= float(MsTrG::CalcTextSize(Content).x / 2.0);
	}

	if (Outlined)
	{
		DrawList->AddText(ImVec2(int(At.X + 1), int(At.Y + 1)), MsTrG::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), Content);
		DrawList->AddText(ImVec2(int(At.X + 1), int(At.Y - 1)), MsTrG::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), Content);
		DrawList->AddText(ImVec2(int(At.X - 1), int(At.Y + 1)), MsTrG::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), Content);
		DrawList->AddText(ImVec2(int(At.X - 1), int(At.Y - 1)), MsTrG::GetColorU32({ 0.f, 0.f, 0.f, 1.f }), Content);
	}

	DrawList->AddText(ImVec2(int(At.X), int(At.Y)), MsTrG::GetColorU32({ float(RenderColor.R), float(RenderColor.G), float(RenderColor.B), float(RenderColor.A) }), Content);
}


float LAPTDGRTRDN(float degree)
{
	return degree * (M_PI / 180);
}


void LApangrtn(const fvector& angles, fvector* forward)
{
	float	sp, sy, cp, cy;

	sy = sin(LAPTDGRTRDN(angles.Y));
	cy = cos(LAPTDGRTRDN(angles.Y));

	sp = sin(LAPTDGRTRDN(angles.X));
	cp = cos(LAPTDGRTRDN(angles.X));

	forward->X = cp * cy;
	forward->Y = cp * sy;
	forward->Z = -sp;
}

fvector bone_matrix(int index, Enemy _player)
{
	size_t size = sizeof(ftransform);
	ftransform first_bone, comp_to_world;

	first_bone = read<ftransform>(_player.bone_array + (size * index));
	if (_player.is_mesh_guarded)
	{
		comp_to_world = read<ftransform>(virtualaddy + _player.mesh + 0x250);
	}
	else
	{
		comp_to_world = read<ftransform>(_player.mesh + 0x250);
	}

	D3DMATRIX matrix = LAmth::MatrixMultiplication(first_bone.ToMatrixWithScale(), comp_to_world.ToMatrixWithScale());
	return fvector(matrix._41, matrix._42, matrix._43);
}

fvector cLAmpasdp205(fvector src, fvector dst)
{
	fvector angle;
	angle.X = -atan2f(dst.X - src.X, dst.Y - src.Y) / M_PI * 180.0f + 180.0f;
	angle.Y = asinf((dst.Z - src.Z) / src.Distance(dst)) * 180.0f / M_PI;
	angle.Z = 0.0f;

	return angle;
}

fvector LAPSNNANGLE(fvector src, fvector dst)
{
	fvector angle;
	fvector delta = fvector((src.X - dst.X), (src.Y - dst.Y), (src.Z - dst.Z));

	double hyp = sqrtf(delta.X * delta.X + delta.Y * delta.Y);

	angle.X = atanf(delta.Z / hyp) * (180.0f / hyp);
	angle.Y = atanf(delta.Y / delta.X) * (180.0f / M_PI);
	angle.Z = 0;
	if (delta.X >= 0.0) angle.Y += 180.0f;

	return angle;
}
void DrawFilledRect3(int x, int y, int w, int h, ImVec4 Color)
{
	MsTrG::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), MsTrG::GetColorU32({ Color.x, Color.y, Color.z, Color.w }), 0, 0);

}
auto DrawLine(const ImVec2& x, const ImVec2 y, ImU32 color, const FLOAT width) -> void
{
	MsTrG::GetBackgroundDrawList()->AddLine(x, y, color, width);
}
void DrawCircleFilled(int x, int y, int radius, ImVec4 color, float segments)
{
	MsTrG::GetBackgroundDrawList()->AddCircleFilled(ImVec2(x, y), radius, MsTrG::ColorConvertFloat4ToU32(color), segments);
}


void skeleton_box(Enemy _player, ImVec4 col, float thickness)
{
	fvector bone_head, bone_neck, bone_chest, bone_pelvis, bone_rshoulder, bone_relbow, bone_rhand, bone_rthigh, bone_rknee, bone_rfoot, bone_lshoulder, bone_lelbow, bone_lhand, bone_lthigh, bone_lknee, bone_lfoot;
	bone_head = bone_matrix(8, _player);
	bone_chest = bone_matrix(6, _player);
	bone_pelvis = bone_matrix(3, _player);


	if (_player.bone_count == 101) //female
	{
		bone_neck = bone_matrix(21, _player);

		bone_lshoulder = bone_matrix(23, _player);
		bone_lelbow = bone_matrix(24, _player);
		bone_lhand = bone_matrix(25, _player);

		bone_rshoulder = bone_matrix(49, _player);
		bone_relbow = bone_matrix(50, _player);
		bone_rhand = bone_matrix(51, _player);

		bone_lthigh = bone_matrix(75, _player);
		bone_lknee = bone_matrix(76, _player);
		bone_lfoot = bone_matrix(78, _player);

		bone_rthigh = bone_matrix(82, _player);
		bone_rknee = bone_matrix(83, _player);
		bone_rfoot = bone_matrix(85, _player);
	}
	else if (_player.bone_count == 103) //npc
	{
		bone_neck = bone_matrix(9, _player);

		bone_lshoulder = bone_matrix(33, _player);
		bone_lelbow = bone_matrix(30, _player);
		bone_lhand = bone_matrix(32, _player);

		bone_rshoulder = bone_matrix(58, _player);
		bone_relbow = bone_matrix(55, _player);
		bone_rhand = bone_matrix(57, _player);

		bone_lthigh = bone_matrix(63, _player);
		bone_lknee = bone_matrix(65, _player);
		bone_lfoot = bone_matrix(69, _player);

		bone_rthigh = bone_matrix(77, _player);
		bone_rknee = bone_matrix(79, _player);
		bone_rfoot = bone_matrix(83, _player);
	}
	else if (_player.bone_count == 104) //male
	{
		bone_neck = bone_matrix(21, _player);

		bone_lshoulder = bone_matrix(23, _player);
		bone_lelbow = bone_matrix(24, _player);
		bone_lhand = bone_matrix(25, _player);

		bone_rshoulder = bone_matrix(49, _player);
		bone_relbow = bone_matrix(50, _player);
		bone_rhand = bone_matrix(51, _player);

		bone_lthigh = bone_matrix(77, _player);
		bone_lknee = bone_matrix(78, _player);
		bone_lfoot = bone_matrix(80, _player);

		bone_rthigh = bone_matrix(84, _player);
		bone_rknee = bone_matrix(85, _player);
		bone_rfoot = bone_matrix(87, _player);
	}


	bone_head = projw2s(bone_head);
	bone_neck = projw2s(bone_neck);
	bone_chest = projw2s(bone_chest);
	bone_pelvis = projw2s(bone_pelvis);
	bone_lshoulder = projw2s(bone_lshoulder);
	bone_lelbow = projw2s(bone_lelbow);
	bone_lhand = projw2s(bone_lhand);
	bone_rshoulder = projw2s(bone_rshoulder);
	bone_relbow = projw2s(bone_relbow);
	bone_rhand = projw2s(bone_rhand);
	bone_lthigh = projw2s(bone_lthigh);
	bone_lknee = projw2s(bone_lknee);
	bone_lfoot = projw2s(bone_lfoot);
	bone_rthigh = projw2s(bone_rthigh);
	bone_rknee = projw2s(bone_rknee);
	bone_rfoot = projw2s(bone_rfoot);

	ImDrawList* draw = MsTrG::GetBackgroundDrawList();

	//top stuff
	draw->AddLine(ImVec2(bone_head.X, bone_head.Y), ImVec2(bone_neck.X, bone_neck.Y), MsTrG::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
	draw->AddLine(ImVec2(bone_neck.X, bone_neck.Y), ImVec2(bone_chest.X, bone_chest.Y), MsTrG::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
	draw->AddLine(ImVec2(bone_chest.X, bone_chest.Y), ImVec2(bone_pelvis.X, bone_pelvis.Y), MsTrG::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);

	//right arm
	draw->AddLine(ImVec2(bone_chest.X, bone_chest.Y), ImVec2(bone_rshoulder.X, bone_rshoulder.Y), MsTrG::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
	draw->AddLine(ImVec2(bone_rshoulder.X, bone_rshoulder.Y), ImVec2(bone_relbow.X, bone_relbow.Y), MsTrG::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
	draw->AddLine(ImVec2(bone_relbow.X, bone_relbow.Y), ImVec2(bone_rhand.X, bone_rhand.Y), MsTrG::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);

	//left arm
	draw->AddLine(ImVec2(bone_chest.X, bone_chest.Y), ImVec2(bone_lshoulder.X, bone_lshoulder.Y), MsTrG::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
	draw->AddLine(ImVec2(bone_lshoulder.X, bone_lshoulder.Y), ImVec2(bone_lelbow.X, bone_lelbow.Y), MsTrG::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
	draw->AddLine(ImVec2(bone_lelbow.X, bone_lelbow.Y), ImVec2(bone_lhand.X, bone_lhand.Y), MsTrG::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);

	//right foot
	draw->AddLine(ImVec2(bone_pelvis.X, bone_pelvis.Y), ImVec2(bone_rthigh.X, bone_rthigh.Y), MsTrG::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
	draw->AddLine(ImVec2(bone_rthigh.X, bone_rthigh.Y), ImVec2(bone_rknee.X, bone_rknee.Y), MsTrG::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
	draw->AddLine(ImVec2(bone_rknee.X, bone_rknee.Y), ImVec2(bone_rfoot.X, bone_rfoot.Y), MsTrG::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);

	//left foot
	draw->AddLine(ImVec2(bone_pelvis.X, bone_pelvis.Y), ImVec2(bone_lthigh.X, bone_lthigh.Y), MsTrG::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
	draw->AddLine(ImVec2(bone_lthigh.X, bone_lthigh.Y), ImVec2(bone_lknee.X, bone_lknee.Y), MsTrG::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
	draw->AddLine(ImVec2(bone_lknee.X, bone_lknee.Y), ImVec2(bone_lfoot.X, bone_lfoot.Y), MsTrG::GetColorU32({ col.x, col.y, col.z, col.w }), thickness);
}
void MapAgentName(std::string& namee)
{
	if (namee.find("Wushu") != std::string::npos)
		namee = "Jett";
	else if (namee.find("Rift") != std::string::npos)
		namee = "Astra";
	else if (namee.find("Grenadier") != std::string::npos)
		namee = "Kay/O";
	else if (namee.find("Breach") != std::string::npos)
		namee = "Breach";
	else if (namee.find("Sarge") != std::string::npos)
		namee = "Brimstone";
	else if (namee.find("Deadeye") != std::string::npos)
		namee = "Chamber";
	else if (namee.find("Gumshoe") != std::string::npos)
		namee = "Cypher";
	else if (namee.find("Killjoy") != std::string::npos)
		namee = "Killjoy";
	else if (namee.find("Sprinter") != std::string::npos)
		namee = "Neon";
	else if (namee.find("Wraith") != std::string::npos)
		namee = "Omen";
	else if (namee.find("Phoenix") != std::string::npos)
		namee = "Phoenix";
	else if (namee.find("Clay") != std::string::npos)
		namee = "Raze";
	else if (namee.find("Vampire") != std::string::npos)
		namee = "Reyna";
	else if (namee.find("Thorne") != std::string::npos)
		namee = "Sage";
	else if (namee.find("Guide") != std::string::npos)
		namee = "Skye";
	else if (namee.find("Hunter_PC_C") != std::string::npos)
		namee = "Sova";
	else if (namee.find("Pandemic") != std::string::npos)
		namee = "Viper";
	else if (namee.find("Stealth") != std::string::npos)
		namee = "Yoru";
	else if (namee.find("BountyHunter") != std::string::npos)
		namee = "Fade";
	else if (namee.find("AggroBot") != std::string::npos)
		namee = "Gekko";
	else if (namee.find("Mage") != std::string::npos)
		namee = "Harbor";
	else if (namee.find("Cable") != std::string::npos)
		namee = "Deadlock";
	else if (namee.find("Sequoia") != std::string::npos)
		namee = "Iso";
	else if (namee.find("TrainingBot") != std::string::npos)
		namee = "Bot";
}
