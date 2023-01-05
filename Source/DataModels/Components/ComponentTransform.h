#pragma once
#include "Components/Component.h"
#include "Math/float3.h"
#include "Math/float3x3.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform(const bool active, GameObject* owner);
	~ComponentTransform() override;

	void Update() override;

	const float3& GetPosition() const;
	const float3& GetRotation() const;
	const float3& GetScale() const;

	void SetPosition(const float3& position);
	void SetRotation(const float3& rotation);
	void SetScale(const float3& scale);

	const float3x3& GetLocalMatrix() const;
	const float3x3& GetGlobalMatrix() const;

	void SetLocalMatrix(const float3x3& matrix);
	void SetGlobalMatrix(const float3x3& matrix);

	void CalculateLocalMatrix();
	void CalculateGlobalMatrix();

	void ResetGlobalMatrix();

private:
	GameObject* ownerParent;

	float posX = 0.0f;
	float posY = 0.0f;
	float posZ = 0.0f;
	float rotX = 0.0f;
	float rotZ = 0.0f;
	float rotY = 0.0f;
	float scaX = 1.0f;
	float scaY = 1.0f;
	float scaZ = 1.0f;

	float3x3 localMatrix = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
	};

	float3x3 globalMatrix = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
	};
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

inline void ComponentTransform::SetPosition(const float3& position)
{
	posX = position.x;
	posY = position.y;
	posZ = position.z;
}

inline void ComponentTransform::SetRotation(const float3& rotation)
{
	rotX = rotation.x;
	rotY = rotation.y;
	rotZ = rotation.z;
}

inline void ComponentTransform::SetScale(const float3& scale)
{
	scaX = scale.x;
	scaY = scale.y;
	scaZ = scale.z;
}

inline const float3x3& ComponentTransform::GetLocalMatrix() const
{
	return localMatrix;
}

inline const float3x3& ComponentTransform::GetGlobalMatrix() const
{
	return globalMatrix;
}

inline void ComponentTransform::SetLocalMatrix(const float3x3& matrix)
{
	this->localMatrix = matrix;
}

inline void ComponentTransform::SetGlobalMatrix(const float3x3& matrix)
{
	this->globalMatrix = matrix;
}

inline void ComponentTransform::ResetGlobalMatrix()
{
	this->globalMatrix = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
	};
}