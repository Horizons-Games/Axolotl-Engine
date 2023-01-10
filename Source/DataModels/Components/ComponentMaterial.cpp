#include "ComponentMaterial.h"
#include "GameObject/GameObject.h"

ComponentMaterial::ComponentMaterial(bool active, GameObject* owner)
	: Component(ComponentType::MATERIAL, active, owner)
{
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::Update()
{
	// TODO: Perform Material updates (draw...)
}

void ComponentMaterial::Display()
{
}