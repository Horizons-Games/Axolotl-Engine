#include "ComponentButton.h"
#include "Application.h"
#include "FileSystem/Json.h"

ComponentButton::ComponentButton(bool active, GameObject* owner)
	: Component(ComponentType::BUTTON, active, owner, true), 
	colorClicked(0.5f,0.5f,0.5f), colorHovered(0.7f,0.7f,0.7f), clicked(false), hovered(false)
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
}

void ComponentButton::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
}

void ComponentButton::OnClicked()
{
	ENGINE_LOG("CLICKED!");
}


