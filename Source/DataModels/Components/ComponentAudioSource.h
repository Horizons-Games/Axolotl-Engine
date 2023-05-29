#pragma once
#include "Components/Component.h"
#include "modules/ModuleAudio.h"

class ComponentTransform;
class AudioList;

class ComponentAudioSource : public Component
{
public:
	ComponentAudioSource(const bool active, GameObject* owner);
	~ComponentAudioSource() override;

	void OnTransformChanged() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;
	void Enable() override;
	void Disable() override;

	void PostEvent(const wchar_t* sound); // Use this function to play a sound
	void SetSwitch(const wchar_t* switchGroup, const wchar_t* switchSound);

private:
	uint64_t sourceID;
	AkTransform sourceTransform;
	ComponentTransform* transform;
};