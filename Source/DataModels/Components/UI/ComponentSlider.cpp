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
			float3 handlePosition = handleTransform->GetPosition();
			float motion;

			if (insideBackground)
			{
				motion = handlePosition.x + (point.x - centerWorldPoint);
			}
			else
			{
				backgroundTransform->GetWorldAABB().minPoint.x > point.x
					? motion = handlePosition.x + (backgroundTransform->GetWorldAABB().minPoint.x - centerWorldPoint)
					: motion = handlePosition.x + (backgroundTransform->GetWorldAABB().maxPoint.x - centerWorldPoint);
			}

			handleTransform->SetPosition(float3(motion, handlePosition.y, handlePosition.z));
			handleTransform->CalculateMatrices();
			OnHandleDragged();
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
	if (fill != nullptr)
	{
		fill->GetComponent<ComponentImage>()->SetRenderPercentage(CalculateNormalizedValue());
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
