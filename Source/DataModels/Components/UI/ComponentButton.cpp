#include "ComponentButton.h"
#include "Application.h"
#include "ModuleScene.h"
#include "FileSystem/Json.h"

ComponentButton::ComponentButton(bool active, GameObject* owner)
	: Component(ComponentType::BUTTON, active, owner, true), 
	colorClicked(0.5f,0.5f,0.5f), colorHovered(0.7f,0.7f,0.7f), clicked(false), hovered(false),sceneName("CLICKED!")
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
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;

	meta["clicked"] = (bool)clicked;
	meta["hovered"] = (bool)hovered;

	meta["colorHovered_x"] = (float)colorHovered.x;
	meta["colorHovered_y"] = (float)colorHovered.y;
	meta["colorHovered_z"] = (float)colorHovered.z;

	meta["colorClicked_x"] = (float)colorClicked.x;
	meta["colorClicked_y"] = (float)colorClicked.y;
	meta["colorClicked_z"] = (float)colorClicked.z;

	meta["sceneName"] = sceneName.c_str();
}

void ComponentButton::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];

	colorHovered.x = (float)meta["colorHovered_x"];
	colorHovered.y = (float)meta["colorHovered_y"];
	colorHovered.z = (float)meta["colorHovered_z"];

	colorClicked.x = (float)meta["colorClicked_x"];
	colorClicked.y = (float)meta["colorClicked_y"];
	colorClicked.z = (float)meta["colorClicked_z"];

	sceneName = meta["sceneName"];
}

void ComponentButton::OnClicked()
{
	App->scene->SetSceneToLoad("Lib/Scenes/" + sceneName + ".axolotl");
}


