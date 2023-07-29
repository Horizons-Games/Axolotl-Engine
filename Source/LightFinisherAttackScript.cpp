#include "StdAfx.h"
#include "LightFinisherAttackScript.h"

#include "Application.h"

#include "Scripting/ScriptFactory.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "Components/ComponentScript.h"

LightFinisherAttackScript::LightFinisherAttackScript()
{
	REGISTER_FIELD(bulletPrefab, GameObject*);
	REGISTER_FIELD(bulletVelocity, float);
}

void LightFinisherAttackScript::Start()
{
	loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();
}

void LightFinisherAttackScript::PerformAttack()
{
	//animation->SetParameter("IsAttacking", true);

	// Create a new bullet
	GameObject* bullet = loadedScene->DuplicateGameObject(bulletPrefab->GetName(), bulletPrefab, owner);

	// Attack the DroneFastBullet script to the new bullet to give it its logic
	ComponentScript* script = bullet->CreateComponent<ComponentScript>();
	script->SetScript(App->GetScriptFactory()->ConstructScript("RangedFastAttackBullet"));
	script->SetConstuctor("RangedFastAttackBullet");
	script->GetScript()->SetOwner(bullet);
	script->GetScript()->SetApplication(App);

	//bullet->GetComponent<RangedFastAttackBullet>()->SetBulletVelocity(bulletVelocity);

	// Once the engine automatically runs the Start() for newly created objects, delete this line
	script->Start();

	//audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT_01);
}
