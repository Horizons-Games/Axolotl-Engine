#include "StdAfx.h"

#include "EmitterInstance.h"

#include "Application.h"
#include "ParticleEmitter.h"
#include "ParticleModule.h"
#include "ModuleRenderer.h"

#include "Modules/ModuleProgram.h"

#include "ParticleSystem/ModuleBase.h"
#include "ParticleSystem/ModuleSpawn.h"
#include "ParticleSystem/ModuleColor.h"
#include "ParticleSystem/ModuleRotation.h"
#include "ParticleSystem/ModuleSize.h"
#include "ParticleSystem/ModulePosition.h"
#include "ParticleSystem/ModuleRenderer.h"

#include <random>

EmitterInstance::EmitterInstance(ParticleEmitter* emitter, ComponentParticleSystem* owner) :
	emitter(emitter),
	owner(owner),
	aliveParticles(0),
	lastEmission(0.0f),
	elapsedTime(0.0f)
{
	ModuleBase* base = new ModuleBase(emitter, 
		static_cast<ModuleBase*>(emitter->GetModule(ParticleModule::ModuleType::BASE)));
	ModuleSpawn* spawn = new ModuleSpawn(emitter, 
		static_cast<ModuleSpawn*>(emitter->GetModule(ParticleModule::ModuleType::SPAWN)));
	ModuleColor* color = new ModuleColor(emitter,
		static_cast<ModuleColor*>(emitter->GetModule(ParticleModule::ModuleType::COLOR)));
	ModuleRotation* rotation = new ModuleRotation(emitter, 
		static_cast<ModuleRotation*>(emitter->GetModule(ParticleModule::ModuleType::ROTATION)));
	ModuleSize* size = new ModuleSize(emitter, 
		static_cast<ModuleSize*>(emitter->GetModule(ParticleModule::ModuleType::SIZE)));
	ModulePosition* position = new ModulePosition(emitter, 
		static_cast<ModulePosition*>(emitter->GetModule(ParticleModule::ModuleType::POSITION)));
	ModuleRenderer* render = new ModuleRenderer(emitter, 
		static_cast<ModuleRenderer*>(emitter->GetModule(ParticleModule::ModuleType::RENDER)));

	modules.push_back(base);
	modules.push_back(spawn);
	modules.push_back(color);
	modules.push_back(rotation);
	modules.push_back(size);
	modules.push_back(position);
	modules.push_back(render);

	srand(static_cast <unsigned> (time(nullptr))); //seeding the random generation once
	program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::PARTICLES);
}

EmitterInstance::~EmitterInstance()
{
	owner = nullptr;
	particles.clear();
	sortedPositions.clear();
}

void EmitterInstance::Init()
{
	particles.resize(emitter->GetMaxParticles());
	sortedPositions.resize(emitter->GetMaxParticles());
	elapsedTime = 0.0f;
	aliveParticles = 0;
	lastEmission = 0.0f;
}

void EmitterInstance::Stop()
{
	elapsedTime = 0.0f;
	aliveParticles = 0;
	lastEmission = 0.0f;

	for (Particle& particle : particles)
	{
		particle.lifespan = 0.000f;
	}
}

void EmitterInstance::UpdateModules()
{	
	sortedPositions.clear();

	for (ParticleModule* module : modules)
	{
		module->Update(this);
	}
}

float EmitterInstance::CalculateRandomValueInRange(float min, float max)
{
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

void EmitterInstance::DrawParticles()
{
	static_cast<ModuleRenderer*>(modules[6])->DrawParticles(this);	
}

void EmitterInstance::DrawDD()
{
	emitter->DrawDD(this);
}
