#pragma once
#include <string>
#include <vector>
#include <span>
#include <memory>


namespace OWC::Graphics
{
	enum class ShaderType : uint8_t
	{
		Vertex = 1<<0,
		Fragment = 1<<1,
		Compute = 1<<2
	};

	enum class DescriptorType : uint8_t
	{
		UniformBuffer,
		Sampler,
		CombinedImageSampler,
		StorageBuffer
	};

	struct BindingDiscription
	{
		uint32_t descriptorCount;
		uint32_t binding;
		DescriptorType descriptorType;
		ShaderType stageFlags;
	};

	struct ShaderData
	{
		enum class ShaderLanguage : uint8_t
		{ // for now only SPIR-V is supported
//			GLSL,
//			HLSL,
//			Slang,
			SPIRV,

			SPV = SPIRV
		};

		std::string ShaderTypeToString() const
		{
			switch (type)
			{
			using enum OWC::Graphics::ShaderType;
			case Vertex:
				return "Vertex";
			case Fragment:
				return "Fragment";
			case Compute:
				return "Compute";
			default:
				return "Unknown";
			}
		}

//		std::string source;
		std::vector<uint32_t> bytecode; // for SPIR-V
		
		ShaderType type;
		ShaderLanguage language;
		std::span<BindingDiscription> descriptorType;
	};

	class BaseShader
	{
	public:
		BaseShader() = default;
		virtual ~BaseShader() = default;
		BaseShader(BaseShader&) = default;
		BaseShader& operator=(BaseShader&) = default;
		BaseShader(BaseShader&&) noexcept = default;
		BaseShader& operator=(BaseShader&&) noexcept = default;

		static std::unique_ptr<BaseShader> CreateShader(const std::span<ShaderData>& shaderDatas);
	};
}
