#include "ComponentAgent.h"
#include "FileSystem/Json.h"

ComponentAgent::ComponentAgent(bool active, GameObject* owner) :
	Component(ComponentType::AGENT, active, owner, true)
{
}

ComponentAgent::~ComponentAgent()
{
}

void ComponentAgent::Update()
{
}

void ComponentAgent::InternalSave(Json& meta)
{
}

void ComponentAgent::InternalLoad(const Json& meta)
{
}