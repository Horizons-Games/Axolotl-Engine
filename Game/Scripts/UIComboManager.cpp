#include "StdAfx.h"
#include "UIComboManager.h"

#include "Components/ComponentScript.h"
#include "Components/UI/ComponentSlider.h"
#include "Components/UI/ComponentImage.h"

#include "Application.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

REGISTERCLASS(UIComboManager);

UIComboManager::UIComboManager() : Script(), clearComboTimer(0.0f), clearCombo(false)
{
	REGISTER_FIELD(inputPrefabSoft, GameObject*);
	REGISTER_FIELD(inputPrefabHeavy, GameObject*);
}

void UIComboManager::Start()
{
	comboBar = owner->GetChildren()[0]->GetComponent<ComponentSlider>();

	inputPositions.push_back(owner->GetChildren()[1]->GetChildren()[0]);
	inputPositions.push_back(owner->GetChildren()[1]->GetChildren()[1]);
	inputPositions.push_back(owner->GetChildren()[1]->GetChildren()[2]);
}

void UIComboManager::Update(float deltaTime)
{
	if (clearCombo)
	{
		if (clearComboTimer <= 0.0f)
		{
			CleanInputVisuals();
			clearCombo = false;
		}
		else
		{
			clearComboTimer -= deltaTime;
		}
	}
}

void UIComboManager::SetActivateSpecial(bool activate) 
{
	if (activate) 
	{
		comboBar->GetFill()->GetComponent<ComponentImage>()->SetColor(float4(0.15,0.15,0,1));
	}
	else 
	{
		comboBar->GetFill()->GetComponent<ComponentImage>()->SetColor(float4(0, 0.5, 0, 1));
	}
}

void UIComboManager::SetComboBarValue(float value)
{
	comboBar->ModifyCurrentValue(value);
}

void UIComboManager::AddInputVisuals(InputVisualType type) 
{
	if (inputVisuals.size() < 3)
	{
		GameObject* prefab = nullptr;
		switch (type)
		{
		case InputVisualType::SOFT:
			prefab = inputPrefabSoft;
			break;
		case InputVisualType::HEAVY:
			prefab = inputPrefabHeavy;
			break;
		default:
			break;
		}
		
		GameObject* newInput = App->GetModule<ModuleScene>()->GetLoadedScene()->DuplicateGameObject(inputPrefabSoft->GetName(), inputPrefabSoft, inputPositions[0]);
		newInput->Enable();
		inputVisuals.push_front(newInput);
		for (int i = 1; i < inputVisuals.size(); i++)
		{
			inputVisuals[i]->SetParent(inputPositions[i]);
		}
	}
}

void UIComboManager::ClearCombo(bool finish) 
{
	clearCombo = true;
	if(finish)
	{
		clearComboTimer = 1;
		//Particles, audio, etc
	}
	else 
	{
		clearComboTimer = 0.1;
		//Particles, audio, etc
	}
}

void UIComboManager::CleanInputVisuals()
{
	for (GameObject* input : inputVisuals) 
	{
		App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(input);
	}
	inputVisuals.clear();
}