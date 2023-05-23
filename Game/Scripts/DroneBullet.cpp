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
}

void DroneBullet::ShootBullet(float deltaTime)
{
	transform->SetPosition(transform->GetGlobalPosition() + transform->GetLocalForward() * velocity * deltaTime * 1000);
	transform->UpdateTransformMatrices();

	Ray ray(transform->GetPosition(), transform->GetLocalForward());
	LineSegment line(ray, 3.0f);
	if (Physics::RaycastFirst(line, transform->GetOwner()))
	{
		audioSource->PostEvent(audio::SFX_PLAYER_LIGHTSABER_CLASH);


		App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(owner);
	}
}