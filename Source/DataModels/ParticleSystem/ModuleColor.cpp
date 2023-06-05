#include "ModuleColor.h"

#include "EmitterInstance.h"
#include "ParticleEmitter.h"

#include "ImGui/imgui.h"

ModuleColor::ModuleColor(ParticleEmitter* emitter) : ParticleModule(ModuleType::SPAWN, emitter),
	color(float4(0.0f, 0.0f, 0.0f, 0.0f))
{
}

ModuleColor::~ModuleColor()
{
}

void ModuleColor::Spawn(EmitterInstance* instance)
{
}

void ModuleColor::Update(EmitterInstance* instance)
{
}

void ModuleColor::DrawImGui()
{
	if (ImGui::TreeNodeEx("Color Module", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_FramePadding))
	{
		if (ImGui::BeginTable("##colorTable", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Gradient");
			ImGui::TableNextColumn();

			bool updated = ImGui::GradientEditor(&gradient, draggingMark, selectedMark);

			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}
