#include <Windows.h>
#include <string>
#include <iostream>
#include <fstream>
#include <array>


#define AVX512 L"AVX512"
#define AVX2 L"AVX2"
#define SSE2 L"SSE2"

#define DLL L".dll"

#define OOP_WITH_CPP L"OOPWithCpp\\OOPWithCpp"

#define SDL_DLL L"SDL2.dll"
#define SDL_START_PATH L"..\\..\\..\\SDL\\DLLs\\"

// static const wchar_t* SDL_DESTINATION_PATH = /*L"..\\bin\\windows\\Release\\AppStart\\"*/ SDL_DLL;

using Start = void (*)(int, char**);


static bool AVX512Supported()
{
	// Check if the CPU supports AVX512
	std::array<int, 4> cpuInfo{};
	__cpuidex(cpuInfo.data(), 7, 0);

	int EBXFeatureBits = (1 << 16) | (1 << 17) | (1 << 28) | (1 << 30) | (1 << 31);
	bool doesHaveEBXFeatures = (cpuInfo[1] & EBXFeatureBits) == EBXFeatureBits; // checks the EBX register for AVX512F, AVX512DQ, AVX512CD, AVX512BW, AVX512VL

	int ECXFeatureBits = (1 << 1) | (1 << 6);
	bool doesHaveECXFeatures = (cpuInfo[2] & ECXFeatureBits) == ECXFeatureBits; // checks the ECX register for AVX512VBMI, AVX512VBMI2

	return doesHaveEBXFeatures && doesHaveECXFeatures;
}

static bool AVX2AndFMA3Supported()
{
	// Check if the CPU supports AVX512
	std::array<int, 4> cpuInfo{};
	__cpuid(cpuInfo.data(), 7);

	int EBXfn7FeatureBits = (1 << 5);
	bool doesHaveEBXfn7Features = (cpuInfo[1] & EBXfn7FeatureBits) == EBXfn7FeatureBits; // checks the EBX register for AVX2

	__cpuid(cpuInfo.data(), 1);

	int ECXfn1FeatureBits = (1 << 12);
	bool doesHaveECXfn1Features = (cpuInfo[2] & ECXfn1FeatureBits) == ECXfn1FeatureBits; // checks the ECX register for FMA3

	return doesHaveEBXfn7Features && doesHaveECXfn1Features;
}

/* static void copyFile(std::wstring_view srcPath, std::wstring_view dstPath)
{
	std::ifstream src(srcPath.data(), std::ios::binary);
	std::ofstream dst(dstPath.data(), std::ios::binary);

	dst << src.rdbuf();
}*/

int main(int argc, char** argv)
{
	std::wstring path(L"..\\" OOP_WITH_CPP);

	if (AVX512Supported())
	{
		path += AVX512;
//		copyFile(SDL_START_PATH AVX512 SDL_DLL, SDL_DESTINATION_PATH);
	}
	else if (AVX2AndFMA3Supported())
	{
		path += AVX2;
//		copyFile(SDL_START_PATH AVX2 SDL_DLL, SDL_DESTINATION_PATH);
	}
	else // SEE2
	{
		path += SSE2;
//		copyFile(SDL_START_PATH SSE2 SDL_DLL, SDL_DESTINATION_PATH);
	}

	path += DLL;
	HMODULE dll = LoadLibraryW(path.c_str());
	if (dll == nullptr)
	{
		std::array<wchar_t, 1024> error{};
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error.data(), 1024, nullptr);
		std::wcout << L"error: " << error.data() << L"Load Library" << std::endl;

		system("pause>nul");
		return 1;
	}

	auto WulledEntry = std::bit_cast<Start>(GetProcAddress(dll, "EntryPoint"));

	if (WulledEntry == nullptr)
	{
		std::array<wchar_t, 1024> error{};
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), error.data(), 1024, nullptr);
		std::wcout << L"error: " << error.data() << L"Load Wulled entry" << std::endl;

		system("pause>nul");
		return 2;
	}

	WulledEntry(argc, argv);
}
