#pragma once
#include "Component.h"
class ComponentPlayer :
    public Component
{
public:
	ComponentPlayer(bool active, GameObject* owner);
	~ComponentPlayer() override;

	void Update() override;
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;
};

