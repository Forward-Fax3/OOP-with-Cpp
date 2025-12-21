#pragma once
#include "Core.hpp"


namespace OWC
{
	class Interval
	{
	public:
		Interval() = delete;
		inline Interval(f32 min, f32 max) : m_MinMax(min, max) {}
		~Interval() = default;

		Interval(const Interval&) = default;
		Interval& operator=(const Interval&) = default;
		Interval(Interval&&) = default;
		Interval& operator=(Interval&&) = default;

		OWC_FORCE_INLINE f32 GetMin() const { return m_MinMax.x; }
		OWC_FORCE_INLINE f32 GetMax() const { return m_MinMax.y; }

		OWC_FORCE_INLINE void SetMin(f32 min) { m_MinMax.x = min; }
		OWC_FORCE_INLINE void SetMax(f32 max) { m_MinMax.y = max; }

		OWC_FORCE_INLINE bool Contains(f32 value) const
		{
			return m_MinMax.x <= value && value <= m_MinMax.y;
		}

	private:
 		Vec2 m_MinMax{};
	};
}
