#include "Core.hpp"
#include "Application.hpp"
#include "DuelWhiteSpheres.hpp"

#include "Hittables.hpp"

#include "Sphere.hpp"
#include "Lambertian.hpp"

#include "BaseEvent.hpp"
#include "WindowResize.hpp"

#include <imgui.h>

#include <glm/gtc/type_ptr.hpp>


namespace OWC
{
	DuelGraySpheres::DuelGraySpheres()
	{
		m_SceneObjects = std::make_shared<Hitables>();
		m_SceneObject = m_SceneObjects;
		m_SceneObjects->Reserve(2);

		auto gray = std::make_shared<Lambertian>(Colour(0.5f));
		auto smallSphere = std::make_shared<Sphere>(Vec3(0.0f), 1.0f, gray);
		auto bigSphere = std::make_shared<Sphere>(Vec3(0.0f, 51.0f, 0.0f), 50.0f, gray);
		m_SceneObjects->AddObject(smallSphere);
		m_SceneObjects->AddObject(bigSphere);
	}

	void DuelGraySpheres::SetBaseCameraSettings(CameraRenderSettings& cameraSettings) const
	{
		cameraSettings.Position = Point(0.0f, 0.0f, -5.0f);
		cameraSettings.Rotation = Vec3(0.0f);
		cameraSettings.FOV = 45.0f;
		cameraSettings.FocalLength = 45.0f;
	}
}
