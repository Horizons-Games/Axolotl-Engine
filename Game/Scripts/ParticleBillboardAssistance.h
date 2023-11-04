#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;

class ParticleBillboardAssistance : public Script
{
public:
	ParticleBillboardAssistance();
	~ParticleBillboardAssistance() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void UpdateTransform();

private:
	ComponentTransform* ownerTransform;
	bool blockY;
};


