#pragma once
#include "Component.h"

#include "Geometry/Frustum.h"
#include "Geometry/Plane.h"
#include "Geometry/OBB.h"
#include "ComponentTransform.h"

#define COMPONENT_CAMERA "Camera"

enum class ECameraFrustumMode
{
	UNKNOWN,
	NORMALFRUSTUM,
	OFFSETFRUSTUM,
	NOFRUSTUM
};

const static std::string GetNameByFrustumMode(ECameraFrustumMode type);
const static ECameraFrustumMode GetFrustumModeByName(const std::string& name);

class Json;

class ComponentCamera : public Component
{
public:
	ComponentCamera(bool active, GameObject* owner);
	~ComponentCamera() override;

	void Update() override;
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void UpdateFrustumOffset();
	bool IsInside(const OBB& obb);
	bool IsInsideOffset(const OBB& obb);

	void SetDrawFrustum(bool newFrustum);
	void SetFrustumMode(ECameraFrustumMode newFrustumMode);
	void SetFrustumOffset(float newFrustumOffset);

	bool IsDrawFrustum() const;
	ECameraFrustumMode GetFrustumMode() const;
	float GetFrustumOffset() const;


private:
	Frustum frustum;
	ECameraFrustumMode frustumMode;
	float frustumOffset;
	Plane offsetFrustumPlanes[6];
	bool drawFrustum;
	ComponentTransform* trans;
};

inline void ComponentCamera::SetDrawFrustum(bool newFrustum)
{
	drawFrustum = newFrustum;
}

inline void ComponentCamera::SetFrustumMode(ECameraFrustumMode newFrustumMode)
{
	frustumMode = newFrustumMode;
}

inline void ComponentCamera::SetFrustumOffset(float newFrustumOffset)
{
	frustumOffset = newFrustumOffset;
}

inline bool ComponentCamera::IsDrawFrustum() const
{
	return drawFrustum;
}

inline ECameraFrustumMode ComponentCamera::GetFrustumMode() const
{
	return frustumMode;
}

inline float ComponentCamera::GetFrustumOffset() const
{
	return frustumOffset;
}

inline const std::string GetNameByFrustumMode(ECameraFrustumMode type)
{
	switch (type)
	{
	case ECameraFrustumMode::NOFRUSTUM:
		return "Frustum_NoFrustum";

	case ECameraFrustumMode::NORMALFRUSTUM:
		return "Frustum_NormalFrustum";

	case ECameraFrustumMode::OFFSETFRUSTUM:
		return "Frustum_OffsetFrustum";

	default:
		assert(false && "Wrong frustum mode introduced");
		return "";
	}
}

inline const ECameraFrustumMode GetFrustumModeByName(const std::string& typeName)
{
	if (typeName == "Frustum_NoFrustum")
	{
		return ECameraFrustumMode::NOFRUSTUM;
	}

	else if (typeName == "Frustum_NormalFrustum")
	{
		return ECameraFrustumMode::NORMALFRUSTUM;
	}

	else if (typeName == "Frustum_OffsetFrustum")
	{
		return ECameraFrustumMode::OFFSETFRUSTUM;
	}

	return ECameraFrustumMode::UNKNOWN;
}