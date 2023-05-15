#pragma once
#include "Component.h"
#include "ComponentTransform.h"
#include "Auxiliar/Generics/Updatable.h"
#include "Bullet/LinearMath/btVector3.h"
#include <functional>
#include <vector>
#include "Bullet/btBulletDynamicsCommon.h"

class btRigidBody;
struct btDefaultMotionState;
class btCollisionShape;

class ComponentRigidBody : public Component, public Updatable
{
public:

    enum class SHAPE
    {
        NONE = 0,
        BOX = 1,
        SPHERE = 2,
        CAPSULE = 3,
        CYLINDER = 4,
        CONE = 5,
        CONVEX_HULL = 6,
        TRIANGLE_MESH = 7,
        TERRAIN = 8
    };

    ComponentRigidBody(const bool active, GameObject* owner);
    ~ComponentRigidBody();


    void OnCollisionEnter(ComponentRigidBody* other);

    void OnCollisionStay(ComponentRigidBody* other);

    void OnCollisionExit(ComponentRigidBody* other);

    void Update() override;

    uint32_t GetID() const { return id; }

    void SaveOptions(Json& meta) override;
    void LoadOptions(Json& meta) override;

    void SetIsKinematic(bool isKinematic);
    bool GetIsKinematic() const;

    void SetIsStatic(bool isStatic);
    bool GetIsStatic() const;
    
    float GetMass() const;
    void SetMass(float newMass);

    btVector3 GetGravity() const;
    void SetGravity(btVector3 newGravity);

    float GetLinearDamping() const;
    void SetLinearDamping(float newDamping);

    float GetAngularDamping() const;
    void SetAngularDamping(float newDamping);

    int GetShape() const;
    void SetCollisionShape(SHAPE newShape);

    btVector3 GetVelocity() const;
    void SetVelocity(const float3& force);

    btScalar GetRestitution() const;
    void SetRestitution(float restitution);

    void SetupMobility();

    void RemoveRigidBodyFromSimulation();
    void UpdateNonSimulatedTransform();

    btRigidBody* GetRigidBody() const { return rigidBody; }

    void UpdateRigidBody();

    template <typename T>
    void AddCollisionEnterDelegate(void (T::* func)(ComponentRigidBody*), T* obj) {
        delegateCollisionEnter.push_back(std::bind(func, obj, std::placeholders::_1));
    }

    void ClearCollisionEnterDelegate();

private:

    btRigidBody* rigidBody = nullptr;
    btDefaultMotionState* motionState = nullptr;
    btCollisionShape* shape = nullptr;

    btVector3 gravity = { 0, -9.81f, 0 };
    float linearDamping = 0.1f;
    float angularDamping = 0.1f;
    float mass = 100.0f;
    float restitution = 0.f;

    bool isKinematic = false;
    bool isStatic = false;

    int currentShape = 0;

    ComponentTransform* transform;

    uint32_t id = 0;

    //Delegate for collision enter event the parameter is the other collider
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

inline bool ComponentRigidBody::GetIsStatic() const
{
    return isStatic;
}

inline void ComponentRigidBody::SetIsStatic(bool newIsStatic)
{
    isStatic = newIsStatic;
}

inline int ComponentRigidBody::GetShape() const
{
    return currentShape;
}

inline float ComponentRigidBody::GetMass() const
{
    return mass;
}

inline void ComponentRigidBody::SetMass(float newMass)
{
    //rigidBody->setMassProps(newMass, rigidBody->getLocalInertia());
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

