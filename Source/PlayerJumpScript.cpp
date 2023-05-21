#include "PlayerJumpScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"

REGISTERCLASS(PlayerJumpScript);

PlayerJumpScript::PlayerJumpScript() : Script(), jumpParameter(2000.0f), canDoubleJump(false) , jumpReset(0)
{
	REGISTER_FIELD_WITH_ACCESSORS(JumpParameter, float);
	REGISTER_FIELD_WITH_ACCESSORS(CanDoubleJump, bool);
}

void PlayerJumpScript::Start()
{
	canDoubleJump ? jumps = 2 : jumps = 1;
}

void PlayerJumpScript::PreUpdate(float deltaTime)
{
	Jump(deltaTime);
}

float PlayerJumpScript::GetJumpParameter() const
{
	return jumpParameter;
}

void PlayerJumpScript::SetJumpParameter(float jumpParameter)
{
	this->jumpParameter = jumpParameter;
}

bool PlayerJumpScript::GetCanDoubleJump() const
{
	return canDoubleJump;
}

void PlayerJumpScript::SetCanDoubleJump(bool canDoubleJump)
{
	this->canDoubleJump = canDoubleJump;
}

void PlayerJumpScript::Jump(float deltatime)
{
	float nDeltaTime = (deltatime < 1.f) ? deltatime : 1.f;
	ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
	ModuleInput* input = App->GetModule<ModuleInput>();
	btRigidBody* btRb = rigidBody->GetRigidBody();
	//btRb->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	btVector3 movement(0, 1, 0);
	float3 direction = float3::zero;

	if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_SPACE) == KeyState::DOWN && jumps > 0)
	{
		btRb->applyCentralImpulse(movement.normalized() * jumpParameter);
		jumps --;
		jumpReset = 0;
	}

	btVector3 currentVelocity = btRb->getLinearVelocity();

	if (currentVelocity.getY() < -0.1 && jumpReset == 0)
	{
		jumpReset = 1;
	}
	else if (currentVelocity.getY() > -0.1 && jumpReset == 1)
	{
		jumpReset = 0;
		canDoubleJump ? jumps = 2 : jumps = 1;
	}
}
