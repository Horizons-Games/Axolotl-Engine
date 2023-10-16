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

UIComboManager::UIComboManager() : Script(), clearComboTimer(0.0f), clearCombo(false), alphaEnabled(false), 
finisherClearEffect(false), forceEnableComboBar(false), specialActivated(false)
{
	REGISTER_FIELD(inputPrefabSoft, GameObject*);
	REGISTER_FIELD(inputPrefabHeavy, GameObject*);
	REGISTER_FIELD(inputPrefabJumpAttack, GameObject*);
	REGISTER_FIELD(inputPrefabLightFinisherAttack, GameObject*);
	REGISTER_FIELD(inputPrefabHeavyFinisherAttack, GameObject*);
	REGISTER_FIELD(noFillBar, GameObject*);
	REGISTER_FIELD(shinyEffectPrefab, GameObject*);
	REGISTER_FIELD(shinyEffectBarPrefab, GameObject*);
	REGISTER_FIELD(forceEnableComboBar, bool);
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
	if (forceEnableComboBar)
	{
		owner->GetChildren()[0]->Enable();
		owner->GetChildren()[1]->Enable();
	}
	else
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
				CleanInputVisuals();
				clearCombo = false;
				owner->GetChildren()[0]->Disable();
				owner->GetChildren()[1]->Disable();
			}
			return;
		}
	}

	if (clearCombo)
	{
		if (clearComboTimer <= 0.0f)
		{
			CleanInputVisuals();
		}
		else
		{
			if (finisherClearEffect) 
			{
				for (int i = 0; i < inputVisuals.size(); ++i)
				{
					inputVisuals[i]->GetComponent<ComponentTransform2D>()->SetSize(float2(106.f, 106.f));
				}
			}
			clearComboTimer -= deltaTime;
		}
	}

	for (auto it = shinyButtonEffect.begin(); it != shinyButtonEffect.end();)
	{
		ComponentImage* image = (*it)->GetComponentInternal<ComponentImage>();
		float4 color = image->GetColor();
		if (color.w <= 0.0f) 
		{
			App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(*it);
			it = shinyButtonEffect.erase(it);
			continue;
		}
		else
		{
			color.w -= deltaTime * 2;
			image->SetColor(color);
		}

		ComponentTransform2D* transform = (*it)->GetComponent<ComponentTransform2D>();
		float2 size = transform->GetSize();
		size = size.Add(deltaTime * 120);
		transform->SetSize(size);
		++it;
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
	specialActivated = activate;
	GameObject* newVisuals;
	if (activate) 
	{
		comboBar->GetOwner()->GetChildren()[0]->Disable();
		newVisuals = comboBar->GetOwner()->GetChildren()[1];
		
		for (int i = 0; i < inputVisuals.size(); ++i)
		{
			inputVisuals[i]->GetChildren()[0]->Disable();
			inputVisuals[i]->GetChildren()[1]->Enable();
		}
	}
	else 
	{
		comboBar->GetOwner()->GetChildren()[1]->Disable();
		newVisuals = comboBar->GetOwner()->GetChildren()[0];
	}

	newVisuals->Enable();
	comboBar->SetBackground(newVisuals->GetChildren()[0]);
	comboBar->SetFill(newVisuals->GetChildren()[1]);

	noFillBar = newVisuals->GetChildren()[2];
	noFillBar->GetComponent<ComponentImage>()->SetColor(float4(1.f, 1.f, 1.f, transparency));
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

void UIComboManager::AddInputVisuals(AttackType type)
{
	if (clearCombo) 
	{
		CleanInputVisuals();
	}

	GameObject* prefab = nullptr;
	switch (type)
	{
	case AttackType::LIGHTNORMAL:
		prefab = inputPrefabSoft;
		break;
	case AttackType::HEAVYNORMAL:
		prefab = inputPrefabHeavy;
		break;
	case AttackType::JUMPNORMAL:
	case AttackType::JUMPFINISHER:
		prefab = inputPrefabJumpAttack;
		break;
	case AttackType::LIGHTFINISHER:
		prefab = inputPrefabLightFinisherAttack;
		break;
	case AttackType::HEAVYFINISHER:
		prefab = inputPrefabHeavyFinisherAttack;
		break;
	default:
		return;
	}

	GameObject* newInput = 
		App->GetModule<ModuleScene>()->GetLoadedScene()->
		DuplicateGameObject(prefab->GetName(), prefab, inputPositions[0]);
	newInput->Enable();

	specialActivated? newInput->GetChildren()[0]->Disable() : newInput->GetChildren()[1]->Disable();

	inputVisuals.push_front(newInput);
	for (int i = 1; i < inputVisuals.size(); ++i)
	{
		inputVisuals[i]->SetParent(inputPositions[i]);
	}
}

void UIComboManager::ClearCombo(bool finish) 
{
	clearCombo = true;

	if(finish)
	{
		clearComboTimer = 1.0f;
		InitFinishComboButtonsEffect();
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
	clearCombo = false;
	finisherClearEffect = false;

	for (GameObject* input : inputVisuals) 
	{
		App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(input);
	}
	inputVisuals.clear();
}

void UIComboManager::UpdateFadeOut(float transparency)
{
	for (int i = 0; i < inputVisuals.size(); ++i)
	{
		GameObject* visual = specialActivated ? inputVisuals[i]->GetChildren()[1] : inputVisuals[i]->GetChildren()[0];
		visual->GetComponent<ComponentImage>()->SetColor(float4(1.f, 1.f, 1.f, transparency));
	}
}

void UIComboManager::InitFinishComboButtonsEffect() //Make a VFX when you get a full combo
{
	finisherClearEffect = true;
	for (int i = 0; i < inputVisuals.size(); ++i)
	{
		inputVisuals[i]->GetComponent<ComponentTransform2D>()->SetSize(float2(111.f, 111.f));

		GameObject* prefab = specialActivated ? shinyEffectPrefab->GetChildren()[1] : shinyEffectPrefab->GetChildren()[0];

		GameObject* newShinyEffect =
			App->GetModule<ModuleScene>()->GetLoadedScene()->
			DuplicateGameObject(prefab->GetName(), prefab, inputPositions[i]);
		newShinyEffect->Enable();
		newShinyEffect->GetComponent<ComponentTransform2D>()->SetSize(float2(154.f, 154.f));
		shinyButtonEffect.push_back(newShinyEffect);
	}
}

bool UIComboManager::IsCombatActive()
{
	return forceEnableComboBar;
}
