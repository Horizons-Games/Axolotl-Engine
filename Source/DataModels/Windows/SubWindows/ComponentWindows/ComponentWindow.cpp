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
	this->component = nullptr;
}

std::unique_ptr<ComponentWindow> ComponentWindow::CreateWindowForComponent(Component* component)
{
	if (component)
	{
		switch (component->GetType())
		{
		case ComponentType::MATERIAL:
			return std::make_unique<WindowComponentMaterial>(static_cast<ComponentMaterial*>(component));
		case ComponentType::MESHRENDERER:
			return std::make_unique<WindowComponentMeshRenderer>(static_cast<ComponentMeshRenderer*>(component));
		case ComponentType::TRANSFORM:
			return std::make_unique<WindowComponentTransform>(static_cast<ComponentTransform*>(component));
		case ComponentType::CAMERA:
			return std::make_unique<WindowComponentCamera>(static_cast<ComponentCamera*>(component));
		case ComponentType::BOUNDINGBOX:
			return std::make_unique<WindowComponentBoundingBoxes>(static_cast<ComponentBoundingBoxes*>(component));
		case ComponentType::LIGHT:
		{
			ComponentLight* asLight = static_cast<ComponentLight*>(component);
			switch (asLight->GetLightType())
			{
			case LightType::AMBIENT:
				return std::make_unique<WindowComponentAmbient>(static_cast<ComponentAmbient*>(component));
			case LightType::DIRECTIONAL:
				return std::make_unique<WindowComponentDirLight>(static_cast<ComponentDirLight*>(component));
			case LightType::POINT:
				return std::make_unique<WindowComponentPointLight>(static_cast<ComponentPointLight*>(component));
			case LightType::SPOT:
				return std::make_unique<WindowComponentSpotLight>(static_cast<ComponentSpotLight*>(component));
			case LightType::UNKNOWN:
			default:
				return std::make_unique<WindowComponentLight>(asLight);
			}
		}
		}
	}
	return nullptr;
}

ComponentWindow::ComponentWindow(const std::string& name, Component* component) :
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
	if (component)
	{
		std::stringstream ss;
		ss << "##Enabled " << windowUUID;

		bool enable = component->GetActive();
		ImGui::Checkbox(ss.str().c_str(), &enable);

		(enable) ? component->Enable() : component->Disable();
	}
}

void ComponentWindow::DrawDeleteComponent()
{
	if (component)
	{
		std::stringstream ss;
		ss << "Remove Comp. " << windowUUID;

		if (ImGui::Button(ss.str().c_str(), ImVec2(90, 20)))
		{
			if (!App->scene->GetSelectedGameObject()->RemoveComponent(component))
			{
				assert(false && "Trying to delete a non-existing component");
			}
			this->component = nullptr;
		}
	}
}
