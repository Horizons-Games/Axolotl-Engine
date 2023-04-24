#pragma once
#include "Component.h"
#include "Auxiliar/Generics/Updatable.h"

#include "Math/float3.h"
#include "Math/Quat.h"


enum class ForceMode
{
	Force,
	Acceleration,
	Impulse,
	VelocityChange
};

class ComponentTransform;

class ComponentRigidBody : public Component, public Updatable
{
public:
	ComponentRigidBody(bool active, GameObject* owner);
	ComponentRigidBody(const ComponentRigidBody& componentRigidBody);
	~ComponentRigidBody() override;

	void Update() override;

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
	
	float GetMass() const;
	void SetMass(float newMass);

	float3 GetGravity() const;
	void SetGravity(float3 newGravity);

	bool IsOnGround() const;
	
	bool GetUsePositionController() const;
	void SetUsePositionController(bool newUsePositionController);
	
	bool GetUseRotationController() const;
	void SetUseRotationController(bool newUsePositionController);
	
	float GetKpForce() const;
	void SetKpForce(float newKpForce);
	
	float GetKpTorque() const;
	void SetKpTorque(float newKpForce);
	
	void SetBottomHitPoint(float height);

private:
	ComponentTransform* transform;
	bool isKinematic;
	float mass;

	float3 x;
	Quat q;
	float3 g;
	float3 v0;
	float3 w0;

	float height;
	bool bootsOnGround = false;

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
	return isKinematic;
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

inline void ComponentRigidBody::SetBottomHitPoint(float height)
{
	this->height = height;
}

inline bool ComponentRigidBody::IsOnGround() const
{
	return bootsOnGround;
}

inline float ComponentRigidBody::GetMass() const
{
	return mass;
}

inline void ComponentRigidBody::SetMass(float newMass)
{
	mass = newMass;
}

inline float3 ComponentRigidBody::GetGravity() const
{
	return g;
}

inline void ComponentRigidBody::SetGravity(float3 newGravity)
{
	g = newGravity;
}

inline bool ComponentRigidBody::GetUsePositionController() const
{
	return usePositionController;
}

inline void ComponentRigidBody::SetUsePositionController(bool newUsePositionController)
{
	usePositionController = newUsePositionController;
}

inline bool ComponentRigidBody::GetUseRotationController() const
{
	return useRotationController;
}

inline void ComponentRigidBody::SetUseRotationController(bool newUseRotationController)
{
	useRotationController = newUseRotationController;
}

inline float ComponentRigidBody::GetKpForce() const
{
	return KpForce;
}

inline void ComponentRigidBody::SetKpForce(float newKpForce)
{
	KpForce = newKpForce;
}

inline float ComponentRigidBody::GetKpTorque() const
{
	return KpTorque;
}

inline void ComponentRigidBody::SetKpTorque(float newKpTorque)
{
	KpTorque = newKpTorque;
}