#include "WindowComponentParticle.h"

#include "Components/ComponentParticleSystem.h"

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
	}

	if (ImGui::Button("Add an Emitter"))
	{
		//TODO: Create an emitter and add it to the component
	}
}
