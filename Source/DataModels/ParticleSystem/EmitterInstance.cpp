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
	ModuleColor* col = new ModuleColor(emitter,
		static_cast<ModuleColor*>(emitter->GetModule(ParticleModule::ModuleType::COLOR)));
	ModuleRotation* rot = new ModuleRotation(emitter,
		static_cast<ModuleRotation*>(emitter->GetModule(ParticleModule::ModuleType::ROTATION)));
	ModuleSize* size = new ModuleSize(emitter, 
		static_cast<ModuleSize*>(emitter->GetModule(ParticleModule::ModuleType::SIZE)));
	ModulePosition* position = new ModulePosition(emitter, 
		static_cast<ModulePosition*>(emitter->GetModule(ParticleModule::ModuleType::POSITION)));
	ModuleRenderer* render = new ModuleRenderer(emitter, 
		static_cast<ModuleRenderer*>(emitter->GetModule(ParticleModule::ModuleType::RENDER)));

	modules.push_back(base);
	modules.push_back(spawn);
	modules.push_back(col);
	modules.push_back(rot);
	modules.push_back(size);
	modules.push_back(position);
	modules.push_back(render);

	name		 = emitter->GetName();

	isLooping	 = emitter->IsLooping();
	maxParticles = emitter->GetMaxParticles();
	duration	 = emitter->GetDuration();
	lifespan	 = emitter->GetLifespanRange();
	speed		 = emitter->GetSpeedRange();
	rotation	 = emitter->GetRotationRange();
	gravity		 = emitter->GetGravityRange();
	color		 = emitter->GetColor();
	this->size   = emitter->GetSizeRange();

	angle		 = emitter->GetAngle();
	radius		 = emitter->GetRadius();

	randomLife	 = emitter->IsRandomLife();
	randomSpeed  = emitter->IsRandomSpeed();
	randomSize	 = emitter->IsRandomSize();
	randomRot	 = emitter->IsRandomRot();
	randomGrav	 = emitter->IsRandomGravity();

	shape		 = emitter->GetShape();

	srand(static_cast <unsigned> (time(nullptr))); //seeding the random generation once
	program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::PARTICLES);
}

EmitterInstance::~EmitterInstance()
{
	for (ParticleModule* module : modules)
	{
		delete module;
	}
	modules.clear();

	owner = nullptr;
	particles.clear();
	sortedPositions.clear();
}

void EmitterInstance::Init()
{
	particles.resize(GetMaxParticles());
	sortedPositions.resize(GetMaxParticles());
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
	for (ParticleModule* module : modules)
	{
		module->DrawDD(this);
	}
}

void EmitterInstance::SaveConfig()
{
	emitter->SetName(name);
	emitter->SetLooping(isLooping);
	emitter->SetMaxParticles(maxParticles);
	emitter->SetDuration(duration);
	emitter->SetLifespanRange(lifespan);
	emitter->SetSpeedRange(speed);
	emitter->SetSizeRange(size);
	emitter->SetRotationRange(rotation);
	emitter->SetGravityRange(gravity);
	emitter->SetColor(color);
	emitter->SetAngle(angle);
	emitter->SetRadius(radius);
	emitter->SetRandomLife(randomLife);
	emitter->SetRandomSpeed(randomSpeed);
	emitter->SetRandomSize(randomSize);
	emitter->SetRandomRotation(randomRot);
	emitter->SetRandomGravity(randomGrav);
	emitter->SetShape(shape);
	emitter->SetVisibleConfig(visibleConfig);

	emitter->SaveModules(modules);
}