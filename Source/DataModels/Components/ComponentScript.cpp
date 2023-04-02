#include "ComponentScript.h"


ComponentScript::ComponentScript(bool active, GameObject* owner): Component(ComponentType::SCRIPT, active, owner, false)
{
	camera = std::make_unique <CameraGameObject>();
	camera->Init();
	camera->SetViewPlaneDistance(DEFAULT_GAMEOBJECT_FRUSTUM_DISTANCE);
	Update();
}



void ComponentScript::Init()
{
	
}


void ComponentScript::Update()
{
}
