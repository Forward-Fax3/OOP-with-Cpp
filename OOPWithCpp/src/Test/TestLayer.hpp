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
		glm::vec4 m_Colour = { 0.0, 0.0, 1.0, 1.0 };
	};
}
