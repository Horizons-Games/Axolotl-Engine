#include "StdAfx.h"

#include "Application.h"

#include "ModuleUI.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"

#include "Scene/Scene.h"

#include "Components/UI/ComponentButton.h"
#include "Components/UI/ComponentCanvas.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentVideo.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Components/UI/ComponentSlider.h"
#include "GL/glew.h"
#include "Physics/Physics.h"

#include "Camera/Camera.h"

#ifdef ENGINE
	#include "Modules/ModuleEditor.h"
	#include "DataModels/Windows/EditorWindows/WindowScene.h"
#endif // ENGINE

namespace
{
bool CompareButtonPositions(const ComponentButton* a, const ComponentButton* b)
{
	return a->GetOwner()->GetComponentInternal<ComponentTransform2D>()->GetPosition().y >
		   b->GetOwner()->GetComponentInternal<ComponentTransform2D>()->GetPosition().y;
}
} // namespace

namespace
{
constexpr const float cooldownTime = 0.15f;
} // namespace

ModuleUI::ModuleUI()
{
}

ModuleUI::~ModuleUI()
{
}

bool ModuleUI::Init()
{
	LoadVBO();
	CreateVAO();
	return true;
}

UpdateStatus ModuleUI::Update()
{
	if (App->GetModule<ModuleScene>()->IsLoading())
	{
		return UpdateStatus::UPDATE_CONTINUE;
	}

	std::vector<ComponentCanvas*> canvasScene = App->GetModule<ModuleScene>()->GetLoadedScene()->GetSceneCanvas();

#ifdef ENGINE
	float2 point = Physics::ScreenToScenePosition(App->GetModule<ModuleInput>()->GetMousePosition());
#else
	float2 point = App->GetModule<ModuleInput>()->GetMousePosition();
#endif
	ModuleInput* input = App->GetModule<ModuleInput>();
	ModuleScene* scene = App->GetModule<ModuleScene>();

	bool leftClickDown = input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::DOWN;

	if (input->GetCurrentInputMethod() == InputMethod::GAMEPAD)
	{
		leftClickDown = input->GetKey(SDL_SCANCODE_SPACE) == KeyState::DOWN;
	}

	lastButtonChange = lastButtonChange + App->GetDeltaTime();

	if (!sortedButtonsIds.empty() && lastButtonChange > cooldownTime)
	{
		JoystickDirection leftJoystickDirection = input->GetLeftJoystickDirection();

		int newIndex = static_cast<int>(currentButtonIndex);
		do
		{
			if (leftJoystickDirection.verticalDirection == JoystickVerticalDirection::FORWARD ||
				input->GetKey(SDL_SCANCODE_TAB) != KeyState::IDLE)
			{
				// We sum the size to avoid negative values, if this is not used we can not jump
				// from the first button to the last
				newIndex = (newIndex - 1 + sortedButtonsIds.size()) % static_cast<int>(sortedButtonsIds.size());
			}
			// When the current button is not enabled we keep looping until we find one enabled,
			// this avoids getting stuck in a disabled button when we change from a scene to another
			else if (leftJoystickDirection.verticalDirection == JoystickVerticalDirection::BACK ||
					 input->GetKey(SDL_SCANCODE_C) != KeyState::IDLE ||
					 !scene->GetLoadedScene()
						  ->SearchGameObjectByID(sortedButtonsIds[newIndex])
						  ->GetComponentInternal<ComponentButton>()
						  ->IsEnabled())
			{
				newIndex = (newIndex + 1) % sortedButtonsIds.size();
			}
		} while (newIndex != currentButtonIndex && !scene->GetLoadedScene()
														   ->SearchGameObjectByID(sortedButtonsIds[newIndex])
														   ->GetComponentInternal<ComponentButton>()
														   ->IsEnabled());
		currentButtonIndex = newIndex;
		lastButtonChange = 0.0f;
	}

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

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(std::strlen("Canvas Drawing")), 
		"Canvas Drawing");

	for (const ComponentCanvas* canvas : canvasScene)
	{
		Draw2DGameObject(canvas->GetOwner());
	}

	glPopDebugGroup();

	glEnable(GL_DEPTH_TEST);
	frustum->SetHorizontalFovAndAspectRatio(math::DegToRad(90), camera->GetAspectRatio());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleUI::PostUpdate()
{
	for (Component* interactable : App->GetModule<ModuleScene>()->GetLoadedScene()->GetSceneInteractable())
	{
		ComponentButton* button = static_cast<ComponentButton*>(interactable);
		ModuleInput* input = App->GetModule<ModuleInput>();

		bool leftClickUp = input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::UP;

		if (input->GetCurrentInputMethod() == InputMethod::GAMEPAD)
		{
			leftClickUp = input->GetKey(SDL_SCANCODE_SPACE) == KeyState::UP;
		}

		if (button->IsClicked())
		{
			if (leftClickUp)
			{
#ifndef ENGINE
				button->OnClicked();
#endif // ENGINE
	   // button->SetHovered(false);
				button->SetClicked(false);
			}
		}
	}
	return UpdateStatus::UPDATE_CONTINUE;
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
		ComponentTransform2D* transform = interactable->GetOwner()->GetComponentInternal<ComponentTransform2D>();
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

void ModuleUI::ResetCurrentButtonIndex()
{
	currentButtonIndex = 0;
}

void ModuleUI::SetUpButtons()
{
	auto filteredButtons = App->GetModule<ModuleScene>()->GetLoadedScene()->GetSceneInteractable()  |
						   std::views::transform(
							   [](const Component* comp)
							   {
								   return dynamic_cast<const ComponentButton*>(comp);
							   }) |
						   std::views::filter(
							   [](const ComponentButton* comp)
							   {
								   return comp != nullptr;
							   });

	std::vector<const ComponentButton*> sortedButtons =
		std::vector<const ComponentButton*>(std::begin(filteredButtons), std::end(filteredButtons));

	std::sort(std::begin(sortedButtons),
			  std::end(sortedButtons),
			  CompareButtonPositions);

	auto sortedButtonsIdsView = sortedButtons | std::views::transform(
													[](const ComponentButton* button)
													{
														return button->GetOwner()->GetUID();
													});

	sortedButtonsIds.clear();
	sortedButtonsIds = std::vector<UID>(std::begin(sortedButtonsIdsView), std::end(sortedButtonsIdsView));
}

void ModuleUI::DetectInteractionWithGameObject(const GameObject* gameObject,
											   float2 mouseCursor,
											   bool leftClicked,
											   bool disabledHierarchy)
{
	if (!gameObject->IsEnabled())
	{
		disabledHierarchy = true;
	}

	for (ComponentSlider* slider : gameObject->GetComponents<ComponentSlider>())
	{
		if (slider->IsEnabled())
		{
			slider->CheckSlider();
		}
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
			const ComponentTransform2D* transform = button->GetOwner()->GetComponentInternal<ComponentTransform2D>();
			AABB2D aabb2d = transform->GetWorldAABB();
			ModuleInput* input = App->GetModule<ModuleInput>();

			if (!sortedButtonsIds.empty())
			{
				if (input->GetCurrentInputMethod() == InputMethod::KEYBOARD && aabb2d.Contains(mouseCursor))
				{
					for (int i = 0; i < sortedButtonsIds.size(); ++i)
					{
						if (button->GetOwner()->GetUID() == sortedButtonsIds[i])
						{
							currentButtonIndex = i;
							break;
						}
					}

					button->SetHovered(true);
					if (leftClicked)
					{
						button->SetClicked(true);
					}
				}
				else if (input->GetCurrentInputMethod() == InputMethod::GAMEPAD &&
						 sortedButtonsIds[currentButtonIndex] == button->GetOwner()->GetUID())
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
					// button->SetClicked(false);
				}
			}
		}
	}

	for (const GameObject* child : gameObject->GetChildren())
	{
		DetectInteractionWithGameObject(child, mouseCursor, leftClicked, disabledHierarchy);
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

		for (ComponentVideo* video : gameObject->GetComponents<ComponentVideo>())
		{
			if (video->IsEnabled())
			{
				video->Draw();
				video->UpdateVideoFrame();
			}
		}

		for (const GameObject* child : gameObject->GetChildren())
		{
			Draw2DGameObject(child);
		}
	}
}