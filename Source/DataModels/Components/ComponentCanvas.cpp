#include "ComponentCanvas.h"

#include "FileSystem/Json.h"

ComponentCanvas::ComponentCanvas(bool active, GameObject* owner)
	: Component(ComponentType::CANVAS, active, owner, false)
{
}

ComponentCanvas::~ComponentCanvas()
{
}

void ComponentCanvas::Update()
{
}

void ComponentCanvas::Draw()
{

}

void ComponentCanvas::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;
}

void ComponentCanvas::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
}