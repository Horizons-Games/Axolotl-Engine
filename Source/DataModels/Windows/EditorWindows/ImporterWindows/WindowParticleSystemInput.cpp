#include "WindowParticleSystemInput.h"

#include "Components/ComponentParticleSystem.h"
#include "Resources/ResourceParticleSystem.h"
#include "Application.h"
#include "FileSystem/ModuleResources.h"

WindowParticleSystemInput::WindowParticleSystemInput(ComponentParticleSystem* state) :
	WindowFileBrowser(), component(state)
{
	dialogName = "Select ParticleSystem";
	title = "Load ParticleSystem";
	filters = PARTICLESYSTEM_EXTENSION;
	startPath = "Assets/ParticleSystem";
}

WindowParticleSystemInput::~WindowParticleSystemInput()
{
}


void WindowParticleSystemInput::DoThisIfOk()
{
	if (component)
	{
		this->isLoading = false;
		std::string filePath = std::string(fileDialogImporter.GetFilePathName());
		std::shared_ptr<ResourceParticleSystem> particleSystem = App->GetModule<ModuleResources>()->RequestResource<ResourceParticleSystem>(filePath);
		component->SetResource(particleSystem);
	}
}


