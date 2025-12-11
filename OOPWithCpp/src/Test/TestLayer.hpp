#pragma once
#include "Layer.hpp"
#include "BaseShader.hpp"
#include "Renderer.hpp"
#include "UniformBuffer.hpp"

#include <memory>
#include <glm/glm.hpp>


namespace OWC
{
	class TestLayer : public Layer
	{
	public:
		TestLayer();
		~TestLayer() override = default;
		TestLayer(const TestLayer&) = delete;
		TestLayer& operator=(const TestLayer&) = delete;
		TestLayer(TestLayer&&) = delete;
		TestLayer& operator=(TestLayer&&) = delete;

		void OnUpdate() override;
		void ImGuiRender() override;
		void OnEvent(class BaseEvent& event) override;

	private:
		void SetupRenderPass();
		void SetupPipeline();

	private:
		std::unique_ptr<Graphics::BaseShader> m_Shader = nullptr;
		std::shared_ptr<Graphics::RenderPassData> m_renderPass = nullptr;
		std::shared_ptr<Graphics::UniformBuffer> m_UniformBuffer = nullptr;
		std::shared_ptr<Graphics::TextureBuffer> m_Image = nullptr;
		float m_Divider = 1.0f;
		float m_ImageScale = 1.0f;
		glm::vec2 m_ImageOffset = glm::vec2(0.0f, 0.0f);
		bool m_ToggleUV = false;
	};
}
