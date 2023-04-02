#include "ComponentCanvas.h"

#include "Application.h"
#include "FileSystem/Json.h"

#include "GameObject/GameObject.h"

#include "ModuleEditor.h"
#include "ModuleWindow.h"

#include "Windows/EditorWindows/WindowScene.h"

ComponentCanvas::ComponentCanvas(bool active, GameObject* owner) :
	Component(ComponentType::CANVAS, active, owner, true), screenReferenceSize(float2(1920, 1080)),
	size(float2(1920, 1080)), screenFactor(0.0f)
{
	RecalculateSizeAndScreenFactor();
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
	meta["active"] = (bool) active;
	meta["removed"] = (bool) canBeRemoved;
}

void ComponentCanvas::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];
	RecalculateSizeAndScreenFactor();
}

void ComponentCanvas::RecalculateSizeAndScreenFactor()
{
	std::pair<int, int> region = App->editor->GetAvailableRegion();
	size = float2(region.first, region.second);
	float2 factor = size.Div(screenReferenceSize);
	screenFactor = factor.x < factor.y ? factor.x : factor.y;
}