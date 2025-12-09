#include "Application.hpp"
#include "VulkanCore.hpp"
#include "VulkanShader.hpp"
#include "Log.hpp"


namespace OWC::Graphics
{
	VulkanShader::VulkanShader(const std::span<ShaderData>& shaderDatas)
	{
		if (shaderDatas.empty())
			Log<LogLevel::Error>("VulkanShader::VulkanShader: No shader data provided!");

		std::vector<VulkanShaderData> vulkanShaderDatas;
		vulkanShaderDatas.reserve(shaderDatas.size());

		for (const auto& shaderData : shaderDatas)
		{
			if (shaderData.language != ShaderData::ShaderLanguage::SPIRV)
				Log<LogLevel::Error>("VulkanShader::VulkanShader: Unsupported shader language for Vulkan shader: {}!", std::to_underlying(shaderData.language));

			Log<LogLevel::Debug>("VulkanShader::VulkanShader: Processing {} shader.", shaderData.ShaderTypeToString());

			vulkanShaderDatas.push_back(ProcessShaderData(shaderData));
		}

		CreateVulkanPipeline(vulkanShaderDatas);
	}

	VulkanShader::~VulkanShader()
	{
		VulkanCore::GetInstance().GetDevice().destroyDescriptorPool(m_DescriptorPool);
		VulkanCore::GetInstance().GetDevice().destroyDescriptorSetLayout(m_DescriptorSetLayout);

		VulkanCore::GetInstance().GetDevice().destroyPipeline(m_Pipeline);
		VulkanCore::GetInstance().GetDevice().destroyPipelineLayout(m_PipelineLayout);

		for (const auto& shaderModule : m_ShaderModules)
			VulkanCore::GetInstance().GetDevice().destroyShaderModule(shaderModule);
	}

	void VulkanShader::CreateVulkanPipeline(const std::span<VulkanShaderData>& vulkanShaderDatas)
	{
		// Create shader modules and pipeline

		Log<LogLevel::Trace>("VulkanShader::CreateVulkanPipeline: Creating Vulkan pipeline with {} shader stages.", vulkanShaderDatas.size());
		const auto& device = VulkanCore::GetConstInstance().GetDevice();

		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
		m_ShaderModules.reserve(vulkanShaderDatas.size());
		shaderStages.reserve(vulkanShaderDatas.size());

		for (const auto& shaderData : vulkanShaderDatas)
		{
			const vk::ShaderModuleCreateInfo shaderModuleCreateInfo = vk::ShaderModuleCreateInfo()
				.setCodeSize(shaderData.bytecode.size() * sizeof(uint32_t))
				.setPCode(shaderData.bytecode.data());

			m_ShaderModules.emplace_back(device.createShaderModule(shaderModuleCreateInfo));

			shaderStages.emplace_back(
				static_cast<vk::PipelineShaderStageCreateFlags>(0),
				shaderData.stage,
				m_ShaderModules.back(),
				shaderData.entryPoint.c_str()
			);
		}

		constexpr std::array<vk::DynamicState, 2> dynamicStates = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

		const vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo()
			.setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()))
			.setPDynamicStates(dynamicStates.data());

		constexpr vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
		constexpr vk::PipelineInputAssemblyStateCreateInfo inputAssembly = vk::PipelineInputAssemblyStateCreateInfo()
			.setTopology(vk::PrimitiveTopology::eTriangleList)
			.setPrimitiveRestartEnable(vk::False);

		const vk::Viewport viewport = vk::Viewport()
			.setHeight(static_cast<float>(Application::GetConstInstance().GetWindow().GetHeight()))
			.setWidth(static_cast<float>(Application::GetConstInstance().GetWindow().GetWidth()))
			.setMinDepth(0.0f)
			.setMaxDepth(1.0f)
			.setX(0.0f)
			.setY(0.0f);
		
		const vk::Rect2D scissor = vk::Rect2D()
			.setOffset({ 0, 0 })
			.setExtent({
				static_cast<uint32_t>(Application::GetConstInstance().GetWindowWidth()),
				static_cast<uint32_t>(Application::GetConstInstance().GetWindowHeight())
			});

		const vk::PipelineViewportStateCreateInfo viewportState = vk::PipelineViewportStateCreateInfo()
			.setViewportCount(1)
			.setPViewports(&viewport)
			.setScissorCount(1)
			.setPScissors(&scissor);

		constexpr vk::PipelineRasterizationStateCreateInfo rasterizer = vk::PipelineRasterizationStateCreateInfo()
			.setPolygonMode(vk::PolygonMode::eFill)
			.setLineWidth(1.0f);

		constexpr vk::PipelineMultisampleStateCreateInfo multisampling = vk::PipelineMultisampleStateCreateInfo()
			.setSampleShadingEnable(vk::False)
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);

		constexpr vk::PipelineColorBlendAttachmentState colorBlendAttachment = vk::PipelineColorBlendAttachmentState()
			.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
			.setBlendEnable(vk::True)
			.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
			.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
			.setColorBlendOp(vk::BlendOp::eAdd)
			.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);

		const vk::PipelineColorBlendStateCreateInfo colorBlending = vk::PipelineColorBlendStateCreateInfo()
			.setLogicOpEnable(vk::False)
			.setAttachmentCount(1)
			.setPAttachments(&colorBlendAttachment);

		const vk::PipelineRenderingCreateInfo pipelineRenderingInfo = vk::PipelineRenderingCreateInfo()
			.setColorAttachmentCount(1)
			.setPColorAttachmentFormats(&VulkanCore::GetConstInstance().GetSwapchainImageFormat());

		std::vector<vk::DescriptorSetLayoutBinding> bindings;

		for (const auto& shaderData : vulkanShaderDatas)
			for (const auto& bindingDescription : shaderData.bindingDescriptions)
				bindings.emplace_back(
					bindingDescription.binding,
					ConvertToVulkanDescriptorType(bindingDescription.descriptorType),
					bindingDescription.descriptorCount,
					ConvertToVulkanShaderStage(bindingDescription.stageFlags)
				);

		vk::DescriptorSetLayoutCreateInfo layoutInfo = vk::DescriptorSetLayoutCreateInfo()
			.setBindings(bindings);

		m_DescriptorSetLayout = device.createDescriptorSetLayout(layoutInfo);

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo = vk::PipelineLayoutCreateInfo()
			.setSetLayouts(m_DescriptorSetLayout);

		m_PipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);

		vk::GraphicsPipelineCreateInfo pipelineInfo = vk::GraphicsPipelineCreateInfo()
			.setPNext(&pipelineRenderingInfo)
			.setStages(shaderStages)
			.setPVertexInputState(&vertexInputInfo)
			.setPInputAssemblyState(&inputAssembly)
			.setPViewportState(&viewportState)
			.setPRasterizationState(&rasterizer)
			.setPMultisampleState(&multisampling)
			.setPColorBlendState(&colorBlending)
			.setPDynamicState(&dynamicStateCreateInfo)
			.setLayout(m_PipelineLayout)
			.setSubpass(0);

		auto result = device.createGraphicsPipelines(
			nullptr,
			1,
			&pipelineInfo,
			nullptr,
			&m_Pipeline
		);

		if (result != vk::Result::eSuccess)
			Log<LogLevel::Error>("VulkanShader::CreateVulkanPipeline: Failed to create Vulkan graphics pipeline!");

		// build Descriptor Pool
		// TODO: make this dynamic based on actual usage

		vk::DescriptorPoolSize poolSize = vk::DescriptorPoolSize()
			.setType(vk::DescriptorType::eUniformBuffer)  
			.setDescriptorCount(10);

		vk::DescriptorPoolCreateInfo poolInfo = vk::DescriptorPoolCreateInfo()
			.setPoolSizes(poolSize)
			.setMaxSets(10);

		m_DescriptorPool = device.createDescriptorPool(poolInfo);
		vk::DescriptorSetAllocateInfo allocInfo = vk::DescriptorSetAllocateInfo()
			.setDescriptorPool(m_DescriptorPool)
			.setSetLayouts(m_DescriptorSetLayout);
		m_DescriptorSet = device.allocateDescriptorSets(allocInfo).front();
	}

	VulkanShader::VulkanShaderData VulkanShader::ProcessShaderData(const ShaderData& shaderData)
	{
		Log<LogLevel::Trace>("VulkanShader::ProcessShaderData: Processing shader data of type {}.", shaderData.ShaderTypeToString());

		VulkanShaderData vulkanShaderData;

		vulkanShaderData.entryPoint = "main";
		if (shaderData.language == ShaderData::ShaderLanguage::SPIRV) // will always be true for now as the check is done earlier
			vulkanShaderData.bytecode = shaderData.bytecode;
		// in the future, add support for other shader languages here (e.g., GLSL to SPIR-V compilation)
		vulkanShaderData.stage = ConvertToVulkanShaderStage(shaderData.type);
		vulkanShaderData.bindingDescriptions = std::vector<BindingDiscription>(shaderData.descriptorType.begin(), shaderData.descriptorType.end());
		return vulkanShaderData;
	}

	vk::ShaderStageFlagBits VulkanShader::ConvertToVulkanShaderStage(ShaderType type)
	{
		vk::ShaderStageFlags stage;

		if (type == ShaderType::Vertex)
			stage |= vk::ShaderStageFlagBits::eVertex;
		if (type == ShaderType::Fragment)
			stage |= vk::ShaderStageFlagBits::eFragment;
		if (type == ShaderType::Compute)
			stage |= vk::ShaderStageFlagBits::eCompute;

		if (stage == vk::ShaderStageFlags())
			Log<LogLevel::Error>("VulkanShader::ConvertToVulkanShaderStage: Unknown shader type provided: {}!", std::to_underlying(type));

		return static_cast<vk::ShaderStageFlagBits>(static_cast<uint32_t>(stage)); // don't ask
	}

	vk::DescriptorType VulkanShader::ConvertToVulkanDescriptorType(DescriptorType type)
	{
		switch (type)
		{
		case DescriptorType::UniformBuffer:
			return vk::DescriptorType::eUniformBuffer;
		case DescriptorType::Sampler:
			return vk::DescriptorType::eSampler;
		case DescriptorType::CombinedImageSampler:
			return vk::DescriptorType::eCombinedImageSampler;
		case DescriptorType::StorageBuffer:
			return vk::DescriptorType::eStorageBuffer;
		default:
			Log<LogLevel::Error>("VulkanShader::ConvertToVulkanDescriptorType: Unknown descriptor type provided: {}!", std::to_underlying(type));
			return static_cast<vk::DescriptorType>(0); // to silence compiler warning
		}
	}
}
