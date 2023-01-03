#pragma once
#include "Components/Component.h"
#include "Math/float3.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform(const bool active, GameObject* owner);
	~ComponentTransform() override;

	void Update() override;

	const float3& GetPosition() const;
	const float3& GetRotation() const;
	const float3& GetScale() const;

private:
	float posX, posY, posZ = 0.0f;
	float rotX, rotY, rotZ = 0.0f;
	float scaX, scaY, scaZ = 1.0f;
};

inline const float3& ComponentTransform::GetPosition() const
{
	return float3(posX, posY, posZ);
}

inline const float3& ComponentTransform::GetRotation() const
{
	return float3(rotX, rotY, rotZ);
}

inline const float3& ComponentTransform::GetScale() const
{
	return float3(scaX, scaY, scaZ);
}