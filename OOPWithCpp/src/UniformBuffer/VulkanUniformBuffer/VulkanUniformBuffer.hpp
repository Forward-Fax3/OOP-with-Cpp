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
		VulkanUniformBuffer(VulkanUniformBuffer&) = delete;
		VulkanUniformBuffer& operator=(VulkanUniformBuffer&) = delete;
		VulkanUniformBuffer(VulkanUniformBuffer&&) noexcept = delete;
		VulkanUniformBuffer& operator=(VulkanUniformBuffer&&) noexcept = delete;

		void UpdateBufferData(std::span<const std::byte> data) override;

		[[nodiscard]] vk::Buffer GetBuffer() const { return m_UniformBuffers[VulkanCore::GetConstInstance().GetCurrentFrameIndex()]; };
		[[nodiscard]] size_t GetBufferSize() const { return static_cast<size_t>(VulkanCore::GetConstInstance().GetDevice().getBufferMemoryRequirements(m_UniformBuffers[0]).size); }

		[[nodiscard]] std::vector<vk::Buffer> GetBuffers() const { return m_UniformBuffers; }

	private:
		std::vector<vk::Buffer> m_UniformBuffers;
		std::vector<vk::DeviceMemory> m_UniformBuffersMemory;
		std::vector<void*> m_MappedUniformBuffers;
	};

	class VulkanTextureBuffer : public TextureBuffer
	{
	public:
		VulkanTextureBuffer() = delete;
		explicit VulkanTextureBuffer(const ImageLoader& image);
		explicit VulkanTextureBuffer(uint32_t width, uint32_t height);
		~VulkanTextureBuffer() override;
		VulkanTextureBuffer(VulkanTextureBuffer&) = delete;
		VulkanTextureBuffer& operator=(VulkanTextureBuffer&) = delete;
		VulkanTextureBuffer(VulkanTextureBuffer&&) noexcept = delete;
		VulkanTextureBuffer& operator=(VulkanTextureBuffer&&) noexcept = delete;

		void UpdateBufferData(const std::vector<glm::vec4>& data) override;

		[[nodiscard]] vk::Image GetImage() const { return m_TextureImage; }
		[[nodiscard]] vk::ImageView GetImageView() const { return m_TextureImageView; }
		[[nodiscard]] vk::Sampler GetSampler() const { return m_TextureSampler; }

	private:
		void InitializeTexture();

	private:
		vk::Image m_TextureImage = vk::Image();
		vk::DeviceMemory m_TextureImageMemory = vk::DeviceMemory();
		vk::ImageView m_TextureImageView = vk::ImageView();
		vk::Sampler m_TextureSampler = vk::Sampler();
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};
}
