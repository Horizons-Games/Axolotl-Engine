#include "PlayerMovilityScript.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Components/ComponentMeshCollider.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components//ComponentPlayer.h"
#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"

REGISTERCLASS(PlayerMovilityScript);

PlayerMovilityScript::PlayerMovilityScript() : Script(), componentPlayer(nullptr), speed(1.0f), jumpParameter(30.0f)
{
	REGISTER_FIELD(Speed, float);
	REGISTER_FIELD(JumpParameter, float);
	//REGISTER_FIELD(MovingGameObject, GameObject*);
}

void PlayerMovilityScript::Start()
{
	componentPlayer = static_cast<ComponentPlayer*>(owner->GetComponent(ComponentType::PLAYER));
}

void PlayerMovilityScript::PreUpdate(float deltaTime)
{
	
	if (!componentPlayer->IsStatic() && App->camera->GetSelectedPosition() == 0
		&& !SDL_ShowCursor(SDL_QUERY))
	{
		Move();
		Rotate();
	}
	
}

void PlayerMovilityScript::Move()
{
	float deltaTime = (App->GetDeltaTime() < 1.f) ? App->GetDeltaTime() : 1.f;

	ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	ComponentMeshCollider* collider = static_cast<ComponentMeshCollider*>(owner->GetComponent(ComponentType::MESHCOLLIDER));
	ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));

	math::vec points[8];
	trans->GetObjectOBB().GetCornerPoints(points);
	std::vector<float3> bottomPoints = { points[0], points[1], points[4], points[5] };

	float3 direction = (points[1] - points[0]).Normalized();
	float3 sideDirection = (points[0] - points[4]).Normalized();
	float3 verticalDirection = float3::unitY;

	RaycastHit hit;

	float forceParameter = 50.0f;

	float size = 0.0f;
	float sizeForce = 0.0f;
	float sizeJump = 0.0f;

	float3 jumpVector = float3::unitY;
	float3 forceVector = float3::zero;

	size = speed * deltaTime * 1.1f;

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
	{
		sizeForce = deltaTime * forceParameter;

	}

	//Forward
	if (App->input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		forceVector += direction;
		jumpVector += trans->GetGlobalForward().Normalized();

		if (sizeForce == 0.0f)
		{
			forceVector += -direction;
		}
		if (sizeJump == 0.0f)
		{
			jumpVector += -trans->GetGlobalForward().Normalized();
		}

		if (!collider->Move(Direction::FRONT, size + sizeForce + sizeJump, hit, trans->GetLocalAABB().Size().y * 0.15f))
		{
			if (sizeForce != 0.0f)
			{
				forceVector += -direction;
			}
			if (sizeJump != 0.0f)
			{
				jumpVector += -trans->GetGlobalForward().Normalized();
			}
		}
	}

	//Backward
	if (App->input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		forceVector += -direction;
		jumpVector += -trans->GetGlobalForward().Normalized();

		if (sizeForce == 0.0f)
		{
			forceVector += direction;
		}

		if (sizeJump == 0.0f)
		{
			jumpVector += trans->GetGlobalForward().Normalized();
		}

		if (!collider->Move(Direction::BACK, size + sizeForce + sizeJump, hit, trans->GetLocalAABB().Size().y * 0.15f))
		{
			if (sizeForce != 0.0f)
			{
				forceVector += direction;
			}

			if (sizeJump != 0.0f)
			{
				jumpVector += trans->GetGlobalForward().Normalized();
			}
		}
	}

	//Left
	if (App->input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{

		forceVector += -sideDirection;
		jumpVector += -trans->GetGlobalRight().Normalized();

		if (sizeForce == 0.0f)
		{
			forceVector += sideDirection;
		}

		if (sizeJump == 0.0f)
		{
			jumpVector += trans->GetGlobalRight().Normalized();
		}

		if (!collider->Move(Direction::LEFT, size + sizeForce + sizeJump, hit, trans->GetLocalAABB().Size().y * 0.15f))
		{
			if (sizeForce != 0.0f)
			{
				forceVector += sideDirection;
			}

			if (sizeJump != 0.0f)
			{
				jumpVector += trans->GetGlobalRight().Normalized();;
			}
		}
	}

	//Right
	if (App->input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		forceVector += sideDirection;
		jumpVector += -trans->GetGlobalRight().Normalized();

		if (sizeForce == 0.0f)
		{
			forceVector += -sideDirection;
		}

		if (sizeJump == 0.0f)
		{
			jumpVector += trans->GetGlobalRight().Normalized();
		}

		if (!collider->Move(Direction::RIGHT, size + sizeForce + sizeJump, hit, trans->GetLocalAABB().Size().y * 0.15f))
		{
			if (sizeForce != 0.0f)
			{
				forceVector += -sideDirection;
			}

			if (sizeJump != 0.0f)
			{
				jumpVector += trans->GetGlobalRight().Normalized();
			}
		}
	}

	//rigidBody->AddForce(forceVector * forceParameter);


	if (App->input->GetKey(SDL_SCANCODE_SPACE) != KeyState::IDLE)
	{
		sizeJump = deltaTime * jumpParameter;
		if (rigidBody->IsOnGround())
		{
			rigidBody->AddForce(jumpVector * jumpParameter, ForceMode::Acceleration);
		}

	}

	trans->UpdateTransformMatrices();

	//bottom
	float maxHeight = -math::inf;

	std::vector<float3> extraPoints;
	collider->GetMinMaxPoints(bottomPoints, extraPoints, 0);
	for (float3 bottomPoint : extraPoints)
	{
		bottomPoint.y += math::Abs(trans->GetEncapsuledAABB().MinY() - trans->GetPosition().y) / 5;
		Ray ray(bottomPoint, -float3::unitY);
		LineSegment line(ray, App->scene->GetLoadedScene()->GetRootQuadtree()->GetBoundingBox().Size().y);
		bool hasHit = Physics::Raycast(line, hit);

		if (hasHit && hit.hitPoint.y > maxHeight)
		{
			maxHeight = hit.hitPoint.y;
		}
	}

	rigidBody->SetBottomHitPoint(maxHeight);

	//top
	/*if (!collider->IsColliding(topPoints, verticalDirection, speed * deltaTime * 1.1f, 0.0f))
	{
	}*/
}


void PlayerMovilityScript::Rotate()
{
	float deltaTime = App->GetDeltaTime();
	ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	float3 newRot = trans->GetRotationXYZ();
	newRot.y += -App->input->GetMouseMotion().x * deltaTime;
	trans->SetRotation(newRot);
	trans->UpdateTransformMatrices();


	//Corroborate that you don't fuse with a owner
	ComponentMeshCollider* collider = static_cast<ComponentMeshCollider*>(owner->GetComponent(ComponentType::MESHCOLLIDER));
	math::vec points[8];
	trans->GetObjectOBB().GetCornerPoints(points);
	std::vector<float3> frontPoints = { points[1], points[3], points[5], points[7] };
	float3 direction = (points[1] - points[0]).Normalized();
	RaycastHit hit;
	if (collider->IsColliding(frontPoints, -direction, trans->GetLocalAABB().Size().z * 0.7, hit))
	{
		float deltaTime = App->GetDeltaTime();
		ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
		float3 newRot = trans->GetRotationXYZ();
		newRot.y += App->input->GetMouseMotion().x * deltaTime;
		trans->SetRotation(newRot);
		trans->UpdateTransformMatrices();
	}
}