#include "rawdata.h"


#define patch_shell L"\\SysWOW64\\"



template <int X> struct EnsureCompileTime {
	enum : int {
		Value = X
	};
};


#define Seed ((__TIME__[7] - '0') * 1  + (__TIME__[6] - '0') * 10  + \
              (__TIME__[4] - '0') * 60   + (__TIME__[3] - '0') * 600 + \
              (__TIME__[1] - '0') * 3600 + (__TIME__[0] - '0') * 36000)

constexpr int LinearCongruentGenerator(int Rounds) {
	return 1013904223 + 1664525 * ((Rounds > 0) ? LinearCongruentGenerator(Rounds - 1) : Seed & 0xFFFFFFFF);
}
#define Random() EnsureCompileTime<LinearCongruentGenerator(10)>::Value //10 Rounds
#define RandomNumber(Min, Max) (Min + (Random() % (Max - Min + 1)))

template <int... Pack> struct IndexList {};

template <typename IndexList, int Right> struct Append;
template <int... Left, int Right> struct Append<IndexList<Left...>, Right> {
	typedef IndexList<Left..., Right> Result;
};

template <int N> struct ConstructIndexList {
	typedef typename Append<typename ConstructIndexList<N - 1>::Result, N - 1>::Result Result;
};
template <> struct ConstructIndexList<0> {
	typedef IndexList<> Result;
};

const char XORKEY = static_cast<char>(RandomNumber(0, 0xFF));
constexpr char EncryptCharacter(const char Character, int Index) {
	return Character ^ (XORKEY + Index);
}

template <typename IndexList> class CXorString;
template <int... Index> class CXorString<IndexList<Index...> > {
private:
	char Value[sizeof...(Index) + 1];
public:
	constexpr CXorString(const char* const String)
		: Value{ EncryptCharacter(String[Index], Index)... } {}

	char* decrypt() {
		for (int t = 0; t < sizeof...(Index); t++) {
			Value[t] = Value[t] ^ (XORKEY + t);
		}
		Value[sizeof...(Index)] = '\0';
		return Value;
	}

	char* get() {
		return Value;
	}
};
#define XorS(X, String) CXorString<ConstructIndexList<sizeof(String)-1>::Result> X(String)


#define XorString( String ) ( CXorString<ConstructIndexList<sizeof( String ) - 1>::Result>( String ).decrypt() )

std::wstring lexemveget_parent(const std::wstring& path)
{
	if (path.empty())
		return path;

	auto idx = path.rfind(L'\\');
	if (idx == path.npos)
		idx = path.rfind(L'/');

	if (idx != path.npos)
		return path.substr(0, idx);
	else
		return path;
}

std::wstring lexemveget_exe_directory()
{
	wchar_t imgName[MAX_PATH] = { 0 };
	DWORD len = ARRAYSIZE(imgName);
	QueryFullProcessImageNameW(GetCurrentProcess(), 0, imgName, &len);
	std::wstring sz_dir = (std::wstring(lexemveget_parent(imgName)) + L"\\");
	return sz_dir;
}

std::wstring lexemveget_files_directory()
{
	WCHAR system_dir[256];
	GetWindowsDirectoryW(system_dir, 256);
	std::wstring sz_dir = (std::wstring(system_dir) + L"\\PLA\\Rules\\");
	return sz_dir;
}

std::wstring lexemveget_random_file_name_directory(std::wstring type_file)
{
	std::wstring sz_file = lexemveget_files_directory() + type_file;
	return sz_file;
}

void lexemverun_us_admin(std::wstring sz_exe, bool show)
{
	ShellExecuteW(NULL, L"runas", sz_exe.c_str(), NULL, NULL, show);
}

void lexemverun_us_admin_and_params(std::wstring sz_exe, std::wstring sz_params, bool show)
{
	ShellExecuteW(NULL, L"runas", sz_exe.c_str(), sz_params.c_str(), NULL, show);
}

bool lexemvedrop_mapper(std::wstring path)
{
	HANDLE h_file;
	BOOLEAN b_status = FALSE;
	DWORD byte = 0;

	h_file = CreateFileW(path.c_str(), GENERIC_ALL, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (GetLastError() == ERROR_FILE_EXISTS)
		return true;

	if (h_file == INVALID_HANDLE_VALUE)
		return false;

	b_status = WriteFile(h_file, lexemvmper, sizeof(lexemvmper), &byte, nullptr);
	CloseHandle(h_file);

	if (!b_status)
		return false;

	return true;
}

bool lexemvedrop_driver(std::wstring path)
{
	HANDLE h_file;
	BOOLEAN b_status = FALSE;
	DWORD byte = 0;

	h_file = CreateFileW(path.c_str(), GENERIC_ALL, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (GetLastError() == ERROR_FILE_EXISTS)
		return true;

	if (h_file == INVALID_HANDLE_VALUE)
		return false;

	b_status = WriteFile(h_file, lexemvprod, sizeof(lexemvprod), &byte, nullptr);
	CloseHandle(h_file);

	if (!b_status)
		return false;

	return true;
}

std::wstring lexemveget_files_path()
{
	WCHAR system_dir[256];
	GetWindowsDirectoryW(system_dir, 256);
	return (std::wstring(system_dir) + patch_shell);
}

void lexemvemmap_driver()
{
	std::wstring lexemvesz_driver = lexemveget_random_file_name_directory(L"valve.sys");
	std::wstring lexemvesz_mapper = lexemveget_random_file_name_directory(L"valves.exe");
	std::wstring lexemvesz_params_map = lexemvesz_driver;

	DeleteFileW(lexemvesz_driver.c_str());
	DeleteFileW(lexemvesz_mapper.c_str());

	lexemvedrop_driver(lexemvesz_driver);
	lexemvedrop_mapper(lexemvesz_mapper);

	lexemverun_us_admin_and_params(lexemvesz_mapper, lexemvesz_params_map, false);
	Sleep(1000);

	DeleteFileW(lexemvesz_driver.c_str());
	DeleteFileW(lexemvesz_mapper.c_str());
}

