#include "StdAfx.h"
#include "UIHackingManager.h"

#include "Components/ComponentScript.h"
#include "Components/UI/ComponentImage.h"

#include "Application.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

REGISTERCLASS(UIHackingManager);

UIHackingManager::UIHackingManager() : Script()
{
	REGISTER_FIELD(command_A, GameObject*);
	REGISTER_FIELD(command_B, GameObject*);
	REGISTER_FIELD(command_Y, GameObject*);
}

void UIHackingManager::Init()
{
	inputPositions.push_back(owner->GetChildren()[0]->GetChildren()[0]);
	inputPositions.push_back(owner->GetChildren()[0]->GetChildren()[1]);
	inputPositions.push_back(owner->GetChildren()[0]->GetChildren()[2]);
	inputPositions.push_back(owner->GetChildren()[0]->GetChildren()[3]);
	inputPositions.push_back(owner->GetChildren()[0]->GetChildren()[4]);
	inputPositions.push_back(owner->GetChildren()[0]->GetChildren()[5]);
}

void UIHackingManager::Update(float deltaTime)
{
	
}

void UIHackingManager::AddInputVisuals(HackingCommandType type)
{
	if (inputVisuals.size() < 3)
	{
		GameObject* prefab = nullptr;
		switch (type)
		{
		case HackingCommandType::COMMAND_A:
			prefab = command_A;
			break;
		case HackingCommandType::COMMAND_B:
			prefab = command_B;
			break;
		case HackingCommandType::COMMAND_Y:
			prefab = command_Y;
			break;
		default:
			break;
		}
		
		GameObject* newInput = 
			App->GetModule<ModuleScene>()->GetLoadedScene()->DuplicateGameObject(prefab->GetName(), prefab, inputPositions[0]);
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