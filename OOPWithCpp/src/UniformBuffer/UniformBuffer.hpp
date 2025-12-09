#pragma once
#include <cstddef>
#include <span>
#include <memory>


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
}
