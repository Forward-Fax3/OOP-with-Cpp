#define STB_IMAGE_IMPLEMENTATION
#include "ImageLoader.hpp"
#include "Log.hpp"

#include <stb_image.h>
#include <memory>


namespace OWC
{
	ImageLoader::ImageLoader(std::string_view path)
	{
		int tempWidth = 0;
		int tempHeight = 0;
		std::unique_ptr<float[], decltype([](float ptr[]) { STBI_FREE(ptr); })> imageDataPtr(stbi_loadf(path.data(), &tempWidth, &tempHeight, nullptr, 4));

		if (!imageDataPtr)
			Log<LogLevel::Error>("Failed to load image from path: {}", path);

		m_Width = static_cast<size_t>(tempWidth);
		m_Height = static_cast<size_t>(tempHeight);

		m_ImageData.reserve(m_Width * m_Height);

		for (size_t i = 0; i < m_Height * m_Width * 4; i += 4)
			m_ImageData.emplace_back(
				imageDataPtr[i + 0],	// R
				imageDataPtr[i + 1],	// G
				imageDataPtr[i + 2],	// B
				imageDataPtr[i + 3]		// A
			);
	}
}