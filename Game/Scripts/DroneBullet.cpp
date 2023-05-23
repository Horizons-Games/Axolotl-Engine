#include "DroneBullet.h"

#include "Application.h"

#include "ModuleInput.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Scripting/ScriptFactory.h"

#include "Physics/Physics.h"

#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

#include "Geometry/Ray.h"

#include "debugdraw.h"

REGISTERCLASS(DroneBullet);

DroneBullet::DroneBullet() : Script(), transform(nullptr), velocity(0.2f), audioSource(nullptr)
{
	REGISTER_FIELD(velocity, float);
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

	if (SDL_GetTicks() / 1000.0f > bulletLifeTime);

}

void DroneBullet::ShootBullet(float deltaTime)
{
	transform->SetPosition(transform->GetGlobalPosition() + transform->GetLocalForward() * velocity * deltaTime * 1000);
	transform->UpdateTransformMatrices();
}

void DroneBullet::CheckCollision()
{
	Ray ray(transform->GetPosition(), transform->GetLocalForward());
	LineSegment line(ray, 10.0f);
	RaycastHit hit;
	if (Physics::Raycast(line, hit, transform->GetOwner()))
	{
		if (hit.gameObject->CompareTag("Player"))
		{
			//get component health and do damage
		}

		audioSource->PostEvent(audio::SFX_PLAYER_LIGHTSABER_CLASH);

		App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
	}
}

void DroneBullet::DestroyBullet()
{
	App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
}
