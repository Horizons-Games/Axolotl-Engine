#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;

class ParticleBillboardAsistance : public Script
{
public:
	ParticleBillboardAsistance();
	~ParticleBillboardAsistance() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	void UpdateTransform();

private:
	ComponentTransform* ownerTransform;
};


