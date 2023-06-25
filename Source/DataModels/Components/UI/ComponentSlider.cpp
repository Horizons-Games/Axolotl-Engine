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

	ComponentButton* button = handle->GetComponent<ComponentButton>();
	if (button && button->IsClicked())
	{
		
#ifdef ENGINE
		float2 point = Physics::ScreenToScenePosition(App->GetModule<ModuleInput>()->GetMousePosition());
#else
		float2 point = App->GetModule<ModuleInput>()->GetMousePosition();
#endif
		ComponentTransform2D* handleTransform = handle->GetComponent<ComponentTransform2D>();
		float centerWorldPoint =
			(handleTransform->GetWorldAABB().maxPoint.x + handleTransform->GetWorldAABB().minPoint.x) / 2.0f;

		if (centerWorldPoint != point.x)
		{
			ComponentTransform2D* backgroundTransform = background->GetComponent<ComponentTransform2D>();
			bool insideBackground = backgroundTransform->GetWorldAABB().maxPoint.x >= point.x &&
									backgroundTransform->GetWorldAABB().minPoint.x <= point.x;
			float newValue;

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
			
			float normalizedValue =
				(newValue - backgroundTransform->GetWorldAABB().minPoint.x) /
				(backgroundTransform->GetWorldAABB().maxPoint.x - backgroundTransform->GetWorldAABB().minPoint.x);

			ModifyCurrentValue(normalizedValue * (maxValue - minValue));
		}
	}
}

void ComponentSlider::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = static_cast<bool>(active);
	meta["removed"] = static_cast<bool>(canBeRemoved);

	SaveGameObject(meta, "background", background);
	SaveGameObject(meta, "fill", fill);
	SaveGameObject(meta, "handle", handle);

	meta["minValue"] = minValue;
	meta["maxValue"] = maxValue;
	meta["currentValue"] = currentValue;
}

void ComponentSlider::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = static_cast<bool>(meta["active"]);
	canBeRemoved = static_cast<bool>(meta["removed"]);

	background = LoadGameObject(meta, "background");
	fill = LoadGameObject(meta, "fill");
	handle = LoadGameObject(meta, "handle");

	minValue = static_cast<float>(meta["minValue"]);
	maxValue = static_cast<float>(meta["maxValue"]);
	currentValue = static_cast<float>(meta["currentValue"]);
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

void ComponentSlider::ModifyCurrentValue(float currentValue)
{
	this->currentValue = currentValue;
	OnHandleDragged();
}

void ComponentSlider::OnHandleDragged()
{
	float normalizedValue = CalculateNormalizedValue();
	if (handle != nullptr)
	{
		ComponentTransform2D* backgroundTransform = background->GetComponent<ComponentTransform2D>();
		ComponentTransform2D* handleTransform = handle->GetComponent<ComponentTransform2D>();

		float newPos = normalizedValue * (backgroundTransform->GetWorldAABB().maxPoint.x -
										  backgroundTransform->GetWorldAABB().minPoint.x) +
					   backgroundTransform->GetWorldAABB().minPoint.x;

		float centerWorldPoint =
			(handleTransform->GetWorldAABB().maxPoint.x + handleTransform->GetWorldAABB().minPoint.x) / 2.0f;
		float3 handlePosition = handleTransform->GetPosition();
		newPos = handlePosition.x + newPos - centerWorldPoint;
		handleTransform->SetPosition(float3(newPos, handlePosition.y, handlePosition.z));
		handleTransform->CalculateMatrices();
	}
	
	if (fill != nullptr)
	{
		fill->GetComponent<ComponentImage>()->SetRenderPercentage(normalizedValue);
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

GameObject* ComponentSlider::LoadGameObject(Json& meta, const char* name)
{
	UID uid = meta[name];
	if (uid != 0)
	{
		UID newUID;
		if (App->GetModule<ModuleScene>()->hasNewUID(uid, newUID))
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
