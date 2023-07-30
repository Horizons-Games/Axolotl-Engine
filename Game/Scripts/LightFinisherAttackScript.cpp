#include "StdAfx.h"
#include "LightFinisherAttackScript.h"

#include "Application.h"

#include "Scripting/ScriptFactory.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentScript.h"

#include "../Scripts/LightAttackBullet.h"
#include "../Scripts/EntityDetection.h"

REGISTERCLASS(LightFinisherAttackScript);

LightFinisherAttackScript::LightFinisherAttackScript() 
	: Script(), 
	loadedScene(nullptr), 
	bulletPrefab(nullptr),
	bulletVelocity(15.0f),
	stunTime(2.0f),
	enemyDetection(nullptr),
	enemyDetectionObject(nullptr),
	currentCooldown(0.0f),
	cooldown(2.0f)
{
	REGISTER_FIELD(bulletPrefab, GameObject*);
	REGISTER_FIELD(bulletVelocity, float);
	REGISTER_FIELD(stunTime, float);
	REGISTER_FIELD(enemyDetectionObject, GameObject*);
}

void LightFinisherAttackScript::Start()
{
	enemyDetection = enemyDetectionObject->GetComponent<EntityDetection>();
	loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();
}

void LightFinisherAttackScript::Update(float deltaTime)
{
	if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_N) != KeyState::IDLE && currentCooldown <= 0) // Bix jump finisher
	{
		if(PerformAttack())
			currentCooldown = cooldown;
	}
	else
	{
		if (currentCooldown > 0)
		{
			currentCooldown -= deltaTime;
			currentCooldown = std::max(0.0f, currentCooldown);
		}
	}
}

bool LightFinisherAttackScript::PerformAttack()
{
	if (!enemyDetection->GetEnemySelected())
	{
		return false;
	}
	
	LOG_DEBUG("Bullet shot");
	//animation->SetParameter("IsAttacking", true);

	// Create a new bullet
	GameObject* bullet = loadedScene->DuplicateGameObject(bulletPrefab->GetName(), bulletPrefab, owner);

	// Attack the DroneFastBullet script to the new bullet to give it its logic
	ComponentScript* script = bullet->CreateComponent<ComponentScript>();
	script->SetScript(App->GetScriptFactory()->ConstructScript("LightAttackBullet"));
	script->SetConstuctor("LightAttackBullet");
	script->GetScript()->SetOwner(bullet);
	script->GetScript()->SetApplication(App);

	bullet->GetComponent<LightAttackBullet>()->SetBulletVelocity(bulletVelocity);
	bullet->GetComponent<LightAttackBullet>()->SetEnemy(enemyDetection->GetEnemySelected());
	bullet->GetComponent<LightAttackBullet>()->SetStunTime(stunTime);

	// Once the engine automatically runs the Start() for newly created objects, delete this line
	script->Start();

	//audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT_01);

	return true;
}
