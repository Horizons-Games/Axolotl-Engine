#include "HealthSystem.h"

#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"
#include "Scene/Scene.h"

REGISTERCLASS(HealthSystem);

HealthSystem::HealthSystem() : Script(), helloWorld("Hello World")
{
	REGISTER_FIELD(HelloWorld, std::string);
}

void HealthSystem::Update(float deltaTime)
{
	// Print "Hello World" each frame
	ENGINE_LOG("%s", helloWorld.c_str());
}

std::string HealthSystem::GetHelloWorld() const
{
	return helloWorld;
}

void HealthSystem::SetHelloWorld(const std::string& helloWorld)
{
	this->helloWorld = helloWorld;
}