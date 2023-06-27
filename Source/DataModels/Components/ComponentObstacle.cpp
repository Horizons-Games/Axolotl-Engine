#include "ComponentObstacle.h"
#include "FileSystem/Json.h"

ComponentObstacle::ComponentObstacle(bool active, GameObject* owner) :
	Component(ComponentType::OBSTACLE, active, owner, true)
{
}

ComponentObstacle::~ComponentObstacle()
{
}

void ComponentObstacle::Update()
{
}

void ComponentObstacle::InternalSave(Json& meta)
{
}

void ComponentObstacle::InternalLoad(const Json& meta)
{
}