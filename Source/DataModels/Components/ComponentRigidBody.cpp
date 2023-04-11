#include "ComponentRigidBody.h"
#include "ComponentTransform.h"
#include "ComponentMockState.h"

#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "DataStructures/Quadtree.h"
#include "Geometry/Frustum.h"

#include "GameObject/GameObject.h"
#include "Application.h"

#include "FileSystem/Json.h"

#include "Geometry/LineSegment.h"
#include "Geometry/Ray.h"
#include "Physics/Physics.h"

#include <iostream>


ComponentRigidBody::ComponentRigidBody(bool active, GameObject* owner)
	: Component(ComponentType::RIGIDBODY, active, owner, true),
	isKinematic(true), m(1.0f), g(9.81f), v0(float3(0.0f, 0.0f, 0.0f))
{
}

ComponentRigidBody::ComponentRigidBody(const ComponentRigidBody& componentRigidBody)
	: Component(componentRigidBody),
	isKinematic(componentRigidBody.isKinematic), m(componentRigidBody.m), g(componentRigidBody.g), v0(componentRigidBody.v0)
{
}


ComponentRigidBody::~ComponentRigidBody()
{
}

void ComponentRigidBody::Update()
{
	
#ifdef ENGINE
	if (App->GetIsOnPlayMode()) 
	{
#endif
		if (isKinematic)
		{
			ComponentTransform* transform = static_cast<ComponentTransform*>(GetOwner()->GetComponent(ComponentType::TRANSFORM));
			float3 currentPos = transform->GetPosition();
			Ray ray(currentPos, -float3::unitY);
			LineSegment line(ray, App->scene->GetLoadedScene()->GetRootQuadtree()->GetBoundingBox().Size().y);
			RaycastHit hit;

			bool hasHit = Physics::Raycast(line, hit);
			float3 x;
			float t = App->GetDeltaTime();
			float3 x0 = currentPos;
			float3 a = float3(0.0f, -0.5 * g * t * t, 0.0f);

			v0.y -= g * t;
			x = x0 + v0 * t + a;
			float verticalDistanceToFeet = math::Abs(transform->GetEncapsuledAABB().MinY() - x0.y);
			if (hasHit && x.y <= hit.hitPoint.y + verticalDistanceToFeet + (x - x0).Length())
			{

				x = hit.hitPoint + float3(0.0f, verticalDistanceToFeet, 0.0f);
				v0 = float3::zero;

				if (hit.gameObject != nullptr && hit.gameObject->GetComponent(ComponentType::MOCKSTATE) != nullptr)
				{
					ComponentMockState* mockState = static_cast<ComponentMockState*>(hit.gameObject->GetComponent(ComponentType::MOCKSTATE));

					if (mockState->GetIsWinState())
					{
						//TODO: win state
					#ifdef ENGINE			
						ENGINE_LOG("Next scene should be %s", mockState->GetSceneName());
						App->OnStopPlay();
					#else
						std::string sceneName = mockState->GetSceneName();
						App->scene->SetSceneToLoad("Lib/Scenes/" + sceneName + ".axolotl");
					#endif				
					}
					else if (mockState->GetIsFailState())
					{
						//TODO fail state
					}
				}
			}

			transform->SetPosition(x);
		}
#ifdef ENGINE
	}
#endif
}

void ComponentRigidBody::Draw()
{
	
}

void ComponentRigidBody::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;
}

void ComponentRigidBody::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
}
