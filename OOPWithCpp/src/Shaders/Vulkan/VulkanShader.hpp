#pragma once
#include "BaseShader.hpp"

#include <string>
#include <vector>
#include <span>

#include <vulkan/vulkan.hpp>


namespace OWC::Graphics
{
	class VulkanShader : public BaseShader
	{
	private:
		struct VulkanShaderData
		{
			std::string entryPoint;
			std::vector<uint32_t> bytecode;
			vk::ShaderStageFlagBits stage;
			std::vector<BindingDiscription> bindingDescriptions;
		};

	public:
		explicit VulkanShader(const std::span<ShaderData>& shaderDatas);
		~VulkanShader() override;
		VulkanShader(VulkanShader&) = default;
		VulkanShader& operator=(VulkanShader&) = default;
		VulkanShader(VulkanShader&&) noexcept = default;
		VulkanShader& operator=(VulkanShader&&) noexcept = default;

		[[nodiscard]] vk::Pipeline GetPipeline() const { return m_Pipeline; }
		[[nodiscard]] vk::PipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
		[[nodiscard]] vk::DescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }
		[[nodiscard]] vk::DescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }

	private:
		void CreateVulkanPipeline(const std::span<VulkanShaderData>& vulkanShaderDatas);

		[[nodiscard]] static VulkanShaderData ProcessShaderData(const ShaderData& shaderData);
		[[nodiscard]] static vk::ShaderStageFlagBits ConvertToVulkanShaderStage(ShaderType type);
		[[nodiscard]] static vk::DescriptorType ConvertToVulkanDescriptorType(DescriptorType type);

	private:
		// store pipeline and shader module handles here
		vk::Pipeline m_Pipeline = vk::Pipeline();
		vk::PipelineLayout m_PipelineLayout = vk::PipelineLayout();
		vk::DescriptorSetLayout m_DescriptorSetLayout = vk::DescriptorSetLayout();
		vk::DescriptorPool m_DescriptorPool = vk::DescriptorPool();
		vk::DescriptorSet m_DescriptorSet = vk::DescriptorSet();
		std::vector<vk::ShaderModule> m_ShaderModules = {};
	};
}