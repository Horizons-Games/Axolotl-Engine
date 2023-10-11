#include "StdAfx.h"

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

	transform = owner->GetComponentInternal<ComponentTransform>();

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
	if (!IsEnabled())
	{
		return;
	}
	const float3& pos = transform->GetGlobalPosition();
	const float3& front = transform->GetGlobalForward().Normalized();
	//const float3& correctFront = -float3(front.x, -front.y, front.z).Normalized();
	const float3& correctFront = (front * -1).Normalized();
	const float3& up = transform->GetGlobalUp().Normalized();

	listenerTransform.Set(pos.x, pos.y, pos.z, correctFront.x, correctFront.y, correctFront.z, up.x, up.y, up.z);

	AK::SoundEngine::SetPosition(listenerID, listenerTransform);
}

void ComponentAudioListener::SignalEnable(bool isSceneLoading)
{
	AK::SoundEngine::RegisterGameObj(listenerID, GetOwner()->GetName().c_str());
	AK::SoundEngine::SetDefaultListeners(&listenerID, 1);

	OnTransformChanged();
}

void ComponentAudioListener::SignalDisable(bool isSceneLoading)
{
	AK::SoundEngine::UnregisterGameObj(listenerID);
}

void ComponentAudioListener::InternalSave(Json& meta)
{
}

void ComponentAudioListener::InternalLoad(const Json& meta)
{
	if (!IsEnabled())
	{
		AK::SoundEngine::UnregisterGameObj(listenerID);
	}
}