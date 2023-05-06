#include "PlayerMoveScript.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentMeshCollider.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"

#include "DataStructures/Quadtree.h"
#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(PlayerMoveScript);

PlayerMoveScript::PlayerMoveScript() : Script(), componentTransform(nullptr),
										componentAudio(nullptr), playerState(PlayerActions::IDLE),
										speed(6.0f),
										jumpParameter(80.0f), dashForce(50.0f), canDash(true),
										canDoubleJump(true), jumps(0), isCrouch(false),
										nextDash(0)
{
	REGISTER_FIELD(Speed, float);
	REGISTER_FIELD(JumpParameter, float);
	REGISTER_FIELD(DashForce, float);
	REGISTER_FIELD(CanDash, bool);
}

void PlayerMoveScript::Start()
{
	componentTransform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	componentAudio = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));

	if (canDoubleJump)
	{
		jumps = 2;
	}
	else
	{
		jumps = 1;
	}
}

void PlayerMoveScript::PreUpdate(float deltaTime)
{
	Move();
}

void PlayerMoveScript::Move()
{
	float deltaTime = (App->GetDeltaTime() < 1.f) ? App->GetDeltaTime() : 1.f;

	ComponentMeshCollider* collider = static_cast<ComponentMeshCollider*>(owner->GetComponent(ComponentType::MESHCOLLIDER));
	ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));

	math::vec points[8];
	componentTransform->GetObjectOBB().GetCornerPoints(points);
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
	if (input->GetKey(SDL_SCANCODE_E) != KeyState::IDLE && canDash)
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
		componentTransform->SetScale(componentTransform->GetScale() / 2);
		std::vector<GameObject*> children = owner->GetChildren();
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
		componentTransform->SetScale(componentTransform->GetScale() * 2);
		std::vector<GameObject*> children = owner->GetChildren();
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
		jumpVector += componentTransform->GetGlobalForward().Normalized();

		if (sizeForce == 0.0f)
		{
			forceVector += -direction;
		}
		if (sizeJump == 0.0f)
		{
			jumpVector += -componentTransform->GetGlobalForward().Normalized();
		}

		if (!collider->Move(Direction::FRONT, size + sizeForce + sizeJump, componentTransform->GetLocalAABB().Size().y * 0.5f))
		{
			if (sizeForce != 0.0f)
			{
				forceVector += -direction;
			}
			if (sizeJump != 0.0f)
			{
				jumpVector += -componentTransform->GetGlobalForward().Normalized();
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
		jumpVector += -componentTransform->GetGlobalForward().Normalized();

		if (sizeForce == 0.0f)
		{
			forceVector += direction;
		}

		if (sizeJump == 0.0f)
		{
			jumpVector += componentTransform->GetGlobalForward().Normalized();
		}

		if (!collider->Move(Direction::BACK, size + sizeForce + sizeJump, componentTransform->GetLocalAABB().Size().y * 0.5f))
		{
			if (sizeForce != 0.0f)
			{
				forceVector += direction;
			}

			if (sizeJump != 0.0f)
			{
				jumpVector += componentTransform->GetGlobalForward().Normalized();
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
		jumpVector += -componentTransform->GetGlobalRight().Normalized();

		if (sizeForce == 0.0f)
		{
			forceVector += sideDirection;
		}

		if (sizeJump == 0.0f)
		{
			jumpVector += componentTransform->GetGlobalRight().Normalized();
		}

		if (!collider->Move(Direction::LEFT, size + sizeForce + sizeJump, componentTransform->GetLocalAABB().Size().y * 0.5f))
		{
			if (sizeForce != 0.0f)
			{
				forceVector += sideDirection;
			}

			if (sizeJump != 0.0f)
			{
				jumpVector += componentTransform->GetGlobalRight().Normalized();;
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
		jumpVector += -componentTransform->GetGlobalRight().Normalized();

		if (sizeForce == 0.0f)
		{
			forceVector += -sideDirection;
		}

		if (sizeJump == 0.0f)
		{
			jumpVector += componentTransform->GetGlobalRight().Normalized();
		}

		if (!collider->Move(Direction::RIGHT, size + sizeForce + sizeJump, componentTransform->GetLocalAABB().Size().y * 0.5f))
		{
			if (sizeForce != 0.0f)
			{
				forceVector += -sideDirection;
			}

			if (sizeJump != 0.0f)
			{
				jumpVector += componentTransform->GetGlobalRight().Normalized();
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

	// Jump
	if (input->GetKey(SDL_SCANCODE_SPACE) == KeyState::DOWN && jumps > 0)
	{
		sizeJump = deltaTime * jumpParameter;
		jumps -= 1;
		if (rigidBody->IsOnGround() || jumps > 0)
		{
			rigidBody->AddForce(jumpVector * jumpParameter, ForceMode::Acceleration);
		}

	}

	// Control Double Jump
	if (rigidBody->IsOnGround() && canDoubleJump)
	{
		jumps = 2;
	}
	else if (rigidBody->IsOnGround() && !canDoubleJump)
	{
		jumps = 1;
	}


	componentTransform->UpdateTransformMatrices();

	//bottom
	float maxHeight = -math::inf;

	std::vector<float3> extraPoints;
	collider->GetMinMaxPoints(bottomPoints, extraPoints, 0);
	for (float3 bottomPoint : extraPoints)
	{
		bottomPoint.y += math::Abs(componentTransform->GetEncapsuledAABB().MinY() - componentTransform->GetPosition().y) / 5;
		Ray ray(bottomPoint, -float3::unitY);
		LineSegment line(ray, App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree()->GetBoundingBox().Size().y);
		bool hasHit = Physics::Raycast(line, hit, owner);

		if (hasHit && hit.hitPoint.y > maxHeight)
		{
			maxHeight = hit.hitPoint.y;
		}
	}

	rigidBody->SetBottomHitPoint(maxHeight);
}

float PlayerMoveScript::GetSpeed() const
{
	return speed;
}

void PlayerMoveScript::SetSpeed(float speed)
{
	this->speed = speed;
}

float PlayerMoveScript::GetJumpParameter() const
{
	return jumpParameter;
}

void PlayerMoveScript::SetJumpParameter(float jumpParameter)
{
	this->jumpParameter = jumpParameter;
}

float PlayerMoveScript::GetDashForce() const
{
	return dashForce;
}

void PlayerMoveScript::SetDashForce(float dashForce)
{
	this->dashForce = dashForce;
}

bool PlayerMoveScript::GetCanDash() const
{
	return canDash;
}

void PlayerMoveScript::SetCanDash(bool canDash)
{
	this->canDash = canDash;
}

bool PlayerMoveScript::GetCanDoubleJump() const
{
	return canDoubleJump;
}

void PlayerMoveScript::SetCanDoubleJump(bool canDoubleJump)
{
	this->canDoubleJump = canDoubleJump;
}