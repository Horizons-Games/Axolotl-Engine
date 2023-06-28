#include "StdAfx.h"

#include "EmitterInstance.h"

#include "Application.h"
#include "ParticleEmitter.h"
#include "ParticleModule.h"
#include "ModuleRenderer.h"

#include "Modules/ModuleProgram.h"

#include <random>

EmitterInstance::EmitterInstance(ParticleEmitter* emitter, ComponentParticleSystem* owner) :
	emitter(emitter),
	owner(owner),
	aliveParticles(0),
	lastEmission(0.0f),
	elapsedTime(0.0f)
{
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
}

void EmitterInstance::UpdateModules()
{
	std::vector<ParticleModule*> modules = emitter->GetModules();
	
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
	static_cast<ModuleRenderer*>(emitter->GetModule(ParticleModule::ModuleType::RENDER))->DrawParticles(this);	
}

void EmitterInstance::DrawDD()
{
	emitter->DrawDD(this);
}
