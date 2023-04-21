#include "ComponentRigidBody.h"
#include "ComponentTransform.h"
#include "ComponentMockState.h"
#include "ComponentTransform.h"

#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "DataStructures/Quadtree.h"
#include "Geometry/Frustum.h"
#include "Math/float3x3.h"

#include "GameObject/GameObject.h"
#include "Application.h"

#include "FileSystem/Json.h"

#include "Geometry/LineSegment.h"
#include "Geometry/Ray.h"
#include "Physics/Physics.h"

#include <iostream>


ComponentRigidBody::ComponentRigidBody(bool active, GameObject* owner)
	: Component(ComponentType::RIGIDBODY, active, owner, true)
{
	transform = static_cast<ComponentTransform*>(GetOwner()->GetComponent(ComponentType::TRANSFORM));
	isKinematic = true;
	isOnSurface = false;
	mass = 1.0f;

	height = -math::inf;
	x = transform->GetPosition();
	q = transform->GetRotation().RotatePart().ToQuat();
	g = float3(0.0f, -2.81f, 0.0f);
	v0 = float3(0.0f, 0.0f, 0.0f);
	w0 = float3(0.0f, 0.0f, 0.0f);
}

ComponentRigidBody::~ComponentRigidBody()
{
}

void ComponentRigidBody::Update()
{

#ifndef ENGINE
	if (isKinematic)
	{
		float deltaTime = App->GetDeltaTime();

		x = transform->GetPosition();
		q = transform->GetRotation().RotatePart().ToQuat();

		float verticalDistanceToFeet = math::Abs(transform->GetEncapsuledAABB().MinY() - x.y);

		// Combine gravity and external forces
		float3 totalAcceleration = g + externalForce;

		//Velocity
		v0 += totalAcceleration * deltaTime;
		x += v0 * deltaTime + 0.5f * totalAcceleration * deltaTime * deltaTime;
	
		externalForce = float3::zero;

		//Apply gravity
		if (x.y <= height + verticalDistanceToFeet)
		{
			x.y = height + verticalDistanceToFeet;
			v0 = float3::zero;
		}

		////Rotation
		//Quat angularVelocityQuat(w0.x, w0.y, w0.z, 0.0f);
		//Quat wq_0 = angularVelocityQuat * q;


		//float deltaValue = 0.5f * deltaTime;
		//Quat deltaRotation = Quat(deltaValue * wq_0.x, deltaValue * wq_0.y, deltaValue * wq_0.z, deltaValue * wq_0.w);

		//Quat nextRotation(q.x + deltaRotation.x,
		//	q.y + deltaRotation.y,
		//	q.z + deltaRotation.z,
		//	q.w + deltaRotation.w);
		//nextRotation.Normalize();

		//q = nextRotation;

		//Apply proportional controllers
		ApplyForce();
		//ApplyTorque();

		//Update Transform
		transform->SetPosition(x);
		/*float4x4 rotationMatrix = float4x4::FromQuat(q);
		transform->SetRotation(rotationMatrix);*/
	}

	
#endif
}

void ComponentRigidBody::Draw()
{
	
}

void ComponentRigidBody::AddForce(const float3& force, ForceMode mode)
{
	switch (mode)
	{
	case ForceMode::Force:
		externalForce += force / mass;
		break;
	case ForceMode::Acceleration:
		externalForce += force;
		break;
	case ForceMode::Impulse:
		//TO DO
		break;
	case ForceMode::VelocityChange:
		v0 += force;
		break;
	}
}

void ComponentRigidBody::AddTorque(const float3& torque, ForceMode mode)
{
	switch (mode)
	{
	case ForceMode::Force:
		externalTorque += torque / mass;
		break;
	case ForceMode::Acceleration:
		externalTorque += torque;
		break;
	case ForceMode::Impulse:
		//TO DO
		break;
	case ForceMode::VelocityChange:
		w0 += torque;
		break;
	}
}


void ComponentRigidBody::ApplyForce()
{
	if (usePositionController)
	{
		float deltaTime = App->GetDeltaTime();
		float3 position = transform->GetPosition();

		float3 positionError = targetPosition - position;
		float3 velocityPosition = positionError * KpForce;
		float3 nextPos = position + velocityPosition * deltaTime;

		transform->SetPosition(nextPos);
	}
}


void ComponentRigidBody::ApplyTorque()
{
	float deltaTime = App->GetDeltaTime();
	if (useRotationController)
	{

		Quat rotationError = targetRotation * q.Normalized().Inverted();
		rotationError.Normalize();

		float3 axis;
		float angle;
		rotationError.ToAxisAngle(axis, angle);
		axis.Normalize();

		float3 velocityRotation = axis * angle * KpTorque + externalTorque;
		Quat angularVelocityQuat(velocityRotation.x, velocityRotation.y, velocityRotation.z, 0.0f);
		Quat wq_0 = angularVelocityQuat * q;

		float deltaValue = 0.5f * deltaTime;
		Quat deltaRotation = Quat(deltaValue * wq_0.x, deltaValue * wq_0.y, deltaValue * wq_0.z, deltaValue * wq_0.w);

		Quat nextRotation(q.x + deltaRotation.x,
			q.y + deltaRotation.y,
			q.z + deltaRotation.z,
			q.w + deltaRotation.w);
		nextRotation.Normalize();

		q = nextRotation;
	}
	else 
	{
		Quat angularVelocityQuat(externalTorque.x, externalTorque.y, externalTorque.z, 0.0f);
		Quat wq_0 = angularVelocityQuat * q;

		float deltaValue = 0.5f * deltaTime;
		Quat deltaRotation = Quat(deltaValue * wq_0.x, deltaValue * wq_0.y, deltaValue * wq_0.z, deltaValue * wq_0.w);

		Quat nextRotation(q.x + deltaRotation.x,
			q.y + deltaRotation.y,
			q.z + deltaRotation.z,
			q.w + deltaRotation.w);
		nextRotation.Normalize();

		q = nextRotation;

	}
	externalTorque = float3::zero;
}

void ComponentRigidBody::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;

	meta["isKinematic"] = (bool)GetIsKinematic();
	meta["mass"] = (float)GetMass();
	meta["usePositionController"] = (bool)GetUsePositionController();
	meta["useRotationController"] = (bool)GetUseRotationController();
	meta["KpForce"] = (float)GetKpForce();
	meta["KpTorque"] = (float)GetKpTorque();
}

void ComponentRigidBody::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];

	SetIsKinematic((bool)meta["isKinematic"]);
	SetMass((float)meta["mass"]);
	SetUsePositionController((bool)meta["usePositionController"]);
	SetUseRotationController((bool)meta["useRotationController"]);
	SetKpForce((float)meta["KpForce"]);
	SetKpTorque((float)meta["KpTorque"]);
}
