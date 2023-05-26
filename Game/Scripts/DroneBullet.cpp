#include "DroneBullet.h"

#include "Application.h"

#include "ModuleInput.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Scripting/ScriptFactory.h"

#include "Physics/Physics.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentScript.h"

#include "HealthSystem.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

#include "Geometry/Ray.h"

#include "debugdraw.h"

REGISTERCLASS(DroneBullet);

DroneBullet::DroneBullet() : Script(), transform(nullptr), velocity(0.2f), audioSource(nullptr), bulletLifeTime(10.0f),
damageAttack(10.0f), rayAttackSize(10.0f)
{
}

void DroneBullet::Start()
{
	transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	audioSource = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));
}

void DroneBullet::Update(float deltaTime)
{
	ShootBullet(deltaTime);

	CheckCollision();
}

void DroneBullet::ShootBullet(float deltaTime)
{
	transform->SetPosition(transform->GetGlobalPosition() + transform->GetLocalForward() * velocity * deltaTime * 1000);
	transform->UpdateTransformMatrices();
}

void DroneBullet::CheckCollision()
{
	Ray ray(transform->GetPosition(), transform->GetLocalForward());
	LineSegment line(ray, rayAttackSize);
	RaycastHit hit;
	if (Physics::Raycast(line, hit, transform->GetOwner()))
	{
		if (hit.gameObject->CompareTag("Player"))
		{
			std::vector<ComponentScript*> gameObjectScripts =
				hit.gameObject->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);

			for (int i = 0; i < gameObjectScripts.size(); ++i)
			{
				if (gameObjectScripts[i]->GetConstructName() == "HealthSystem")
				{
					HealthSystem* healthScript = static_cast<HealthSystem*>(gameObjectScripts[i]->GetScript());
					healthScript->TakeDamage(damageAttack);
				}
			}
		}

		audioSource->PostEvent(audio::SFX_PLAYER_LIGHTSABER_CLASH);//Provisional sfx

		DestroyBullet();
	}
}

void DroneBullet::DestroyBullet()
{
	App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
}
