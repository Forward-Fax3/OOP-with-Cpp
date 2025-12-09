#include "VulkanUniformBuffer.hpp"
#include "VulkanCore.hpp"


namespace OWC::Graphics
{
	VulkanUniformBuffer::VulkanUniformBuffer(size_t size)
	{
		const auto& vkCore = VulkanCore::GetInstance();
		const auto& device = vkCore.GetDevice();

		vk::DeviceSize bufferSize = size;

		m_UniformBuffers.reserve(vkCore.GetNumberOfFramesInFlight());
		m_UniformBuffersMemory.reserve(vkCore.GetNumberOfFramesInFlight());
		m_MappedUniformBuffers.reserve(vkCore.GetNumberOfFramesInFlight());


		for (size_t i = 0; i < vkCore.GetNumberOfFramesInFlight(); i++)
		{
			vk::Buffer buffer = device.createBuffer(vk::BufferCreateInfo()
				.setSize(bufferSize)
				.setUsage(vk::BufferUsageFlagBits::eUniformBuffer)
				.setSharingMode(vk::SharingMode::eExclusive));
			m_UniformBuffers.emplace_back(buffer);

			vk::MemoryRequirements memRequirements = device.getBufferMemoryRequirements(buffer);

			vk::DeviceMemory memory = device.allocateMemory(vk::MemoryAllocateInfo()
				.setAllocationSize(memRequirements.size)
				.setMemoryTypeIndex(vkCore.FindMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)));
			m_UniformBuffersMemory.emplace_back(memory);

			device.bindBufferMemory(buffer, memory, 0);
			m_MappedUniformBuffers.emplace_back(device.mapMemory(memory, 0, bufferSize));
		}
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		const auto& vkCore = VulkanCore::GetInstance();
		const auto& device = vkCore.GetDevice();
		for (const auto& buffer : m_UniformBuffers)
			device.destroyBuffer(buffer);

		for (const auto& memory : m_UniformBuffersMemory)
		{
			device.unmapMemory(memory);
			device.freeMemory(memory);
		}
	}

	void VulkanUniformBuffer::UpdateBufferData(std::span<const std::byte> data)
	{
		const auto& vkCore = VulkanCore::GetInstance();
		size_t currentFrame = vkCore.GetCurrentFrameIndex();
		std::memcpy(m_MappedUniformBuffers[currentFrame], data.data(), data.size());

		// todo: add staging buffers
	}
}