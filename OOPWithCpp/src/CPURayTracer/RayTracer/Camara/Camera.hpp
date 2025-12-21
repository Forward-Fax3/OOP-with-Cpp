#pragma once
#include "Core.hpp"
#include "Ray.hpp"
#include "BaseHittable.hpp"

#include <memory>


namespace OWC
{
	class RTCamera
	{
	public:
		RTCamera() = delete;
		explicit RTCamera(const std::shared_ptr<BaseHittable>& hittables, std::vector<Colour>& pixels);

		OWC_FORCE_INLINE void SetRotate(const Vec3& eulerAngles) { m_Rotation = eulerAngles; m_PositionNeedsUpdating = true; }
		OWC_FORCE_INLINE void SetPosition(const Vec3& position) { m_Position = position; m_PositionNeedsUpdating = true; }
		OWC_FORCE_INLINE void SetScreenSize(const Vec2& size) { m_ScreenSize = size; m_PositionNeedsUpdating = true; }
		OWC_FORCE_INLINE void SetFOV(const f32 fov) { m_FOV = fov; m_PositionNeedsUpdating = true; }
		OWC_FORCE_INLINE void SetVup(const Vec3& vup) { m_Vup = vup; m_PositionNeedsUpdating = true; }
		OWC_FORCE_INLINE void SetFocalLength(const f32 focalLength) { m_FocalLength = focalLength; m_PositionNeedsUpdating = true; }

		OWC_FORCE_INLINE void SetNumberOfSamplesPerPass(const uSize samples) { m_NumberOfSamplesPerPass = samples; }

		void SingleThreadedRenderPass();
//		void MultiThreadedRenderPass();

	private:
		void CreateViewMatrix();
		Ray CreateRay(uSize i, uSize j) const;

		Colour RayColour(Ray ray);

	private:
		Vec3 m_Position{ 0.0f };
		Vec3 m_Rotation{ 0.0f };
		Vec3 m_Vup{ 0.0f, 1.0f, 0.0f };
		Vec3 m_PixelDeltaU{};
		Vec3 m_PixelDeltaV{};
		Point m_Pixel100Location{};
		Vec2 m_ScreenSize{};
		f32 m_FocalLength = 5.0f;
		f32 m_FOV = 30.0f;

		bool m_PositionNeedsUpdating = false;

		uSize m_NumberOfSamplesPerPass = 1;
		uSize m_MaxBounces = 5;

		std::vector<Colour>& m_Pixels;
		std::vector<Colour> m_BouncedColours;
		std::shared_ptr<BaseHittable> m_Hittables;
	};
}
