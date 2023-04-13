#pragma once
#include "Components/Component.h"

class ComponentAudioSource : public Component
{
public:
	ComponentAudioSource(const bool active, GameObject* owner);
	~ComponentAudioSource() override;

	void Update() override;

	void Draw() override;
	void DrawHighlight();

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void Enable();
	void Disable();

	void PostEvent();

private:


};