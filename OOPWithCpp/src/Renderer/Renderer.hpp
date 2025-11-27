#pragma once
#include "GraphicsContext.hpp"


namespace OWC
{
	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void FinishRender();

	private:
		static std::shared_ptr<Graphics::GraphicsContext> s_GC;
	};
}
