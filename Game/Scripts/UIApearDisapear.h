#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class HealthSystem;
class ComponentImage;


class UIApearDisapear : public Script
{

public:
	UIApearDisapear();
	~UIApearDisapear();

	void Start() override;
	void Update(float deltaTime) override;

	void SetObjective(float newAlpha);

private:
	float objectiveAlpha;
	bool inObjective;
	ComponentImage* image;
};