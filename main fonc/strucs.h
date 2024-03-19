#pragma once

namespace Valorant
{
	namespace SDK
	{


		struct FVector
		{
			FVector() : X(), Y(), Z() { }
			FVector(float X, float Y, float Z) : X(X), Y(Y), Z(Z) { }

			FVector operator + (const FVector& other) const { return { this->X + other.X, this->Y + other.Y, this->Z + other.Z }; }
			FVector operator - (const FVector& other) const { return { this->X - other.X, this->Y - other.Y, this->Z - other.Z }; }
			FVector operator * (float offset) const { return { this->X * offset, this->Y * offset, this->Z * offset }; }
			FVector operator / (float offset) const { return { this->X / offset, this->Y / offset, this->Z / offset }; }

			FVector& operator *= (const float other) { this->X *= other; this->Y *= other; this->Z *= other; return *this; }
			FVector& operator /= (const float other) { this->X /= other; this->Y /= other; this->Z /= other; return *this; }

			FVector& operator = (const FVector& other) { this->X = other.X; this->Y = other.Y; this->Z = other.Z; return *this; }
			FVector& operator += (const FVector& other) { this->X += other.X; this->Y += other.Y; this->Z += other.Z; return *this; }
			FVector& operator -= (const FVector& other) { this->X -= other.X; this->Y -= other.Y; this->Z -= other.Z; return *this; }
			FVector& operator *= (const FVector& other) { this->X *= other.X; this->Y *= other.Y; this->Z *= other.Z; return *this; }
			FVector& operator /= (const FVector& other) { this->X /= other.X; this->Y /= other.Y; this->Z /= other.Z; return *this; }

			operator bool() { return bool(this->X || this->Y || this->Z); }
			friend bool operator == (const FVector& a, const FVector& b) { return a.X == b.X && a.Y == b.Y && a.Z == b.Z; }
			friend bool operator != (const FVector& a, const FVector& b) { return !(a == b); }

			float Dot(const FVector& V) { return X * V.X + Y * V.Y + Z * V.Z; }
			float Distance(FVector V) { return float(sqrtf(powf(V.X - this->X, 2.0) + powf(V.Y - this->Y, 2.0) + powf(V.Z - this->Z, 2.0))); }

			float X, Y, Z;
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

		struct FMinimalViewInfo
		{
			FMinimalViewInfo() : Location(), Rotation(), FOV(FOV) { }
			FMinimalViewInfo(FVector Location, FRotator Rotation, float FOV) : Location(Location), Rotation(Rotation), FOV(FOV) { }

			FVector Location;
			FRotator Rotation;
			float FOV;
		};

		struct FLinearColor
		{
			FLinearColor() : R(), G(), B(), A() { }
			FLinearColor(float R, float G, float B, float A) : R(R), G(G), B(B), A(A) { }

			float R, G, B, A;
		};

		struct FPlane : FVector
		{
			FPlane() : W() { }
			FPlane(float W) : W(W) { }

			float W;
		};

		struct FMatrix
		{
			FMatrix() : XPlane(), YPlane(), ZPlane(), WPlane() { }
			FMatrix(FPlane XPlane, FPlane YPlane, FPlane ZPlane, FPlane WPlane) : XPlane(XPlane), YPlane(YPlane), ZPlane(ZPlane), WPlane(WPlane) { }

			FMatrix MatrixMultiplication(FMatrix Other)
			{
				FMatrix Copy = {};

				Copy.WPlane.X = this->WPlane.X * Other.XPlane.X + this->WPlane.Y * Other.YPlane.X + this->WPlane.Z * Other.ZPlane.X + this->WPlane.W * Other.WPlane.X;
				Copy.WPlane.Y = this->WPlane.X * Other.XPlane.Y + this->WPlane.Y * Other.YPlane.Y + this->WPlane.Z * Other.ZPlane.Y + this->WPlane.W * Other.WPlane.Y;
				Copy.WPlane.Z = this->WPlane.X * Other.XPlane.Z + this->WPlane.Y * Other.YPlane.Z + this->WPlane.Z * Other.ZPlane.Z + this->WPlane.W * Other.WPlane.Z;
				Copy.WPlane.W = this->WPlane.X * Other.XPlane.W + this->WPlane.Y * Other.YPlane.W + this->WPlane.Z * Other.ZPlane.W + this->WPlane.W * Other.WPlane.W;

				return Copy;
			}

			static FMatrix RotatorToMatrix(FRotator Rotation)
			{
				FMatrix Copy = {};

				float Pitch = (Rotation.Pitch * 3.14159265358979323846 / 180);
				float Yaw = (Rotation.Yaw * 3.14159265358979323846 / 180);
				float Roll = (Rotation.Roll * 3.14159265358979323846 / 180);

				float SP = sin(Pitch);
				float CP = cos(Pitch);
				float SY = sin(Yaw);
				float CY = cos(Yaw);
				float SR = sin(Roll);
				float CR = cos(Roll);

				Copy.XPlane.X = CP * CY;
				Copy.XPlane.Y = CP * SY;
				Copy.XPlane.Z = SP;

				Copy.YPlane.X = SR * SP * CY - CR * SY;
				Copy.YPlane.Y = SR * SP * SY + CR * CY;
				Copy.YPlane.Z = -SR * CP;

				Copy.ZPlane.X = -(CR * SP * CY + SR * SY);
				Copy.ZPlane.Y = CY * SR - CR * SP * SY;
				Copy.ZPlane.Z = CR * CP;

				Copy.WPlane.W = 1.f;

				return Copy;
			}

			FPlane XPlane, YPlane, ZPlane, WPlane;
		};

		struct FTransform
		{
			FTransform() : Rotation(), Translation(), Scale3D(), UnknownValue() { }
			FTransform(FPlane Rotation, FPlane Translation, FVector Scale3D, float UnknownValue) : Rotation(Rotation), Translation(Translation), Scale3D(Scale3D), UnknownValue(UnknownValue) { }

			FMatrix ToMatrix()
			{
				FMatrix Copy = {};

				auto x2 = this->Rotation.X * 2;
				auto y2 = this->Rotation.Y * 2;
				auto z2 = this->Rotation.Z * 2;

				auto xx2 = this->Rotation.X * x2;
				auto yy2 = this->Rotation.Y * y2;
				auto zz2 = this->Rotation.Z * z2;

				auto yz2 = this->Rotation.Y * z2;
				auto wx2 = this->Rotation.W * x2;

				auto xy2 = this->Rotation.X * y2;
				auto wz2 = this->Rotation.W * z2;

				auto xz2 = this->Rotation.X * z2;
				auto wy2 = this->Rotation.W * y2;

				Copy.XPlane.X = (1.0 - (yy2 + zz2)) * this->Scale3D.X;
				Copy.XPlane.Y = (xy2 + wz2) * this->Scale3D.X;
				Copy.XPlane.Z = (xz2 - wy2) * this->Scale3D.X;

				Copy.YPlane.X = (xy2 - wz2) * this->Scale3D.Y;
				Copy.YPlane.Y = (1.0 - (xx2 + zz2)) * this->Scale3D.Y;
				Copy.YPlane.Z = (yz2 + wx2) * this->Scale3D.Y;

				Copy.ZPlane.X = (xz2 + wy2) * this->Scale3D.Z;
				Copy.ZPlane.Y = (yz2 - wx2) * this->Scale3D.Z;
				Copy.ZPlane.Z = (1.0 - (xx2 + yy2)) * this->Scale3D.Z;

				Copy.WPlane.X = this->Translation.X;
				Copy.WPlane.Y = this->Translation.Y;
				Copy.WPlane.Z = this->Translation.Z;

				Copy.WPlane.W = 1.0;

				return Copy;
			}

			FPlane Rotation;
			FPlane Translation;
			FVector Scale3D;
			float UnknownValue;
		};
	}
}