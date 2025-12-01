#pragma once
#define RENDERER_HPP
#ifndef GRAPHICSCONTEXT_HPP
#include "GraphicsContext.hpp"
#endif
#include "BaseShader.hpp"


namespace OWC::Graphics
{
	class GraphicsContext;

	enum class RendererAPI
	{
//		None = 0,
		Vulkan = 1,
	}; // only Vulkan is supported for now

	enum class RenderPassType
	{
		Graphics = 0,
		Compute = 1,
		Transfer = 2,
	};

	class Renderer;

	class RenderPassData
	{
		friend class Renderer;

	public:
		virtual ~RenderPassData() = default;

	private:
		void virtual AddPipeline(const BaseShader& shader) = 0;
		void virtual Draw(uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0) = 0;
		void virtual EndRenderPass() = 0;
		void virtual submitRenderPass() = 0;

		void virtual RestartRenderPass() = 0;

	protected:
		explicit RenderPassData(RenderPassType type) : type(type) {}
		// delete copy/move constructor and copy/move assignment operator
		RenderPassData(const RenderPassData&) = delete;
		RenderPassData& operator=(const RenderPassData&) = delete;
		RenderPassData(RenderPassData&&) = delete;
		RenderPassData& operator=(RenderPassData&&) = delete;

		RenderPassType GetRenderPassType() const { return type; }

	private:
		RenderPassType type;
	};

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void FinishRender();

		static std::shared_ptr<RenderPassData> BeginPass(RenderPassType type = RenderPassType::Graphics);
		static void PipelineBind(const std::shared_ptr<RenderPassData>& data, const BaseShader& shader);
		static void Draw(const std::shared_ptr<RenderPassData>& data, uint32_t vertexCount, uint32_t instanceCount = 1, uint32_t firstVertex = 0, uint32_t firstInstance = 0);
		static void EndPass(const std::shared_ptr<RenderPassData>& data);
		static void SubmitRenderPass(const std::shared_ptr<RenderPassData>& data);

		static void RestartRenderPass(const std::shared_ptr<RenderPassData>& data);

		static inline RendererAPI GetAPI() { return s_API; }

	private:
		static std::shared_ptr<GraphicsContext> s_GC;
		static RendererAPI s_API;
	};
}
