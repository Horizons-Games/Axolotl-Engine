#include "ParticleEmitter.h"

#include "EmitterInstance.h"
#include "ModuleBase.h"
#include "ModuleSpawn.h"
#include "ModulePosition.h"
#include "ParticleModule.h"


ParticleEmitter::ParticleEmitter() : name(""), shape(ShapeType::CIRCLE), particleTexture(nullptr)
{
	ModuleBase* base = new ModuleBase(this);
	ModuleSpawn* spawn = new ModuleSpawn(this);
	ModulePosition* position = new ModulePosition(this);

	modules.push_back(base);
	modules.push_back(spawn);
	modules.push_back(position);

	angle = DEFAULT_ANGLE;
	radius = DEFAULT_RADIUS;

	maxParticles = DEFAULT_MAX_PARTICLES;
	duration = DEFAULT_DURATION;
	lifespan.x = DEFAULT_LIFESPAN_X;
	lifespan.y = DEFAULT_LIFESPAN_Y;
	speed.x = DEFAULT_SPEED_X;
	speed.y = DEFAULT_SPEED_Y;
	size.x = DEFAULT_SIZE_X;
	size.y = DEFAULT_SIZE_Y;
	rotation.x = DEFAULT_ROTATION_X;
	rotation.y = DEFAULT_ROTATION_Y;
	gravity.x = DEFAULT_GRAVITY_X;
	gravity.y = DEFAULT_GRAVITY_Y;
	color = float4(DEFAULT_COLOR);

	randomLife = true;
	randomSpeed = true;
	randomSize = true;
	randomRot = true;
	randomGrav = true;

	visibleConfig = true;
}

ParticleEmitter::~ParticleEmitter()
{
	for (ParticleModule* module : modules)
	{
		delete module;
	}
	modules.clear();
}

void ParticleEmitter::Save()
{
}

void ParticleEmitter::Load()
{
}

void ParticleEmitter::DrawDD(EmitterInstance* instance) const
{
	for (ParticleModule* module : modules)
	{
		module->DrawDD(instance);
	}
}