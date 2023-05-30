#include "ComponentRigidBody.h"
#include "Application.h"
#include "ComponentTransform.h"
#include "FileSystem/Json.h"
#include "GameObject/GameObject.h"
#include "Geometry/Sphere.h"
#include "Math/Quat.h"
#include "Math/float3x3.h"
#include "Math/float4x4.h"
#include "ModulePhysics.h"
#include "debugdraw.h"
#include <ImGui/imgui.h>

#include "ComponentScript.h"

ComponentRigidBody::ComponentRigidBody(bool active, GameObject* owner) :
	Component(ComponentType::RIGIDBODY, active, owner, true)
{
	id = GenerateId();

	btTransform startTransform;
	startTransform.setIdentity();
	transform = static_cast<ComponentTransform*>(GetOwner()->GetComponent(ComponentType::TRANSFORM));
	boxSize = transform->GetLocalAABB().HalfSize().Mul(transform->GetScale());
	radius = transform->GetLocalAABB().MinimalEnclosingSphere().Diameter();
	factor = 0.5f;
	// WIP set proper default value
	height = 2.0f;

	currentShape = Shape::BOX;
	motionState = std::make_unique<btDefaultMotionState>(startTransform);
	shape = std::make_unique<btBoxShape>(btVector3{ boxSize.x, boxSize.y, boxSize.z });
	rigidBody = std::make_unique<btRigidBody>(100, motionState.get(), shape.get());

	App->GetModule<ModulePhysics>()->AddRigidBody(this, rigidBody.get());
	SetUpMobility();

	rigidBody->setUserPointer(this); // Set this component as the rigidbody's user pointer
#ifdef ENGINE
	rigidBody->setCollisionFlags(btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
#endif // ENGINE

	SetLinearDamping(linearDamping);
	SetAngularDamping(angularDamping);

	SetCollisionShape(static_cast<ComponentRigidBody::Shape>(Shape::BOX));
}

ComponentRigidBody::ComponentRigidBody(const ComponentRigidBody& toCopy) :
	Component(ComponentType::RIGIDBODY, toCopy.active, toCopy.owner, true),
	isKinematic(toCopy.isKinematic),
	isStatic(toCopy.isStatic),
	isTrigger(toCopy.isTrigger),
	currentShape(toCopy.currentShape),
	boxSize(toCopy.boxSize),
	radius(toCopy.radius),
	factor(toCopy.factor),
	height(toCopy.height),
	usePositionController(toCopy.usePositionController),
	useRotationController(toCopy.useRotationController),
	KpForce(toCopy.KpForce),
	KpTorque(toCopy.KpTorque),
	mass(toCopy.mass)
{
	id = GenerateId();

	transform = toCopy.transform;

	motionState = std::unique_ptr<btDefaultMotionState>(new btDefaultMotionState(*toCopy.motionState.get()));

	rigidBody = std::make_unique<btRigidBody>(toCopy.mass, motionState.get(), toCopy.shape.get());

	App->GetModule<ModulePhysics>()->AddRigidBody(this, rigidBody.get());
	SetUpMobility();

	rigidBody->setUserPointer(this); // Set this component as the rigidbody's user pointer
#ifdef ENGINE
	rigidBody->setCollisionFlags(btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
#endif

	SetLinearDamping(toCopy.linearDamping);
	SetAngularDamping(toCopy.angularDamping);
	SetRestitution(toCopy.restitution);

	SetCollisionShape(currentShape);

	SetGravity(toCopy.gravity);
}

ComponentRigidBody::~ComponentRigidBody()
{
	App->GetModule<ModulePhysics>()->RemoveRigidBody(this, rigidBody.get());
}

void ComponentRigidBody::OnCollisionEnter(ComponentRigidBody* other)
{
	assert(other);

	for (ComponentScript* script : owner->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT))
	{
		script->OnCollisionEnter(other);
	}

}

void ComponentRigidBody::OnCollisionStay(ComponentRigidBody* other)
{
	// TODO: Implement delegate for this
	assert(other);
}

void ComponentRigidBody::OnCollisionExit(ComponentRigidBody* other)
{
	assert(other);

	for (ComponentScript* script : owner->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT))
	{
		script->OnCollisionExit(other);
	}
}

void ComponentRigidBody::OnTransformChanged()
{
#ifdef ENGINE
    if (!App->IsOnPlayMode())
    {
		/*if (isFromSceneLoad) 
		{
			UpdateRigidBodyTranslation();
			isFromSceneLoad = false;
		}*/
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
		float3 newPos = { pos.x() - offset.x(), pos.y() - offset.y(), pos.z() - offset.z() };
		newPos -= float3(translation.x(),translation.y(),translation.z());
		transform->SetGlobalPosition(newPos);
		transform->RecalculateLocalMatrix();
		transform->UpdateTransformMatrices();
	}

	if (usePositionController)
	{
		float3 x = transform->GetGlobalPosition();
		float3 positionError = targetPosition - x;
		float3 velocityPosition = positionError * KpForce;

		btVector3 velocity(velocityPosition.x, velocityPosition.y, velocityPosition.z);
		rigidBody->setLinearVelocity(velocity);
	}

	if (useRotationController)
	{
		
		/*Quat q = transform->GetGlobalRotation();
		Quat rotationError = targetRotation.Mul(q.Normalized().Inverted());
		rotationError.Normalize();*/

		float3 axis;
		float angle;
		targetRotation.ToAxisAngle(axis, angle);
		axis.Normalize();

		float3 angularVelocity = axis * angle * KpTorque;
		btVector3 bulletAngularVelocity(0.0f, angularVelocity.y, 0.0f);
		rigidBody->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));
		rigidBody->setAngularVelocity(bulletAngularVelocity);
	}
}

void ComponentRigidBody::SetOwner(GameObject* owner)
{
	Component::SetOwner(owner);
	transform = static_cast<ComponentTransform*>(GetOwner()->GetComponent(ComponentType::TRANSFORM));
}

void ComponentRigidBody::UpdateRigidBody()
{
	btTransform worldTransform = rigidBody->getWorldTransform();
	float3 transPos = transform->GetGlobalPosition();
	btVector3 transPosBt = btVector3(transPos.x, transPos.y, transPos.z);
	worldTransform.setOrigin(transPosBt + translation);
	Quat rot = transform->GetGlobalRotation();
	worldTransform.setRotation({ rot.x, rot.y, rot.z, rot.w });
	rigidBody->setWorldTransform(worldTransform);
	motionState->setWorldTransform(worldTransform);
}
int ComponentRigidBody::GenerateId() const
{
	static uint32_t nextId = 1;

	assert(nextId != 0); // if this assert triggers, we have reached the maximum number of rigidbodies 2^32 - 1. This is
						 // a very unlikely scenario.

	return nextId++;
}

void ComponentRigidBody::SetRigidBodyOrigin(btVector3 origin) 
{
    btTransform worldTransform = rigidBody->getWorldTransform();
    worldTransform.setOrigin(origin);
    rigidBody->setWorldTransform(worldTransform);
}


void ComponentRigidBody::UpdateRigidBodyTranslation()
{
	float3 transPos = transform->GetGlobalPosition();
	btVector3 transPosBt = btVector3(transPos.x, transPos.y, transPos.z);

	translation = (rigidBody->getWorldTransform().getOrigin() - transPosBt);
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
		rigidBody->setMassProps(0, { 0, 0, 0 }); // Toreview: is this necessary here?
		isStatic = false;
	}
	else if (isStatic)
	{
		rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
		rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_DYNAMIC_OBJECT);
		rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		rigidBody->setActivationState(DISABLE_DEACTIVATION);
		rigidBody->setMassProps(0, { 0, 0, 0 }); // static objects have no mass to avoid collision pushes
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
		// inertia for local rotation
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
	meta["active"] = static_cast<bool>(active);
	meta["removed"] = static_cast<bool>(canBeRemoved);
	meta["isKinematic"] = static_cast<bool>(GetIsKinematic());
	meta["isStatic"] = static_cast<bool>(IsStatic());
	meta["isTrigger"] = static_cast<bool>(IsTrigger());
	meta["drawCollider"] = static_cast<bool>(GetDrawCollider());
	meta["mass"] = static_cast<float>(GetMass());
	meta["linearDamping"] = static_cast<float>(GetLinearDamping());
	meta["angularDamping"] = static_cast<float>(GetAngularDamping());
	meta["restitution"] = static_cast<float>(GetRestitution());
	meta["currentShape"] = static_cast<int>(GetShape());
	meta["usePositionController"] = static_cast<bool>(GetUsePositionController());
	meta["useRotationController"] = static_cast<bool>(GetUseRotationController());
	meta["KpForce"] = static_cast<float>(GetKpForce());
	meta["KpTorque"] = static_cast<float>(GetKpTorque());
	meta["gravity_Y"] = static_cast<float>(GetGravity().getY());
	meta["boxSize_X"] = static_cast<float>(GetBoxSize().x);
	meta["boxSize_Y"] = static_cast<float>(GetBoxSize().y);
	meta["boxSize_Z"] = static_cast<float>(GetBoxSize().z);
	meta["radius"] = static_cast<float>(GetRadius());
	meta["factor"] = static_cast<float>(GetFactor());
	meta["height"] = static_cast<float>(GetHeight());
	/*meta["rbPos_X"] = static_cast<float>(GetRigidBodyOrigin().getX());
    meta["rbPos_Y"] = static_cast<float>(GetRigidBodyOrigin().getY());
    meta["rbPos_Z"] = static_cast<float>(GetRigidBodyOrigin().getZ());*/
}

void ComponentRigidBody::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = static_cast<bool>(meta["active"]);
	canBeRemoved = static_cast<bool>(meta["removed"]);
	SetIsKinematic(static_cast<bool>(meta["isKinematic"]));
	SetIsStatic(static_cast<bool>(meta["isStatic"]));
#ifdef ENGINE
	SetDrawCollider(static_cast<bool>(meta["drawCollider"]), false);
#endif
	SetIsTrigger(static_cast<bool>(meta["isTrigger"]));
	SetMass(static_cast<float>(meta["mass"]));
	SetLinearDamping(static_cast<float>(meta["linearDamping"]));
	SetAngularDamping(static_cast<float>(meta["angularDamping"]));

	SetRestitution(static_cast<float>(meta["restitution"]));
	SetUsePositionController(static_cast<bool>(meta["usePositionController"]));
	SetUseRotationController(static_cast<bool>(meta["useRotationController"]));
	SetKpForce(static_cast<float>(meta["KpForce"]));
	SetKpTorque(static_cast<float>(meta["KpTorque"]));
	SetBoxSize({ static_cast<float>(meta["boxSize_X"]), static_cast<float>(meta["boxSize_Y"]), static_cast<float>(meta["boxSize_Z"]) });
	SetRadius(static_cast<float>(meta["radius"]));
	SetFactor(static_cast<float>(meta["factor"]));
	SetHeight(static_cast<float>(meta["height"]));
	//SetRigidBodyOrigin({ static_cast<float>(meta["rbPos_X"]), static_cast<float>(meta["rbPos_Y"]), static_cast<float>(meta["rbPos_Z"]) });
	
	int currentShape = static_cast<int>(meta["currentShape"]);

	if (currentShape != 0)
	{
		SetCollisionShape(static_cast<ComponentRigidBody::Shape>(currentShape));
	}

	SetUpMobility();
	SetGravity({ 0, static_cast<float>(meta["gravity_Y"]), 0 });
}

void ComponentRigidBody::Enable()
{
	Component::Enable();
	App->GetModule<ModulePhysics>()->AddRigidBody(this, rigidBody.get());
	rigidBody->setGravity(gravity);
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

void ComponentRigidBody::SetDefaultSize(Shape resetShape)
{
	switch (resetShape)
	{
		case Shape::BOX:
			boxSize = transform->GetLocalAABB().HalfSize().Mul(transform->GetScale());
			break;
		case Shape::SPHERE:
			radius = transform->GetLocalAABB().MinimalEnclosingSphere().Diameter();
			factor = 0.5f;
			break;
		case Shape::CAPSULE:
			radius = transform->GetLocalAABB().MinimalEnclosingSphere().Diameter();
			height = 2.0f;
			break;
		case Shape::CONE:
			radius = transform->GetLocalAABB().MinimalEnclosingSphere().Diameter();
			height = 2.0f;
			break;
	}

	SetCollisionShape(resetShape);
	// WIP: reset 5th shape
}

void ComponentRigidBody::SetIsTrigger(bool newTrigger)
{
	isTrigger = newTrigger;
	if (newTrigger)
	{
		rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CO_GHOST_OBJECT);
	}
	else
	{
		rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CO_GHOST_OBJECT);
	}
}

void ComponentRigidBody::SetDefaultPosition()
{
	float3 transPos = transform->GetGlobalPosition();
	btVector3 transPosBt = btVector3(transPos.x, transPos.y, transPos.z);
	SetRigidBodyOrigin(transPosBt);
	UpdateRigidBodyTranslation();
	UpdateRigidBody();
}
