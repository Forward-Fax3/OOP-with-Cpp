#include "UniformBuffer.hpp"
#include "VulkanUniformBuffer.hpp"


namespace OWC::Graphics
{
	std::shared_ptr<UniformBuffer> UniformBuffer::CreateUniformBuffer(size_t size)
	{
		// For now, only Vulkan is supported
		return std::make_shared<VulkanUniformBuffer>(size);
	}
}