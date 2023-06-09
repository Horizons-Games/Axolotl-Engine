#include "UITrigger.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentPlayer.h"

#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"
#include "Auxiliar/Audio/AudioData.h"

#include "UIGameStates.h"
#include "HealthSystem.h"
#include "Components/ComponentScript.h"

REGISTERCLASS(UITrigger);

UITrigger::UITrigger() : Script(),componentAudio(nullptr), activeState(ActiveActions::INACTIVE), setGameStateObject(nullptr),
uiGameStatesClass(nullptr), isLoseTrigger (nullptr), isWinTrigger(nullptr), isNextSceneTrigger(nullptr), isLoseByDamage(false),
playerHealthSystem(nullptr), setPlayer(nullptr)
{
	REGISTER_FIELD(isWinTrigger, bool);
	REGISTER_FIELD(isLoseTrigger, bool);
	REGISTER_FIELD(isNextSceneTrigger, bool);
	REGISTER_FIELD(isLoseByDamage, bool);
	REGISTER_FIELD(setGameStateObject, GameObject*);
	REGISTER_FIELD(setPlayer, GameObject*);
}

UITrigger::~UITrigger()
{
}

void UITrigger::Start()
{
	//componentAudio = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));
	//componentAnimation = static_cast<ComponentAnimation*>(owner->GetComponent(ComponentType::ANIMATION));
	componentRigidBody = owner->GetComponent<ComponentRigidBody>();

	if (setGameStateObject != nullptr)
	{
		uiGameStatesClass = setGameStateObject->GetComponent<UIGameStates>();
	}

	if (isLoseByDamage != false)
	{
		playerHealthSystem = setPlayer->GetComponent<HealthSystem>();
	}
}

void UITrigger::Update(float deltaTime)
{

}

void UITrigger::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetComponent<ComponentPlayer>())
	{
		if (isWinTrigger == true)
		{
			uiGameStatesClass->WinStateScene(true);
		}
		
		if (isLoseTrigger == true)
		{
			uiGameStatesClass->LoseStateScene(true);
		}
		if (isLoseByDamage == true)
		{
			playerHealthSystem->TakeDamage(100);
			//uiGameStatesClass->LoseStateScene(true);
		}
	}
}

void UITrigger::OnCollisionExit(ComponentRigidBody* other)
{

}