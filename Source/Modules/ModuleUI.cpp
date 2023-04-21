#include "ModuleUI.h"

#include "ModuleScene.h"

#include "Scene/Scene.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"

#include "GL/glew.h"
#include "Physics/Physics.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Components/UI/ComponentCanvas.h"
#include "Components/UI/ComponentButton.h"

ModuleUI::ModuleUI() 
{
};

ModuleUI::~ModuleUI() {
};

bool ModuleUI::Init()
{
	LoadVBO();
	CreateVAO();
	return true;
}

bool ModuleUI::Start()
{
	return true;
}

update_status ModuleUI::Update()
{
	for (Component* interactable : App->scene->GetLoadedScene()->GetSceneInteractable())
	{
		ComponentButton* button = static_cast<ComponentButton*>(interactable);
		ComponentTransform2D* transform = static_cast<ComponentTransform2D*>(interactable->GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
		AABB2D aabb2d = transform->GetWorldAABB();
		float2 point = App->input->GetMousePosition();
#ifdef ENGINE
		point = Physics::ScreenToScenePosition(App->input->GetMousePosition());
#endif // ENGINE
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

	App->camera->GetCamera()->GetFrustum()->SetOrthographic((float)width, (float)height);

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
	for (GameObject* canvas : App->scene->GetLoadedScene()->GetSceneCanvas())
	{
		((ComponentCanvas*)(canvas->GetComponent(ComponentType::CANVAS)))->RecalculateSizeAndScreenFactor();
	}

	for (Component* interactable : App->scene->GetLoadedScene()->GetSceneInteractable())
	{
		ComponentTransform2D* transform = 
			static_cast<ComponentTransform2D*>(interactable->GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
		transform->CalculateWorldBoundingBox();
	}
}

void ModuleUI::LoadVBO()
{
	float vertices[] = {
		// positions          
		-0.5,  0.5, 0.0f, 1.0f,
		-0.5, -0.5, 0.0f, 0.0f,
		 0.5, -0.5, 1.0f, 0.0f,
		 0.5, -0.5, 1.0f, 0.0f,
		 0.5,  0.5, 1.0f, 1.0f,
		-0.5,  0.5, 0.0f, 1.0f
	};

	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
}

void ModuleUI::CreateVAO()
{
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glBindVertexArray(0);
}