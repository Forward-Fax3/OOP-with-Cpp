#pragma once
#include <cstdint>
#include <string>
#include <bit>


namespace OWC
{
	using bool32 = uint32_t;

    [[nodiscard]] consteval bool IsDebugMode() noexcept
    {
    #ifdef NDEBUG
        return false;
    #else
        return true;
    #endif
    }

    [[nodiscard]] consteval bool IsDistributionMode() noexcept
	{
    #ifdef DIST
		return true;
    #else
		return false;
    #endif
	}

    [[nodiscard]] constexpr const char* ToCharPtr(const std::u8string& str)
    {
		return std::bit_cast<char const*>(str.c_str());
    }
}
