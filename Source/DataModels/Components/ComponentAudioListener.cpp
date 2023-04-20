#include "ComponentAudioListener.h"
#include "DataModels/GameObject/GameObject.h"
#include "DataModels/Components/ComponentTransform.h"


ComponentAudioListener::ComponentAudioListener(const bool active, GameObject* owner)
    : Component(ComponentType::AUDIOLISTENER, active, owner, true)
{
    listenerID = owner->GetUID();

    AK::SoundEngine::RegisterGameObj(listenerID, owner->GetName().c_str());
    AK::SoundEngine::SetDefaultListeners(&listenerID, 1);
    transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
}

ComponentAudioListener::~ComponentAudioListener()
{
    AK::SoundEngine::UnregisterGameObj(listenerID);
}

void ComponentAudioListener::Update()
{
}

void ComponentAudioListener::Draw()
{
}

void ComponentAudioListener::OnTransformChanged()
{
    const float3& pos = transform->GetGlobalPosition();
    const float3& front = transform->GetGlobalForward().Normalized();
    const float3& up = transform->GetGlobalUp();

    listenerTransform.Set(pos.x, pos.y, pos.z,
        front.x, front.y, front.z,
        up.x, up.y, up.z
    );

    AK::SoundEngine::SetPosition(listenerID, listenerTransform);
}

void ComponentAudioListener::SaveOptions(Json& meta)
{
}

void ComponentAudioListener::LoadOptions(Json& meta)
{
}

void ComponentAudioListener::Enable()
{
}

void ComponentAudioListener::Disable()
{
}