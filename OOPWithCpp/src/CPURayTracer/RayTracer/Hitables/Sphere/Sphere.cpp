#include "Sphere.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>


namespace OWC
{
	HitData Sphere::IsHit(const Ray& ray) const
	{
		HitData hitData{};

		glm::vec3 oc = ray.GetOrigin() - m_Center;

		float a = glm::length2(ray.GetDirection());
		float h = glm::dot(oc, ray.GetDirection());
		float c = glm::length2(oc) - m_Radius * m_Radius;

		float discriminant = h * h - a * c;
		if (discriminant <= 0.0f)
		{
			hitData.hasHit = false;
			return hitData;
		}

		float sqrtDiscriminant = glm::sqrt(discriminant);

		float root = (-h - sqrtDiscriminant) / a;
		if (root < 0.001f)
		{
			root = (-h + sqrtDiscriminant) / a;
			if (root < 0.001f)
			{
				hitData.hasHit = false;
				return hitData;
			}
		}

		hitData.hasHit = true;
		hitData.t = root;
		hitData.point = ray.GetPointAtDistance(hitData.t);
		hitData.material = m_Material.get();

		Vec3 normal = (hitData.point - m_Center) / m_Radius;
		hitData.SetFaceNormal(ray, normal);

		return hitData;
	}
}