#include "DroneBullet.h"

#include "Application.h"

#include "ModuleInput.h"
#include "Scripting/ScriptFactory.h"

#include "Components/ComponentTransform.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

#include "Geometry/Ray.h"

#include "debugdraw.h"

REGISTERCLASS(DroneBullet);

DroneBullet::DroneBullet() : Script(), transform(nullptr), velocity(100.6f)
{
	REGISTER_FIELD(velocity, float);
}

void DroneBullet::Start()
{
	transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
}

void DroneBullet::Update(float deltaTime)
{
	ShootBullet(deltaTime);
}

void DroneBullet::ShootBullet(float deltaTime)
{
	transform->SetPosition(transform->GetGlobalPosition() + transform->GetGlobalForward() * velocity * deltaTime * 1000);
	transform->UpdateTransformMatrices();
}