#pragma once

#include "ParticleModule.h"

class ModulePosition : public ParticleModule
{
public:
	enum class ShapeType : int { CIRCLE = 0, CONE, BOX };

public:
	ModulePosition();
	~ModulePosition() override;

	void Spawn(EmitterInstance* emitter) override;
	void Update(EmitterInstance* emitter) override;
};

