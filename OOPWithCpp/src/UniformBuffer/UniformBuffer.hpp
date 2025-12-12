#pragma once
#include "ImageLoader.hpp"

#include <cstddef>
#include <span>
#include <memory>
#include <vector>
#include <glm/vec4.hpp>


namespace OWC::Graphics
{
	class UniformBuffer
	{
	public:
		UniformBuffer() = default;
		virtual ~UniformBuffer() = default;
		UniformBuffer(UniformBuffer&) = default;
		UniformBuffer& operator=(UniformBuffer&) = default;
		UniformBuffer(UniformBuffer&&) noexcept = default;
		UniformBuffer& operator=(UniformBuffer&&) noexcept = default;

		virtual void UpdateBufferData(std::span<const std::byte> data) = 0;

		static std::shared_ptr<UniformBuffer> CreateUniformBuffer(size_t size);
	};

	class TextureBuffer
	{
	public:
		TextureBuffer() = default;
		virtual ~TextureBuffer() = default;
		TextureBuffer(TextureBuffer&) = default;
		TextureBuffer& operator=(TextureBuffer&) = default;
		TextureBuffer(TextureBuffer&&) noexcept = default;
		TextureBuffer& operator=(TextureBuffer&&) noexcept = default;

		virtual void UpdateBufferData(const std::vector<glm::vec4>& data) = 0;

		static std::shared_ptr<TextureBuffer> CreateTextureBuffer(const ImageLoader& image);
		static std::shared_ptr<TextureBuffer> CreateTextureBuffer(uint32_t width, uint32_t height);
	};
}
