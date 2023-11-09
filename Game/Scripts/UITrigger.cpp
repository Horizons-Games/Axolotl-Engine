#include "StdAfx.h"
#include "UITrigger.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "ModulePlayer.h"

#include "Scene/Scene.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentPlayer.h"
#include "Components/UI/ComponentVideo.h"

#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"
#include "Auxiliar/Audio/AudioData.h"

#include "UIGameManager.h"
#include "HealthSystem.h"
#include "SceneLoadingScript.h"
#include "HackZoneScript.h"
#include "Components/ComponentScript.h"

REGISTERCLASS(UITrigger);

UITrigger::UITrigger() : Script(), componentAudio(nullptr), activeState(ActiveActions::INACTIVE), setUiGameManager(nullptr),
uiGameManagerClass(nullptr), isLoseTrigger(nullptr), isWinTrigger(nullptr), isNextSceneTrigger(nullptr), isLoseByDamage(false),
playerHealthSystem(nullptr), onTriggerState(false), damageTaken(1), setLoadingScreenImage(nullptr), noRestrictions(nullptr),
timer(0.0f), timerImg(2.0f), damageTimer(0.0f)
{
	REGISTER_FIELD(isLoseTrigger, bool);
	REGISTER_FIELD(setUiGameManager, GameObject*);
	REGISTER_FIELD(isLoseByDamage, bool);
	REGISTER_FIELD(damageTaken, float);
	REGISTER_FIELD(isNextSceneTrigger, bool);
	REGISTER_FIELD(noRestrictions, bool);
	REGISTER_FIELD(setLoadingScreenImage, GameObject*);
	REGISTER_FIELD(loadingScreenScript, SceneLoadingScript*);
	REGISTER_FIELD(hackZoneScript, HackZoneScript*);
}

UITrigger::~UITrigger()
{
}

void UITrigger::Start()
{
	setPlayer = App->GetModule<ModulePlayer>()->GetPlayer();
	modulePlayer = App->GetModule<ModulePlayer>();
	//componentAudio = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));
	//componentAnimation = static_cast<ComponentAnimation*>(owner->GetComponent(ComponentType::ANIMATION));
	componentRigidBody = owner->GetComponent<ComponentRigidBody>();

	if (setUiGameManager)
	{
		uiGameManagerClass = setUiGameManager->GetComponent<UIGameManager>();
	}

	if (isLoseByDamage)
	{
		playerHealthSystem = setPlayer->GetComponent<HealthSystem>();
	}
}

void UITrigger::Update(float deltaTime)
{
	if (setPlayer != modulePlayer->GetPlayer())
	{
		setPlayer = App->GetModule<ModulePlayer>()->GetPlayer();
		if (isLoseByDamage)
		{
			playerHealthSystem = setPlayer->GetComponent<HealthSystem>();
		}
	}

	if(onTriggerState)
	{
		if (isLoseTrigger)
		{
			uiGameManagerClass->LoseGameState(0.0f);
			return;
		}
		else if (isLoseByDamage)
		{
			damageTimer += deltaTime;
			if (damageTimer >= timer)
			{
				timer++;
				playerHealthSystem->TakeDamage(damageTaken);
			}
		}
		else if (isNextSceneTrigger)
		{
			LoadScene(deltaTime);	
		}
	}
}

void UITrigger::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player") || other->GetOwner()->CompareTag("PlayerSpaceship"))
	{
		onTriggerState = true;
	}
}

void UITrigger::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player") || other->GetOwner()->CompareTag("PlayerSpaceship"))
	{
		onTriggerState = false;
	}
}

void UITrigger::LoadScene(float deltaTime)
{
	if (timerImg <= 0.0f)
	{
		timerImg = 2.0f;
	}
	else
	{
		timerImg -= deltaTime;
		return;
	}

	if (hackZoneScript->IsCompleted() || !hackZoneScript && noRestrictions)
	{
		if (setLoadingScreenImage)
		{
			setLoadingScreenImage->Enable();
		}

		if (loadingScreenScript)
		{
			LOG_INFO("STARTING LOAD SCRIPT");
#ifndef ENGINE
			loadingScreenScript->StartLoad();
#endif // 
		}
	}
}

void UITrigger::SetNextSceneTrigger(bool isEnable)
{
	isNextSceneTrigger = isEnable;
}

bool UITrigger::IsNextSceneTriggerEnable() const
{
	return isNextSceneTrigger;
}
