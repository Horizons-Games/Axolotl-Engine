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
	handle(nullptr),
	wasClicked(false)
{
}

ComponentSlider::~ComponentSlider()
{
}

void ComponentSlider::Update()
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
		if (!wasClicked)
		{
			actualClickHandlePosition = point;
			wasClicked = true;
		}
		else
		{
			if (actualClickHandlePosition.x != point.x)
			{
				ComponentTransform2D* handleTransform = handle->GetComponent<ComponentTransform2D>();
				ComponentTransform2D* backgroundTransform = background->GetComponent<ComponentTransform2D>();
				float3 handlePosition = handleTransform->GetPosition();
				float3 backgroundPosition = backgroundTransform->GetGlobalPosition();
				float motion = point.x - actualClickHandlePosition.x;
				handleTransform->SetPosition(float3(handlePosition.x + motion, handlePosition.y, handlePosition.z));
				handleTransform->CalculateMatrices();
				actualClickHandlePosition = point;
				OnHandleDragged();
			}
		}
	}
	else if (wasClicked)
	{
		wasClicked = false;
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
