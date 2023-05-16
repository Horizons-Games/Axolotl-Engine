#include "WindowComponentParticle.h"

#include "Components/ComponentParticleSystem.h"

#include "ParticleSystem/EmitterInstance.h"
#include "ParticleSystem/ParticleEmitter.h"

WindowComponentParticle::WindowComponentParticle(ComponentParticleSystem* component) :
	ComponentWindow("Particle System", component)
{
}

WindowComponentParticle::~WindowComponentParticle()
{
}

void WindowComponentParticle::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ImGui::Text("");

	ComponentParticleSystem* component = static_cast<ComponentParticleSystem*>(this->component);
	
	for (EmitterInstance* emitter : component->GetEmitters())
	{
		//TODO: Draw all the emitters and its configurable parameters (SHAPE, modules, etc...)
		if (emitter->GetEmitter())
		{
			//TODO: Draw emitter configuration
		}
		else
		{
			//TODO: Select a ParticleEmitter to assign to the EmitterInstance (it acts as a Resource for the instance)
		}
	}

	if (ImGui::Button("Add an Emitter"))
	{
		component->CreateEmitterInstance();
	}
}
