#include "ComponentRigidBody.h"
#include "ComponentTransform.h"
#include "GameObject/GameObject.h"
#include "ModulePhysics.h"
#include "Application.h"
#include "Bullet/btBulletDynamicsCommon.h"
#include "Geometry/Sphere.h"
#include <ImGui/imgui.h>
#include "debugdraw.h"
#include "Math/Quat.h"

ComponentRigidBody::ComponentRigidBody(const bool active, GameObject* owner)
    : Component(ComponentType::RIGIDBODY, active, owner, false)
{
    static uint32_t nextId = 1;

    assert(nextId != 0); //if this assert triggers, we have reached the maximum number of rigidbodies 2^32 - 1. This is a very unlikely scenario.

    id = nextId++;

    btTransform startTransform;
    startTransform.setIdentity();
    motionState = new btDefaultMotionState(startTransform);
    shape = new btBoxShape({ 0, 0, 0 });
    rigidBody = new btRigidBody(100, motionState, shape);
    App->GetModule<ModulePhysics>()->AddRigidBody(this, rigidBody);
    SetupMobility();

    rigidBody->setUserPointer(this); // Set this component as the rigidbody's user pointer

    SetLinearDamping(linearDamping);
    SetAngularDamping(angularDamping);

    transform = static_cast<ComponentTransform*>(GetOwner()->GetComponent(ComponentType::TRANSFORM));

}

ComponentRigidBody::~ComponentRigidBody()
{
    App->GetModule<ModulePhysics>()->RemoveRigidBody(this, rigidBody);
}


void ComponentRigidBody::OnCollisionEnter(ComponentRigidBody* other)
{
    assert(other);
    //delegate to notify other components
    for (auto& delegate : delegateCollisionEnter)
        delegate(other);

}

void ComponentRigidBody::OnCollisionStay(ComponentRigidBody* other)
{
    //TODO: Implement delegate for this
    assert(other);
}

void ComponentRigidBody::OnCollisionExit(ComponentRigidBody* other)
{
    //TODO: Implement delegate for this
    assert(other);
}


void ComponentRigidBody::Update()
{
    if (rigidBody && !rigidBody->isStaticOrKinematicObject())
    {
        //used to detect collisions between fixed time steps
        rigidBody->setCcdMotionThreshold(0.1);
        rigidBody->setCcdSweptSphereRadius(0.1f);

        btTransform trans;
        trans = rigidBody->getWorldTransform();
        btQuaternion rot = trans.getRotation();
        Quat currentRot = Quat(rot.x(), rot.y(), rot.z(), rot.w());
        transform->SetRotation(currentRot.ToEulerXYZ());
        btVector3 pos = rigidBody->getCenterOfMassTransform().getOrigin();
        float3 centerPoint = transform->GetLocalAABB().CenterPoint();
        btVector3 offset = trans.getBasis() * btVector3(centerPoint.x, centerPoint.y, centerPoint.z);
        transform->SetPosition({ pos.x() - offset.x(), pos.y() - offset.y(), pos.z() - offset.z() });
        transform->UpdateTransformMatrices();
    }
}



void ComponentRigidBody::SetupMobility()
{
    App->GetModule<ModulePhysics>()->RemoveRigidBody(this, rigidBody);
    if (isKinematic)
    {
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_DYNAMIC_OBJECT);
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
        rigidBody->setActivationState(DISABLE_DEACTIVATION);
        rigidBody->setMassProps(0, { 0, 0, 0 }); //Toreview: is this necessary here?
        isStatic = false;
    }
    else if (isStatic)
    {
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_DYNAMIC_OBJECT);
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
        rigidBody->setActivationState(DISABLE_DEACTIVATION);
        rigidBody->setMassProps(0, { 0, 0, 0 }); //static objects have no mass to avoid collision pushes
        isKinematic = false;
    }
    else
    {
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_DYNAMIC_OBJECT);
        rigidBody->setActivationState(ACTIVE_TAG);
        btVector3 localInertia;
        rigidBody->getCollisionShape()->calculateLocalInertia(mass, localInertia);
        rigidBody->setMassProps(mass, localInertia);
    }
    App->GetModule<ModulePhysics>()->AddRigidBody(this, rigidBody);
}

void ComponentRigidBody::SetCollisionShape(SHAPE newShape)
{
    delete shape;

    switch (static_cast<int>(newShape))
    {
    case 1: // Box
    {
        float3 aabbHalfSize = transform->GetLocalAABB().HalfSize().Mul(transform->GetScale());
        shape = new btBoxShape({ aabbHalfSize.x, aabbHalfSize.y, aabbHalfSize.z });
        break;
    }
    case 2: // Sphere
        shape = new btSphereShape(transform->GetLocalAABB().MinimalEnclosingSphere().Diameter() * .5f);
        break;
        /*
        case 3: // Capsule
            shape = new btCapsuleShape(1, 2);
            break;
        case 4: // Cylinder
            shape = new btCylinderShape(btVector3(1, 1, 1));
            break;
        case 54: // Cone
            shape = new btConeShape(1, 2);
            break;
            */
    }

    if (shape)
    {
        currentShape = static_cast<int>(newShape);
        rigidBody->setCollisionShape(shape);
        //inertia for local rotation
        btVector3 localInertia;
        rigidBody->getCollisionShape()->calculateLocalInertia(mass, localInertia);
        rigidBody->setMassProps(mass, localInertia);
        rigidBody->updateInertiaTensor();
    }
}

void ComponentRigidBody::SetVelocity(const float3& velocity)
{
    if (rigidBody && shape)
    {
        rigidBody->setLinearVelocity({ velocity.x, velocity.y, velocity.z });
    }
}

void ComponentRigidBody::SetMass(float newMass)
{
    //rigidBody->setMassProps(newMass, rigidBody->getLocalInertia());
    mass = newMass;
}

void ComponentRigidBody::SetGravity(btVector3 newGravity)
{
    rigidBody->setGravity(newGravity);
    gravity = newGravity;
}

void ComponentRigidBody::SetRestitution(float newRestitution) 
{
    rigidBody->setRestitution(newRestitution);
    restitution = newRestitution;
}

void ComponentRigidBody::SetLinearDamping(float newDamping)
{
    rigidBody->setDamping(newDamping, rigidBody->getAngularDamping());
    linearDamping = newDamping;
}

void ComponentRigidBody::SetAngularDamping(float newDamping)
{
    rigidBody->setDamping(rigidBody->getLinearDamping(), newDamping);
    angularDamping = newDamping;
}

void ComponentRigidBody::RemoveRigidBodyFromSimulation()
{
    App->GetModule<ModulePhysics>()->RemoveRigidBody(this, rigidBody);
}



void ComponentRigidBody::SaveOptions(Json& meta)
{
	//// Do not delete these
	//meta["type"] = GetNameByType(type).c_str();
	//meta["active"] = (bool)active;
	//meta["removed"] = (bool)canBeRemoved;

	//meta["isKinematic"] = (bool)GetIsKinematic();
	//meta["mass"] = (float)GetMass();
	//meta["usePositionController"] = (bool)GetUsePositionController();
	//meta["useRotationController"] = (bool)GetUseRotationController();
	//meta["KpForce"] = (float)GetKpForce();
	//meta["KpTorque"] = (float)GetKpTorque();
	//meta["gravity.Y"] = (float)GetGravity().y;
}

void ComponentRigidBody::LoadOptions(Json& meta)
{
	//// Do not delete these
	//type = GetTypeByName(meta["type"]);
	//active = (bool)meta["active"];
	//canBeRemoved = (bool)meta["removed"];

	//SetIsKinematic((bool)meta["isKinematic"]);
	//SetMass((float)meta["mass"]);
	//SetUsePositionController((bool)meta["usePositionController"]);
	//SetUseRotationController((bool)meta["useRotationController"]);
	//SetKpForce((float)meta["KpForce"]);
	//SetKpTorque((float)meta["KpTorque"]);
	//g.y = (float)meta["gravity.Y"];
}
