#include "BixAttackScript.h"

#include "Application.h"

#include "ModuleInput.h"

#include "Physics/Physics.h"

#include "Components/ComponentAudioSource.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAnimation.h"

#include "GameObject/GameObject.h"

#include "Auxiliar/Audio/AudioData.h"

#include "Geometry/Ray.h"

#include "debugdraw.h"

REGISTERCLASS(BixAttackScript);

BixAttackScript::BixAttackScript() : Script(), attackCooldown(0.6f), lastAttackTime(0.f), audioSource(nullptr),
input(nullptr), rayAttackSize(10.0f), animation(nullptr), animationGO(nullptr),
transform(nullptr), 
//Provisional
ray1GO(nullptr), ray2GO(nullptr), ray3GO(nullptr), ray4GO(nullptr),
ray1Transform(nullptr), ray2Transform(nullptr), ray3Transform(nullptr), ray4Transform(nullptr)
//--Provisional
{
	REGISTER_FIELD(attackCooldown, float);
	REGISTER_FIELD(rayAttackSize, float);
	REGISTER_FIELD(animationGO, GameObject*);

	//Provisional
	REGISTER_FIELD(ray1GO, GameObject*);
	REGISTER_FIELD(ray2GO, GameObject*);
	REGISTER_FIELD(ray3GO, GameObject*);
	REGISTER_FIELD(ray4GO, GameObject*);
	//--Provisional
}

void BixAttackScript::Start()
{
	audioSource = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));
	transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	if (animationGO)
		animation = static_cast<ComponentAnimation*>(animationGO->GetComponent(ComponentType::ANIMATION));

	input = App->GetModule<ModuleInput>();

	audioSource->PostEvent(audio::SFX_PLAYER_LIGHTSABER_OPEN);
	audioSource->PostEvent(audio::SFX_PLAYER_LIGHTSABER_HUM);

	//Provisional
	ray1Transform = static_cast<ComponentTransform*>(ray1GO->GetComponent(ComponentType::TRANSFORM));
	ray2Transform = static_cast<ComponentTransform*>(ray2GO->GetComponent(ComponentType::TRANSFORM));
	ray3Transform = static_cast<ComponentTransform*>(ray3GO->GetComponent(ComponentType::TRANSFORM));
	ray4Transform = static_cast<ComponentTransform*>(ray4GO->GetComponent(ComponentType::TRANSFORM));
	//--Provisional
}

void BixAttackScript::Update(float deltaTime)
{
	//DEBUG
	Ray ray(transform->GetPosition(), transform->GetLocalForward());
	dd::arrow(ray.pos, ray.pos + ray.dir * rayAttackSize, dd::colors::Red, 0.05f);
	//Provisional
	Ray ray1(ray1Transform->GetGlobalPosition(), transform->GetLocalForward());
	dd::arrow(ray1.pos, ray1.pos + ray1.dir * rayAttackSize, dd::colors::Red, 0.05f);
	Ray ray2(ray2Transform->GetGlobalPosition(), transform->GetLocalForward());
	dd::arrow(ray2.pos, ray2.pos + ray2.dir * rayAttackSize, dd::colors::Red, 0.05f);
	Ray ray3(ray3Transform->GetGlobalPosition(), transform->GetLocalForward());
	dd::arrow(ray3.pos, ray3.pos + ray3.dir * rayAttackSize, dd::colors::Red, 0.05f);
	Ray ray4(ray4Transform->GetGlobalPosition(), transform->GetLocalForward());
	dd::arrow(ray4.pos, ray4.pos + ray4.dir * rayAttackSize, dd::colors::Red, 0.05f);
	//--Provisional

	// Attack
	if (input->GetKey(SDL_SCANCODE_Q) != KeyState::IDLE)
	{
		PerformAttack();
	}
}

void BixAttackScript::PerformAttack()
{
	if (isAttackAvailable())
	{
		if (animation)
			animation->SetParameter("attack", true);

		lastAttackTime = SDL_GetTicks() / 1000.0f;

		audioSource->PostEvent(audio::SFX_PLAYER_LIGHTSABER_SWING);

		CheckCollision();
	}
}

void BixAttackScript::CheckCollision()
{
	//Provisional
	std::vector<Ray> rays;
	Ray ray1(transform->GetPosition(), transform->GetLocalForward());
	Ray ray2(ray1Transform->GetGlobalPosition(), transform->GetLocalForward());
	Ray ray3(ray2Transform->GetGlobalPosition(), transform->GetLocalForward());
	Ray ray4(ray3Transform->GetGlobalPosition(), transform->GetLocalForward());
	Ray ray5(ray4Transform->GetGlobalPosition(), transform->GetLocalForward());
	rays.push_back(ray1);
	rays.push_back(ray2);
	rays.push_back(ray3);
	rays.push_back(ray4);
	rays.push_back(ray5);

	for (int i = 0; i < rays.size(); i++) {
		LineSegment line(rays[i], rayAttackSize);
		RaycastHit hit;
		if (Physics::Raycast(line, hit, transform->GetOwner()))
		{
			if (hit.gameObject->CompareTag("Enemy"))
			{
				//get component health and do damage
			}
			audioSource->PostEvent(audio::SFX_PLAYER_LIGHTSABER_CLASH);
		}
	}
	//--Provisional
}

bool BixAttackScript::isAttackAvailable()
{
	return (SDL_GetTicks() / 1000.0f > lastAttackTime + attackCooldown);
}
