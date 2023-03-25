#include "ModuleUI.h"

#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Application.h"

ModuleUI::ModuleUI() 
{
};

ModuleUI::~ModuleUI() {
};

bool ModuleUI::Init()
{
	return true;
}

bool ModuleUI::Start()
{
	return true;
}

update_status ModuleUI::Update()
{
	std::vector<GameObject*> canvasScene = App->scene->GetLoadedScene()->GetSceneCanvas();

	for (GameObject* canvas : canvasScene)
	{
		for (GameObject* children : canvas->GetChildren())
		{
			DrawChildren(children);
		}
	}

	return update_status::UPDATE_CONTINUE;
}

void ModuleUI::DrawChildren(GameObject* gameObject)
{
	if (gameObject->IsEnabled())
	{
		gameObject->Draw();
		for (GameObject* children : gameObject->GetChildren())
		{
			DrawChildren(children);
		}
	}
}