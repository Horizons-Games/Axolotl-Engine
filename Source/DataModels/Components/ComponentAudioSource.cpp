#include "ComponentAudioSource.h"
#include "DataModels/GameObject/GameObject.h"
#include "DataModels/Components/ComponentTransform.h"
#include "Auxiliar/Audios/AudioList.h"


ComponentAudioSource::ComponentAudioSource(const bool active, GameObject* owner)
    : Component(ComponentType::AUDIOSOURCE, active, owner, true)
{
    sourceID = owner->GetUID();

    AK::SoundEngine::RegisterGameObj(sourceID, owner->GetName().c_str());
    transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
    PostEvent();
}

ComponentAudioSource::~ComponentAudioSource()
{
    AK::SoundEngine::UnregisterGameObj(sourceID);
}

void ComponentAudioSource::Update()
{
}

void ComponentAudioSource::Draw()
{
    const float3& pos = transform->GetGlobalPosition();
    const float3& front = transform->GetGlobalForward().Normalized();
    const float3& up = transform->GetGlobalUp();

    sourceTransform.Set(pos.x, pos.y, pos.z,
        front.x, front.y, front.z,
        up.x, up.y, up.z
    );

    AK::SoundEngine::SetPosition(sourceID, sourceTransform);
}

void ComponentAudioSource::SaveOptions(Json& meta)
{
}

void ComponentAudioSource::LoadOptions(Json& meta)
{
}

void ComponentAudioSource::Enable()
{
}

void ComponentAudioSource::Disable()
{
}

void ComponentAudioSource::PostEvent()
{
    AK::SoundEngine::PostEvent(
        audios::MUSIC_TEST,    // Name of the Event (not case sensitive).
        sourceID           // Associated game object ID
    );
}
