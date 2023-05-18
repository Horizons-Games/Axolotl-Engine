#include "ModuleBase.h"

#include "EmitterInstance.h"

#include "ImGui/imgui.h"

ModuleBase::ModuleBase() : ParticleModule(ModuleType::BASE)
{
	originLocation = DEFAULT_ORIGIN;
	alignment = Alignment::WORLD;
}

ModuleBase::~ModuleBase()
{
}

void ModuleBase::Spawn(EmitterInstance* emitter)
{
}

void ModuleBase::Update(EmitterInstance* emitter)
{
}

void ModuleBase::DrawImGui()
{
	if (ImGui::TreeNodeEx("Base Module", ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_FramePadding))
	{
		if (ImGui::BeginTable("##baseTable", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Origin");

			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::Text("x:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##XTrans", &originLocation.x, 1.0f,
				std::numeric_limits<float>::min(), std::numeric_limits<float>::min()))
			{
			}
			ImGui::PopStyleVar(); ImGui::SameLine();

			ImGui::Text("y:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##YTrans", &originLocation.y, 1.0f,
				std::numeric_limits<float>::min(), std::numeric_limits<float>::min()))
			{
			}
			ImGui::PopStyleVar(); ImGui::SameLine();

			ImGui::Text("z:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##ZTrans", &originLocation.z, 1.0f,
				std::numeric_limits<float>::min(), std::numeric_limits<float>::min()))
			{
			}
			ImGui::PopStyleVar();

			ImGui::TableNextColumn();
			ImGui::Text("Alignment");
			ImGui::TableNextColumn();
			ImGui::Dummy(ImVec2(2.0f, 0.0f)); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);

			ModuleBase::Alignment alignment = GetAlignment();

			const char* items[] = { "SCREEN", "WORLD", "AXIAL" };

			static const char* currentItem;
			switch (alignment)
			{
			case ModuleBase::Alignment::SCREEN:
				currentItem = items[0];
				break;
			case ModuleBase::Alignment::WORLD:
				currentItem = items[1];
				break;
			case ModuleBase::Alignment::AXIAL:
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
					alignment = ModuleBase::Alignment::SCREEN;
				}
				else if (currentItem == items[1])
				{
					alignment = ModuleBase::Alignment::WORLD;
				}
				else if (currentItem == items[2])
				{
					alignment = ModuleBase::Alignment::AXIAL;
				}

				SetAlignment(alignment);

				ImGui::EndCombo();
			}
			ImGui::EndTable();
		}
		ImGui::TreePop();
	}
}
