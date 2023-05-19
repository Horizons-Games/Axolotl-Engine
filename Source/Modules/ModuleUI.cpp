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

update_status ModuleUI::Update()
{
	ModuleInput* input = App->GetModule<ModuleInput>();
	ModuleScene* scene = App->GetModule<ModuleScene>();
	Scene* loadedScene = scene->GetLoadedScene();

	for (Component* interactable : loadedScene->GetSceneInteractable())
	{
		ComponentButton* button = static_cast<ComponentButton*>(interactable);
		ComponentTransform2D* transform =
			static_cast<ComponentTransform2D*>(interactable->GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
		AABB2D aabb2d = transform->GetWorldAABB();
		float2 point = input->GetMousePosition();
#ifdef ENGINE
		point = Physics::ScreenToScenePosition(input->GetMousePosition());
#endif // ENGINE
		if (aabb2d.Contains(point))
		{
			button->SetHovered(true);
			if (input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::DOWN)
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

	std::vector<ComponentCanvas*> canvasScene = loadedScene->GetSceneCanvas();
	int width, height;
	SDL_GetWindowSize(App->GetModule<ModuleWindow>()->GetWindow(), &width, &height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, height, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);

	ModuleCamera* moduleCamera = App->GetModule<ModuleCamera>();
	Camera* camera = moduleCamera->GetCamera();
	Frustum* frustum = camera->GetFrustum();

	frustum->SetOrthographic(static_cast<float>(width), static_cast<float>(height));

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
	frustum->SetHorizontalFovAndAspectRatio(math::DegToRad(90), camera->GetAspectRatio());

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
	ModuleScene* scene = App->GetModule<ModuleScene>();
	Scene* loadedScene = scene->GetLoadedScene();
	std::vector<ComponentCanvas*> canvasScene = loadedScene->GetSceneCanvas();

	for (ComponentCanvas* canvas : canvasScene)
	{
		canvas->RecalculateSizeAndScreenFactor();
	}

	for (Component* interactable : loadedScene->GetSceneInteractable())
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