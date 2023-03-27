#include "ComponentCanvas.h"

#include "Application.h"
#include "FileSystem/Json.h"

#include "GameObject/GameObject.h"

#include "ModuleWindow.h"

ComponentCanvas::ComponentCanvas(bool active, GameObject* owner)
	: Component(ComponentType::CANVAS, active, owner, true),
	screenReferenceSize(float2(1920, 1080)), size(float2(1920, 1080)),
	screenFactor(0.0f)
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
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;
}

void ComponentCanvas::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
	RecalculateSizeAndScreenFactor();
}


void ComponentCanvas::RecalculateSizeAndScreenFactor() 
{

	int w, h;
	SDL_GetWindowSize(App->window->GetWindow(), &w, &h);
	size = float2(w,h);
	float2 factor = size.Div(screenReferenceSize);
	screenFactor = factor.x < factor.y ? factor.x : factor.y;
}

bool ComponentCanvas::AnyParentHasCanvas() 
{
	GameObject* parent = GetOwner()->GetParent();

	while (parent != nullptr) {
		if ((ComponentCanvas*)(parent->GetComponent(ComponentType::CANVAS))) {
			return true;
		}
		parent = parent->GetParent();
	}

	return false;
}