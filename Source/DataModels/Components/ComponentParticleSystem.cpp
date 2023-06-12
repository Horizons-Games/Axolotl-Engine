#include "ComponentParticleSystem.h"

#include "Application.h"

#include "Modules/ModuleCamera.h"
#include "Modules/ModuleProgram.h"
#include "Program/Program.h"

#include "ParticleSystem/EmitterInstance.h"

ComponentParticleSystem::ComponentParticleSystem(const bool active, GameObject* owner) :
	Component(ComponentType::PARTICLE, active, owner, true), resource(nullptr)
{
}

ComponentParticleSystem::~ComponentParticleSystem()
{
	for (EmitterInstance* instance : emitters)
	{
		delete instance;
	}

	emitters.clear();
}

void ComponentParticleSystem::SaveOptions(Json& meta)
{
}

void ComponentParticleSystem::LoadOptions(Json& meta)
{
}

void ComponentParticleSystem::Update()
{
	for (EmitterInstance* emitter : emitters)
	{
		emitter->UpdateModules();
	}
}

void ComponentParticleSystem::Draw() const
{
	for (EmitterInstance* instance : emitters)
	{
#ifdef ENGINE
		if (!App->IsOnPlayMode())
		{
			instance->DrawDD();
			//instance->SimulateParticles();
		}
#endif //ENGINE
	}
}

void ComponentParticleSystem::Render()
{
	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::PARTICLES);

	program->Activate();

	const float4x4& view = App->GetModule<ModuleCamera>()->GetCamera()->GetViewMatrix();
	const float4x4& proj = App->GetModule<ModuleCamera>()->GetCamera()->GetProjectionMatrix();

	program->BindUniformFloat4x4(0, reinterpret_cast<const float*>(&proj), true);
	program->BindUniformFloat4x4(1, reinterpret_cast<const float*>(&view), true);

	for (EmitterInstance* instance : emitters)
	{
		instance->DrawParticles();
	}

	program->Deactivate();
}

void ComponentParticleSystem::Reset()
{
}

void ComponentParticleSystem::CreateEmitterInstance()
{
	EmitterInstance* instance = new EmitterInstance(nullptr, this);
	instance->Init();
	emitters.push_back(instance);
}

void ComponentParticleSystem::CreateEmitterInstance(std::shared_ptr<ParticleEmitter> emitter)
{
	EmitterInstance* instance = new EmitterInstance(emitter, this);
	instance->Init();
	emitters.push_back(instance);
}

void ComponentParticleSystem::AddEmitterInstance(EmitterInstance* emitter)
{
	emitters.push_back(emitter);
}
