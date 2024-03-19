#include "blabla/include.h"
#include "driver/driver.h"
#include "sdk/cheats.h"
#include "auth/auth.hpp"
#include "blabla/skStr.h"
#include <locale>
#include <codecvt>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "ws2_32.lib")
std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);
const std::string compilation_date = (std::string)skCrypt(__DATE__);
const std::string compilation_time = (std::string)skCrypt(__TIME__);
#pragma optimize("", off)
using namespace KeyAuth;
std::string name = "asd"; // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = "QC3i8vSLJp"; // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = "b2a9eaaf7d2c80aa6f9bc9b45a87df5eb27130c06f5cb02e88f9e5c343fe3809"; // app secret, the blurred text on licenses tab and other tabs
std::string version = "1.3"; // leave alone unless you've changed version on website
std::string url = skCrypt("https://keyauth.win/api/1.2/").decrypt();

api KeyAuthApp(name, ownerid, secret, version, url);
using namespace std;
namespace fs = std::filesystem;

void ClientFunction() {
	const char* vdsIP = "91.151.95.125";
	const int vdsPort = 12345;

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "error: sock error." << std::endl;
		Sleep(1500);
		exit(0);
		return;
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Error: socket error." << std::endl;
		WSACleanup();
		Sleep(1500);
		exit(0);
		return;
	}

	sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(vdsIP);
	serverAddr.sin_port = htons(vdsPort);

	if (connect(clientSocket, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "\n connect error." << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		Sleep(1500);
		exit(0);
		return;
	}

	// İstemciden sunucuya gönderilecek mesaj
	const char* messageToSend = "[+] Dll Giris Yapildi.";

	int bytesSent = send(clientSocket, messageToSend, strlen(messageToSend), 0);
	if (bytesSent == SOCKET_ERROR) {
		std::cerr << "send error." << std::endl;
	}
	else {
		//std::cout << "Gönderilen HWID: " << messageToSend << std::endl;
	}

	// Sunucudan gelen mesajı al
	char buffer[1024];
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (bytesRead > 0) {
		buffer[bytesRead] = '\0'; // Null terminator ekleyin
		//std::cout << "Gelen Mesaj: " << buffer << std::endl;
	}
	else {
		std::cerr << "recv error." << std::endl;
		Sleep(1500);
		exit(0);
	}

	closesocket(clientSocket);
	WSACleanup();
}


bool DllMain(HMODULE Module, DWORD CallReason, LPVOID Reserved)
{
	if (CallReason == DLL_PROCESS_ATTACH)
	{


		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			std::cerr << "Error: WSAStartup failed." << std::endl;
			exit(1);
		}ClientFunction();


		AllocConsole();
		freopen("CON", "w", stdout);
		freopen("CON", "w", stderr);
		if (mem::find_driver()) {
		}
		else
		{
			printf(_("[System] Driver Not Loaded!\n")); Sleep(-1);
		}
		printf(_("[System] Cheat Activated Good Games.\n"));
		if (mem::find_process(L"VALORANT-Win64-Shipping.exe")) {
			virtualaddy = mem::get_guarded_region();
			check::guard = virtualaddy;
			base = mem::base_address();
			Knzstrtcnnmw();
		}
	}
	return FALSE;
}

#pragma optimize("", on)