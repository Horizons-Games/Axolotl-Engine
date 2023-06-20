#include "ModuleUI.h"

#include "ModuleScene.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "Scene/Scene.h"

#include "Components/UI/ComponentButton.h"
#include "Components/UI/ComponentCanvas.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentTransform2D.h"
#include "GL/glew.h"
#include "Physics/Physics.h"

#ifdef ENGINE
	#include "Modules/ModuleEditor.h"
	#include "DataModels/Windows/EditorWindows/WindowScene.h"
#endif // ENGINE

ModuleUI::ModuleUI(){};

ModuleUI::~ModuleUI(){};

bool ModuleUI::Init()
{
	LoadVBO();
	CreateVAO();
	return true;
}

update_status ModuleUI::Update()
{
	std::vector<ComponentCanvas*> canvasScene = App->GetModule<ModuleScene>()->GetLoadedScene()->GetSceneCanvas();

#ifdef ENGINE
	float2 point = Physics::ScreenToScenePosition(App->GetModule<ModuleInput>()->GetMousePosition());
#else
	float2 point = App->GetModule<ModuleInput>()->GetMousePosition();
#endif

	bool leftClickDown = App->GetModule<ModuleInput>()->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::DOWN;

	for (const ComponentCanvas* canvas : canvasScene)
	{
		const GameObject* canvasGameObject = canvas->GetOwner();
		DetectInteractionWithGameObject(
			canvasGameObject, point, leftClickDown, !canvasGameObject->GetParent()->IsEnabled());
	}

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

	for (const ComponentCanvas* canvas : canvasScene)
	{
		Draw2DGameObject(canvas->GetOwner());
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
		if (button->IsClicked())
		{
			if (App->GetModule<ModuleInput>()->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::UP)
			{
#ifndef ENGINE
				button->OnClicked();
#endif // ENGINE
	   // button->SetHovered(false);
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
		ComponentTransform2D* transform = interactable->GetOwner()->GetComponent<ComponentTransform2D>();
		transform->CalculateWorldBoundingBox();
	}
}

void ModuleUI::LoadVBO()
{
	float vertices[] = { // positions
						 -0.5, 0.5,	 0.0f, 1.0f, -0.5, -0.5, 0.0f, 0.0f, 0.5,  -0.5, 1.0f, 0.0f,
						 0.5,  -0.5, 1.0f, 0.0f, 0.5,  0.5,	 1.0f, 1.0f, -0.5, 0.5,	 0.0f, 1.0f
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
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);

	glBindVertexArray(0);
}

void ModuleUI::DetectInteractionWithGameObject(const GameObject* gameObject,
											   float2 mousePosition,
											   bool leftClicked,
											   bool disabledHierarchy)
{
	if (!gameObject->IsEnabled())
	{
		disabledHierarchy = true;
	}

	for (ComponentButton* button : gameObject->GetComponents<ComponentButton>())
	{
		if (disabledHierarchy
#ifdef ENGINE
			|| !App->GetModule<ModuleEditor>()->GetScene()->IsFocused()
#endif // ENGINE
		)
		{
			button->SetHovered(false);
			button->SetClicked(false);
		}
		else if (button->IsEnabled())
		{
			const ComponentTransform2D* transform = button->GetOwner()->GetComponent<ComponentTransform2D>();

			AABB2D aabb2d = transform->GetWorldAABB();

			if (aabb2d.Contains(mousePosition))
			{
				button->SetHovered(true);
				if (leftClicked)
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
	}

	for (const GameObject* child : gameObject->GetChildren())
	{
		DetectInteractionWithGameObject(child, mousePosition, leftClicked, disabledHierarchy);
	}
}

void ModuleUI::Draw2DGameObject(const GameObject* gameObject)
{
	if (gameObject->IsEnabled())
	{
		for (const ComponentImage* image : gameObject->GetComponents<ComponentImage>())
		{
			if (image->IsEnabled())
			{
				image->Draw();
			}
		}

		for (const GameObject* child : gameObject->GetChildren())
		{
			Draw2DGameObject(child);
		}
	}
}