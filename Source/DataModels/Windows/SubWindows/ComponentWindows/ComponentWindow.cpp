#include "ComponentWindow.h"

#include <sstream>

#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentAmbient.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentBoundingBoxes.h"

#include "Application.h"
#include "ModuleScene.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentLight.h"
#include "Components/ComponentAmbient.h"
#include "Components/ComponentBoundingBoxes.h"

ComponentWindow::~ComponentWindow()
{
	this->component = std::weak_ptr<Component>();
}

std::unique_ptr<ComponentWindow> ComponentWindow::CreateWindowForComponent(const std::weak_ptr<Component>& component)
{
	std::shared_ptr<Component> asShared = component.lock();
	if (asShared)
	{
		switch (asShared->GetType())
		{
		case ComponentType::MATERIAL:
			return nullptr;
		case ComponentType::MESHRENDERER:
			return nullptr;
		case ComponentType::TRANSFORM:
			return nullptr;
		case ComponentType::CAMERA:
			return nullptr;
		case ComponentType::BOUNDINGBOX:
			return std::make_unique<WindowComponentBoundingBoxes>
				(std::static_pointer_cast<ComponentBoundingBoxes>(asShared));
		case ComponentType::LIGHT:
		{
			std::shared_ptr<ComponentLight> asLight = std::dynamic_pointer_cast<ComponentLight>(asShared);
			switch (asLight->GetLightType())
			{
			case LightType::AMBIENT:
				return std::make_unique<WindowComponentAmbient>
					(std::static_pointer_cast<ComponentAmbient>(asLight));
			case LightType::DIRECTIONAL:
				return nullptr;
			case LightType::POINT:
				return nullptr;
			case LightType::SPOT:
				return nullptr;
			case LightType::UNKNOWN:
			default:
				return nullptr;
			}
		}
		case ComponentType::UNKNOWN:
		default:
			return nullptr;
		}
	}
	return nullptr;
}

ComponentWindow::ComponentWindow(const std::string& name, const std::weak_ptr<Component>& component) :
	SubWindow(name),
	component(component),
	windowUUID(UniqueID::GenerateUUID())
{
	flags |= ImGuiTreeNodeFlags_DefaultOpen;
}

void ComponentWindow::DrawEnableAndDeleteComponent()
{
	DrawEnableComponent();
	ImGui::SameLine();
	DrawDeleteComponent();
	ImGui::SameLine();
}

void ComponentWindow::DrawEnableComponent()
{
	std::shared_ptr<Component> asShared = component.lock();
	if (asShared)
	{
		std::stringstream ss;
		ss << "##Enabled " << windowUUID;

		bool enable = asShared->GetActive();
		ImGui::Checkbox(ss.str().c_str(), &enable);

		(enable) ? asShared->Enable() : asShared->Disable();
	}
}

void ComponentWindow::DrawDeleteComponent()
{
	std::shared_ptr<Component> asShared = component.lock();
	if (asShared)
	{
		std::stringstream ss;
		ss << "Remove Comp. " << windowUUID;

		if (ImGui::Button(ss.str().c_str(), ImVec2(90, 20)))
		{
			if (!App->scene->GetSelectedGameObject().lock()->RemoveComponent(asShared))
			{
				assert(false && "Trying to delete a non-existing component");
			}
		}
	}
}
