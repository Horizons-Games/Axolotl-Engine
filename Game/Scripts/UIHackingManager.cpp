#include "StdAfx.h"
#include "UIHackingManager.h"

#include "Components/UI/ComponentSlider.h"

#include "Application.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

REGISTERCLASS(UIHackingManager);

UIHackingManager::UIHackingManager() : Script()
{
	REGISTER_FIELD(command_A, GameObject*);
	REGISTER_FIELD(command_X, GameObject*);
	REGISTER_FIELD(command_Y, GameObject*);
	REGISTER_FIELD(hackingTimer, GameObject*);
}

void UIHackingManager::Init()
{
	for (int i = 0; i < 6; i++)
	{
		inputPositions.push_back(owner->GetChildren()[0]->GetChildren()[i]);
	}
}

void UIHackingManager::AddInputVisuals(HackingCommandType type)
{
	if (inputVisuals.size() < 6)
	{
		GameObject* prefab = nullptr;
		switch (type)
		{
		case HackingCommandType::COMMAND_A:
			prefab = command_A;
			break;
		case HackingCommandType::COMMAND_X:
			prefab = command_X;
			break;
		case HackingCommandType::COMMAND_Y:
			prefab = command_Y;
			break;
		default:
			break;
		}
		
		Scene* loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

		GameObject* newInput = loadedScene->DuplicateGameObject(prefab->GetName(), prefab, inputPositions[0]);
		newInput->Enable();

		inputVisuals.push_front(newInput);
		for (int i = 1; i < inputVisuals.size(); i++)
		{
			inputVisuals[i]->SetParent(inputPositions[i]);
		}
	}
}

void UIHackingManager::RemoveInputVisuals()
{
	const GameObject* inputDeleted = inputVisuals[inputVisuals.size()-1];
	inputVisuals.pop_back();
	App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(inputDeleted);
}

void UIHackingManager::CleanInputVisuals()
{
	for (GameObject* input : inputVisuals) 
	{
		App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(input);
	}
	inputVisuals.clear();
}

void UIHackingManager::EnableHackingTimer()
{
	hackingTimer->Enable();
}

void UIHackingManager::DisableHackingTimer()
{
	hackingTimer->Disable();
}

void UIHackingManager::SetHackingTimerValue(float maxHackTime, float currentTime)
{
	ComponentSlider* hackingTimerSlider = hackingTimer->GetComponent<ComponentSlider>();
	hackingTimerSlider->ModifyCurrentValue((maxHackTime - currentTime) / maxHackTime * 100.f);
}
