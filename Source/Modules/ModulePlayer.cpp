
#include "Application.h"

#include "ModulePlayer.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "Scene/Scene.h"
#include "ModuleInput.h"

#include "Camera/Camera.h"
#include "Camera/CameraGameObject.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentMeshCollider.h"
#include "Components/ComponentRigidBody.h"
#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"

#include "Components/ComponentTransform.h"

ModulePlayer::ModulePlayer(): cameraPlayer(nullptr), player(nullptr),componentPlayer(nullptr) {};

ModulePlayer::~ModulePlayer() {
};

bool ModulePlayer::Init()
{
	return true;
}

bool ModulePlayer::Start()
{
	//Initialize the player
	LoadNewPlayer();
	return true;
}

update_status ModulePlayer::PreUpdate()
{
	if (player && !componentPlayer->IsStatic() && App->camera->GetSelectedPosition() == 0)
	{
		Move();
		Rotate();
	}
	return update_status::UPDATE_CONTINUE;
}

update_status ModulePlayer::Update()
{
	player->Update();
	ComponentTransform* trans = static_cast<ComponentTransform*>(player->GetComponent(ComponentType::TRANSFORM));
	trans->UpdateTransformMatrices();
	return update_status::UPDATE_CONTINUE;
}

GameObject* ModulePlayer::GetPlayer()
{
	return player.get();
}

void ModulePlayer::SetPlayer(std::unique_ptr<GameObject> newPlayer)
{
	player = std::move(newPlayer);
	componentPlayer = static_cast<ComponentPlayer*>(player->GetComponent(ComponentType::PLAYER));
}

Camera* ModulePlayer::GetCameraPlayer()
{
	return cameraPlayer;
}

void ModulePlayer::Move()
{
	float deltaTime = (App->GetDeltaTime() < 1.f)?App->GetDeltaTime():1.f;
	ComponentTransform* trans = static_cast<ComponentTransform*>(player->GetComponent(ComponentType::TRANSFORM));
	ComponentMeshCollider* collider = static_cast<ComponentMeshCollider*>(player->GetComponent(ComponentType::MESHCOLLIDER));
	ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(player->GetComponent(ComponentType::RIGIDBODY));
	float3 position = trans->GetPosition();

	math::vec points[8];
	trans->GetObjectOBB().GetCornerPoints(points);
	std::vector<float3> frontPoints = { points[1], points[3], points[5], points[7] };
	std::vector<float3> backPoints = { points[0], points[2], points[4], points[6] };
	std::vector<float3> leftPoints = { points[4], points[6], points[5],  points[7] };
	std::vector<float3> rightPoints = { points[0], points[2], points[1], points[3] };
	std::vector<float3> bottomPoints = { points[0], points[1], points[4], points[5] };
	std::vector<float3> topPoints = { points[2], points[3], points[6], points[7] };

	float3 direction = (points[1] - points[0]).Normalized();
	float3 sideDirection = (points[0] - points[4]).Normalized();
	float3 verticalDirection = float3::unitY;

	RaycastHit hit;

	float forceParameter = 50.0f;
	float jumpParameter = 20.0f;

	float minXPoint = trans->GetEncapsuledAABB().MinX();
	float minZPoint = trans->GetEncapsuledAABB().MinZ();
	float maxXPoint = trans->GetEncapsuledAABB().MaxX();
	float maxZPoint = trans->GetEncapsuledAABB().MaxZ();

	float leftStep = math::Abs(minXPoint - position.x);
	float rightStep = math::Abs(maxXPoint - position.x);
	float forwardStep = math::Abs(maxZPoint - position.z);
	float backwardStep = math::Abs(minZPoint - position.z);

	float size = 0.0f;
	float sizeForce = 0.0f;
	float sizeJump = 0.0f;

	float3 jumpVector = float3::unitY;
	float3 forceVector = float3::zero;

	size = speed * deltaTime * 1.1f;
	
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
	{
		sizeForce = deltaTime * forceParameter;
		sizeJump = deltaTime * jumpParameter;
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

		if (!collider->IsColliding(frontPoints, direction, size + sizeForce + sizeJump, hit, trans->GetLocalAABB().Size().y * 0.15f))
		{
			position += trans->GetGlobalForward().Normalized() * speed * deltaTime;
			trans->SetPosition(position);

			trans->UpdateTransformMatrices();
			trans->GetObjectOBB().GetCornerPoints(points);
			backPoints = { points[0], points[2], points[4], points[6] };
			leftPoints = { points[4], points[6], points[5],  points[7] };
			rightPoints = { points[0], points[2], points[1], points[3] };
			
		}
		else 
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

		if (!collider->IsColliding(backPoints, -direction, size + sizeForce + sizeJump, hit, trans->GetLocalAABB().Size().y * 0.15f))
		{
			position += -trans->GetGlobalForward().Normalized() * speed * deltaTime;
			trans->SetPosition(position);

			trans->UpdateTransformMatrices();
			trans->GetObjectOBB().GetCornerPoints(points);
			leftPoints = { points[4], points[6], points[5],  points[7] };
			rightPoints = { points[0], points[2], points[1], points[3] };
		}
		else 
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
			jumpVector += trans->GetGlobalRight().Normalized();;
		}

		if (!collider->IsColliding(leftPoints, -sideDirection, size + sizeForce + sizeJump, hit, trans->GetLocalAABB().Size().y * 0.15f))
		{
			position += trans->GetGlobalRight().Normalized() * speed * 2 / 3 * deltaTime;
			trans->SetPosition(position);

			trans->UpdateTransformMatrices();
			trans->GetObjectOBB().GetCornerPoints(points);
			rightPoints = { points[0], points[2], points[1], points[3] };
		}
		else 
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

		if (!collider->IsColliding(rightPoints, sideDirection, size + sizeForce + sizeJump, hit, trans->GetLocalAABB().Size().y * 0.15f))
		{
			position -= trans->GetGlobalRight().Normalized() * speed * 2 / 3 * deltaTime;
			trans->SetPosition(position);

			trans->UpdateTransformMatrices();
		}
		else 
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


	

	rigidBody->AddForce(forceVector * forceParameter);
	trans->UpdateTransformMatrices();
	
	if (App->input->GetKey(SDL_SCANCODE_SPACE) != KeyState::IDLE)
	{
		rigidBody->AddForce(jumpVector * jumpParameter, ForceMode::Acceleration);
		trans->UpdateTransformMatrices();
	}

	//bottom
	float maxHeight = -math::inf;

	std::vector<float3> extraPoints;
	collider->GetMinMaxPoints(bottomPoints, extraPoints, 0);
	for (float3 bottomPoint : extraPoints) 
	{
		bottomPoint.y += math::Abs(trans->GetEncapsuledAABB().MinY() - trans->GetPosition().y)/5;
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

void ModulePlayer::Rotate()
{
	if (App->input->GetMouseMotion().x != 0)
	{
		float deltaTime = App->GetDeltaTime();
		ComponentTransform* trans = static_cast<ComponentTransform*>(player->GetComponent(ComponentType::TRANSFORM));
		float3 newRot = trans->GetRotationXYZ();
		newRot.y += - App->input->GetMouseMotion().x * deltaTime;
		trans->SetRotation(newRot);
		trans->UpdateTransformMatrices();


		//Corroborate that you don't fuse with a wall
		ComponentMeshCollider* collider = static_cast<ComponentMeshCollider*>(player->GetComponent(ComponentType::MESHCOLLIDER));
		math::vec points[8];
		trans->GetObjectOBB().GetCornerPoints(points);
		std::vector<float3> frontPoints = { points[1], points[3], points[5], points[7] };
		float3 direction = (points[1] - points[0]).Normalized();
		RaycastHit hit;
		if (collider->IsColliding(frontPoints, -direction, trans->GetLocalAABB().Size().z * 0.7, hit))
		{
			float deltaTime = App->GetDeltaTime();
			ComponentTransform* trans = static_cast<ComponentTransform*>(player->GetComponent(ComponentType::TRANSFORM));
			float3 newRot = trans->GetRotationXYZ();
			newRot.y += App->input->GetMouseMotion().x * deltaTime;
			trans->SetRotation(newRot);
			trans->UpdateTransformMatrices();
		}

	}
}

void ModulePlayer::LoadNewPlayer()
{
	std::vector<GameObject*> cameras = App->scene->GetLoadedScene()->GetSceneCameras();
	for (GameObject* camera : cameras)
	{
		if (camera->GetParent()->GetComponent(ComponentType::PLAYER))
		{
			SetPlayer(std::unique_ptr<GameObject>(camera->GetParent()->GetParent()->UnlinkChild(camera->GetParent())));
			cameraPlayer = static_cast<ComponentCamera*>(camera->GetComponent(ComponentType::CAMERA))->GetCamera();
			App->scene->RemoveGameObjectAndChildren(camera->GetParent());
			App->camera->SetSelectedCamera(0);
			if(componentPlayer->HaveMouseActivated()) 
			{
				App->input->SetShowCursor(true);
			}
			else 
			{
				App->input->SetShowCursor(false);
			}
		}
	}
}


bool ModulePlayer::IsStatic()
{
	return componentPlayer->IsStatic();
}