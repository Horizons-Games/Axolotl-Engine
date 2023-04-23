#include "ComponentAudioSource.h"
#include "DataModels/GameObject/GameObject.h"
#include "DataModels/Components/ComponentTransform.h"
#include "Auxiliar/Audio/AudioData.h"


ComponentAudioSource::ComponentAudioSource(const bool active, GameObject* owner)
    : Component(ComponentType::AUDIOSOURCE, active, owner, true)
{
    sourceID = owner->GetUID();

    AK::SoundEngine::RegisterGameObj(sourceID, owner->GetName().c_str());
    transform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));

    OnTransformChanged();

    PostEvent(audio::CANTINA_MUSIC); //REMOVE (just for testing)
    AK::SoundEngine::SetState(audio::STATE_GROUP_ZONE, audio::STATE_ID_ZONE_SEWERS);
    AK::SoundEngine::SetState(audio::STATE_GROUP_LIFE, audio::STATE_ID_LIFE_ALIVE);
    SetSwitch(audio::SWITCH_GROUP_MUSIC_GAMEPLAY, audio::SWITCH_ID_MUSIC_EXPLORATION);
}

ComponentAudioSource::~ComponentAudioSource()
{
    AK::SoundEngine::StopAll(sourceID);
    AK::SoundEngine::UnregisterGameObj(sourceID);
}

void ComponentAudioSource::Update()
{
}

void ComponentAudioSource::Draw()
{
}

void ComponentAudioSource::OnTransformChanged()
{
    const float3& pos = transform->GetGlobalPosition();
    const float3& front = transform->GetGlobalForward();
    const float3& correctFront = -float3(front.x, -front.y, front.z).Normalized();
    const float3& up = transform->GetGlobalUp();

    sourceTransform.Set(pos.x, pos.y, pos.z,
        correctFront.x, correctFront.y, correctFront.z,
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

void ComponentAudioSource::PostEvent(const wchar_t* sound)
{
    AK::SoundEngine::PostEvent(
        sound,    // Name of the Event (not case sensitive).
        sourceID           // Associated game object ID
    );
}

void ComponentAudioSource::SetSwitch(const wchar_t* switchGroup, const wchar_t* switchSound)
{
    AK::SoundEngine::SetSwitch(
        switchGroup,
        switchSound,
        sourceID
    );
}
