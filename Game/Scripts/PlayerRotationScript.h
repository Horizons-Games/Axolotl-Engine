#pragma once
#include "Script.h"

class ComponentTransform;

class PlayerRotationScript :
    public Script
{
public:
	PlayerRotationScript();
	~PlayerRotationScript() override = default;

	void Start() override;
	void PreUpdate(float deltaTime) override;

private:
	void Rotate();

private:
	ComponentTransform* componentTransform;
};

