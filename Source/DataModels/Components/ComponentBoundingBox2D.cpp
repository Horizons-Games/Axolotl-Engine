#include "ComponentBoundingBox2D.h"

#include "Application.h"
#include "Modules/ModuleInput.h"

#include "UI/ComponentTransform2D.h"
#include "UI/ComponentCanvas.h"
#include "GameObject/GameObject.h"

#include "Modules/ModuleWindow.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleUI.h"

#include "debugdraw.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB2D.h"
#include "Geometry/Circle.h"
#include "imgui.h"
#include "Math/float3x3.h"

#include "FileSystem/Json.h"

ComponentBoundingBox2D::ComponentBoundingBox2D(bool active, GameObject* owner)
	: Component(ComponentType::BOUNDINGBOX2D, active, owner, false),
	localAABB({ {0, 0}, {0, 0} }), worldAABB(localAABB)
{
}

ComponentBoundingBox2D::~ComponentBoundingBox2D()
{
}


void ComponentBoundingBox2D::Init() 
{
	ComponentTransform2D* transform2D = (ComponentTransform2D*)(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
	if (transform2D) {
		float2 minPoint = float2(-0.5f, -0.5f);
		float2 maxPoint = float2(0.5f, 0.5f);

		SetLocalBoundingBox(AABB2D(minPoint, maxPoint));
		CalculateWorldBoundingBox();
	}
}

void ComponentBoundingBox2D::Update() 
{
	CalculateWorldBoundingBox();
}

void ComponentBoundingBox2D::SaveOptions(Json& meta)
{
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = static_cast<bool>(active);
	meta["removed"] = static_cast<bool>(canBeRemoved);
}

void ComponentBoundingBox2D::LoadOptions(Json& meta)
{
	type = GetTypeByName(meta["type"]);
	active = static_cast<bool>(meta["active"]);
	canBeRemoved = static_cast<bool>(meta["removed"]);
}

void ComponentBoundingBox2D::SetLocalBoundingBox(const AABB2D& boundingBox) 
{
	localAABB = boundingBox;
}

void ComponentBoundingBox2D::CalculateWorldBoundingBox() 
{
	
	ComponentTransform2D* transform2D = (ComponentTransform2D*)(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
	ComponentCanvas* canvasRenderer = (ComponentCanvas*)(GetOwner()->GetComponent(ComponentType::CANVAS));
	float screenFactor = 1.0f;
	float2 screenSize(0, 0);
	float3 position(0, 0, 0);
	float2 pivotPosition(0, 0);
	if (canvasRenderer != nullptr) {
		/*screenFactor = canvasRenderer->GetCanvasScreenFactor();
		screenSize = canvasRenderer->GetCanvasSize();
		position = transform2D->GetScreenPosition();
		pivotPosition = transform2D->GetPivot();*/
	}

	float2 pivotDifference = float2::zero;
	pivotDifference.x = -pivotPosition.x + 0.5f;
	pivotDifference.y = pivotPosition.y - 0.5f;

	worldAABB.minPoint = position.xy().Mul(float2(1.0f, -1.0f).Mul(screenFactor)) + screenSize / 2.0f
		+ (localAABB.minPoint + pivotDifference).Mul(transform2D->GetSize().Mul(transform2D->GetScale().xy()).Mul(screenFactor));
	worldAABB.maxPoint = position.xy().Mul(float2(1.0f, -1.0f).Mul(screenFactor)) + screenSize / 2.0f
		+ (localAABB.maxPoint + pivotDifference).Mul(transform2D->GetSize().Mul(transform2D->GetScale().xy()).Mul(screenFactor));
#ifndef ENGINE
	float2 windowPos = float2(App->window->GetPositionX(), App->window->GetPositionY());
	worldAABB.minPoint += windowPos;
	worldAABB.maxPoint += windowPos;
#endif
	
}



const AABB2D& ComponentBoundingBox2D::GetWorldAABB() const 
{
	return worldAABB;
}