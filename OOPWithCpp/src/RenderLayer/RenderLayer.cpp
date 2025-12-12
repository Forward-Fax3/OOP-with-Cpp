#include "core.hpp"
#include "RenderLayer.hpp"
#include "LoadFile.hpp"
#include "Renderer.hpp"

#include "BaseEvent.hpp"
#include "WindowMinimizeEvent.hpp"
#include "WindowRestoreEvent.hpp"
#include "WindowResize.hpp"

#include <array>

#include <glm/gtc/type_ptr.hpp>


namespace OWC
{
	RenderLayer::RenderLayer(const std::shared_ptr<InterLayerData>& ILD)
		: m_ILD(ILD)
	{
		m_UniformBuffer = Graphics::UniformBuffer::CreateUniformBuffer((sizeof(float) * 2) + sizeof(glm::vec2) + sizeof(bool32));
		m_Image = Graphics::TextureBuffer::CreateTextureBuffer(1, 1);
		std::vector<glm::vec4> emptyImageData = { glm::vec4(0.0f) };
		m_Image->UpdateBufferData(emptyImageData);
		SetupPipeline();
		SetupRenderPass();
	}

	void RenderLayer::OnUpdate()
	{
		using namespace OWC::Graphics;

		std::array<std::string_view, 1> waitSemaphorenames = { "ImageReady" };
		std::array<std::string_view, 1> signalSemaphoreNames = { "RenderLayer" };

		struct UniformBufferObject
		{
			float divider = 0.0;
			float imageScale = 0.0;
			glm::vec2 imageOffset = glm::vec2(0.0f, 0.0f);
			bool32 toggleUV = 0;
		};

		UniformBufferObject ubo{};
		ubo.divider = m_Divider;
		ubo.imageScale = m_ImageScale;
		ubo.imageOffset = m_ImageOffset;
		ubo.toggleUV = static_cast<bool32>(m_ToggleUV);

		m_UniformBuffer->UpdateBufferData(std::as_bytes(std::span<const UniformBufferObject>(&ubo, 1)));

		if (m_ILD->ImageUpdates.any())
		{
			if (m_ILD->ImageUpdates[1] && m_ILD->imageWidth > 0 && m_ILD->imageHeight > 0) // image resize
			{
				m_Image = TextureBuffer::CreateTextureBuffer(m_ILD->imageWidth, m_ILD->imageHeight);
				m_Image->UpdateBufferData(m_ILD->imageData);
				SetupPipeline();
				SetupRenderPass();
			}
			else if (m_ILD->imageWidth == 0 || m_ILD->imageHeight == 0) // clear image
			{
				m_Image = TextureBuffer::CreateTextureBuffer(1, 1);
				std::vector<glm::vec4> emptyImageData = { glm::vec4(0.0f) };
				m_Image->UpdateBufferData(emptyImageData);
				SetupPipeline();
				SetupRenderPass();
			}
			else if (m_ILD->ImageUpdates[0]) // update image
			{
				m_Image->UpdateBufferData(m_ILD->imageData);
			}
			m_ILD->ImageUpdates.reset();
		}

		Renderer::RestartRenderPass(m_renderPass);
		Renderer::SubmitRenderPass(m_renderPass, waitSemaphorenames, signalSemaphoreNames);
	}

	void RenderLayer::ImGuiRender()
	{ 
		constexpr float min = 0.001f;
		constexpr float max = 5.0f;

		constexpr float imageOffsetMin = 0.0f;
		constexpr float imageOffsetMax = 1.0f;
		constexpr float iamgeOffsetStep = 0.01f;

		float DividerStep = m_Divider / 100.0f;
		float ImageScaleStep = m_ImageScale / 100.0f;

		ImGui::Begin("Render Layer");
		ImGui::DragFloat("Divider", &m_Divider, DividerStep, min, max);
		ImGui::DragFloat("Image Scale", &m_ImageScale, ImageScaleStep, min, max);
		ImGui::DragFloat2("Image Offset", glm::value_ptr(m_ImageOffset), iamgeOffsetStep, imageOffsetMin, imageOffsetMax);
		ImGui::Checkbox("Toggle UV", &m_ToggleUV);
		ImGui::End();
	}

	void RenderLayer::OnEvent(class BaseEvent& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowRestore>([this](const WindowRestore& /*event*/) {
			this->SetupRenderPass();
			this->SetActive(true);
			return false;
			});

		if (!IsActive())
			return;

		dispatcher.Dispatch<WindowMinimize>([this](const WindowMinimize& /*event*/) {
			this->SetActive(false);
			return false;
			});

		dispatcher.Dispatch<WindowResize>([this](const WindowResize& /*event*/) {
			this->SetupPipeline();
			this->SetupRenderPass();
			return false;
			});
	}

	void RenderLayer::SetupRenderPass()
	{
		using namespace OWC::Graphics;
		constexpr uint32_t numberOfVertices = 6;

		m_renderPass = Renderer::BeginPass();
		Renderer::PipelineBind(m_renderPass, *m_Shader);
		Renderer::BindUniform(m_renderPass, *m_Shader);
		Renderer::BindTexture(m_renderPass, *m_Shader, 1, 0);
		Renderer::Draw(m_renderPass, numberOfVertices);
		Renderer::EndPass(m_renderPass);
	}

	void RenderLayer::SetupPipeline()
	{
		using namespace OWC::Graphics;

		std::vector<BindingDiscription> fragmentBindingDiscriptions = {
			{
				.descriptorCount = 1,
				.binding = 0,
				.descriptorType = DescriptorType::UniformBuffer,
				.stageFlags = ShaderType::Fragment
			},
			{
				.descriptorCount = 1,
				.binding = 1,
				.descriptorType = DescriptorType::CombinedImageSampler,
				.stageFlags = ShaderType::Fragment
			}
		};

		std::vector<ShaderData> shaderDatas = {
			{
				.bytecode = LoadFileToBytecode<uint32_t>("../ShaderSrc/testImage.vert.spv"),
				.type = ShaderType::Vertex,
				.language = ShaderData::ShaderLanguage::SPIRV,
				.descriptorType = {}
			},
			{
				.bytecode = LoadFileToBytecode<uint32_t>("../ShaderSrc/testImage.frag.spv"),
				.type = ShaderType::Fragment,
				.language = ShaderData::ShaderLanguage::SPIRV,
				.descriptorType = fragmentBindingDiscriptions
			}
		};

		m_Shader = BaseShader::CreateShader(shaderDatas);
		m_Shader->BindUniform(0, m_UniformBuffer);
		m_Shader->BindTexture(1, m_Image);
	}
}
