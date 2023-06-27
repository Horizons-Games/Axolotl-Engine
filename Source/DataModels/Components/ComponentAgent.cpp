#include "ComponentAgent.h"

#include "Application.h"
#include "ModuleNavigation.h"

#include "GameObject/GameObject.h"
#include "Resources/ResourceNavMesh.h"
#include "FileSystem/Json.h"

#include "DetourCrowd/DetourCrowd.h"

ComponentAgent::ComponentAgent(bool active, GameObject* owner) :
	Component(ComponentType::AGENT, active, owner, true)
{
}

ComponentAgent::~ComponentAgent()
{
	RemoveAgentFromCrowd();
}

void ComponentAgent::Update()
{
}

void ComponentAgent::RemoveAgentFromCrowd()
{
	shouldAddAgentToCrowd = false;

	/*if (App->GetModule<ModuleScene>()->GetLoadedScene() != GetOwner().scene)
		return;*/
	std::shared_ptr<ResourceNavMesh> navMesh = App->GetModule<ModuleNavigation>()->GetNavMesh();
	if (!navMesh->IsGenerated() || agentId == -1)
		return;

	navMesh->GetCrowd()->removeAgent(agentId);
	agentId = -1;
}

void ComponentAgent::InternalSave(Json& meta)
{
}

void ComponentAgent::InternalLoad(const Json& meta)
{
}