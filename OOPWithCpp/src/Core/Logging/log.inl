#include <iostream>
#include <windows.h>
#undef max // undefine max macro to avoid conflicts with std::numeric_limits

#include "core.hpp"


namespace OWC
{
	template<LogLevel level, typename... Args>
	inline void Log(const std::format_string<Args...> str, Args&&... args)
	{
		if constexpr (level == LogLevel::Trace)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 8);
			std::cout << "[Trace]: " << std::format(str, std::forward<Args>(args)...) << '\n';
		}
		else if constexpr (level == LogLevel::Warn)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
			std::cout << "[Warning]: " << std::format(str, std::forward<Args>(args)...) << '\n';
		}
		else if constexpr (level == LogLevel::Error)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
			std::cout << "[Error]: " << std::format(str, std::forward<Args>(args)...) << '\n';
		}
		else if constexpr (level == LogLevel::Critical)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 192);
			std::cout << "[Critical]: " << std::format(str, std::forward<Args>(args)...) << '\n';

			if constexpr (IsDebugMode()) // TODO: use message box for when in distribution mode
				__debugbreak();
			else
				exit(EXIT_FAILURE);
		}
		else // compile-time safeguard
		{
			static_assert(false, "Unknown LogLevel!");
		}
	}
}