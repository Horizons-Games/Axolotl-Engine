#include "StdAfx.h"

#include "ComponentSlider.h"
#include "ComponentTransform2D.h"
#include "ComponentButton.h"
#include "ComponentImage.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Physics/Physics.h"

#include "GameObject/GameObject.h"

#include "FileSystem/Json.h"

ComponentSlider::ComponentSlider(bool active, GameObject* owner) :
	Component(ComponentType::SLIDER, active, owner, true),
	background(nullptr),
	fill(nullptr),
	handle(nullptr)
{
}

ComponentSlider::~ComponentSlider()
{
}

void ComponentSlider::CheckSlider()
{
	if (background == nullptr || handle == nullptr || fill == nullptr) return;

	ComponentButton* button = handle->GetComponentInternal<ComponentButton>();
	if (button && button->IsClicked())
	{
		
#ifdef ENGINE
		float2 point = Physics::ScreenToScenePosition(App->GetModule<ModuleInput>()->GetMousePosition());
#else
		float2 point = App->GetModule<ModuleInput>()->GetMousePosition();
#endif
		ComponentTransform2D* handleTransform = handle->GetComponentInternal<ComponentTransform2D>();
		float centerWorldPoint =
			(handleTransform->GetWorldAABB().maxPoint.x + handleTransform->GetWorldAABB().minPoint.x) / 2.0f;

		if (centerWorldPoint != point.x)
		{
			ComponentTransform2D* backgroundTransform = background->GetComponentInternal<ComponentTransform2D>();
			bool insideBackground;
			float newValue;
			if (direction == DirectionSlider::LEFT_TO_RIGHT || direction == DirectionSlider::RIGHT_TO_LEFT)
			{
				insideBackground = backgroundTransform->GetWorldAABB().maxPoint.x >= point.x &&
					backgroundTransform->GetWorldAABB().minPoint.x <= point.x;

				if (insideBackground)
				{
					newValue = point.x;
				}
				else
				{
					backgroundTransform->GetWorldAABB().minPoint.x > point.x
						? newValue = backgroundTransform->GetWorldAABB().minPoint.x
						: newValue = backgroundTransform->GetWorldAABB().maxPoint.x;
				}
			}
			else
			{
				insideBackground = backgroundTransform->GetWorldAABB().maxPoint.y >= point.y &&
								   backgroundTransform->GetWorldAABB().minPoint.y <= point.y;

				if (insideBackground)
				{
					newValue = point.y;
				}
				else
				{
					backgroundTransform->GetWorldAABB().minPoint.y > point.y
						? newValue = backgroundTransform->GetWorldAABB().minPoint.y
						: newValue = backgroundTransform->GetWorldAABB().maxPoint.y;
				}
			}
			
			float normalizedValue = 0.f;
			switch (direction)
			{
				case DirectionSlider::LEFT_TO_RIGHT:
					normalizedValue = (newValue - backgroundTransform->GetWorldAABB().minPoint.x) /
					(backgroundTransform->GetWorldAABB().maxPoint.x - backgroundTransform->GetWorldAABB().minPoint.x);
					break;
				case DirectionSlider::RIGHT_TO_LEFT:
					normalizedValue = (newValue - backgroundTransform->GetWorldAABB().maxPoint.x) /
					(backgroundTransform->GetWorldAABB().minPoint.x - backgroundTransform->GetWorldAABB().maxPoint.x);
					break;
				case DirectionSlider::DOWN_TO_TOP:
					normalizedValue = (newValue - backgroundTransform->GetWorldAABB().minPoint.y) /
									  (backgroundTransform->GetWorldAABB().maxPoint.y -
									   backgroundTransform->GetWorldAABB().minPoint.y);
					break;
				case DirectionSlider::TOP_TO_DOWN:
					normalizedValue = (newValue - backgroundTransform->GetWorldAABB().maxPoint.y) /
									  (backgroundTransform->GetWorldAABB().minPoint.y -
									   backgroundTransform->GetWorldAABB().maxPoint.y);
					break;
				default:
					break;
			}

			ModifyCurrentValue(normalizedValue * (maxValue - minValue));
		}
	}
}

void ComponentSlider::InternalSave(Json& meta)
{
	SaveGameObject(meta, "background", background);
	SaveGameObject(meta, "fill", fill);
	SaveGameObject(meta, "handle", handle);

	meta["minValue"] = static_cast<float>(minValue);
	meta["maxValue"] = static_cast<float>(maxValue);
	meta["currentValue"] = static_cast<float>(currentValue);
	meta["direction"] = static_cast<int>(direction);
}

void ComponentSlider::InternalLoad(const Json& meta)
{
	background = LoadGameObject(meta, "background");
	fill = LoadGameObject(meta, "fill");
	handle = LoadGameObject(meta, "handle");

	minValue = static_cast<float>(meta["minValue"]);
	maxValue = static_cast<float>(meta["maxValue"]);
	currentValue = static_cast<float>(meta["currentValue"]);
	direction = static_cast<DirectionSlider>(static_cast<int>(meta["direction"]));
}

void ComponentSlider::SetMaxValue(float maxValue)
{
	this->maxValue = maxValue;
	if (currentValue > maxValue)
	{
		currentValue = maxValue;
	}
	OnHandleDragged();
}

void ComponentSlider::SetMinValue(float minValue)
{
	this->minValue = minValue;
	if (currentValue < minValue)
	{
		currentValue = minValue;
	}
	OnHandleDragged();
}

void ComponentSlider::SetDirection(int direction)
{
	this->direction = static_cast<DirectionSlider>(direction);
	if (fill != nullptr)
	{
		fill->GetComponentInternal<ComponentImage>()->SetDirection(direction);
	}
	OnHandleDragged();
}

void ComponentSlider::ModifyCurrentValue(float currentValue)
{
	this->currentValue = currentValue;
	OnHandleDragged();
}

void ComponentSlider::OnHandleDragged()
{
	float normalizedValue = CalculateNormalizedValue();
	if (handle != nullptr && background != nullptr)
	{
		ComponentTransform2D* backgroundTransform = background->GetComponentInternal<ComponentTransform2D>();
		ComponentTransform2D* handleTransform = handle->GetComponentInternal<ComponentTransform2D>();
		
		float3 handlePosition = handleTransform->GetPosition();
		float centerWorldPoint;

		switch (direction)
		{
			case DirectionSlider::LEFT_TO_RIGHT:
				handlePosition.x += normalizedValue * (backgroundTransform->GetWorldAABB().maxPoint.x -
											backgroundTransform->GetWorldAABB().minPoint.x) +
						 backgroundTransform->GetWorldAABB().minPoint.x;
				centerWorldPoint = (handleTransform->GetWorldAABB().maxPoint.x + handleTransform->GetWorldAABB().minPoint.x) / 2.0f;
				handlePosition.x -= centerWorldPoint;
				break;
			case DirectionSlider::RIGHT_TO_LEFT:
				handlePosition.x += normalizedValue * (backgroundTransform->GetWorldAABB().minPoint.x -
											backgroundTransform->GetWorldAABB().maxPoint.x) +
						 backgroundTransform->GetWorldAABB().maxPoint.x;
				centerWorldPoint =
					(handleTransform->GetWorldAABB().maxPoint.x + handleTransform->GetWorldAABB().minPoint.x) / 2.0f;
				handlePosition.x -= centerWorldPoint;
				break;
			case DirectionSlider::DOWN_TO_TOP:
				handlePosition.y += normalizedValue * (backgroundTransform->GetWorldAABB().minPoint.y -
											backgroundTransform->GetWorldAABB().maxPoint.y) +
						 backgroundTransform->GetWorldAABB().maxPoint.y;
				centerWorldPoint =
					(handleTransform->GetWorldAABB().maxPoint.y + handleTransform->GetWorldAABB().minPoint.y) / 2.0f;
				handlePosition.y -= centerWorldPoint;
			case DirectionSlider::TOP_TO_DOWN:
				handlePosition.y += normalizedValue * (backgroundTransform->GetWorldAABB().maxPoint.y -
											backgroundTransform->GetWorldAABB().minPoint.y) +
						 backgroundTransform->GetWorldAABB().minPoint.y;
				centerWorldPoint =
					(handleTransform->GetWorldAABB().maxPoint.y + handleTransform->GetWorldAABB().minPoint.y) / 2.0f;
				handlePosition.y -= centerWorldPoint;
			default:
				break;
		}
		handleTransform->SetPosition(handlePosition);
		handleTransform->CalculateMatrices();
	}
	
	if (fill != nullptr)
	{
		fill->GetComponentInternal<ComponentImage>()->SetRenderPercentage(normalizedValue);
	}
}

void ComponentSlider::SaveGameObject(Json& meta, const char* name, GameObject* go)
{
	if (go != nullptr)
	{
		meta[name] = go->GetUID();
	}
	else
	{
		meta[name] = 0;
	}
}

GameObject* ComponentSlider::LoadGameObject(const Json& meta, const char* name)
{
	UID uid = meta[name];
	if (uid != 0)
	{
		UID newUID;
		if (App->GetModule<ModuleScene>()->HasNewUID(uid, newUID))
		{
			return App->GetModule<ModuleScene>()->GetLoadedScene()->SearchGameObjectByID(newUID);
		}
		else
		{
			return App->GetModule<ModuleScene>()->GetLoadedScene()->SearchGameObjectByID(uid);
		}
	}

	return nullptr;
}

void ComponentSlider::SetFill(GameObject* fill, int direction)
{
	if (direction != -1)
	{
		fill->GetComponentInternal<ComponentImage>()->SetDirection(direction);
	}
	if (fill != nullptr)
	{
		fill->GetComponentInternal<ComponentImage>()->SetRenderPercentage(CalculateNormalizedValue());
	}
	this->fill = fill;
}