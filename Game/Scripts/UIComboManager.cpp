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

UIComboManager::UIComboManager() : Script(), clearComboTimer(0.0f), clearCombo(false), alphaEnabled(false)
{
	REGISTER_FIELD(inputPrefabSoft, GameObject*);
	REGISTER_FIELD(inputPrefabHeavy, GameObject*);
	REGISTER_FIELD(noFillBar, GameObject*);
	REGISTER_FIELD(comboButtons, std::vector<GameObject*>);
}

void UIComboManager::Init()
{
	comboBar = owner->GetChildren()[0]->GetComponent<ComponentSlider>();

	inputPositions.push_back(owner->GetChildren()[1]->GetChildren()[0]);
	inputPositions.push_back(owner->GetChildren()[1]->GetChildren()[1]);
	inputPositions.push_back(owner->GetChildren()[1]->GetChildren()[2]);
	
	if (noFillBar)
	{
		transparency = noFillBar->GetComponent<ComponentImage>()->GetColor().w;
	}
}

void UIComboManager::Update(float deltaTime)
{
	if (!clearCombo)
	{
		return;
	}

	if (clearComboTimer <= 0.0f)
	{
		CleanInputVisuals();
		clearCombo = false;

		for (GameObject* comboButton : comboButtons)
		{
			if (comboButton)
			{
				comboButton->Disable();
			}
		}
	}
	else
	{

		if (clearComboTimer -= deltaTime)
		{
			for (int i = 0; i < inputVisuals.size(); i++)
			{
				inputVisuals[i]->GetComponent<ComponentTransform2D>()->SetSize(float2(370.f, 370.f));
			}

			for (GameObject* comboButton : comboButtons)
			{
				if (comboButton)
				{
					comboButton->Enable();
				}
			}
		}
		else
		{
			clearComboTimer += deltaTime;

			for (int i = 0; i < inputVisuals.size(); i++)
			{
				inputVisuals[i]->GetComponent<ComponentTransform2D>()->SetSize(float2(350.f, 350.f));
			}
		}
		clearComboTimer -= deltaTime;
	}

	if (noFillBar && noFillBar->IsEnabled())
	{
		if (alphaEnabled)
		{
			transparency -= deltaTime;

			if (transparency <= 0.5f)
			{
				transparency = 0.5f;
				alphaEnabled = false;
			}
		}
		else
		{
			transparency += deltaTime;

			if (transparency <= 1.0f)
			{
				transparency = 1.0f;
				alphaEnabled = true;
			}
		}
		
		noFillBar->GetComponent<ComponentImage>()->SetColor(float4(1.f, 1.f, 1.f, transparency));
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
		comboBar->GetFill()->GetComponent<ComponentImage>()->SetColor(float4::one);
		comboBar->GetBackground()->GetComponent<ComponentImage>()->SetColor(float4::one);
	}
	else 
	{
		comboBar->GetFill()->GetComponent<ComponentImage>()->SetColor(float4::one);
		comboBar->GetBackground()->GetComponent<ComponentImage>()->SetColor(float4::one);
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
		case InputVisualType::JUMP:
			AXO_TODO("Add here the CORRECT input prefab for the jump");
			prefab = inputPrefabSoft;
			break;
		default:
			return;
		}

		GameObject* newInput = 
			App->GetModule<ModuleScene>()->GetLoadedScene()->
			DuplicateGameObject(prefab->GetName(), prefab, inputPositions[0]);
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
		inputVisuals[i]->GetComponent<ComponentImage>()->SetColor(float4(1.f, 1.f, 1.f, transparency));
	}
}
