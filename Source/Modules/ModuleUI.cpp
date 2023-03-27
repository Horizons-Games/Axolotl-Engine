#include "ModuleUI.h"

#include "ModuleScene.h"

#include "Scene/Scene.h"
#include "Components/UI/ComponentCanvas.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"

#include "GL/glew.h"

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
	int width, height;
	SDL_GetWindowSize(App->window->GetWindow(), &width, &height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);

	App->camera->GetCamera()->GetFrustum()->SetOrthographic(width, height);

	glDisable(GL_DEPTH_TEST);

	for (GameObject* canvas : canvasScene)
	{
		if (canvas->IsEnabled())
		{
			for (GameObject* children : canvas->GetChildren())
			{
				DrawChildren(children);
			}
		}
	}

	glEnable(GL_DEPTH_TEST);
	App->camera->GetCamera()->GetFrustum()->
		SetHorizontalFovAndAspectRatio(math::DegToRad(90), App->camera->GetCamera()->GetAspectRatio());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);

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

void ModuleUI::RecalculateCanvasSizeAndScreenFactor()
{
	std::vector<GameObject*> canvasScene = App->scene->GetLoadedScene()->GetSceneCanvas();
	for (GameObject* canvas : canvasScene)
	{
		((ComponentCanvas*)(canvas->GetComponent(ComponentType::CANVAS)))->RecalculateSizeAndScreenFactor();
	}
}
