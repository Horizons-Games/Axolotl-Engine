#include "PlayerMobilityScript.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Components/ComponentMeshCollider.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentAudioSource.h"

#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"
#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(PlayerMobilityScript);

PlayerMobilityScript::PlayerMobilityScript() : Script(), componentPlayer(nullptr), speed(6.0f),
												jumpParameter(80.0f), dashForce(50.0f), canDash(true),
												canDoubleJump(true) , jumps(0), isCrouch(false),
												componentAudio(nullptr), playerState(PlayerActions::IDLE)
{
	REGISTER_FIELD_WITH_ACCESSORS(Speed, float);
	REGISTER_FIELD_WITH_ACCESSORS(JumpParameter, float);
	REGISTER_FIELD_WITH_ACCESSORS(DashForce, float);
	REGISTER_FIELD_WITH_ACCESSORS(CanDash, bool);
}

PlayerMobilityScript::~PlayerMobilityScript()
{
	delete componentPlayer;
}

void PlayerMobilityScript::Start()
{
	if (canDoubleJump) 
	{
		jumps = 2;
	}
	else
	{
		jumps = 1;
	}

	componentPlayer = static_cast<ComponentPlayer*>(owner->GetComponent(ComponentType::PLAYER));
	componentAudio = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));
}

void PlayerMobilityScript::PreUpdate(float deltaTime)
{
	
	if (!componentPlayer->IsStatic() && App->GetModule<ModuleCamera>()->GetSelectedPosition() == 0
		&& !SDL_ShowCursor(SDL_QUERY))
	{
		Move();
		Rotate();
	}
	
}

void PlayerMobilityScript::Move()
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

	// Dash pressing E during 0.2 sec
	ModuleInput* input = App->GetModule<ModuleInput>();
	if (input->GetKey(SDL_SCANCODE_E) != KeyState::IDLE && GetCanDash())
	{
		sizeForce = deltaTime * dashForce;
		if (nextDash == 0)
		{
			nextDash = static_cast<float>(SDL_GetTicks()) + 200.0f;
		}

		if (nextDash < SDL_GetTicks())
		{
			canDash = false;
			nextDash += 5000;
		}	
	}

	// Cooldown Dash
	if (nextDash > 0 && nextDash < SDL_GetTicks())
	{
		canDash = true;
		nextDash = 0;
	}

	// Run, duplicate the speed
	if (input->GetKey(SDL_SCANCODE_LSHIFT) != KeyState::IDLE)
	{
		size *= 2;
	}

	// Crouch
	if (input->GetKey(SDL_SCANCODE_LCTRL) != KeyState::IDLE && !isCrouch)
	{
		isCrouch = true;
		trans->SetScale(trans->GetScale() / 2);
		GameObject::GameObjectView children = owner->GetChildren();
		for (auto child : children) 
		{
			if (child->GetComponent(ComponentType::CAMERA))
			{
				ComponentTransform* childTrans = static_cast<ComponentTransform*>(child->GetComponent(ComponentType::TRANSFORM));
				childTrans->SetScale(childTrans->GetScale() * 2);
			}

		}
		size /= 4.f;
	}
	else if (input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::IDLE && isCrouch)
	{
		isCrouch = false;
		trans->SetScale(trans->GetScale() * 2);
		GameObject::GameObjectView children = owner->GetChildren();
		for (auto child : children)
		{
			if (child->GetComponent(ComponentType::CAMERA))
			{
				ComponentTransform* childTrans = static_cast<ComponentTransform*>(child->GetComponent(ComponentType::TRANSFORM));
				childTrans->SetScale(childTrans->GetScale() / 2);
			}
		}
	}
	else if (isCrouch)
	{
		size /= 4.f;
	}

	// Forward
	if (input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		if (playerState == PlayerActions::IDLE)
		{
			componentAudio->PostEvent(audio::SFX_PLAYER_FOOTSTEPS_WALK);
			playerState = PlayerActions::WALKING;
		}

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

		if (!collider->Move(Direction::FRONT, size + sizeForce + sizeJump, trans->GetLocalAABB().Size().y * 0.15f))
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

	// Backward
	if (input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		if (playerState == PlayerActions::IDLE)
		{
			componentAudio->PostEvent(audio::SFX_PLAYER_FOOTSTEPS_WALK);
			playerState = PlayerActions::WALKING;
		}

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

		if (!collider->Move(Direction::BACK, size + sizeForce + sizeJump, trans->GetLocalAABB().Size().y * 0.15f))
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

	// Left
	if (input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		if (playerState == PlayerActions::IDLE)
		{
			componentAudio->PostEvent(audio::SFX_PLAYER_FOOTSTEPS_WALK);
			playerState = PlayerActions::WALKING;
		}

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

		if (!collider->Move(Direction::LEFT, size + sizeForce + sizeJump, trans->GetLocalAABB().Size().y * 0.15f))
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

	// Right
	if (input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		if (playerState == PlayerActions::IDLE)
		{
			componentAudio->PostEvent(audio::SFX_PLAYER_FOOTSTEPS_WALK);
			playerState = PlayerActions::WALKING;
		}

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

		if (!collider->Move(Direction::RIGHT, size + sizeForce + sizeJump, trans->GetLocalAABB().Size().y * 0.15f))
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

	// No movement input
	if (input->GetKey(SDL_SCANCODE_W) == KeyState::IDLE &&
		input->GetKey(SDL_SCANCODE_A) == KeyState::IDLE &&
		input->GetKey(SDL_SCANCODE_S) == KeyState::IDLE &&
		input->GetKey(SDL_SCANCODE_D) == KeyState::IDLE)
	{
		if (playerState == PlayerActions::WALKING)
		{
			componentAudio->PostEvent(audio::SFX_PLAYER_FOOTSTEPS_WALK_STOP);
			playerState = PlayerActions::IDLE;
		}
	}

	//rigidBody->AddForce(forceVector * forceParameter);

	//// Jump
	//if (input->GetKey(SDL_SCANCODE_SPACE) == KeyState::DOWN && jumps > 0)
	//{
	//	sizeJump = deltaTime * jumpParameter;
	//	jumps -= 1;
	//	if (rigidBody->IsOnGround() || jumps > 0)
	//	{
	//		rigidBody->AddForce(jumpVector * jumpParameter, ForceMode::Acceleration);
	//	}

	//}
	//
	//// Control Double Jump
	//if (rigidBody->IsOnGround() && canDoubleJump)
	//{
	//	jumps = 2;
	//}
	//else if (rigidBody->IsOnGround() && !canDoubleJump)
	//{
	//	jumps = 1;
	//}
	

	trans->UpdateTransformMatrices();

	//bottom
	float maxHeight = -math::inf;

	std::vector<float3> extraPoints;
	collider->GetMinMaxPoints(bottomPoints, extraPoints, 0);
	for (float3 bottomPoint : extraPoints)
	{
		bottomPoint.y += math::Abs(trans->GetEncapsuledAABB().MinY() - trans->GetPosition().y) / 5;
		Ray ray(bottomPoint, -float3::unitY);
		LineSegment line(ray, App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree()->GetBoundingBox().Size().y);
		bool hasHit = Physics::Raycast(line, hit, owner);

		if (hasHit && hit.hitPoint.y > maxHeight)
		{
			maxHeight = hit.hitPoint.y;
		}
	}

	//rigidBody->SetBottomHitPoint(maxHeight);

	//top
	/*if (!collider->IsColliding(topPoints, verticalDirection, speed * deltaTime * 1.1f, 0.0f))
	{
	}*/
}

void PlayerMobilityScript::Rotate()
{
	float deltaTime = App->GetDeltaTime();
	ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	float3 newRot = trans->GetRotationXYZ();
	ModuleInput* input = App->GetModule<ModuleInput>();
	newRot.y += -input->GetMouseMotion().x * deltaTime;
	trans->SetRotation(newRot);
	trans->UpdateTransformMatrices();


	//Corroborate that you don't fuse with a owner
	ComponentMeshCollider* collider = static_cast<ComponentMeshCollider*>(owner->GetComponent(ComponentType::MESHCOLLIDER));
	math::vec points[8];
	trans->GetObjectOBB().GetCornerPoints(points);
	std::vector<float3> frontPoints = { points[1], points[3], points[5], points[7] };
	float3 direction = (points[1] - points[0]).Normalized();
	if (collider->IsColliding(frontPoints, -direction, trans->GetLocalAABB().Size().z * 0.5f))
	{
		float deltaTime = App->GetDeltaTime();
		ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
		float3 newRot = trans->GetRotationXYZ();
		newRot.y += input->GetMouseMotion().x * deltaTime;
		trans->SetRotation(newRot);
		trans->UpdateTransformMatrices();
	}
}

float PlayerMobilityScript::GetSpeed() const
{
	return speed;
}

void PlayerMobilityScript::SetSpeed(float speed)
{
	this->speed = speed;
}

float PlayerMobilityScript::GetJumpParameter() const
{
	return jumpParameter;
}

void PlayerMobilityScript::SetJumpParameter(float jumpParameter)
{
	this->jumpParameter = jumpParameter;
}

float PlayerMobilityScript::GetDashForce() const
{
	return dashForce;
}

void PlayerMobilityScript::SetDashForce(float dashForce)
{
	this->dashForce = dashForce;
}

bool PlayerMobilityScript::GetCanDash() const
{
	return canDash;
}

void PlayerMobilityScript::SetCanDash(bool canDash)
{
	this->canDash = canDash;
}

bool PlayerMobilityScript::GetCanDoubleJump() const
{
	return canDoubleJump;
}

void PlayerMobilityScript::SetCanDoubleJump(bool canDoubleJump)
{
	this->canDoubleJump = canDoubleJump;
}