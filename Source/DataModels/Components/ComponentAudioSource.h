#pragma once
#include "Components/Component.h"

class ComponentAudioSource : public Component
{
public:
	ComponentAudioSource(const bool active, GameObject* owner);
	~ComponentAudioSource() override;

	void Update() override;

	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void Enable();
	void Disable();

	void PostEvent();

private:
	uint64_t sourceID;

};