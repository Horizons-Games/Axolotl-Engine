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
	
	int id = 0;
	for (EmitterInstance* instance : component->GetEmitters())
	{
		DrawEmitter(instance, id);
		++id;
	}

	if (ImGui::Button("Add an Emitter"))
	{
		std::shared_ptr<ParticleEmitter> emitter = std::make_shared<ParticleEmitter>();
		emitter->SetName("DefaultEmitter");

		component->CreateEmitterInstance(emitter);
	}
}

void WindowComponentParticle::DrawEmitter(EmitterInstance* instance, int id)
{
	std::shared_ptr<ParticleEmitter> emitter = instance->GetEmitter();

	ImGui::PushID(id);

	if (emitter)
	{
		ParticleEmitter::ShapeType shape = emitter->GetShape();

		const char* items[] = { "CIRCLE", "CONE", "BOX" };

		static const char* currentItem;
		switch (shape)
		{
		case ParticleEmitter::ShapeType::CIRCLE:
			currentItem = items[0];
			break;
		case ParticleEmitter::ShapeType::CONE:
			currentItem = items[1];
			break;
		case ParticleEmitter::ShapeType::BOX:
			currentItem = items[2];
			break;
		}

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImGui::Text("Shape"); 

		ImGui::SameLine(0.0f, 10.0f);
		ImGui::SetNextItemWidth(80.0f);
		
		if (ImGui::BeginCombo("##shapeCombo", currentItem))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				// You can store your selection however you want, outside or inside your objects
				bool isSelected = (currentItem == items[n]);
				if (ImGui::Selectable(items[n], isSelected))
				{
					currentItem = items[n];
					if (isSelected)
					{
						// You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
						ImGui::SetItemDefaultFocus();
					}
				}
			}

			if (currentItem == items[0])
			{
				shape = ParticleEmitter::ShapeType::CIRCLE;
			}
			else if (currentItem == items[1])
			{
				shape = ParticleEmitter::ShapeType::CONE;
			}
			else if (currentItem == items[2])
			{
				shape = ParticleEmitter::ShapeType::BOX;
			}

			emitter->SetShape(shape);

			ImGui::EndCombo();
		}
		
		//TODO: Draw Emitter Modules

		ImGui::Separator();
	}
	else
	{
		//TODO: Select a ParticleEmitter to assign to the EmitterInstance (it acts as a Resource for the instance)
	}

	ImGui::PopID();
}
