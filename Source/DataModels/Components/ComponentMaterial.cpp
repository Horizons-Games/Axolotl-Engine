<<<<<<< HEAD
/*#include "ComponentMaterial.h"
=======
#include "ComponentMaterial.h"
#include "GameObject/GameObject.h"
>>>>>>> origin/scenemanagement

ComponentMaterial::ComponentMaterial(bool active, GameObject* owner)
	: Component(ComponentType::MATERIAL, active, owner)
{
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::Init()
{
}

void ComponentMaterial::Update()
{
	// TODO: Perform Material updates (draw...)
<<<<<<< HEAD
}*/
=======
}

void ComponentMaterial::Display()
{
}
>>>>>>> origin/scenemanagement
