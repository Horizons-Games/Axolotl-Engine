#include "StdAfx.h"
#include "UIComboManager.h"

#include "Application.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentScript.h"
#include "Components/UI/ComponentSlider.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentTransform2D.h"

REGISTERCLASS(UIComboManager);

UIComboManager::UIComboManager() : Script(), clearComboTimer(0.0f), clearCombo(false), alphaActivated(false)
{
	REGISTER_FIELD(inputPrefabSoft, GameObject*);
	REGISTER_FIELD(inputPrefabHeavy, GameObject*);
	REGISTER_FIELD(noFillBar, GameObject*);
	REGISTER_FIELD(shinnyButton, GameObject*);
	REGISTER_FIELD(shinnyButton1, GameObject*);
	REGISTER_FIELD(shinnyButton2, GameObject*);
}

void UIComboManager::Init()
{
	comboBar = owner->GetChildren()[0]->GetComponent<ComponentSlider>();

	inputPositions.push_back(owner->GetChildren()[1]->GetChildren()[0]);
	inputPositions.push_back(owner->GetChildren()[1]->GetChildren()[1]);
	inputPositions.push_back(owner->GetChildren()[1]->GetChildren()[2]);
	
	if (noFillBar)
	{
		transparency = noFillBar->GetComponent<ComponentImage>()->GetColor().w / 255;
	}

	if (shinnyButton)
	{
		shinnyButton->GetComponent<ComponentImage>();
	}

	if (shinnyButton1)
	{
		shinnyButton1->GetComponent<ComponentImage>();
	}

	if (shinnyButton2)
	{
		shinnyButton2->GetComponent<ComponentImage>();
	}
}

void UIComboManager::Update(float deltaTime)
{
	if (clearCombo)
	{

		if (clearComboTimer <= 0.0f)
		{
			//los borra
			CleanInputVisuals();
			clearCombo = false;

			if (shinnyButton)
			{
				shinnyButton->Disable();
			}

			if (shinnyButton1)
			{
				shinnyButton1->Disable();
			}

			if (shinnyButton2)
			{
				shinnyButton2->Disable();
			}
		}
		else
		{

			if (clearComboTimer -= deltaTime)
			{
				for (int i = 0; i < inputVisuals.size(); i++)
				{
					inputVisuals[i]->GetComponent<ComponentTransform2D>()->SetSize(float2(370, 370));
				}

				if (shinnyButton)
				{
					shinnyButton->Enable();
				}

				if (shinnyButton1)
				{
					shinnyButton1->Enable();
				}

				if (shinnyButton2)
				{
					shinnyButton2->Enable();
				}
			}
			else
			{
				clearComboTimer += deltaTime;

				for (int i = 0; i < inputVisuals.size(); i++)
				{
					inputVisuals[i]->GetComponent<ComponentTransform2D>()->SetSize(float2(350, 350));
				}
			}
			clearComboTimer -= deltaTime;
		
		}
	}

	if (noFillBar && noFillBar->IsEnabled())
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

		if (noFillBar)
		{
			noFillBar->Disable();
		}
	}

	else if (value >= GetMaxComboBarValue() && noFillBar)
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
		clearComboTimer = 1.0f;
		
		//Particles, audio, etc
	}
	else 
	{
		clearComboTimer = 0.5f;
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

void UIComboManager::UpdateFadeOut(float transparency)
{
	for (int i = 0; i < inputVisuals.size(); i++)
	{
		inputVisuals[i]->GetComponent<ComponentImage>()->SetColor(float4(1, 1, 1, transparency));
	}
}
