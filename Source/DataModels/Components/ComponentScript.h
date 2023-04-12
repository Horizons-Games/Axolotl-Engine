#pragma once
#include "Component.h"

class Json;

class ComponentScript : public Component
{
public:

	ComponentScript();

	ComponentScript(bool active, GameObject* owner);

	~ComponentScript() override;

	void Draw() override {};

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;
};

