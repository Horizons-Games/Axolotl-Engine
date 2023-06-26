#include "ModuleColor.h"

#include "EmitterInstance.h"
#include "ParticleEmitter.h"

#include "ImGui/imgui.h"

ModuleColor::ModuleColor(ParticleEmitter* emitter) : ParticleModule(ModuleType::COLOR, emitter),
	initAlpha(1.0f), endAlpha(0.0f), draggingMark(nullptr), selectedMark(nullptr)
{
	gradient = new ImGradient();
}

ModuleColor::~ModuleColor()
{
	delete gradient;
}

void ModuleColor::Spawn(EmitterInstance* instance)
{
}

void ModuleColor::Update(EmitterInstance* instance)
{
	if (enabled)
	{
		std::vector<EmitterInstance::Particle>& particles = instance->GetParticles();
		std::list<ImGradientMark*> marks = gradient->getMarks();

		float4 initColor = float4(marks.front()->color);
		initColor.w = initAlpha;
		float4 endColor = float4(marks.back()->color);
		endColor.w = endAlpha;

		for (int i = 0; i < particles.size(); ++i)
		{
			EmitterInstance::Particle& particle = particles[i];

			if (particle.lifespan >= 0.0f)
			{
				float lifeRatio = 1.0f - particle.lifespan / particle.initLife;

				particle.color = instance->lerp(initColor, endColor, lifeRatio);
			}
		}
	}
}

void ModuleColor::DrawImGui()
{
	if (ImGui::TreeNodeEx("Color Module", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_FramePadding))
	{
		ImGui::SameLine();
		ImGui::Checkbox("##enabled1", &enabled);

		ImGui::Text("Color Gradient:");
		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::GradientEditor(gradient, draggingMark, selectedMark);

		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		
		ImGui::Text("Alpha Gradient:");
		if (ImGui::BeginTable("##alphaTable", 4))
		{			
			ImGui::TableNextColumn();
			ImGui::Text("Init value");
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::DragFloat("##initAlpha", &initAlpha, 0.01, 0.0f, 1.0f, "%.2f");
			ImGui::TableNextColumn();
			ImGui::Text("End value");
			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::DragFloat("##endAlpha", &endAlpha, 0.01, 0.0f, 1.0f, "%.2f");
			
			ImGui::EndTable();
		}

		ImGui::TreePop();
	}
	else
	{
		ImGui::SameLine();
		ImGui::Checkbox("##enabled1", &enabled);
	}
}
