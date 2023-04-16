#pragma once
#pragma once
#include "Components/Component.h"
#include "modules/ModuleAudio.h"

class ComponentTransform;

class ComponentAudioListener : public Component
{
public:
	ComponentAudioListener(const bool active, GameObject* owner);
	~ComponentAudioListener() override;

	void Update() override;

	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void Enable();
	void Disable();

private:
	uint64_t listenerID;
	AkTransform listenerTransform;
	ComponentTransform* transform;

};