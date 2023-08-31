#include "MeleeFastAttackBehaviourScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"

#include "../Scripts/HealthSystem.h"

#include "Auxiliar/Audio/AudioData.h"

#include "SDL.h"

REGISTERCLASS(MeleeFastAttackBehaviourScript);

MeleeFastAttackBehaviourScript::MeleeFastAttackBehaviourScript() : Script(), attackCooldown(6.f), lastAttackTime(0.f),
	damageAttack(10.f), ownerTransform(nullptr), audioSource(nullptr)
{
	REGISTER_FIELD(attackCooldown, float);
	REGISTER_FIELD(damageAttack, float);
}

void MeleeFastAttackBehaviourScript::Start()
{
	ownerTransform = owner->GetComponent<ComponentTransform>();
	audioSource = owner->GetComponent<ComponentAudioSource>();
}

void MeleeFastAttackBehaviourScript::PerformAttack(GameObject* target)
{
	audioSource->PostEvent(AUDIO::SFX::PLAYER::WEAPON::LIGHTSABER_SWING);//Change sound
	lastAttackTime = SDL_GetTicks() / 1000.0f;
	HealthSystem* healthScript = target->GetRootGO()->GetComponent<HealthSystem>();
	healthScript->TakeDamage(damageAttack);
}

bool MeleeFastAttackBehaviourScript::IsAttackAvailable()
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown);
}

void MeleeFastAttackBehaviourScript::ResetScriptValues()
{
	lastAttackTime = 0.0f;
}