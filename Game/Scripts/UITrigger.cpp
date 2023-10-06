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

#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"
#include "Auxiliar/Audio/AudioData.h"

#include "UIGameManager.h"
#include "HealthSystem.h"
#include "SceneLoadingScript.h"
#include "Components/ComponentScript.h"

REGISTERCLASS(UITrigger);

UITrigger::UITrigger() : Script(),componentAudio(nullptr), activeState(ActiveActions::INACTIVE), setGameStateObject(nullptr),
UIGameManagerClass(nullptr), isLoseTrigger (nullptr), isWinTrigger(nullptr), isNextSceneTrigger(nullptr), isLoseByDamage(false), 
playerHealthSystem(nullptr), onTriggerState(false), damageTaken(1)
{
	REGISTER_FIELD(isWinTrigger, bool);
	REGISTER_FIELD(isLoseTrigger, bool);
	REGISTER_FIELD(isNextSceneTrigger, bool);
	REGISTER_FIELD(isLoseByDamage, bool);
	REGISTER_FIELD(setGameStateObject, GameObject*);
	REGISTER_FIELD(damageTaken, float);
	REGISTER_FIELD(loadingScreenScript, SceneLoadingScript*);
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

	if (setGameStateObject != nullptr)
	{
		UIGameManagerClass = setGameStateObject->GetComponent<UIGameManager>();
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
	}

	if(onTriggerState)
	{
		if (isWinTrigger)
		{
			LoadScene("Lib/Scenes/00_WinScene_VS3.axolotl");

		}
		else if (isLoseTrigger)
		{

		}
		else if (isLoseByDamage)
		{
			damageTimer += deltaTime;
			if (damageTimer >= timer)
			{
				timer++;
				playerHealthSystem->TakeDamage(damageTaken);
			}
			//UIGameManagerClass->LoseStateScene(true);
		}
		else if (isNextSceneTrigger)
		{
			App->GetModule<ModuleScene>()->LoadScene("Lib/Scenes/_LEVEL02_ALFA.axolotl");
		}
	}
}

void UITrigger::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player"))
	{
		onTriggerState = true;
	}
}

void UITrigger::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->CompareTag("Player"))
	{
		onTriggerState = false;
	}
}

void UITrigger::LoadScene(const std::string& sceneToLoadIfNoLoadingScreen)
{
	if (loadingScreenScript != nullptr)
	{
		loadingScreenScript->StartLoad();
	}
	else
	{
		App->GetModule<ModuleScene>()->SetSceneToLoad(sceneToLoadIfNoLoadingScreen);
	}
}
