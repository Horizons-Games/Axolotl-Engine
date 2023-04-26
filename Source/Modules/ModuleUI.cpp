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
#include "Components/UI/ComponentImage.h"
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
	for (Component* interactable : App->GetModule<ModuleScene>()->GetLoadedScene()->GetSceneInteractable())
	{
		ComponentButton* button = static_cast<ComponentButton*>(interactable);
		ComponentTransform2D* transform =
			static_cast<ComponentTransform2D*>(interactable->GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
		AABB2D aabb2d = transform->GetWorldAABB();
		float2 point = App->GetModule<ModuleInput>()->GetMousePosition();
#ifdef ENGINE
		point = Physics::ScreenToScenePosition(App->GetModule<ModuleInput>()->GetMousePosition());
#endif // ENGINE
		if (aabb2d.Contains(point))
		{
			button->SetHovered(true);
			if (App->GetModule<ModuleInput>()->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::DOWN)
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

	std::vector<ComponentCanvas*> canvasScene = App->GetModule<ModuleScene>()->GetLoadedScene()->GetSceneCanvas();
	int width, height;
	SDL_GetWindowSize(App->GetModule<ModuleWindow>()->GetWindow(), &width, &height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);

	App->GetModule<ModuleCamera>()->GetCamera()->GetFrustum()->SetOrthographic(static_cast<float>(width), static_cast<float>(height));

	glDisable(GL_DEPTH_TEST);

	for (ComponentCanvas* canvas : canvasScene)
	{
		GameObject* owner = canvas->GetOwner();
		if (owner->IsEnabled())
		{
			for (GameObject* child : owner->GetChildren())
			{
				//ugh, should look for a better way, but it's 2AM
				for (ComponentImage* image : child->GetComponentsByType<ComponentImage>(ComponentType::IMAGE))
				{
					image->Draw();
				}
			}
		}
	}

	glEnable(GL_DEPTH_TEST);
	App->GetModule<ModuleCamera>()->GetCamera()->GetFrustum()->
		SetHorizontalFovAndAspectRatio(math::DegToRad(90), App->GetModule<ModuleCamera>()->GetCamera()->GetAspectRatio());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleUI::PostUpdate()
{
	for (Component* interactable : App->GetModule<ModuleScene>()->GetLoadedScene()->GetSceneInteractable())
	{
		ComponentButton* button = static_cast<ComponentButton*>(interactable);
		if(button->IsClicked())
		{
			if (App->GetModule<ModuleInput>()->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::UP)
			{
#ifndef ENGINE
				button->OnClicked();
#endif // ENGINE
				//button->SetHovered(false);
				button->SetClicked(false);
			}
		}
	}
	return update_status::UPDATE_CONTINUE;
}

void ModuleUI::RecalculateCanvasSizeAndScreenFactor()
{
	std::vector<ComponentCanvas*> canvasScene = App->GetModule<ModuleScene>()->GetLoadedScene()->GetSceneCanvas();
	for (ComponentCanvas* canvas : canvasScene)
	{
		canvas->RecalculateSizeAndScreenFactor();
	}

	for (Component* interactable : App->GetModule<ModuleScene>()->GetLoadedScene()->GetSceneInteractable())
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