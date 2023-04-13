#include "ComponentAudioSource.h"
#include "ModuleAudio.h"
#include "DataModels/GameObject/GameObject.h"


ComponentAudioSource::ComponentAudioSource(const bool active, GameObject* owner)
    : Component(ComponentType::AUDIOSOURCE, active, owner, true)
{
    PostEvent();
}

ComponentAudioSource::~ComponentAudioSource()
{
}

void ComponentAudioSource::Update()
{
}

void ComponentAudioSource::Draw()
{
}

void ComponentAudioSource::DrawHighlight()
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
    AkGameObjectID gameObj = owner->GetUID();

    AK::SoundEngine::RegisterGameObj(gameObj);


    AkPlayingID playingID = AK::SoundEngine::PostEvent(
        L"Play_Engine",                   // Name of the Event (not case sensitive).

        gameObj                             // Associated game object ID

    );
}
