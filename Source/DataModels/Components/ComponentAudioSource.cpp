#include "StdAfx.h"

#include "ComponentAudioSource.h"
#include "Auxiliar/Audio/AudioData.h"
#include "DataModels/Components/ComponentTransform.h"
#include "DataModels/GameObject/GameObject.h"

#include "FileSystem/Json.h"

ComponentAudioSource::ComponentAudioSource(const bool active, GameObject* owner) :
	Component(ComponentType::AUDIOSOURCE, active, owner, true),
	sourceID(owner->GetUID())
{
	AK::SoundEngine::RegisterGameObj(sourceID, owner->GetName().c_str());
	transform = owner->GetComponent<ComponentTransform>();

	if (transform)
	{
		OnTransformChanged();
	}
}

ComponentAudioSource::~ComponentAudioSource()
{
	AK::SoundEngine::StopAll(sourceID);
	AK::SoundEngine::UnregisterGameObj(sourceID);
}

void ComponentAudioSource::OnTransformChanged()
{
	if (!IsEnabled())
	{
		return;
	}
	const float3& pos = transform->GetGlobalPosition();
	const float3& front = transform->GetGlobalForward();
	const float3& correctFront = float3(front.x, front.y, front.z).Normalized();
	const float3& up = transform->GetGlobalUp().Normalized();

	sourceTransform.Set(pos.x, pos.y, pos.z, correctFront.x, correctFront.y, correctFront.z, up.x, up.y, up.z);

	AK::SoundEngine::SetPosition(sourceID, sourceTransform);
}

void ComponentAudioSource::PostEvent(const wchar_t* sound)
{
	if (!IsEnabled())
	{
		return;
	}
	AK::SoundEngine::PostEvent(sound,	// Name of the Event (not case sensitive).
							   sourceID // Associated game object ID
	);
}

void ComponentAudioSource::SetSwitch(const wchar_t* switchGroup, const wchar_t* switchSound)
{
	AK::SoundEngine::SetSwitch(switchGroup, switchSound, sourceID);
}

void ComponentAudioSource::Enable()
{
	Component::Enable();

	OnTransformChanged();
}

void ComponentAudioSource::Disable()
{
	Component::Disable();

	AK::SoundEngine::StopAll(sourceID);
}

void ComponentAudioSource::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool) active;
	meta["removed"] = (bool) canBeRemoved;
}

void ComponentAudioSource::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];
}
