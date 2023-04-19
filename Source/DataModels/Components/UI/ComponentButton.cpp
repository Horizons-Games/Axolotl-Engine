#include "ComponentButton.h"
#include "Application.h"
#include "FileSystem/Json.h"
#include "ModuleScene.h"

ComponentButton::ComponentButton(bool active, GameObject* owner) :
	Component(ComponentType::BUTTON, active, owner, true),
	colorClicked(0.5f, 0.5f, 0.5f, 1.0f),
	colorHovered(0.7f, 0.7f, 0.7f, 1.0f),
	clicked(false),
	hovered(false),
	sceneName("")
{
}

ComponentButton::~ComponentButton()
{
}

void ComponentButton::Update()
{
}

void ComponentButton::Draw()
{
}

void ComponentButton::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = static_cast<bool>(active);
	meta["removed"] = static_cast<bool>(canBeRemoved);

	meta["colorHovered_x"] = static_cast<float>(colorHovered.x);
	meta["colorHovered_y"] = static_cast<float>(colorHovered.y);
	meta["colorHovered_z"] = static_cast<float>(colorHovered.z);
	meta["colorHovered_w"] = static_cast<float>(colorHovered.w);

	meta["colorClicked_x"] = static_cast<float>(colorClicked.x);
	meta["colorClicked_y"] = static_cast<float>(colorClicked.y);
	meta["colorClicked_z"] = static_cast<float>(colorClicked.z);
	meta["colorClicked_w"] = static_cast<float>(colorClicked.w);

	meta["sceneName"] = sceneName.c_str();
}

void ComponentButton::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = static_cast<bool>(meta["active"]);
	canBeRemoved = static_cast<bool>(meta["removed"]);

	colorHovered.x = static_cast<float>(meta["colorHovered_x"]);
	colorHovered.y = static_cast<float>(meta["colorHovered_y"]);
	colorHovered.z = static_cast<float>(meta["colorHovered_z"]);
	colorHovered.w = static_cast<float>(meta["colorHovered_w"]);

	colorClicked.x = static_cast<float>(meta["colorClicked_x"]);
	colorClicked.y = static_cast<float>(meta["colorClicked_y"]);
	colorClicked.z = static_cast<float>(meta["colorClicked_z"]);
	colorClicked.w = static_cast<float>(meta["colorClicked_w"]);

	sceneName = meta["sceneName"];
}

void ComponentButton::OnClicked()
{
	App->scene->SetSceneToLoad("Lib/Scenes/" + sceneName + ".axolotl");
}
