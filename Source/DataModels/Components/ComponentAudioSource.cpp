#include "ComponentAudioSource.h"
#include "ModuleAudio.h"
#include "DataModels/GameObject/GameObject.h"


ComponentAudioSource::ComponentAudioSource(const bool active, GameObject* owner)
    : Component(ComponentType::AUDIOSOURCE, active, owner, true)
{
    sourceID = owner->GetUID();

    AK::SoundEngine::RegisterGameObj(sourceID, owner->GetName().c_str());
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
        L"Play_RecordableMusic",    // Name of the Event (not case sensitive).
        sourceID           // Associated game object ID
    );
}
