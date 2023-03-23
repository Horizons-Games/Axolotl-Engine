#include "ComponentPlayer.h"
#include "ComponentTransform.h"

#include "GameObject/GameObject.h"
#include "Application.h"

#include "FileSystem/Json.h"

ComponentPlayer::ComponentPlayer(bool active, GameObject* owner)
	: Component(ComponentType::PLAYER, active, owner, true)
{
	isKinematic = false;
	m = 1.0f;
	g = 9.81;
	v0 = float3(0.0f, 0.0f, 0.0f);
}

ComponentPlayer::~ComponentPlayer()
{
}

void ComponentPlayer::Update()
{
	if (isKinematic) 
	{
		ComponentTransform* transform = static_cast<ComponentTransform*>(GetOwner()->GetComponent(ComponentType::TRANSFORM));
		float3 x;
		float t = App->GetDeltaTime();
		float3 x0 = transform->GetGlobalPosition();
		float3 a = float3(0.0f, 0.5 * g * t * t, 0.0f);

		x = x0 + v0 * t + a;

		transform->SetPosition(x);
	}
}

void ComponentPlayer::Draw()
{

}

void ComponentPlayer::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;
}

void ComponentPlayer::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
}
