#include "StdAfx.h"
#include "UIComboManager.h"

#include "Application.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentScript.h"
#include "Components/UI/ComponentSlider.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentTransform2D.h"
#include "ModulePlayer.h"
#include "CameraControllerScript.h"


REGISTERCLASS(UIComboManager);

UIComboManager::UIComboManager() : Script(), clearComboTimer(0.0f), clearCombo(false), alphaEnabled(false), isEffectEnabled(-1)
{
	REGISTER_FIELD(inputPrefabSoft, GameObject*);
	REGISTER_FIELD(inputPrefabHeavy, GameObject*);
	REGISTER_FIELD(inputPrefabJumpAttack, GameObject*);
	REGISTER_FIELD(noFillBar, GameObject*);
	REGISTER_FIELD(shinyButtonEffect, std::vector<GameObject*>);
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
	if (App->GetModule<ModulePlayer>()->GetCameraPlayerObject()->GetComponent<CameraControllerScript>()->IsInCombat() 
		&& !owner->GetChildren()[0]->IsEnabled())
	{
		owner->GetChildren()[0]->Enable();
		owner->GetChildren()[1]->Enable();
	}
	else if (!App->GetModule<ModulePlayer>()->GetCameraPlayerObject()->GetComponent<CameraControllerScript>()->IsInCombat())
	{
		if (owner->GetChildren()[0]->IsEnabled())
		{
			owner->GetChildren()[0]->Disable();
			owner->GetChildren()[1]->Disable();
		}
		return;
	}

	if (clearCombo)
	{

		if (clearComboTimer <= 0.0f)
		{
			CleanInputVisuals();
			clearCombo = false;
			isEffectEnabled = -1;
		}
		else if (isEffectEnabled != -1)
		{
			if (isEffectEnabled)
			{
				for (int i = 0; i < inputVisuals.size(); i++)
				{
					inputVisuals[i]->GetComponent<ComponentTransform2D>()->SetSize(float2(106.f, 106.f));
				}

				for (GameObject* shinyButton : shinyButtonEffect)
				{
					if (shinyButton)
					{
						shinyButton->GetComponent<ComponentTransform2D>()->SetSize(float2(146.f, 149.f));
					}
				}

				isEffectEnabled = false;
			}
			else
			{
				FinishComboButtonsEffect();
				isEffectEnabled = true;
			}
			clearComboTimer -= 0.05f;
		}
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

			if (transparency >= 1.0f)
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
			noFillBar->GetComponent<ComponentImage>()->SetColor(float4(1.f, 1.f, 1.f, 0.5f));
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
			prefab = inputPrefabJumpAttack;
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

	for (GameObject* shinyButton : shinyButtonEffect)
	{
		if (shinyButton)
		{
			shinyButton->Disable();
		}
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

void UIComboManager::FinishComboButtonsEffect() //Make a VFX when you get a full combo
{
	for (int i = 0; i < inputVisuals.size(); i++)
	{
		inputVisuals[i]->GetComponent<ComponentTransform2D>()->SetSize(float2(111.f, 111.f));
	}

	if (inputVisuals.size() >= 3)
	{
		for (GameObject* shinyButton : shinyButtonEffect)
		{
			if (shinyButton)
			{
				shinyButton->Enable();
				shinyButton->GetComponent<ComponentTransform2D>()->SetSize(float2(151.f, 154.f));
			}
		}
	}
}
