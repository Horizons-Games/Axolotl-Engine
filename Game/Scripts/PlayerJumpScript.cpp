#include "PlayerJumpScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentScript.h"

#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(PlayerJumpScript);

PlayerJumpScript::PlayerJumpScript() : Script(), jumpParameter(2000.0f), canDoubleJump(false) , jumpReset(0), 
	componentAnimation(nullptr), jumps(0), componentAudio(nullptr)
{
	REGISTER_FIELD(jumpParameter, float);
	REGISTER_FIELD(canDoubleJump, bool);
}

void PlayerJumpScript::Start()
{
	canDoubleJump ? jumps = 2 : jumps = 1;

	componentAnimation = owner->GetComponent<ComponentAnimation>();
	componentAudio = owner->GetComponent<ComponentAudioSource>();

	std::vector<ComponentScript*> gameObjectScripts =
		owner->GetParent()->GetComponents<ComponentScript>();
	for (int i = 0; i < gameObjectScripts.size(); ++i)
	{
		if (gameObjectScripts[i]->GetConstructName() == "HealthSystem")
		{
			healthScript = static_cast<HealthSystem*>(gameObjectScripts[i]->GetScript());
		}
	}
}

void PlayerJumpScript::PreUpdate(float deltaTime)
{
	Jump(deltaTime);
}

void PlayerJumpScript::Jump(float deltatime)
{
	float nDeltaTime = (deltatime < 1.f) ? deltatime : 1.f;
	const ComponentRigidBody* rigidBody = owner->GetComponent<ComponentRigidBody>();
	const ModuleInput* input = App->GetModule<ModuleInput>();
	btRigidBody* btRb = rigidBody->GetRigidBody();

	btVector3 movement(0, 1, 0);
	float3 direction = float3::zero;

	if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_SPACE) == KeyState::DOWN && jumps > 0)
	{
		btRb->applyCentralImpulse(movement.normalized() * jumpParameter);
		jumps --;
		jumpReset = 0;
		componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::FOOTSTEPS_WALK_STOP);

		if ((canDoubleJump && jumps == 1) || (!canDoubleJump && jumps == 0))
		{
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::JUMP);
			componentAnimation->SetParameter("IsJumping", true);
		}

		if (canDoubleJump && jumps == 0)
		{
			componentAudio->PostEvent(AUDIO::SFX::PLAYER::LOCOMOTION::DOUBLE_JUMP);
			componentAnimation->SetParameter("IsDoubleJumping", true);
		}
	}

	btVector3 currentVelocity = btRb->getLinearVelocity();

	if (currentVelocity.getY() < -0.1 && jumpReset == 0)
	{
		jumpReset = 1;

		componentAnimation->SetParameter("IsJumping", false);
	}
	else if (currentVelocity.getY() > -0.1 && jumpReset == 1)
	{
		jumpReset = 0;
		canDoubleJump ? jumps = 2 : jumps = 1;

		componentAnimation->SetParameter("IsDoubleJumping", false);
	}
}
