#include "ComponentAmbient.h"

#include "FileSystem/Json.h"

#include "imgui.h"

ComponentAmbient::ComponentAmbient() : ComponentLight(LightType::AMBIENT)
{
}

ComponentAmbient::ComponentAmbient(GameObject* parent) : ComponentLight(LightType::AMBIENT, parent)
{
}

ComponentAmbient::ComponentAmbient(const float3& color) : ComponentLight(LightType::AMBIENT, color, 1.0f)
{
}

ComponentAmbient::ComponentAmbient(const float3& color, GameObject* parent) : 
	ComponentLight(LightType::AMBIENT, color, 1.0f, parent)
{
}

void ComponentAmbient::Display()
{
	ImGui::Text("AMBIENT LIGHT");
	ImGui::Dummy(ImVec2(0.0f, 2.5f));
	if (ImGui::BeginTable("AmbientLightTable", 2))
	{
		ImGui::TableNextColumn();
		
		float intensity = GetIntensity();
		ImGui::Text("Intensity"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##Intensity", &intensity, 0.01f,
			0.0f, 1.0f
		); ImGui::PopStyleVar();
		SetIntensity(intensity);

		static float3 color = GetColor();
		ImGui::Text("Color"); ImGui::SameLine();
		if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
			SetColor(color);

		ImGui::EndTable();
		ImGui::Separator();
	}
}

void ComponentAmbient::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["owner"] = (GameObject*)owner;
	meta["removed"] = (bool)canBeRemoved;

	meta["color_light_X"] = (float)color.x;
	meta["color_light_Y"] = (float)color.y;
	meta["color_light_Z"] = (float)color.z;

	meta["intensity"] = (float)intensity;

	meta["lightType"] = GetNameByLightType(lightType).c_str();
}

void ComponentAmbient::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	//owner = (GameObject*) meta["owner"];
	canBeRemoved = (bool)meta["removed"];

	color.x = (float)meta["color_light_X"];
	color.y = (float)meta["color_light_Y"];
	color.z = (float)meta["color_light_Z"];

	intensity = (float)meta["intensity"];

	lightType = GetLightTypeByName(meta["type"]);
}