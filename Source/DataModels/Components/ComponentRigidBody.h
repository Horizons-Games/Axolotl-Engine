#pragma once
#include "Auxiliar/Generics/Updatable.h"
#include "Bullet/LinearMath/btVector3.h"
#include "Bullet/btBulletDynamicsCommon.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "Math/Quat.h"
#include <functional>
#include <memory>
#include <vector>

class btRigidBody;
struct btDefaultMotionState;
class btCollisionShape;

class ComponentRigidBody : public Component, public Updatable
{
public:
	enum class Shape
	{
		NONE = 0,
		BOX = 1,
		SPHERE = 2,
		CAPSULE = 3,
		CONE = 4,
		CYLINDER = 5,
		CONVEX_HULL = 6,
		TRIANGLE_MESH = 7,
		TERRAIN = 8
	};

	ComponentRigidBody(bool active, GameObject* owner);
	ComponentRigidBody(const ComponentRigidBody& toCopy);
	~ComponentRigidBody();

	void OnCollisionEnter(ComponentRigidBody* other);

	void OnCollisionStay(ComponentRigidBody* other);

	void OnCollisionExit(ComponentRigidBody* other);

	void OnTransformChanged() override;

	void Update() override;

	void SetOwner(GameObject* owner) override;

	uint32_t GetID() const
	{
		return id;
	}

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void Enable() override;
	void Disable() override;

	void SetIsKinematic(bool isKinematic);
	bool GetIsKinematic() const;

	void SetIsStatic(bool isStatic);
	bool IsStatic() const;

	void SetDrawCollider(bool newDrawCollider, bool substract = true);
	bool GetDrawCollider() const;

	float GetMass() const;
	void SetMass(float newMass);

	btVector3 GetGravity() const;
	void SetGravity(btVector3 newGravity);

	float GetLinearDamping() const;
	void SetLinearDamping(float newDamping);

	float GetAngularDamping() const;
	void SetAngularDamping(float newDamping);

	Shape GetShape() const;
	void SetCollisionShape(Shape newShape);

	btVector3 GetVelocity() const;
	void SetVelocity(const float3& force);

	btScalar GetRestitution() const;
	void SetRestitution(float restitution);

	float3 GetBoxSize() const;
	void SetBoxSize(float3 newBoxSize);

	float GetRadius() const;
	void SetRadius(float newRadius);

	float GetFactor() const;
	void SetFactor(float newFactor);

	float GetHeight() const;
	void SetHeight(float newHeight);

	void SetDefaultSize(Shape resetShape);
	void SetDefaultPosition();

    btVector3 GetRigidBodyOrigin() const;
    void SetRigidBodyOrigin(btVector3 origin);

	btVector3 GetRigidBodyTranslation() const;
	void UpdateRigidBodyTranslation();

    void SetPositionTarget(const float3& targetPos);
    void SetRotationTarget(const Quat& targetRot);

	bool GetUsePositionController() const;
	void SetUsePositionController(bool newUsePositionController);

	bool GetUseRotationController() const;
	void SetUseRotationController(bool newUseRotationController);

	void DisablePositionController();
	void DisableRotationController();

	float GetKpForce() const;
	void SetKpForce(float newKpForce);

	float GetKpTorque() const;
	void SetKpTorque(float newKpTorque);

    void RemoveRigidBodyFromSimulation();

    btRigidBody* GetRigidBody() const;
    ComponentTransform* GetOwnerTransform() const;
	void SetUpMobility();

	void UpdateRigidBody();

	template<typename T>
	void AddCollisionEnterDelegate(void (T::*func)(ComponentRigidBody*), T* obj)
	{
		delegateCollisionEnter.push_back(std::bind(func, obj, std::placeholders::_1));
	}

    void ClearCollisionEnterDelegate();

private:
	int GenerateId() const;

	std::unique_ptr<btRigidBody> rigidBody = nullptr;
	std::unique_ptr<btDefaultMotionState> motionState = nullptr;
	std::unique_ptr<btCollisionShape> shape = nullptr;

    btVector3 gravity = { 0, -9.81f, 0 };
    btVector3 translation = { 0.0f, 0.0f, 0.0f };
    float linearDamping = 0.1f;
    float angularDamping = 0.1f;
    float mass = 100.0f;
    float restitution = 0.f;
    float3 boxSize;
    float radius;
    float factor;
    float height;

	bool isKinematic = false;
	bool isStatic = false;
	bool drawCollider = false;

	Shape currentShape = Shape::NONE;

	float3 targetPosition;
	Quat targetRotation;
	bool usePositionController = false;
	bool useRotationController = false;
	float KpForce = 5.0f;
	float KpTorque = 0.05f;

	bool isFromSceneLoad = true;

	ComponentTransform* transform;

	uint32_t id = 0;

	// Delegate for collision enter event the parameter is the other collider
	std::vector<std::function<void(ComponentRigidBody*)>> delegateCollisionEnter;
};

inline bool ComponentRigidBody::GetIsKinematic() const
{
	return isKinematic;
}

inline void ComponentRigidBody::SetIsKinematic(bool newIsKinematic)
{
	isKinematic = newIsKinematic;
}

inline bool ComponentRigidBody::IsStatic() const
{
	return isStatic;
}

inline void ComponentRigidBody::SetIsStatic(bool newIsStatic)
{
	isStatic = newIsStatic;
}

inline bool ComponentRigidBody::GetDrawCollider() const
{
	return drawCollider;
}

inline ComponentRigidBody::Shape ComponentRigidBody::GetShape() const
{
	return currentShape;
}

inline float ComponentRigidBody::GetMass() const
{
	return mass;
}

inline void ComponentRigidBody::SetMass(float newMass)
{
	// rigidBody->setMassProps(newMass, rigidBody->getLocalInertia());
	mass = newMass;
}

inline float ComponentRigidBody::GetLinearDamping() const
{
	return linearDamping;
}

inline void ComponentRigidBody::SetLinearDamping(float newDamping)
{
	rigidBody->setDamping(newDamping, rigidBody->getAngularDamping());
	linearDamping = newDamping;
}

inline float ComponentRigidBody::GetAngularDamping() const
{
	return angularDamping;
}

inline void ComponentRigidBody::SetAngularDamping(float newDamping)
{
	rigidBody->setDamping(rigidBody->getLinearDamping(), newDamping);
	angularDamping = newDamping;
}

inline btVector3 ComponentRigidBody::GetGravity() const
{
	return gravity;
}

inline void ComponentRigidBody::SetGravity(btVector3 newGravity)
{
	rigidBody->setGravity(newGravity);
	gravity = newGravity;
}

inline btVector3 ComponentRigidBody::GetVelocity() const
{
	return rigidBody->getLinearVelocity();
}

inline void ComponentRigidBody::SetVelocity(const float3& velocity)
{
	if (rigidBody && shape)
	{
		rigidBody->setLinearVelocity({ velocity.x, velocity.y, velocity.z });
	}
}

inline btScalar ComponentRigidBody::GetRestitution() const
{
	return rigidBody->getRestitution();
}

inline void ComponentRigidBody::SetRestitution(float newRestitution)
{
	rigidBody->setRestitution(newRestitution);
	restitution = newRestitution;
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
inline float3 ComponentRigidBody::GetBoxSize() const
{
	return boxSize;
}

inline void ComponentRigidBody::SetBoxSize(float3 newBoxSize)
{
	boxSize = newBoxSize;
}

inline float ComponentRigidBody::GetRadius() const
{
	return radius;
}

inline void ComponentRigidBody::SetRadius(float newRadius)
{
	radius = newRadius;
}

inline float ComponentRigidBody::GetFactor() const
{
	return factor;
}

inline void ComponentRigidBody::SetFactor(float newFactor)
{
	factor = newFactor;
}

inline float ComponentRigidBody::GetHeight() const
{
	return height;
}

inline void ComponentRigidBody::SetHeight(float newHeight)
{
	height = newHeight;
}

inline btRigidBody* ComponentRigidBody::GetRigidBody() const
{ 
    return rigidBody.get(); 
}

inline ComponentTransform* ComponentRigidBody::GetOwnerTransform() const
{
    return transform;
}

inline btVector3 ComponentRigidBody::GetRigidBodyOrigin() const
{
    return rigidBody->getWorldTransform().getOrigin();
}

inline btVector3 ComponentRigidBody::GetRigidBodyTranslation() const
{
	return translation;
}