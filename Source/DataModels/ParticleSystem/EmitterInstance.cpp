#include "EmitterInstance.h"

#include "Application.h"
#include "ParticleEmitter.h"
#include "ParticleModule.h"
#include "ModuleRenderer.h"

#include "Modules/ModuleCamera.h"
#include "Modules/ModuleProgram.h"
#include "Program/Program.h"

#include <random>

EmitterInstance::EmitterInstance(const std::shared_ptr<ParticleEmitter> emitter, ComponentParticleSystem* owner) :
	emitter(emitter), owner(owner), aliveParticles(0), lastEmission(0.0f)
{
	srand(static_cast <unsigned> (time(nullptr))); //seeding the random generation once
	program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::PARTICLES);
}

EmitterInstance::~EmitterInstance()
{
	owner = nullptr;
	particles.clear();
}

void EmitterInstance::Init()
{
	particles.resize(emitter->GetMaxParticles());
	aliveParticles = 0;
	lastEmission = 0.0f;
}

void EmitterInstance::UpdateModules()
{
	std::vector<ParticleModule*> modules = emitter->GetModules();
	
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
	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::PARTICLES);

	program->Activate();

	const float4x4& view = App->GetModule<ModuleCamera>()->GetCamera()->GetViewMatrix();
	const float4x4& proj = App->GetModule<ModuleCamera>()->GetCamera()->GetProjectionMatrix();

	program->BindUniformFloat4x4(0, reinterpret_cast<const float*>(&proj), true);
	program->BindUniformFloat4x4(1, reinterpret_cast<const float*>(&view), true);

	static_cast<ModuleRenderer*>(emitter->GetModule(ParticleModule::ModuleType::RENDER))->DrawParticles(this);
	
	program->Deactivate();
}

void EmitterInstance::DrawDD()
{
	emitter->DrawDD(this);
}
