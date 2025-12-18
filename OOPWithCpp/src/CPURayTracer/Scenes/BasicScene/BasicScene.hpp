#pragma once
#include "Scene.hpp"
#include "BaseHittable.hpp"
#include "Camera.hpp"


namespace OWC
{
	class BasicScene : public BaseScene
	{
	public:
		BasicScene() = delete;
		explicit BasicScene(std::vector<Vec4>& frameBuffer);
		~BasicScene() override = default;

		BasicScene(const BasicScene&) = delete;
		BasicScene& operator=(const BasicScene&) = delete;
		BasicScene(BasicScene&&) = delete;
		BasicScene& operator=(BasicScene&&) = delete;

		RenderPassReturnData RenderNextPass() override;

		void OnImGuiRender() override;
		bool OnEvent(BaseEvent& e) override;

	private:
		std::shared_ptr<BaseHittable> m_Hittables;
		std::unique_ptr<RTCamera> m_Camera = nullptr;
		Vec3 m_CameraRotation{ 0.0 };
		Vec3 m_CameraPosition{ 0.0, 0.0, -5.0 };
		float m_FOV = 45.0f;
		float m_FocalLength = 45.0f;
		bool m_ImageNeedsClearing = false;
	};
}
