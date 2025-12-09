#include "TestLayer.hpp"
#include "Application.hpp"
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
	TestLayer::TestLayer()
	{
//		m_Image = Renderer::LoadImage();
		m_UniformBuffer = Graphics::UniformBuffer::CreateUniformBuffer(sizeof(glm::vec4)); // example size for a float4
		SetupPipeline();
		SetupRenderPass();
	}

	void TestLayer::OnUpdate()
	{
		using namespace OWC::Graphics;

		std::array<std::string_view, 1> waitSemaphorenames = { "ImageReady" };
		std::array<std::string_view, 1> signalSemaphoreNames = { "TestLayer" };

		m_UniformBuffer->UpdateBufferData(std::as_bytes(std::span<const glm::vec4>(&m_Colour, 1)));
		Renderer::RestartRenderPass(m_renderPass);
		Renderer::SubmitRenderPass(m_renderPass, waitSemaphorenames, signalSemaphoreNames);
	}

	void TestLayer::ImGuiRender()
	{ 
		ImGui::Begin("Test Layer");
		ImGui::ColorEdit4("Colour", glm::value_ptr(m_Colour));
		ImGui::End();
	}

	void TestLayer::OnEvent(class BaseEvent& e)
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

	void TestLayer::SetupRenderPass()
	{
		using namespace OWC::Graphics;
		m_renderPass = Renderer::BeginPass();
		Renderer::PipelineBind(m_renderPass, *m_Shader);
		Renderer::BindUniform(m_renderPass, *m_Shader, m_UniformBuffer);
		Renderer::Draw(m_renderPass, 6);
		Renderer::EndPass(m_renderPass);
	}

	void TestLayer::SetupPipeline()
	{
		using namespace OWC::Graphics;

//		std::vector<BindingDiscription> fragmentBindingDiscriptions = {
//			{
//				.descriptorCount = 1,
//				.binding = 0,
//				.descriptorType = DescriptorType::UniformBuffer,
//				.stageFlags = ShaderType::Fragment
//			},
//			{
//				.descriptorCount = 1,
//				.binding = 1,
//				.descriptorType = DescriptorType::CombinedImageSampler,
//				.stageFlags = ShaderType::Fragment
//			}
//		};

		std::vector<BindingDiscription> fragmentBindingDiscriptions = {
			{
				.descriptorCount = 1,
				.binding = 0,
				.descriptorType = DescriptorType::UniformBuffer,
				.stageFlags = ShaderType::Fragment
			}
		};

		std::vector<ShaderData> shaderDatas = {
			{
				.bytecode = LoadFileToBytecode<uint32_t>("../ShaderSrc/test.vert.spv"),
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
	}
}
