#include "StdAfx.h"
#include "PlayerHackingUseScript.h"

#include "HackZoneScript.h"
#include "UIHackingManager.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentRigidBody.h"
#include "Physics/Physics.h"

#include "MathGeoLib/Include/Geometry/Ray.h"

REGISTERCLASS(PlayerHackingUseScript);

PlayerHackingUseScript::PlayerHackingUseScript()
	: Script(), isHackingActive(false), hackingTag("Hackeable")
{
	REGISTER_FIELD(hackingManager, UIHackingManager*);
}

void PlayerHackingUseScript::Start()
{
	input = App->GetModule<ModuleInput>();
	transform = GetOwner()->GetComponentInternal<ComponentTransform>();
	rigidBody = GetOwner()->GetComponentInternal<ComponentRigidBody>();
	hackZone = nullptr;
}

void PlayerHackingUseScript::Update(float deltaTime)
{

	currentTime += deltaTime;

	if (input->GetKey(SDL_SCANCODE_H) != KeyState::IDLE && !isHackingActive)
	{
		FindHackZone(hackingTag);
		if (hackZone && !hackZone->IsCompleted())
		{
			InitHack();
		}
	}

	if (isHackingActive)
	{
		if (currentTime > maxHackTime)
		{
			FinishHack();
		}

		else
		{

			if (input->GetKey(keyCombination[keyIndex]) == KeyState::UP)
			{
				userKeyInputs.push_back(keyCombination[keyIndex]);
				keyIndex++;
				LOG_DEBUG("user add key to combination");

				hackingManager->RemoveInputVisuals();
			}

			if (input->GetGamepadButton(buttonCombination[buttonIndex]) == KeyState::UP)
			{
				userButtonInputs.push_back(buttonCombination[buttonIndex]);
				buttonIndex++;
				LOG_DEBUG("user add button to combination");

				hackingManager->RemoveInputVisuals();
			}


			if (userKeyInputs == keyCombination)
			{
				LOG_DEBUG("hacking completed");
				FinishHack();
				hackZone->SetCompleted();
			}

			if (userButtonInputs == buttonCombination)
			{
				LOG_DEBUG("hacking completed");
				FinishHack();
				hackZone->SetCompleted();
			}

		}
	}
}

//TEMPORARY METHOD
void PlayerHackingUseScript::PrintCombination()
{
	std::string combination;
	for (auto element : keyCombination)
	{
		char c = '\0';
		switch (element) {
		case SDL_SCANCODE_SPACE: 
			c = '_'; 
			break;
		case SDL_SCANCODE_R: 
			c = 'R'; 
			break;
		case SDL_SCANCODE_E: 
			c = 'E'; 
			break;
		case SDL_SCANCODE_T:
			c = 'T';
			break;
		default: 
			break;
		}
		combination += c;
		
	}

	LOG_DEBUG(combination);
}

void PlayerHackingUseScript::InitHack()
{
	DisableAllInteractions();
	isHackingActive = true;
	currentTime = App->GetDeltaTime();
	maxHackTime = hackZone->GetMaxTime();
	hackZone->GenerateCombination();

	userKeyInputs.reserve(hackZone->GetSequenceSize());
	userButtonInputs.reserve(hackZone->GetSequenceSize());

	keyCombination = hackZone->GetKeyCombination();

	for (auto key : keyCombination)
	{
		hackingManager->AddInputVisuals(HackingCommand::GetCommand(key));
	}

	buttonCombination = hackZone->GetButtonCombination();

	buttonIndex = 0;
	keyIndex = 0;

	PrintCombination(); //SUBSTITUTE WITH CANVAS UI
	LOG_DEBUG("hacking is active");
}

void PlayerHackingUseScript::FinishHack()
{
	EnableAllInteractions();
	isHackingActive = false;

	userKeyInputs.clear();
	userButtonInputs.clear();

	hackingManager->CleanInputVisuals();

	LOG_DEBUG("hacking is finished");
}

void PlayerHackingUseScript::DisableAllInteractions()
{

}

void PlayerHackingUseScript::EnableAllInteractions()
{

}

void PlayerHackingUseScript::FindHackZone(const std::string& tag)
{
	RaycastHit hit;
	btVector3 rigidBodyOrigin = rigidBody->GetRigidBodyOrigin();
	float3 origin = float3(rigidBodyOrigin.getX(), rigidBodyOrigin.getY(), rigidBodyOrigin.getZ());
	int raytries = 0;

	while (!hackZone && raytries < 4)
	{
		Ray ray(origin + float3(0.f, 1 * raytries, 0.f), transform->GetGlobalForward());
		LineSegment line(ray, 300);
		raytries++;

		if (Physics::RaycastToTag(line, hit, owner, tag))
		{
			GameObject* hackZoneObject = hit.gameObject;
			HackZoneScript* hackZoneScript = hackZoneObject->GetComponent<HackZoneScript>();
			ComponentTransform* hackZoneTransform = hackZoneObject->GetComponent<ComponentTransform>();

			float3 hackZonePosition = hackZoneTransform->GetGlobalPosition();
			float3 playerPosition = transform->GetGlobalPosition();

			float distance = (playerPosition - hackZonePosition).Length();

			if (distance < hackZoneScript->GetInfluenceRadius())
			{
				hackZone = hackZoneScript;
			}

		}
	}
}
