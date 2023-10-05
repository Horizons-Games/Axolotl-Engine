#include "StdAfx.h"

#include "ComponentButton.h"
#include "Application.h"
#include "FileSystem/Json.h"
#include "ModuleScene.h"

#include "Auxiliar/ConnectedCallback.h"

ComponentButton::ComponentButton(bool active, GameObject* owner) :
	Component(ComponentType::BUTTON, active, owner, true),
	colorClicked(0.5f, 0.5f, 0.5f, 1.0f),
	colorHovered(0.7f, 0.7f, 0.7f, 1.0f),
	clicked(false),
	hovered(false),
	sceneName(std::string())
{
}

ComponentButton::~ComponentButton()
{
}

void ComponentButton::InternalSave(Json& meta)
{
	meta["colorHovered_x"] = static_cast<float>(colorHovered.x);
	meta["colorHovered_y"] = static_cast<float>(colorHovered.y);
	meta["colorHovered_z"] = static_cast<float>(colorHovered.z);
	meta["colorHovered_w"] = static_cast<float>(colorHovered.w);

	meta["colorClicked_x"] = static_cast<float>(colorClicked.x);
	meta["colorClicked_y"] = static_cast<float>(colorClicked.y);
	meta["colorClicked_z"] = static_cast<float>(colorClicked.z);
	meta["colorClicked_w"] = static_cast<float>(colorClicked.w);

	meta["sceneName"] = sceneName.c_str();
}

void ComponentButton::InternalLoad(const Json& meta)
{
	colorHovered.x = static_cast<float>(meta["colorHovered_x"]);
	colorHovered.y = static_cast<float>(meta["colorHovered_y"]);
	colorHovered.z = static_cast<float>(meta["colorHovered_z"]);
	colorHovered.w = static_cast<float>(meta["colorHovered_w"]);

	colorClicked.x = static_cast<float>(meta["colorClicked_x"]);
	colorClicked.y = static_cast<float>(meta["colorClicked_y"]);
	colorClicked.z = static_cast<float>(meta["colorClicked_z"]);
	colorClicked.w = static_cast<float>(meta["colorClicked_w"]);

	sceneName = meta["sceneName"];
}

void ComponentButton::SignalDisable()
{
	clicked = false;
	hovered = false;
}

void ComponentButton::OnClicked()
{
	if (!sceneName.empty())
	{
		LOG_WARNING("Loading scenes through buttons is deprecated, please update your scripts so they use "
					"SetOnClickedCallback instead");
		App->GetModule<ModuleScene>()->SetSceneToLoad("Lib/Scenes/" + sceneName + ".axolotl");
	}
	if (onClickedCallback.has_value())
	{
		// invoke the callback
		onClickedCallback.value()();
	}
}

std::unique_ptr<ConnectedCallback> ComponentButton::SetOnClickedCallback(std::function<void(void)>&& callback)
{
	if (onClickedCallback.has_value())
	{
		LOG_WARNING("Overriding OnClickedCallback for component button owned by {}", GetOwner());
	}
	onClickedCallback = std::move(callback);
	return std::make_unique<ConnectedCallback>(
		[this]()
		{
			if (this != nullptr)
			{
				onClickedCallback = std::nullopt;
				LOG_VERBOSE("Disconnected callback from button owned by {}", GetOwner());
			}
			else
			{
				// Should never happen
				LOG_ERROR("Callback lifetime exceeded lifetime of component button, please review your usage");
			}
		});
}
