#pragma once
#include "UniformBuffer.hpp"
#include "VulkanCore.hpp"

#include <vulkan/vulkan.hpp>


namespace OWC::Graphics
{
    class VulkanUniformBuffer : public UniformBuffer
    {
       public:
        VulkanUniformBuffer() = delete;
		explicit VulkanUniformBuffer(size_t size);
        ~VulkanUniformBuffer() override;
        VulkanUniformBuffer(VulkanUniformBuffer&) = default;
        VulkanUniformBuffer& operator=(VulkanUniformBuffer&) = default;
        VulkanUniformBuffer(VulkanUniformBuffer&&) noexcept = default;
        VulkanUniformBuffer& operator=(VulkanUniformBuffer&&) noexcept = default;

		void UpdateBufferData(std::span<const std::byte> data) override;

        [[nodiscard]] vk::Buffer GetBuffer() const { return m_UniformBuffers[VulkanCore::GetConstInstance().GetCurrentFrameIndex()]; };
		[[nodiscard]] size_t GetBufferSize() const { return static_cast<size_t>(VulkanCore::GetConstInstance().GetDevice().getBufferMemoryRequirements(m_UniformBuffers[0]).size); }

		[[nodiscard]] std::vector<vk::Buffer> GetBuffers() const { return m_UniformBuffers; }

    private:
		std::vector<vk::Buffer> m_UniformBuffers;
		std::vector<vk::DeviceMemory> m_UniformBuffersMemory;
		std::vector<void*> m_MappedUniformBuffers;
    };
}
