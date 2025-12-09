#include "VulkanRenderPass.hpp"
#include "Application.hpp"

#include "VulkanCore.hpp"
#include "VulkanShader.hpp"
#include "VulkanUniformBuffer.hpp"

#include <backends/imgui_impl_vulkan.h>


namespace OWC::Graphics
{
	VulkanRenderPass::VulkanRenderPass(RenderPassType type)
		: RenderPassData(type)
	{
		const VulkanCore& vkCore = VulkanCore::GetConstInstance();

		vk::Rect2D rect(
			vk::Offset2D(0, 0),
			{
				static_cast<uint32_t>(Application::GetConstInstance().GetWindowWidth()),
				static_cast<uint32_t>(Application::GetConstInstance().GetWindowHeight())
			});

		if (type == RenderPassType::Static)
		{
			m_CommandBuffers = vkCore.GetGraphicsCommandBuffer();
			for (size_t i = 0; i < m_CommandBuffers.size(); ++i)
			{
				vk::CommandBuffer cmdbuf = m_CommandBuffers[i];
				cmdbuf.begin(vk::CommandBufferBeginInfo());

				cmdbuf.beginRendering(vk::RenderingInfo()
					.setRenderArea(rect)
					.setLayerCount(1)
					.setColorAttachmentCount(1)
					.setPColorAttachments(&vk::RenderingAttachmentInfo()
						.setImageView(vkCore.GetSwapchainImageViews()[i])
						.setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
						.setLoadOp(vk::AttachmentLoadOp::eLoad)
						.setStoreOp(vk::AttachmentStoreOp::eStore)
					)
				);

				cmdbuf.setViewport(0, vk::Viewport(0.0f, 0.0f,
					static_cast<float>(Application::GetConstInstance().GetWindowWidth()),
					static_cast<float>(Application::GetConstInstance().GetWindowHeight()),
					0.0f, 1.0f));
				cmdbuf.setScissor(0, rect);
			}
		}
		else
			m_CommandBuffers = vkCore.GetDynamicGraphicsCommandBuffer();
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		const VulkanCore& vkCore = VulkanCore::GetConstInstance();
		const vk::Device& device = vkCore.GetDevice();

		if (GetRenderPassType() == RenderPassType::Static)
			for (auto& cmdBuf : m_CommandBuffers)
				device.freeCommandBuffers(vkCore.GetGraphicsCommandPool(), cmdBuf);
		else
			for (auto& cmdBuf : m_CommandBuffers)
				device.freeCommandBuffers(vkCore.GetDynamicGraphicsCommandPool(), cmdBuf);

		device.destroyFence(m_Fence);
	}

	void VulkanRenderPass::BeginDynamicPass()
	{
		VulkanCore& vkCore = VulkanCore::GetInstance();
		const size_t frameIndex = vkCore.GetCurrentFrameIndex();

		const vk::CommandBuffer& cmdbuf = m_CommandBuffers[frameIndex];
		cmdbuf.reset(vk::CommandBufferResetFlags());

		vk::Rect2D rect(
			vk::Offset2D(0, 0),
			{
				static_cast<uint32_t>(Application::GetConstInstance().GetWindowWidth()),
				static_cast<uint32_t>(Application::GetConstInstance().GetWindowHeight())
			});

		cmdbuf.begin(vk::CommandBufferBeginInfo().setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

		cmdbuf.beginRendering(vk::RenderingInfo()
			.setRenderArea(rect)
			.setLayerCount(1)
			.setColorAttachmentCount(1)
			.setColorAttachments(vk::RenderingAttachmentInfo()
				.setImageView(vkCore.GetSwapchainImageViews()[frameIndex])
				.setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
				.setLoadOp(vk::AttachmentLoadOp::eLoad)
				.setStoreOp(vk::AttachmentStoreOp::eStore)
			)
		);

		cmdbuf.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(Application::GetConstInstance().GetWindowWidth()), static_cast<float>(Application::GetConstInstance().GetWindowHeight()), 0.0f, 1.0f));
		cmdbuf.setScissor(0, rect);
	}

	void VulkanRenderPass::AddPipeline(const BaseShader& shader)
	{
		const auto& vulkanShader = dynamic_cast<const VulkanShader&>(shader);
		if (GetRenderPassType() == RenderPassType::Dynamic)
		{
			size_t frameIndex = VulkanCore::GetConstInstance().GetCurrentFrameIndex();
			m_CommandBuffers[frameIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, vulkanShader.GetPipeline());
		}
		else
			for (auto& cmdBuf : m_CommandBuffers)
				cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, vulkanShader.GetPipeline());
	}

	void VulkanRenderPass::BindUniform(const BaseShader& shader, const std::shared_ptr<UniformBuffer>& uniformBuffer)
	{
		const auto& vulkanShader = dynamic_cast<const VulkanShader&>(shader);
		const auto& vulkanUniformBuffer = std::dynamic_pointer_cast<VulkanUniformBuffer>(uniformBuffer);
		(void)vulkanUniformBuffer; // to avoid unused variable warning if assertions are disabled

		if (GetRenderPassType() == RenderPassType::Dynamic)
		{
			size_t frameIndex = VulkanCore::GetConstInstance().GetCurrentFrameIndex();
			m_CommandBuffers[frameIndex].bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics,
				vulkanShader.GetPipelineLayout(),
				0,
				vulkanShader.GetDescriptorSet(),
				{}
			);
		}
		else
		{
			// update descriptor sets if needed

			std::vector<vk::WriteDescriptorSet> writeDescriptorSets;

			for (size_t i = 0; i < VulkanCore::GetConstInstance().GetNumberOfFramesInFlight(); ++i)
			{
				vk::DescriptorBufferInfo bufferInfo = vk::DescriptorBufferInfo()
					.setBuffer(vulkanUniformBuffer->GetBuffers()[i])
					.setOffset(0)
					.setRange(vulkanUniformBuffer->GetBufferSize());

				vk::WriteDescriptorSet writeDescriptorSet = vk::WriteDescriptorSet()
					.setDstSet(vulkanShader.GetDescriptorSet())
					.setDstBinding(0)
					.setDstArrayElement(0)
					.setDescriptorType(vk::DescriptorType::eUniformBuffer)
					.setBufferInfo(bufferInfo);

				writeDescriptorSets.push_back(writeDescriptorSet);
			}

			VulkanCore::GetConstInstance().GetDevice().updateDescriptorSets(writeDescriptorSets, {});


			for (auto& cmdBuf : m_CommandBuffers)
				cmdBuf.bindDescriptorSets(
					vk::PipelineBindPoint::eGraphics,
					vulkanShader.GetPipelineLayout(),
					0,
					vulkanShader.GetDescriptorSet(),
					{}
				);
		}
	}

	void VulkanRenderPass::Draw(uint32_t vertexCount, uint32_t instanceCount /*= 1*/, uint32_t firstVertex /*= 0*/, uint32_t firstInstance /*= 0*/)
	{
		if (GetRenderPassType() == RenderPassType::Dynamic)
		{
			size_t frameIndex = VulkanCore::GetConstInstance().GetCurrentFrameIndex();
			m_CommandBuffers[frameIndex].draw(vertexCount, instanceCount, firstVertex, firstInstance);
		}
		else
			for (auto& cmdBuf : m_CommandBuffers)
				cmdBuf.draw(vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void VulkanRenderPass::EndRenderPass()
	{
		if (GetRenderPassType() == RenderPassType::Static)
			for (size_t i = 0; i != VulkanCore::GetConstInstance().GetSwapchainImages().size(); i++)
			{
				vk::CommandBuffer& cmdBuf = m_CommandBuffers[i];
				cmdBuf.endRendering();
				cmdBuf.end();
			}
		else
		{
			VulkanCore& vkCore = VulkanCore::GetInstance();
			const size_t i = vkCore.GetCurrentFrameIndex();
			vk::CommandBuffer& cmdBuf = m_CommandBuffers[i];
			cmdBuf.endRendering();
			cmdBuf.end();
		}

		if (m_Fence)
			VulkanCore::GetConstInstance().GetDevice().destroyFence(m_Fence);

		m_Fence = VulkanCore::GetConstInstance().GetDevice().createFence(vk::FenceCreateInfo());
	}

	void VulkanRenderPass::submitRenderPass(std::span<std::string_view> waitSemaphoreNames, std::span<std::string_view> startSemaphore)
	{
		VulkanCore& vkCore = VulkanCore::GetInstance();

		std::vector<vk::Semaphore> waitSemaphores = vkCore.GetSemaphoresFromNames(waitSemaphoreNames);
		std::vector<vk::Semaphore> signalSemaphores = vkCore.GetSemaphoresFromNames(startSemaphore);

		vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
		vk::SubmitInfo submitInfo = vk::SubmitInfo()
			.setWaitSemaphores(waitSemaphores)
			.setSignalSemaphores(signalSemaphores)
			.setWaitDstStageMask(waitDestinationStageMask)
			.setCommandBuffers(m_CommandBuffers[vkCore.GetCurrentFrameIndex()])
			.setCommandBufferCount(1);

		vkCore.GetGraphicsQueue().submit(submitInfo, m_Fence);
	}

	void VulkanRenderPass::RestartRenderPass()
	{
		const VulkanCore& vkCore = VulkanCore::GetConstInstance();
		vkCore.GetDevice().resetFences(m_Fence);
		if (GetRenderPassType() == RenderPassType::Dynamic)
			m_CommandBuffers[vkCore.GetCurrentFrameIndex()].reset(vk::CommandBufferResetFlags());
	}

	void VulkanRenderPass::DrawImGui(ImDrawData* drawData)
	{
		const vk::CommandBuffer& cmdBuf = m_CommandBuffers[VulkanCore::GetConstInstance().GetCurrentFrameIndex()];
		ImGui_ImplVulkan_RenderDrawData(drawData, cmdBuf);
	}
}