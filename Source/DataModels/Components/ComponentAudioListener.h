#pragma once
#include "Components/Component.h"
#include "modules/ModuleAudio.h"

class ComponentTransform;

class ComponentAudioListener : public Component
{
public:
	ComponentAudioListener(const bool active, GameObject* owner);
	~ComponentAudioListener() override;

	void OnTransformChanged() override;

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;
	void SignalEnable() override;
	void SignalDisable() override;

private:
	uint64_t listenerID;
	AkTransform listenerTransform;
	ComponentTransform* transform;
};