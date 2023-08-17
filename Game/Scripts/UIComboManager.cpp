#include "StdAfx.h"
#include "UIComboManager.h"

#include "Components/ComponentScript.h"
#include "Components/UI/ComponentSlider.h"
#include "Components/UI/ComponentImage.h"

#include "Application.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

REGISTERCLASS(UIComboManager);

UIComboManager::UIComboManager() : Script(), clearComboTimer(0.0f), clearCombo(false), alphaActivated(false)
{
	REGISTER_FIELD(inputPrefabSoft, GameObject*);
	REGISTER_FIELD(inputPrefabHeavy, GameObject*);
	REGISTER_FIELD(noFillBar, GameObject*);
}

void UIComboManager::Init()
{
	comboBar = owner->GetChildren()[0]->GetComponent<ComponentSlider>();

	inputPositions.push_back(owner->GetChildren()[1]->GetChildren()[0]);
	inputPositions.push_back(owner->GetChildren()[1]->GetChildren()[1]);
	inputPositions.push_back(owner->GetChildren()[1]->GetChildren()[2]);
	
	transparency = noFillBar->GetComponent<ComponentImage>()->GetColor().w/255;

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

	if (noFillBar->IsEnabled())
	{

		if (alphaActivated)
		{
			transparency -= deltaTime;

			if (transparency <= 0.5)
			{
				transparency = 0.5;
				alphaActivated = false;
			}
		}
		else
		{
			transparency += deltaTime;

			if (transparency <= 1.0)
			{
				transparency = 1.0;
				alphaActivated = true;
			}
		}

		noFillBar->GetComponent<ComponentImage>()->SetColor(float4(1, 1, 1, transparency));

	}

}

float UIComboManager::GetMaxComboBarValue() const
{
	return comboBar->GetMaxValue();
}

void UIComboManager::SetActivateSpecial(bool activate) 
{
	if (activate) 
	{
		comboBar->GetFill()->GetComponent<ComponentImage>()->SetColor(float4(1, 1, 1, 1));
		comboBar->GetBackground()->GetComponent<ComponentImage>()->SetColor(float4(1, 1, 1, 1));
	}
	else 
	{
		comboBar->GetFill()->GetComponent<ComponentImage>()->SetColor(float4(1, 1, 1, 1));
		comboBar->GetBackground()->GetComponent<ComponentImage>()->SetColor(float4(1, 1, 1, 1));
	}
}

void UIComboManager::SetComboBarValue(float value)
{
	comboBar->ModifyCurrentValue(value);
	if (value == 0)
	{
		SetActivateSpecial(false);
	}

	else if (value >= GetMaxComboBarValue())
	{
		noFillBar->Enable();
	}

}

void UIComboManager::AddInputVisuals(InputVisualType type) 
{
	if (inputVisuals.size() < 3)
	{
		GameObject* prefab = nullptr;
		switch (type)
		{
		case InputVisualType::LIGHT:
			prefab = inputPrefabSoft;
			break;
		case InputVisualType::HEAVY:
			prefab = inputPrefabHeavy;
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

void UIComboManager::ClearCombo(bool finish) 
{
	clearCombo = true;
	if(finish)
	{
		clearComboTimer = 0.5f;
		
		//Particles, audio, etc
	}
	else 
	{
		clearComboTimer = 0.1f;
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