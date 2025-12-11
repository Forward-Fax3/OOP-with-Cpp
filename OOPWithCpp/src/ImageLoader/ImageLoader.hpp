#pragma once
#include <string_view>
#include <vector>

#include <glm/glm.hpp>


namespace OWC
{
	class ImageLoader
	{
	public:
		ImageLoader() = delete;
		ImageLoader(const ImageLoader&) = delete;
		ImageLoader& operator=(const ImageLoader&) = delete;
		ImageLoader(ImageLoader&&) = delete;
		ImageLoader& operator=(ImageLoader&&) = delete;
		ImageLoader(std::string_view path);
		virtual ~ImageLoader() = default;

		[[nodiscard]] const std::vector<glm::vec4>& GetImageData() const { return m_ImageData; }
		[[nodiscard]] const glm::vec4& GetPixel(size_t x, size_t y) const { return m_ImageData[y * m_Width + x]; }
		// expects x and y to be in the range [0.0, 1.0]
		[[nodiscard]] const glm::vec4& GetPixel(float x, float y) const { return m_ImageData[static_cast<size_t>(y * static_cast<float>(m_Height)) * m_Width + static_cast<size_t>(x * static_cast<float>(m_Width))]; }
		[[nodiscard]] size_t GetWidth() const { return m_Width; }
		[[nodiscard]] size_t GetHeight() const { return m_Height; }

	private:
		std::vector<glm::vec4> m_ImageData;
		size_t m_Width = 0;
		size_t m_Height = 0;
	};
}
 