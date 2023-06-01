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
damageAttack(10.0f), rayAttackSize(10.0f), originTime(0.0f)
{
}

void DroneBullet::Start()
{
	transform = owner->GetComponent<ComponentTransform>();
	audioSource = owner->GetComponent<ComponentAudioSource>();

	originTime = SDL_GetTicks() / 1000.0f;
}

void DroneBullet::Update(float deltaTime)
{
#ifdef DEBUG

	Ray rayDebug(transform->GetPosition(), transform->GetLocalForward());
	dd::arrow(rayDebug.pos, rayDebug.pos + rayDebug.dir * rayAttackSize, dd::colors::Red, 0.05f);
	
#endif // DEBUG

	ShootBullet(deltaTime);

	CheckCollision();

	if (SDL_GetTicks() / 1000.0f > originTime + bulletLifeTime)
	{
		DestroyBullet();
	}
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
		if (hit.gameObject->GetRootGO())
		{
			if (hit.gameObject->GetRootGO()->CompareTag("Player"))
			{
				std::vector<ComponentScript*> gameObjectScripts =
					hit.gameObject->GetRootGO()->GetComponents<ComponentScript>();

					for (int i = 0; i < gameObjectScripts.size(); ++i)
					{
						if (gameObjectScripts[i]->GetConstructName() == "HealthSystem")
						{
							HealthSystem* healthScript = static_cast<HealthSystem*>(gameObjectScripts[i]->GetScript());
							healthScript->TakeDamage(damageAttack);
						}
					}
			}
		}

		audioSource->PostEvent(AUDIO::SFX::NPC::DRON::SHOT_IMPACT_01);//Provisional sfx

		DestroyBullet();
	}
}

void DroneBullet::DestroyBullet()
{
	App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
}
