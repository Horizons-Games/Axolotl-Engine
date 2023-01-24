#include "ComponentWindow.h"

#include <sstream>

#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentAmbient.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentBoundingBoxes.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentCamera.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentDirLight.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentLight.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentMaterial.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentMeshRenderer.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentPointLight.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentSpotLight.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentTransform.h"

#include "Application.h"
#include "ModuleScene.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentLight.h"
#include "Components/ComponentAmbient.h"
#include "Components/ComponentBoundingBoxes.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentDirLight.h"
#include "Components/ComponentMaterial.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentPointLight.h"
#include "Components/ComponentSpotLight.h"
#include "Components/ComponentTransform.h"

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
			return std::make_unique<WindowComponentMaterial>
				(std::static_pointer_cast<ComponentMaterial>(asShared));
		case ComponentType::MESHRENDERER:
			return std::make_unique<WindowComponentMeshRenderer>
				(std::static_pointer_cast<ComponentMeshRenderer>(asShared));
		case ComponentType::TRANSFORM:
			/*return std::make_unique<WindowComponentTransform>
				(std::static_pointer_cast<ComponentTransform>(asShared));*/
			return nullptr;
		case ComponentType::CAMERA:
			return std::make_unique<WindowComponentCamera>
				(std::static_pointer_cast<ComponentCamera>(asShared));
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
				return std::make_unique<WindowComponentDirLight>
					(std::static_pointer_cast<ComponentDirLight>(asLight));
			case LightType::POINT:
				return std::make_unique<WindowComponentPointLight>
					(std::static_pointer_cast<ComponentPointLight>(asLight));
			case LightType::SPOT:
				return std::make_unique<WindowComponentSpotLight>
					(std::static_pointer_cast<ComponentSpotLight>(asLight));
			case LightType::UNKNOWN:
			default:
				return nullptr;
			}
		}
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
