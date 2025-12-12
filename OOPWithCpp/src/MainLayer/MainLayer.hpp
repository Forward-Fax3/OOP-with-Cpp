#pragma once
#include "Layer.hpp"
#include "RenderLayer.hpp"
#include "CPURayTracer.hpp"


namespace OWC
{
	class MainLayer : public Layer
	{
	public:
		MainLayer();
		~MainLayer() override;
		MainLayer(const MainLayer&) = delete;
		MainLayer& operator=(const MainLayer&) = delete;
		MainLayer(MainLayer&&) = delete;
		MainLayer&& operator=(MainLayer&&) = delete;
		void OnUpdate() override;
		void ImGuiRender() override;
		void OnEvent(class BaseEvent& event) override;

	private:
		std::shared_ptr<RenderLayer> m_TestLayer;
		std::shared_ptr<CPURayTracer> m_CPURayTracerLayer;
	};
}