#include "StdAfx.h"
#include "PlayerHackingUseScript.h"

#include "HackZoneScript.h"

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
	if (input->GetKey(SDL_SCANCODE_E) != KeyState::IDLE && !isHackingActive)
	{
		FindHackZone(hackingTag);
		if (hackZone && !hackZone->GetCompleted())
		{
			InitHack();
		}
	}

	if (isHackingActive)
	{
		if ((SDL_GetTicks() - currentTime) / 1000.0f > maxHackTime)
		{
			FinishHack();
		}

		else
		{

			if (input->GetKey(keyCombination[keyIndex]) != KeyState::IDLE)
			{
				userKeyInputs.push_back(keyCombination[keyIndex]);
				keyIndex++;
			}

			if (input->GetGamepadButton(buttonCombination[buttonIndex]) != KeyState::IDLE)
			{
				userButtonInputs.push_back(buttonCombination[buttonIndex]);
				buttonIndex++;
			}

			if (userKeyInputs == keyCombination)
			{
				FinishHack();
				hackZone->SetCompleted();
			}

			if (userButtonInputs == buttonCombination)
			{
				FinishHack();
				hackZone->SetCompleted();
			}

		}
	}
}

void PlayerHackingUseScript::PrintCombination()
{
	std::string combination;
	for (auto element : keyCombination)
	{
		char c = '\0';  // Initialize with null character
		switch (element) {
		case SDL_SCANCODE_A: 
			c = 'A'; 
			break;
		case SDL_SCANCODE_B: 
			c = 'B'; 
			break;
		case SDL_SCANCODE_C: 
			c = 'C'; 
			break;
		default: 
			break; // do nothing
		}
		combination += c;
		
	}

	LOG_INFO(combination);
}

void PlayerHackingUseScript::InitHack()
{
	DisableAllInteractions();
	isHackingActive = true;
	currentTime = SDL_GetTicks();
	maxHackTime = hackZone->GetMaxTime();
	hackZone->GenerateCombination();

	userKeyInputs.reserve(hackZone->GetSequenceSize());
	userButtonInputs.reserve(hackZone->GetSequenceSize());

	keyCombination = hackZone->GetKeyCombination();
	buttonCombination = hackZone->GetButtonCombination();

	buttonIndex = 0;
	keyIndex = 0;

	PrintCombination();
}

void PlayerHackingUseScript::FinishHack()
{
	EnableAllInteractions();
	isHackingActive = false;

	userKeyInputs.clear();
	userButtonInputs.clear();
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
