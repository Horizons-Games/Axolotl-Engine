#include "HealthSystem.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

REGISTERCLASS(HealthSystem);

HealthSystem::HealthSystem() : Script(), health(100)
{
	REGISTER_FIELD(Health, float);
}

void HealthSystem::Start()
{
	maxHealth = health;
}

void HealthSystem::Update(float deltaTime)
{
	if (health <= 0) {
		ENGINE_LOG("Murió :(");
	}
}


float HealthSystem::GetHealth() const
{
	return health;
}

void HealthSystem::SetHealth(float health) 
{
	this->health = health;
}