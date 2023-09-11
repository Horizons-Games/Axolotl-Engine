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
			SDL_Scancode key;
			SDL_GameControllerButton button;
			for (auto command : allHackingCommands)
			{
				auto keyButtonPair = HackingCommand::FromCommand(command);
				key = keyButtonPair->first;
				button = keyButtonPair->second;

				if (input->GetKey(key) == KeyState::UP || input->GetGamepadButton(button) == KeyState::UP)
				{
					userCommandInputs.push_back(command);
					LOG_DEBUG("user add key/button to combination");

					hackingManager->RemoveInputVisuals();
					break;
				}
			}

			bool isMismatch = false;
			for (int i = 0; i < userCommandInputs.size(); ++i)
			{
				if (userCommandInputs[i] != commandCombination[i])
				{
					isMismatch = true;
					break;
				}
			}

			if (isMismatch)
			{
				LOG_DEBUG("Mismatch detected. Hacking will fail.");
				FinishHack();
			}

			//TODO: Compare if the current input entered by user makes that the following condition will never be true
			if (userCommandInputs == commandCombination)
			{
				LOG_DEBUG("hacking completed");
				FinishHack();
				hackZone->SetCompleted();
			}

		}
	}
}

//DEBUG METHOD
void PlayerHackingUseScript::PrintCombination()
{
	std::string combination;
	for (auto element :commandCombination)
	{
		char c = '\0';
		switch (element) {
		case COMMAND_A: 
			c = '_'; 
			break;
		case COMMAND_B:
			c = 'R'; 
			break;
		case COMMAND_X:
			c = 'E'; 
			break;
		case COMMAND_Y:
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

	userCommandInputs.reserve(hackZone->GetSequenceSize());

	commandCombination = hackZone->GetCommandCombination();
	for (auto command : commandCombination)
	{
		hackingManager->AddInputVisuals(command);
	}

	PrintCombination(); //SUBSTITUTE WITH CANVAS UI
	LOG_DEBUG("hacking is active");
}

void PlayerHackingUseScript::FinishHack()
{
	EnableAllInteractions();
	isHackingActive = false;

	userCommandInputs.clear();

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