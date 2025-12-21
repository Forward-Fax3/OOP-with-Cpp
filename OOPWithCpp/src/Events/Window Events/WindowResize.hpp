#pragma once
#include "Core.hpp"
#include "BaseEvent.hpp"


namespace OWC
{
	class WindowResize final : public BaseEvent
	{
	public:
		WindowResize(i32 width, i32 height)
			: BaseEvent(EventType::WindowResize), m_Width(width), m_Height(height) {}
		~WindowResize() override = default;

		static EventType GetStaticType() { return EventType::WindowResize; }

		inline i32 GetWidth() const { return m_Width; }
		inline i32 GetHeight() const { return m_Height; }

	private:
		i32 m_Width;
		i32 m_Height;
	};
} // namespace OWC