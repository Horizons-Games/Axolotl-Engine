#include "ComponentAudioListener.h"
#include "DataModels/Components/ComponentTransform.h"
#include "DataModels/GameObject/GameObject.h"

#include "FileSystem/Json.h"

ComponentAudioListener::ComponentAudioListener(const bool active, GameObject* owner) :
	Component(ComponentType::AUDIOLISTENER, active, owner, true),
	listenerID(owner->GetUID())
{
	AK::SoundEngine::RegisterGameObj(listenerID, owner->GetName().c_str());
	AK::SoundEngine::SetDefaultListeners(&listenerID, 1);
	transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));

	if (transform)
	{
		OnTransformChanged();
	}
}

ComponentAudioListener::~ComponentAudioListener()
{
	AK::SoundEngine::UnregisterGameObj(listenerID);
}

void ComponentAudioListener::OnTransformChanged()
{
	const float3& pos = transform->GetGlobalPosition();
	const float3& front = transform->GetGlobalForward();
	const float3& correctFront = -float3(front.x, -front.y, front.z).Normalized();
	const float3& up = transform->GetGlobalUp();

	listenerTransform.Set(pos.x, pos.y, pos.z, correctFront.x, correctFront.y, correctFront.z, up.x, up.y, up.z);

	AK::SoundEngine::SetPosition(listenerID, listenerTransform);
}

void ComponentAudioListener::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool) active;
	meta["removed"] = (bool) canBeRemoved;
}

void ComponentAudioListener::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];
}

void ComponentAudioListener::Enable()
{
}

void ComponentAudioListener::Disable()
{
}