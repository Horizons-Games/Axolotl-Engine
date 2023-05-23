#include "ModuleRenderer.h"

#include "ImGui/imgui.h"

ModuleRenderer::ModuleRenderer(ParticleEmitter* emitter) : ParticleModule(ModuleType::RENDER, emitter)
{
	alignment = Alignment::WORLD;
}

ModuleRenderer::~ModuleRenderer()
{
}

void ModuleRenderer::Spawn(EmitterInstance* instance)
{
}

void ModuleRenderer::Update(EmitterInstance* instance)
{
}

void ModuleRenderer::DrawImGui()
{
	if (ImGui::TreeNodeEx("Render Module", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_FramePadding))
	{
		if (ImGui::BeginTable("##renderTable", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Alignment");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);

			ModuleRenderer::Alignment alignment = GetAlignment();

			const char* items[] = { "SCREEN", "WORLD", "AXIAL" };

			static const char* currentItem;
			switch (alignment)
			{
			case ModuleRenderer::Alignment::SCREEN:
				currentItem = items[0];
				break;
			case ModuleRenderer::Alignment::WORLD:
				currentItem = items[1];
				break;
			case ModuleRenderer::Alignment::AXIAL:
				currentItem = items[2];
				break;
			}

			if (ImGui::BeginCombo("##alignmentCombo", currentItem))
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
					alignment = ModuleRenderer::Alignment::SCREEN;
				}
				else if (currentItem == items[1])
				{
					alignment = ModuleRenderer::Alignment::WORLD;
				}
				else if (currentItem == items[2])
				{
					alignment = ModuleRenderer::Alignment::AXIAL;
				}

				SetAlignment(alignment);

				ImGui::EndCombo();
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}
