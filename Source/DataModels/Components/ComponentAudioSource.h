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

	void Update() override;

	void Draw() override;
	void OnTransformChanged() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void Enable();
	void Disable();

	void PostEvent(static const wchar_t* sound);

private:
	uint64_t sourceID;
	AkTransform sourceTransform;
	ComponentTransform* transform;

};