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

		for (size_t y = 0; y < m_Height; y++)
			for (size_t x = 0; x < m_Width; x++)
			{
				size_t index = (y * m_Width + x) * 3;

				m_ImageData.emplace_back(
					imageDataPtr[index + 0],
					imageDataPtr[index + 1],
					imageDataPtr[index + 2],
					1.0f);
			}
	}
}