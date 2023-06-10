#include "PlayerForceAttackScript.h"

REGISTERCLASS(PlayerForceAttackScript);

PlayerForceAttackScript::PlayerForceAttackScript() : Script()
{
	enemiesInTheArea.reserve(10);
}

PlayerForceAttackScript::~PlayerForceAttackScript()
{
}

void PlayerForceAttackScript::Start()
{
}

void PlayerForceAttackScript::Update(float deltaTime)
{
	const ModuleInput* input = App->GetModule<ModuleInput>();
}

void PlayerForceAttackScript::OnCollisionEnter(ComponentRigidBody* other)
{
	enemiesInTheArea.push_back(other);
}

void PlayerForceAttackScript::OnCollisionExit(ComponentRigidBody* other)
{
	for (std::vector<ComponentRigidBody*>::iterator it = enemiesInTheArea.begin(); it < enemiesInTheArea.end(); it++)
	{
		if (*it == other)
		{
			enemiesInTheArea.erase(it);
		}
	}
}
