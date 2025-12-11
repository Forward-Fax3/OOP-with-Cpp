#include "UniformBuffer.hpp"
#include "VulkanUniformBuffer.hpp"


namespace OWC::Graphics
{
	std::shared_ptr<UniformBuffer> UniformBuffer::CreateUniformBuffer(size_t size)
	{
		// For now, only Vulkan is supported
		return std::make_shared<VulkanUniformBuffer>(size);
	}

	std::shared_ptr<TextureBuffer> TextureBuffer::CreateTextureBuffer(const ImageLoader& image)
	{
		// For now, only Vulkan is supported
		return std::make_shared<VulkanTextureBuffer>(image);
	}
}
