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
finisherClearEffect(false), specialActivated(false)
{
	REGISTER_FIELD(inputPrefabSoft, GameObject*);
	REGISTER_FIELD(inputPrefabHeavy, GameObject*);
	REGISTER_FIELD(inputPrefabJumpAttack, GameObject*);
	REGISTER_FIELD(inputPrefabLightFinisherAttack, GameObject*);
	REGISTER_FIELD(inputPrefabHeavyFinisherAttack, GameObject*);
	REGISTER_FIELD(noFillBar, GameObject*);
	REGISTER_FIELD(shinyEffectPrefab, GameObject*);
	REGISTER_FIELD(shinyEffectBarPrefab, GameObject*);

	REGISTER_FIELD(specialBox.text, GameObject*);
	REGISTER_FIELD(specialBox.lb, GameObject*);
	REGISTER_FIELD(specialBox.separator, GameObject*);
	REGISTER_FIELD(specialBox.rb, GameObject*);
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

	GameObject* specialBoxGO = owner->GetChildren()[2];
	specialBoxGO->Disable();
	specialBox.text = specialBoxGO->GetChildren()[1];
	GameObject* buttonsGO = specialBoxGO->GetChildren()[2];
	specialBox.lb = buttonsGO->GetChildren()[0];
	specialBox.separator = buttonsGO->GetChildren()[1];
	specialBox.rb = buttonsGO->GetChildren()[2];
}

void UIComboManager::Update(float deltaTime)
{
	if (noFillBar && noFillBar->IsEnabled() && !finisherClearEffect)
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
		float4 newColor = float4(1.f, 1.f, 1.f, transparency);
		noFillBar->GetComponent<ComponentImage>()->SetColor(newColor);

		specialBox.text->GetChildren()[0]->GetComponent<ComponentImage>()->SetColor(newColor);
		specialBox.lb->GetChildren()[0]->GetComponent<ComponentImage>()->SetColor(newColor);
		specialBox.separator->GetChildren()[0]->GetComponent<ComponentImage>()->SetColor(newColor);
		specialBox.rb->GetChildren()[0]->GetComponent<ComponentImage>()->SetColor(newColor);

		ComponentTransform2D* transformText = specialBox.text->GetComponent<ComponentTransform2D>();
		ComponentTransform2D* transformButtons = specialBox.lb->GetParent()->GetComponent<ComponentTransform2D>();
		float growVelocity = (transparency - 0.5f) / 10.0f;
		float3 sizeText = float3(1.0f + growVelocity, 1.0f + growVelocity, 1.0f);
		float3 sizeButton = float3(1.0f + growVelocity, 1.0f + growVelocity, 1.0f);
		transformText->SetScale(sizeText);
		transformButtons->SetScale(sizeButton);
		transformText->CalculateMatrices();
		transformButtons->CalculateMatrices();
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
				if(clearComboTimer < 0.9f) finisherClearEffect = false;
			}
			clearComboTimer -= deltaTime;
		}
	}

	for (auto it = shinyButtonEffect.begin(); it != shinyButtonEffect.end();)
	{
		ComponentImage* image = (*it)->GetComponent<ComponentImage>();
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

	for (auto it = shinyBarEffect.begin(); it != shinyBarEffect.end();)
	{
		ComponentImage* image = (*it)->GetComponent<ComponentImage>();
		float4 color = image->GetColor();
		if (color.w <= 0.0f)
		{
			App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(*it);
			it = shinyBarEffect.erase(it);
			continue;
		}
		else
		{
			color.w -= deltaTime * 2;
			image->SetColor(color);
		}

		ComponentTransform2D* transform = (*it)->GetComponent<ComponentTransform2D>();
		float2 size = transform->GetSize();
		size.x += deltaTime * 120;
		size.y += deltaTime * 40;
		transform->SetSize(size);
		++it;
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

		specialBox.text->GetParent()->Enable();

		GameObject* newShinyBarEffect =
			App->GetModule<ModuleScene>()->GetLoadedScene()->
			DuplicateGameObject(shinyEffectBarPrefab->GetName(), shinyEffectBarPrefab, shinyEffectBarPrefab->GetParent());
		newShinyBarEffect->Enable();
		newShinyBarEffect->GetComponent<ComponentTransform2D>()->SetSize(float2(115.f, 827.f));
		shinyBarEffect.push_back(newShinyBarEffect);
	}
	else 
	{
		comboBar->GetOwner()->GetChildren()[1]->Disable();
		newVisuals = comboBar->GetOwner()->GetChildren()[0];
		specialBox.text->GetParent()->Disable();

		float4 newColor = float4(1.f, 1.f, 1.f, 0.5f);
		specialBox.text->GetChildren()[0]->GetComponent<ComponentImage>()->SetColor(newColor);
		specialBox.lb->GetChildren()[0]->GetComponent<ComponentImage>()->SetColor(newColor);
		specialBox.separator->GetChildren()[0]->GetComponent<ComponentImage>()->SetColor(newColor);
		specialBox.rb->GetChildren()[0]->GetComponent<ComponentImage>()->SetColor(newColor);

		ComponentTransform2D* transformText = specialBox.text->GetComponent<ComponentTransform2D>();
		ComponentTransform2D* transformButtons = specialBox.lb->GetParent()->GetComponent<ComponentTransform2D>();
		float3 size = float3(1.0f, 1.0f, 1.0f);
		transformText->SetScale(size);
		transformButtons->SetScale(size);
		transformText->CalculateMatrices();
		transformButtons->CalculateMatrices();
	}

	newVisuals->Enable();
	comboBar->SetBackground(newVisuals->GetChildren()[0]);
	comboBar->SetFill(newVisuals->GetChildren()[1], static_cast<int> (comboBar->GetDirection()));

	noFillBar = newVisuals->GetChildren()[2];
	noFillBar->GetComponent<ComponentImage>()->SetColor(float4(1.f, 1.f, 1.f, transparency));
}

void UIComboManager::SetComboBarValue(float value)
{
	comboBar->ModifyCurrentValue(value);
	if (value == 0.0f)
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
		clearComboTimer = 0.0f;
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

	if (specialActivated)
	{
		GameObject* newShinyBarEffect =
			App->GetModule<ModuleScene>()->GetLoadedScene()->
			DuplicateGameObject(shinyEffectBarPrefab->GetName(), shinyEffectBarPrefab, shinyEffectBarPrefab->GetParent());
		newShinyBarEffect->Enable();
		newShinyBarEffect->GetComponent<ComponentTransform2D>()->SetSize(float2(115.f, 827.f));
		shinyBarEffect.push_back(newShinyBarEffect);

		float4 newColor = float4(1.f, 1.f, 1.f, 1.0f);
		transparency = 1.0f;
		noFillBar->GetComponent<ComponentImage>()->SetColor(newColor);
		specialBox.text->GetChildren()[0]->GetComponent<ComponentImage>()->SetColor(newColor);
		specialBox.lb->GetChildren()[0]->GetComponent<ComponentImage>()->SetColor(newColor);
		specialBox.separator->GetChildren()[0]->GetComponent<ComponentImage>()->SetColor(newColor);
		specialBox.rb->GetChildren()[0]->GetComponent<ComponentImage>()->SetColor(newColor);

		ComponentTransform2D* transformText = specialBox.text->GetComponent<ComponentTransform2D>();
		ComponentTransform2D* transformButtons = specialBox.lb->GetParent()->GetComponent<ComponentTransform2D>();
		float3 sizeText = float3(1.05f, 1.05f, 1.0f);
		float3 sizeButton = float3(1.05f, 1.05f, 1.0f);
		transformText->SetScale(sizeText);
		transformButtons->SetScale(sizeButton);
		transformText->CalculateMatrices();
		transformButtons->CalculateMatrices();
	}
}

bool UIComboManager::IsInCombat() 
{
	return owner->GetChildren()[0]->IsEnabled();
}

void UIComboManager::InitComboUI() 
{
	owner->GetChildren()[0]->Enable();
	owner->GetChildren()[1]->Enable();
}

void UIComboManager::HideComboUI() 
{
	owner->GetChildren()[0]->Disable();
	owner->GetChildren()[1]->Disable();

	clearComboTimer = 0.0f;
	alphaEnabled = false;

	noFillBar->GetComponent<ComponentImage>()->SetColor(float4(1.f, 1.f, 1.f, 0.5f));

	for (GameObject* shiny : shinyButtonEffect) 
	{
		App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(shiny);
	}
	shinyButtonEffect.clear();

	for (GameObject* shiny : shinyBarEffect)
	{
		App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(shiny);
	}
	shinyBarEffect.clear();

	CleanInputVisuals();
	SetComboBarValue(0.0f);
}