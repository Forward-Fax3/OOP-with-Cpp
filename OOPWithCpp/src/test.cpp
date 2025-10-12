#include "iostream"


extern "C" __declspec(dllexport) void EntryPoint(int, char**)
{
	std::cout << "Hello, World!" << std::endl;
	std::cin.get();
}
