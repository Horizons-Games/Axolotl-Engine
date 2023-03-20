#include "ComponentTransform2D.h"

ComponentTransform2D::ComponentTransform2D(const bool active, GameObject* owner)
	: Component(ComponentType::TRANSFORM2D, active, owner, true)
{
}

ComponentTransform2D::~ComponentTransform2D()
{
	delete eulerAngles;
	eulerAngles = nullptr;
	delete localPosition;
	localPosition = nullptr;
	delete localRotation;
	localRotation = nullptr;
	delete localScale;
	localScale = nullptr;
	delete localToWorldMatrix;
	localToWorldMatrix = nullptr;
	delete worldToLocalMatrix;
	worldToLocalMatrix = nullptr;
	delete anchorMax;
	anchorMax = nullptr;
	delete anchorMin;
	anchorMin = nullptr;
	delete pivot;
	pivot = nullptr;
	delete sizeDelta;
	sizeDelta = nullptr;
}

void ComponentTransform2D::Update()
{

}

void ComponentTransform2D::SaveOptions(Json& meta)
{

}

void ComponentTransform2D::LoadOptions(Json& meta)
{

}