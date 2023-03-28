#include "ModuleUI.h"

#include "ModuleScene.h"

#include "Scene/Scene.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"

#include "GL/glew.h"
#include "Physics/Physics.h"
#include "Components/ComponentBoundingBox2D.h"
#include "Components/UI/ComponentCanvas.h"
#include "Components/UI/ComponentButton.h"

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

	//For de objects selectable when click
	//Mantener click mientras no se hace release del botón?

	for (Component* interactable : App->scene->GetLoadedScene()->GetSceneInteractable())
	{
		ComponentButton* button = static_cast<ComponentButton*>(interactable);
		ComponentBoundingBox2D* boundingBox = static_cast<ComponentBoundingBox2D*>(interactable->GetOwner()->GetComponent(ComponentType::BOUNDINGBOX2D));
		AABB2D aabb2d = boundingBox->GetWorldAABB();
		float2 point = Physics::ScreenToScenePosition(App->input->GetMousePosition());
		if (aabb2d.Contains(point))
		{
			button->SetHovered(true);
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::DOWN)
			{
				button->SetClicked(true);
			}
		}
		else
		{
			button->SetHovered(false);
			button->SetClicked(false);
		}
	}

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

update_status ModuleUI::PostUpdate()
{
	for (Component* interactable : App->scene->GetLoadedScene()->GetSceneInteractable())
	{
		ComponentButton* button = static_cast<ComponentButton*>(interactable);
		if(button->IsClicked())
		{
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::UP)
			{
#ifndef ENGINE
				button->OnClicked();
#endif // ENGINE
				//BORRAR ESTE TODO POR DIOS
				button->OnClicked();
				button->SetHovered(false);
				button->SetClicked(false);
			}
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

void ModuleUI::RecalculateCanvasSizeAndScreenFactor()
{
	std::vector<GameObject*> canvasScene = App->scene->GetLoadedScene()->GetSceneCanvas();
	for (GameObject* canvas : canvasScene)
	{
		((ComponentCanvas*)(canvas->GetComponent(ComponentType::CANVAS)))->RecalculateSizeAndScreenFactor();
	}
}
