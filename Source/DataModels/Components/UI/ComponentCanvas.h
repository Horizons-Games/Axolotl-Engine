#pragma once
#include "../Component.h"
class ComponentCanvas :  public Component
{

public:
	ComponentCanvas(bool active, GameObject* owner);
	~ComponentCanvas() override;

	void Update() override;
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;
};

