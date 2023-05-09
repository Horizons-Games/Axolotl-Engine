#pragma once
#include "Component.h"
#include "ComponentTransform.h"
#include "Auxiliar/Generics/Updatable.h"
#include "Bullet/LinearMath/btVector3.h"
#include <functional>
#include <vector>

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

    bool GetIsKinematic() const;
    bool GetIsStatic() const;
    int GetShape() const;

    void SetIsKinematic(bool isKinematic);
    void SetIsStatic(bool isStatic);

    void SetupMobility();

    void SetCollisionShape(SHAPE newShape);

    void SetVelocity(const float3& force);

    void SetMass(float newMass);
    
    void SetGravity(btVector3 newGravity);

    void SetLinearDamping(float newDamping);

    void SetAngularDamping(float newDamping);

    void SetRestitution(float restitution);

    void RemoveRigidBodyFromSimulation();

    btRigidBody* GetRigidBody() const { return rigidBody; }

    template <typename T>
    void AddCollisionEnterDelegate(void (T::* func)(ComponentRigidBody*), T* obj) {
        delegateCollisionEnter.push_back(std::bind(func, obj, std::placeholders::_1));
    }


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

inline bool ComponentRigidBody::GetIsStatic() const
{
    return isStatic;
}

inline int ComponentRigidBody::GetShape() const
{
    return currentShape;
}

inline void ComponentRigidBody::SetIsKinematic(bool newIsKinematic)
{
    isKinematic = newIsKinematic;
}

inline void ComponentRigidBody::SetIsStatic(bool newIsStatic)
{
    isStatic = newIsStatic;
}
