#pragma once
#include "Core.hpp"
#include "BaseHittable.hpp"

#include <memory>
#include <vector>


namespace OWC
{
	class Hitables : public BaseHittable
	{
	public:
		Hitables() = default;

		void AddObject(const std::shared_ptr<BaseHittable>& newHittable);
		void AddObjects(const std::vector<std::shared_ptr<BaseHittable>>& newHittables);
		void AddObjects(const std::shared_ptr<Hitables>& newHittables);

		OWC_FORCE_INLINE void Reserve(uSize numberOfObjects) { m_Hitables.reserve(numberOfObjects); }
		OWC_FORCE_INLINE uSize GetNumberOfObjects() const { return m_Hitables.size(); }
		OWC_FORCE_INLINE void ClearObjects() { m_Hitables.clear(); }

		HitData __vectorcall IsHit(const Ray& ray, const Interval& range) const override;

		Colour BackgroundColour(const Ray& ray) const override
		{
			return m_BackgroundFunction ? 
				m_BackgroundFunction(ray) : 
				BaseHittable::BackgroundColour(ray);
		}

		OWC_FORCE_INLINE void SetBackgroundFunction(const std::function<Colour(const Ray& ray)>& backgroundFunction)
		{
			m_BackgroundFunction = backgroundFunction;
		}

	private:
		std::vector<std::shared_ptr<BaseHittable>> m_Hitables;
		std::function<Colour(const Ray& ray)> m_BackgroundFunction;
	};
}
