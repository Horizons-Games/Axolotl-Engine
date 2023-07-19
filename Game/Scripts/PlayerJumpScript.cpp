#include "StdAfx.h"
#include "PlayerJumpScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentScript.h"

#include "Auxiliar/Audio/AudioData.h"
#include "MathGeoLib/Include/Geometry/Ray.h"
#include "Physics/Physics.h"

#include "DebugDraw.h"

REGISTERCLASS(PlayerJumpScript);

PlayerJumpScript::PlayerJumpScript() : Script(), jumpParameter(2000.0f), canDoubleJump(false),
componentAnimation(nullptr), componentAudio(nullptr), canJump(true), rigidbody(nullptr),
coyoteTime(0.4f), groundedCount(0), grounded(false)
{
	REGISTER_FIELD(coyoteTime, float);
	REGISTER_FIELD(grounded, bool);
	REGISTER_FIELD(coyoteTimerCount, float);
	REGISTER_FIELD(doubleJumpAvailable, bool);

	REGISTER_FIELD(jumpParameter, float);
	REGISTER_FIELD(canDoubleJump, bool);
	REGISTER_FIELD(canJump, bool);
}

bool PlayerJumpScript::isGrounded()
{
	return grounded;
}

void PlayerJumpScript::Start()
{
	rigidbody = owner->GetComponent<ComponentRigidBody>();
	//Avoid Y rotation on player this should go on movement not here but
	rigidbody->GetRigidBody()->setAngularFactor(btVector3(1.0f, 0.0f, 1.0f));
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	componentAudio = owner->GetComponent<ComponentAudioSource>();
}

void PlayerJumpScript::PreUpdate(float deltaTime)
{
	if (!grounded && coyoteTimerCount > 0.0f)
	{
		coyoteTimerCount -= deltaTime;
	}

	CheckGround();
	Jump(deltaTime);
}

void PlayerJumpScript::CheckGround() 
{
	RaycastHit hit;
	btVector3 minPoint, maxPoint;
	rigidbody->GetRigidBody()->getAabb(minPoint, maxPoint);
	btVector3 rigidBodyOrigin = rigidbody->GetRigidBodyOrigin();
	float3 origin = float3((maxPoint.getX() + minPoint.getX()) / 2.0f, minPoint.getY(), (maxPoint.getZ() + minPoint.getZ()) / 2.0f);
	Ray ray(origin, -(rigidbody->GetOwnerTransform()->GetGlobalUp()));
	LineSegment line(ray, 0.01f);

	if (Physics::RaycastFirst(line, owner))
	{
		grounded = true;
		componentAnimation->SetParameter("IsJumping", false);
		componentAnimation->SetParameter("IsDoubleJumping", false);
		doubleJumpAvailable = true;
		coyoteTimerCount = 0.0f;
	}
	else 
	{
		if (grounded)
		{
			grounded = false;
			coyoteTimerCount = coyoteTime;
		}
	}
}

void PlayerJumpScript::Jump(float deltatime)
{
	if (canJump) 
	{
		float nDeltaTime = (deltatime < 1.f) ? deltatime : 1.f;
		const ComponentRigidBody* rigidBody = owner->GetComponent<ComponentRigidBody>();
		const ModuleInput* input = App->GetModule<ModuleInput>();
		btRigidBody* btRb = rigidBody->GetRigidBody();

		btVector3 movement(0, 1, 0);
		float3 direction = float3::zero;

		if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_SPACE) == KeyState::DOWN && (grounded || coyoteTimerCount > 0.0f || (doubleJumpAvailable && canDoubleJump)))
		{
			btRb->applyCentralImpulse(movement.normalized() * jumpParameter);
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);

			if (grounded || coyoteTimerCount > 0.0f)
			{
				componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::JUMP);
				componentAnimation->SetParameter("IsJumping", true);
				grounded = false;
				coyoteTimerCount = 0.0f;
			}
			else
			{
				componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::DOUBLE_JUMP);
				componentAnimation->SetParameter("IsJumping", true);
				componentAnimation->SetParameter("IsDoubleJumping", true);
				doubleJumpAvailable = false;
			}
		}
	}
}

bool PlayerJumpScript::GetCanJump() const
{
	return canJump;
}

void PlayerJumpScript::SetCanJump(bool canJump)
{
	this->canJump = canJump;
}
