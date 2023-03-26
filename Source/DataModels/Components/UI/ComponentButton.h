#pragma once
#include "../Component.h"
class ComponentButton :
	public Component
{
public:
	ComponentButton(bool active, GameObject* owner);
	~ComponentButton() override;

	void Update() override;
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

private:
};
