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

	void PostEvent(const wchar_t* sound); // Use this function to play a sound
	void SetSwitch(const wchar_t* switchGroup, const wchar_t* switchSound);

	uint64_t GetID() const;

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;
	void SignalEnable(bool isSceneLoading) override;
	void SignalDisable(bool isSceneLoading) override;

private:
	uint64_t sourceID;
	AkTransform sourceTransform;
	ComponentTransform* transform;
};