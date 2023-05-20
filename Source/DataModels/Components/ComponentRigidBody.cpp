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

ComponentRigidBody::ComponentRigidBody(bool active, GameObject* owner)
    : Component(ComponentType::RIGIDBODY, active, owner, true)
{
    id = GenerateId();

    btTransform startTransform;
    startTransform.setIdentity();
    transform = static_cast<ComponentTransform*>(GetOwner()->GetComponent(ComponentType::TRANSFORM));
    boxSize = transform->GetLocalAABB().HalfSize().Mul(transform->GetScale());
    radius = transform->GetLocalAABB().MinimalEnclosingSphere().Diameter();
    factor = 0.5f;
    //WIP set proper default value
    height = 2.0f;
    
    currentShape = Shape::BOX;
    motionState = std::make_unique<btDefaultMotionState>(startTransform);
    shape = std::make_unique<btBoxShape>(btVector3{ boxSize.x, boxSize.y, boxSize.z });
    rigidBody = std::make_unique<btRigidBody>(100, motionState.get(), shape.get());
    
    App->GetModule<ModulePhysics>()->AddRigidBody(this, rigidBody.get());
    SetUpMobility();

    rigidBody->setUserPointer(this); // Set this component as the rigidbody's user pointer
    rigidBody->setCollisionFlags(btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);

    SetLinearDamping(linearDamping);
    SetAngularDamping(angularDamping);

    SetCollisionShape(static_cast<ComponentRigidBody::Shape>(Shape::BOX));
}

ComponentRigidBody::ComponentRigidBody(const ComponentRigidBody& toCopy)
    : Component(ComponentType::RIGIDBODY, toCopy.active, toCopy.owner, true)
{
    id = GenerateId();

    transform = toCopy.transform;
    boxSize = toCopy.boxSize;
    radius = toCopy.radius;
    factor = toCopy.factor;
    height = toCopy.height;

    currentShape = toCopy.currentShape;
    motionState = std::unique_ptr<btDefaultMotionState>(new btDefaultMotionState(*toCopy.motionState.get()));
    switch (static_cast<Shape>(currentShape))
    {
    default:
    case Shape::BOX:
        shape = std::unique_ptr<btBoxShape>(new btBoxShape(*static_cast<btBoxShape*>(toCopy.shape.get())));
        break;
    case Shape::SPHERE:
        shape = std::unique_ptr<btSphereShape>(new btSphereShape(*static_cast<btSphereShape*>(toCopy.shape.get())));
        break;
    case Shape::CAPSULE:
        shape = std::unique_ptr<btCapsuleShape>(new btCapsuleShape(*static_cast<btCapsuleShape*>(toCopy.shape.get())));
        break;
    case Shape::CONE:
        shape = std::unique_ptr<btConeShape>(new btConeShape(*static_cast<btConeShape*>(toCopy.shape.get())));
        break;
    }
    rigidBody = std::make_unique<btRigidBody>(toCopy.rigidBody->getMass(), motionState.get(), shape.get());

    App->GetModule<ModulePhysics>()->AddRigidBody(this, rigidBody.get());
    SetUpMobility();

    rigidBody->setUserPointer(this); // Set this component as the rigidbody's user pointer
    rigidBody->setCollisionFlags(btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);

    SetLinearDamping(linearDamping);
    SetAngularDamping(angularDamping);

    SetCollisionShape(Shape::BOX);
}

ComponentRigidBody::~ComponentRigidBody()
{
    App->GetModule<ModulePhysics>()->RemoveRigidBody(this, rigidBody.get());
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
#ifdef ENGINE
    if (!App->IsOnPlayMode())
    {
        UpdateRigidBody();
    }
#endif
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

void ComponentRigidBody::SetOwner(GameObject* owner)
{
    Component::SetOwner(owner);
    transform = static_cast<ComponentTransform*>(GetOwner()->GetComponent(ComponentType::TRANSFORM));
    boxSize = transform->GetLocalAABB().HalfSize().Mul(transform->GetScale());
    radius = transform->GetLocalAABB().MinimalEnclosingSphere().Diameter();
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
int ComponentRigidBody::GenerateId() const
{
    static uint32_t nextId = 1;

    assert(nextId != 0); //if this assert triggers, we have reached the maximum number of rigidbodies 2^32 - 1. This is a very unlikely scenario.

    return nextId++;
}
void ComponentRigidBody::SetUpMobility()
{
    App->GetModule<ModulePhysics>()->RemoveRigidBody(this, rigidBody.get());
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
    App->GetModule<ModulePhysics>()->AddRigidBody(this, rigidBody.get());
}

void ComponentRigidBody::SetCollisionShape(Shape newShape)
{
    switch (newShape)
    {
    case Shape::BOX: // Box
        shape = std::make_unique<btBoxShape>(btVector3{ boxSize.x, boxSize.y, boxSize.z });
        break;
    case Shape::SPHERE: // Sphere
        shape = std::make_unique<btSphereShape>(radius * factor);
        break;
    case Shape::CAPSULE: // Capsule
        shape = std::make_unique<btCapsuleShape>(radius, height);
        break;
    case Shape::CONE: // Cone
        shape = std::make_unique<btConeShape>(radius, height);
        break;
        
    /*case SHAPE::CYLINDER: // Cylinder
        shape = new btCylinderShape(btVector3(1, 1, 1));
        break;*/
    }

    if (shape)
    {
        currentShape = newShape;
        rigidBody->setCollisionShape(shape.get());
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
	meta["isStatic"] = (bool)IsStatic();
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
        SetCollisionShape(static_cast<ComponentRigidBody::Shape>(currentShape));
    }
    
    SetUpMobility();
    SetGravity({ 0, (float)meta["gravity_Y"], 0 });
}

void ComponentRigidBody::Enable()
{
    Component::Enable();
    App->GetModule<ModulePhysics>()->AddRigidBody(this, rigidBody.get());
}

void ComponentRigidBody::Disable()
{
    Component::Disable();
    App->GetModule<ModulePhysics>()->RemoveRigidBody(this, rigidBody.get());
}

void ComponentRigidBody::RemoveRigidBodyFromSimulation()
{
    App->GetModule<ModulePhysics>()->RemoveRigidBody(this, rigidBody.get());
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

void ComponentRigidBody::SetDefaultSize(Shape resetShape)
{
    switch (resetShape)
    {
    case Shape::BOX:
        boxSize = transform->GetLocalAABB().HalfSize().Mul(transform->GetScale());
        break;
    case Shape::SPHERE:
        break;
    case Shape::CONE:
        radius = transform->GetLocalAABB().MinimalEnclosingSphere().Diameter();
        factor = 0.5f;
    case Shape::CAPSULE:
        radius = transform->GetLocalAABB().MinimalEnclosingSphere().Diameter();
        height = 2.0f;
        break;
    }
    // WIP: reset 5th shape
}