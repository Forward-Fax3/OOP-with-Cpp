#pragma once
#include "Core.hpp"
#include "Ray.hpp"
#include "BaseHittable.hpp"

#include <memory>


namespace OWC
{
	using RenderPassReturnData = bool;

	struct CameraRenderSettings
	{
		Vec3 Position{ 0.0f };
		Vec3 Rotation{ 0.0f };

		Vec2 ScreenSize{ 800.0f, 600.0f };

		f32 FOV = 30.0f;
		f32 FocalLength = 5.0f;
		Vec3 Vup{ 0.0f, 1.0f, 0.0f };

		uSize NumberOfSamplesPerPass = 1;
		uSize MaxBounces = 5;
	};

	class RTCamera
	{
	public:
		RTCamera() = delete;
		explicit RTCamera(std::vector<Colour>& pixels) : m_Pixels(pixels) {}

		OWC_FORCE_INLINE CameraRenderSettings& GetSettings() { return m_Settings; }

		RenderPassReturnData SingleThreadedRenderPass(const std::shared_ptr<BaseHittable>& hittables);
//		RenderPassReturnData MultiThreadedRenderPass(const std::shared_ptr<BaseHittable>& hittables);

		void UpdateCameraSettings();

	private:
		Ray CreateRay(uSize i, uSize j) const;

		Colour RayColour(Ray ray, const std::shared_ptr<BaseHittable>& hittables);

	private:
		CameraRenderSettings m_Settings;

		Point m_Pixel100Location = Point(0.0f);
		Vec3 m_PixelDeltaU = Vec3(0.0f);
		Vec3 m_PixelDeltaV = Vec3(0.0f);

		std::vector<Colour>& m_Pixels;
		std::vector<Colour> m_BouncedColours;
	};
}
