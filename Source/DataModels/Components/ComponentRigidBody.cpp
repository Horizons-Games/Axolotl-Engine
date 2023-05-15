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
    float3 aabbHalfSize = transform->GetLocalAABB().HalfSize().Mul(transform->GetScale());
    
    currentShape = 1;
    motionState = new btDefaultMotionState(startTransform);
    shape = new btBoxShape({ aabbHalfSize.x, aabbHalfSize.y, aabbHalfSize.z });
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


void ComponentRigidBody::Update()
{
    if (!rigidBody->isStaticOrKinematicObject())
    {
        rigidBody->setCcdMotionThreshold(0.1);
        rigidBody->setCcdSweptSphereRadius(0.1f);

        btTransform trans;
        trans = rigidBody->getWorldTransform();
        btQuaternion rot = trans.getRotation();
        Quat currentRot = Quat(rot.x(), rot.y(), rot.z(), rot.w());
        float4x4 rotationMatrix = float4x4::FromQuat(currentRot);
        transform->SetRotation(rotationMatrix);
        btVector3 pos = rigidBody->getCenterOfMassTransform().getOrigin();
        float3 centerPoint = transform->GetLocalAABB().CenterPoint();
        btVector3 offset = trans.getBasis() * btVector3(centerPoint.x, centerPoint.y, centerPoint.z);
        transform->SetPosition({ pos.x() - offset.x(), pos.y() - offset.y(), pos.z() - offset.z() });
        transform->UpdateTransformMatrices();
    }
}


void ComponentRigidBody::UpdateRigidBody() 
{
    btTransform worldTransform;
    float3 pos = transform->GetPosition();
    worldTransform.setOrigin({ pos.x, pos.y, pos.z });
    Quat rot = transform->GetRotation().RotatePart().ToQuat();
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
	/*meta["usePositionController"] = (bool)GetUsePositionController();
	meta["useRotationController"] = (bool)GetUseRotationController();
	meta["KpForce"] = (float)GetKpForce();
	meta["KpTorque"] = (float)GetKpTorque();*/
    meta["gravity_Y"] = (float)GetGravity().getY();
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
	/*SetUsePositionController((bool)meta["usePositionController"]);
	SetUseRotationController((bool)meta["useRotationController"]);
	SetKpForce((float)meta["KpForce"]);
	SetKpTorque((float)meta["KpTorque"]);*/

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