#include "ComponentRigidBody.h"
#include "ComponentTransform.h"
#include "GameObject/GameObject.h"
#include "ModulePhysics.h"
#include "Application.h"
#include "Geometry/Sphere.h"
#include <ImGui/imgui.h>
#include "debugdraw.h"
#include "Math/Quat.h"
#include "Math/float3x3.h"
#include "Math/float4x4.h"
#include "FileSystem/Json.h"

ComponentRigidBody::ComponentRigidBody(const bool active, GameObject* owner)
    : Component(ComponentType::RIGIDBODY, active, owner, true)
{
    static uint32_t nextId = 1;

    assert(nextId != 0); //if this assert triggers, we have reached the maximum number of rigidbodies 2^32 - 1. This is a very unlikely scenario.

    id = nextId++;

    btTransform startTransform;
    startTransform.setIdentity();
    transform = static_cast<ComponentTransform*>(GetOwner()->GetComponent(ComponentType::TRANSFORM));
    boxSize = transform->GetLocalAABB().HalfSize().Mul(transform->GetScale());
    radius = transform->GetLocalAABB().MinimalEnclosingSphere().Diameter();
    factor = 0.5f;
    //WIP set proper default value
    height = 2.0f;
    
    currentShape = 1;
    motionState = new btDefaultMotionState(startTransform);
    shape = new btBoxShape({ boxSize.x, boxSize.y, boxSize.z });
    rigidBody = new btRigidBody(100, motionState, shape);
    
    App->GetModule<ModulePhysics>()->AddRigidBody(this, rigidBody);
    SetupMobility();

    rigidBody->setUserPointer(this); // Set this component as the rigidbody's user pointer
    rigidBody->setCollisionFlags(btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);

    SetLinearDamping(linearDamping);
    SetAngularDamping(angularDamping);

    SetCollisionShape(static_cast<ComponentRigidBody::SHAPE>(SHAPE::BOX));
    UpdateRigidBody();
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

void ComponentRigidBody::OnTransformChanged() 
{
    if (!App->IsOnPlayMode()) 
    {
        UpdateRigidBody();
    }
}


void ComponentRigidBody::Update()
{
    float deltaTime = App->GetDeltaTime();

    if (!rigidBody->isStaticOrKinematicObject())
    {
        rigidBody->setCcdMotionThreshold(0.1);
        rigidBody->setCcdSweptSphereRadius(0.1f);

        btTransform trans;
        trans = rigidBody->getWorldTransform();
        btQuaternion rot = trans.getRotation();
        Quat currentRot = Quat(rot.x(), rot.y(), rot.z(), rot.w());
        transform->SetGlobalRotation(currentRot);
        btVector3 pos = rigidBody->getCenterOfMassTransform().getOrigin();
        float3 centerPoint = transform->GetLocalAABB().CenterPoint();
        btVector3 offset = trans.getBasis() * btVector3(centerPoint.x, centerPoint.y, centerPoint.z);
        transform->SetGlobalPosition({ pos.x() - offset.x(), pos.y() - offset.y(), pos.z() - offset.z() });
        transform->RecalculateLocalMatrix();
        transform->UpdateTransformMatrices();
    }

    if (usePositionController)
    {
        float3 x = transform->GetPosition();
        float3 positionError = targetPosition - x;
        float3 velocityPosition = positionError * KpForce;

        btVector3 velocity(velocityPosition.x, velocityPosition.y, velocityPosition.z);
        rigidBody->setLinearVelocity(velocity);
    }

    if (useRotationController)
    {
        btTransform trans;
        trans = rigidBody->getWorldTransform();

        btQuaternion bulletQ = trans.getRotation();
        Quat q = Quat(bulletQ.getX(), bulletQ.getY(), bulletQ.getZ(), bulletQ.getW());
        Quat rotationError = targetRotation * q.Normalized().Inverted();
        rotationError.Normalize();

        float3 axis;
        float angle;
        rotationError.ToAxisAngle(axis, angle);
        axis.Normalize();

        float3 angularVelocity = axis * angle * KpTorque;
        btVector3 bulletAngularVelocity(angularVelocity.x, angularVelocity.y, angularVelocity.z);
        rigidBody->setAngularVelocity(bulletAngularVelocity);
    }
}


void ComponentRigidBody::UpdateRigidBody() 
{
    btTransform worldTransform;
    float3 pos = transform->GetGlobalPosition();
    worldTransform.setOrigin({ pos.x, pos.y, pos.z });
    Quat rot = transform->GetGlobalRotation();
    worldTransform.setRotation({ rot.x, rot.y, rot.z, rot.w });
    rigidBody->setWorldTransform(worldTransform);
    motionState->setWorldTransform(worldTransform);
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
        rigidBody->setActivationState(DISABLE_DEACTIVATION);
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
        shape = new btBoxShape({ boxSize.x, boxSize.y, boxSize.z });
        break;
    case 2: // Sphere
        shape = new btSphereShape(radius * factor);
        break;
    case 3: // Capsule
        shape = new btCapsuleShape(radius, height);
        break;
    case 4: // Cone
        shape = new btConeShape(radius, height);
        break;
    /*case 5: // Cylinder
        shape = new btCylinderShape(btVector3(1, 1, 1));
        break;*/
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

void ComponentRigidBody::SaveOptions(Json& meta)
{
	// Do not delete these
    meta["type"] = GetNameByType(type).c_str();
    meta["active"] = (bool)active;
    meta["removed"] = (bool)canBeRemoved;

	meta["isKinematic"] = (bool)GetIsKinematic();
	meta["isStatic"] = (bool)GetIsStatic();
    meta["drawCollider"] = (bool)GetDrawCollider();
	meta["mass"] = (float)GetMass();
	meta["linearDamping"] = (float)GetLinearDamping();
	meta["angularDamping"] = (float)GetAngularDamping();
	meta["restitution"] = (float)GetRestitution();
	meta["currentShape"] = (int)GetShape();
	meta["usePositionController"] = (bool)GetUsePositionController();
	meta["useRotationController"] = (bool)GetUseRotationController();
	meta["KpForce"] = (float)GetKpForce();
	meta["KpTorque"] = (float)GetKpTorque();
    meta["gravity_Y"] = (float)GetGravity().getY();
    meta["boxSize_X"] = (float)GetBoxSize().x;
    meta["boxSize_Y"] = (float)GetBoxSize().y;
    meta["boxSize_Z"] = (float)GetBoxSize().z;
    meta["radius"] = (float)GetRadius();
    meta["factor"] = (float)GetFactor();
    meta["height"] = (float)GetHeight();
}

void ComponentRigidBody::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];

	SetIsKinematic((bool)meta["isKinematic"]);
	SetIsStatic((bool)meta["isStatic"]);
    SetDrawCollider((bool)meta["drawCollider"], false);
	SetMass((float)meta["mass"]);
    SetLinearDamping((float)meta["linearDamping"]);
    SetAngularDamping((float)meta["angularDamping"]);
    SetGravity({ 0, (float)meta["gravity_Y"], 0 });
    SetRestitution((float)meta["restitution"]);
	SetUsePositionController((bool)meta["usePositionController"]);
	SetUseRotationController((bool)meta["useRotationController"]);
	SetKpForce((float)meta["KpForce"]);
	SetKpTorque((float)meta["KpTorque"]);
    SetBoxSize({ (float)meta["boxSize_X"], (float)meta["boxSize_Y"], (float)meta["boxSize_Z"] });
    SetRadius((float)meta["radius"]);
    SetFactor((float)meta["factor"]);
    SetHeight((float)meta["height"]);

    int currentShape = (int)meta["currentShape"];

    if (currentShape != 0)
    {
        SetCollisionShape(static_cast<ComponentRigidBody::SHAPE>(currentShape));
    }
    
    SetupMobility();
}

void ComponentRigidBody::RemoveRigidBodyFromSimulation()
{
    App->GetModule<ModulePhysics>()->RemoveRigidBody(this, rigidBody);
}

void ComponentRigidBody::ClearCollisionEnterDelegate()
{
    delegateCollisionEnter.clear();
}
void ComponentRigidBody::SetDrawCollider(bool newDrawCollider, bool substract)
{
    drawCollider = newDrawCollider;
    int value = 0;

    if (newDrawCollider)
    {
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
        value = 1;
    }
    else
    {
        rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);

        if (substract)
        {
            value = -1;
        }
    }

    App->GetModule<ModulePhysics>()->UpdateDrawableRigidBodies(value);
}

void ComponentRigidBody::SetDefaultSize(int resetShape)
{
    if (resetShape == 1)
    {
        boxSize = transform->GetLocalAABB().HalfSize().Mul(transform->GetScale());
    } else if (resetShape == 2)
    {
        radius = transform->GetLocalAABB().MinimalEnclosingSphere().Diameter();
        factor = 0.5f; 
    } else if (resetShape == 3 || resetShape == 4)
    {
        radius = transform->GetLocalAABB().MinimalEnclosingSphere().Diameter();
        height = 2.0f;
    }
    // WIP: reset 5th shape
}