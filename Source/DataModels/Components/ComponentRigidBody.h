#pragma once
#include "Component.h"
#include "ComponentTransform.h"

#include "Math/float3.h"
#include "Math/Quat.h"


enum class ForceMode
{
	Force,
	Acceleration,
	Impulse,
	VelocityChange
};

class ComponentRigidBody :
	public Component
{
public:
	ComponentRigidBody(bool active, GameObject* owner);
	~ComponentRigidBody() override;

	void Update() override;
	void Draw() override;

	void AddForce(const float3& force, ForceMode mode = ForceMode::Force);
	void AddTorque(const float3& torque, ForceMode mode = ForceMode::Force);

	void SetPositionTarget(const float3& targetPos);
	void SetRotationTarget(const Quat& targetRot);
	void DisablePositionController();
	void DisableRotationController();

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	bool GetIsKinematic() const;
	void SetIsKinematic(bool newIsKinematic);

private:
	ComponentTransform* transform;
	bool isKinematic;
	float mass;

	float3 x;
	Quat q;
	float3 g;
	float3 v0;
	float3 w0;

	float3 targetPosition;
	Quat targetRotation;
	bool usePositionController = false;
	bool useRotationController = false;
	float KpForce = 5.0f;
	float KpTorque = 0.05f;
	float3 externalForce = float3::zero;
	float3 externalTorque = float3::zero;

	void ApplyForce();
	void ApplyTorque();
};

inline bool ComponentRigidBody::GetIsKinematic() const
{
	return this->isKinematic;
}

inline void ComponentRigidBody::SetIsKinematic(bool newIsKinematic)
{
	isKinematic = newIsKinematic;
}

inline void ComponentRigidBody::SetPositionTarget(const float3& targetPos)
{
	targetPosition = targetPos;
	usePositionController = true;
}

inline void ComponentRigidBody::SetRotationTarget(const Quat& targetRot)
{
	targetRotation = targetRot;
	useRotationController = true;
}

inline void ComponentRigidBody::DisablePositionController()
{
	usePositionController = false;
}

inline void ComponentRigidBody::DisableRotationController()
{
	useRotationController = false;
}
