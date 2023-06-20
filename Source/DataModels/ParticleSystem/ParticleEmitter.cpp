#include "ParticleEmitter.h"

#include "EmitterInstance.h"
#include "ModuleBase.h"
#include "ModuleColor.h"
#include "ModuleSpawn.h"
#include "ModulePosition.h"
#include "ModuleRenderer.h"
#include "ModuleRotation.h"
#include "ModuleSize.h"
#include "ParticleModule.h"


ParticleEmitter::ParticleEmitter() : name(""), shape(ShapeType::CONE), particleTexture(nullptr)
{
	ModuleBase* base = new ModuleBase(this);
	ModuleSpawn* spawn = new ModuleSpawn(this);
	ModuleColor* color = new ModuleColor(this);
	ModuleRotation* rotation = new ModuleRotation(this);
	ModuleSize* size = new ModuleSize(this);
	ModulePosition* position = new ModulePosition(this);
	ModuleRenderer* render = new ModuleRenderer(this);

	modules.push_back(base);
	modules.push_back(spawn);
	modules.push_back(color);
	modules.push_back(rotation);
	modules.push_back(size);
	modules.push_back(position);
	modules.push_back(render);

	color->SetEnabled(false);
	rotation->SetEnabled(false);
	size->SetEnabled(false);

	angle = DEFAULT_ANGLE;
	radius = DEFAULT_RADIUS;

	elapsedTime = 0.0f;
	maxParticles = DEFAULT_MAX_PARTICLES;
	duration = DEFAULT_DURATION;
	lifespan.x = DEFAULT_LIFESPAN_X;
	lifespan.y = DEFAULT_LIFESPAN_Y;
	speed.x = DEFAULT_SPEED_X;
	speed.y = DEFAULT_SPEED_Y;
	this->size.x = DEFAULT_SIZE_X;
	this->size.y = DEFAULT_SIZE_Y;
	this->rotation.x = DEFAULT_ROTATION_X;
	this->rotation.y = DEFAULT_ROTATION_Y;
	gravity.x = DEFAULT_GRAVITY_X;
	gravity.y = DEFAULT_GRAVITY_Y;
	this->color = float4(DEFAULT_COLOR);

	isLooping = true;
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