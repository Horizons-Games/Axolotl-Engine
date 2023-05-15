#include "ComponentWindow.h"

#include <sstream>

#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentCamera.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentDirLight.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentLight.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentMeshRenderer.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentPointLight.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentSpotLight.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentTransform.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentTransform2D.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentPlayer.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentAnimation.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentCanvas.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentImage.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentButton.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentRigidBody.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentMockStates.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentAudioSource.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentAudioListener.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentMeshCollider.h"
#include "DataModels/Windows/SubWindows/ComponentWindows/WindowComponentScript.h"
#include "WindowComponentBreakable.h"

#include "Application.h"
#include "ModuleScene.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentDirLight.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentPointLight.h"
#include "Components/ComponentSpotLight.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentAnimation.h"
#include "Components/UI/ComponentCanvas.h"
#include "Components/UI/ComponentImage.h"
#include "Components/UI/ComponentButton.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentMockState.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAudioListener.h"
#include "Components/ComponentMeshCollider.h"
#include "Components/ComponentScript.h"
#include "ComponentBreakable.h"

#include "ComponentWindow.h"
#include "ModuleCommand.h"
#include "Commands/CommandComponentEnabled.h"

ComponentWindow::~ComponentWindow()
{
	component = nullptr;
}

std::unique_ptr<ComponentWindow> ComponentWindow::CreateWindowForComponent(Component* component)
{
	if (component)
	{
		switch (component->GetType())
		{
		case ComponentType::MESHRENDERER:
			return std::make_unique<WindowComponentMeshRenderer>(static_cast<ComponentMeshRenderer*>(component));
		case ComponentType::TRANSFORM2D:
			return std::make_unique<WindowComponentTransform2D>(static_cast<ComponentTransform2D*>(component));
		case ComponentType::TRANSFORM:
			return std::make_unique<WindowComponentTransform>(static_cast<ComponentTransform*>(component));
		case ComponentType::CAMERA:
			return std::make_unique<WindowComponentCamera>(static_cast<ComponentCamera*>(component));
		case ComponentType::PLAYER:
			return std::make_unique<WindowComponentPlayer>(static_cast<ComponentPlayer*>(component));
		case ComponentType::ANIMATION:
			return std::make_unique<WindowComponentAnimation>(static_cast<ComponentAnimation*>(component));
		case ComponentType::CANVAS:
			return std::make_unique<WindowComponentCanvas>(static_cast<ComponentCanvas*>(component));
		case ComponentType::IMAGE:
			return std::make_unique<WindowComponentImage>(static_cast<ComponentImage*>(component));
		case ComponentType::BUTTON:
			return std::make_unique<WindowComponentButton>(static_cast<ComponentButton*>(component));
		case ComponentType::RIGIDBODY:
			return std::make_unique<WindowComponentRigidBody>(static_cast<ComponentRigidBody*>(component));
		case ComponentType::AUDIOSOURCE:
			return std::make_unique<WindowComponentAudioSource>(static_cast<ComponentAudioSource*>(component));
		case ComponentType::AUDIOLISTENER:
			return std::make_unique<WindowComponentAudioListener>(static_cast<ComponentAudioListener*>(component));
		case ComponentType::MESHCOLLIDER:
			return std::make_unique<WindowComponentMeshCollider>(static_cast<ComponentMeshCollider*>(component));
		case ComponentType::MOCKSTATE:
			return std::make_unique<WindowComponentMockStates>(static_cast<ComponentMockState*>(component));
		case ComponentType::SCRIPT:
			return std::make_unique<WindowComponentScript>(static_cast<ComponentScript*>(component));
		case ComponentType::BREAKABLE:
			return std::make_unique<WindowComponentBreakable>(static_cast<ComponentBreakable*>(component));
		case ComponentType::LIGHT:
			ComponentLight* asLight = static_cast<ComponentLight*>(component);
			switch (asLight->GetLightType())
			{
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

		ImGui::Text("Enabled"); ImGui::SameLine();
		bool enable = component->IsEnabled();
		if (ImGui::Checkbox(ss.str().c_str(), &enable))
		{
			App->GetModule<ModuleCommand>()->CreateAndExecuteCommand<CommandComponentEnabled>(component, enable);
		}
	}
}

void ComponentWindow::DrawDeleteComponent()
{
	if (component && component->CanBeRemoved())
	{
		std::stringstream ss;
		ss << "Remove Comp. " << windowUUID;

		if (ImGui::Button(ss.str().c_str(), ImVec2(90, 20)))
		{
			if (!App->GetModule<ModuleScene>()->GetSelectedGameObject()->RemoveComponent(component))
			{
				assert(false && "Trying to delete a non-existing component");
			}
			component = nullptr;
		}
	}
}
