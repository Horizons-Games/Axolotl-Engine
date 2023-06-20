#include "ComponentSlider.h"
#include "ComponentTransform2D.h"
#include "ComponentButton.h"

#include "Application.h"
#include "ModuleInput.h"
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

}

void ComponentSlider::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = static_cast<bool>(meta["active"]);
	canBeRemoved = static_cast<bool>(meta["removed"]);
}

void ComponentSlider::ModifyCurrentValue(float currentValue)
{
	this->currentValue = currentValue;
	CalculateNormalizedValue();
	OnHandleDragged();
}

void ComponentSlider::OnHandleDragged()
{
	LOG_DEBUG("SLIDER MOVED");
}

void ComponentSlider::CalculateNormalizedValue()
{
	if (maxValue - minValue == 0)
	{
		normalizedValue = 0;
	}
	else
	{
		normalizedValue = (currentValue - minValue) / (maxValue - minValue);
	}
}
