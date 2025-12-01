#pragma once
#include "BaseShader.hpp"

#include <string>
#include <vector>

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
		};

	public:
		explicit VulkanShader(const std::vector<ShaderData>& shaderDatas);
		~VulkanShader() override;
		VulkanShader(VulkanShader&) = default;
		VulkanShader& operator=(VulkanShader&) = default;
		VulkanShader(VulkanShader&&) noexcept = default;
		VulkanShader& operator=(VulkanShader&&) noexcept = default;

		[[nodiscard]] vk::Pipeline GetPipeline() const { return m_Pipeline; }

	private:
		void CreateVulkanPipeline(const std::vector<VulkanShaderData>& vulkanShaderDatas);

		[[nodiscard]] static VulkanShaderData ProcessShaderData(const ShaderData& shaderData);
		[[nodiscard]] static vk::ShaderStageFlagBits ConvertToVulkanShaderStage(ShaderData::ShaderType type);

	private:
		// store pipeline and shader module handles here
		vk::Pipeline m_Pipeline = vk::Pipeline();
		vk::PipelineLayout m_PipelineLayout = vk::PipelineLayout();
		std::vector<vk::ShaderModule> m_ShaderModules = {};
	};
}