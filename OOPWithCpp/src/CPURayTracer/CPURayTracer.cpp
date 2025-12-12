#include "Application.hpp"
#include "CPURayTracer.hpp"
#include "ImageLoader.hpp"


namespace OWC
{
	CPURayTracer::CPURayTracer(const std::shared_ptr<InterLayerData>& interLayerData)
		: m_InterLayerData(interLayerData)
	{
		m_ImageLoader = std::make_unique<ImageLoader>("../cat.jpg");
		m_SecondImageLoader = std::make_unique<ImageLoader>("../cat2.jpg");
	}

	void CPURayTracer::OnUpdate()
	{
		if (!(m_ImageStateUpdated || m_SecondImageStateUpdated))
			return;

		if (m_ToggleImage)
		{
			if (!m_ToggleSecondImage)
			{
				m_InterLayerData->imageData = m_ImageLoader->GetImageData();
				m_InterLayerData->imageHeight = static_cast<uint32_t>(m_ImageLoader->GetHeight());
				m_InterLayerData->imageWidth = static_cast<uint32_t>(m_ImageLoader->GetWidth());
			}
			else
			{
				m_InterLayerData->imageData = m_SecondImageLoader->GetImageData();
				m_InterLayerData->imageHeight = static_cast<uint32_t>(m_SecondImageLoader->GetHeight());
				m_InterLayerData->imageWidth = static_cast<uint32_t>(m_SecondImageLoader->GetWidth());
			}

			m_InterLayerData->ImageUpdates |= 0b01;

			if (m_ImageStateUpdated)
				m_InterLayerData->ImageUpdates |= 0b10;
		}
		else
		{
			m_InterLayerData->imageData.clear();
			m_InterLayerData->imageHeight = 0;
			m_InterLayerData->imageWidth = 0;
			m_InterLayerData->ImageUpdates |= 0b10;
		}

		m_ImageStateUpdated = false;
		m_SecondImageStateUpdated = false;
	}

	void CPURayTracer::ImGuiRender()
	{
		ImGui::Begin("CPU Ray Tracer");
		ImGui::Text("CPU Ray Tracer Layer");
		m_ImageStateUpdated = ImGui::Checkbox("toggle image", &m_ToggleImage);
		if (m_ToggleImage)
			m_SecondImageStateUpdated = ImGui::Checkbox("toggle second image", &m_ToggleSecondImage);
		ImGui::End();
	}

	void CPURayTracer::OnEvent(class BaseEvent&)
	{
	}
}