#pragma once

#include "Scripting\Script.h"

class HealthSystem : public Script
{
public:
	HealthSystem();
	~HealthSystem() override = default;

	void Update(float deltaTime) override;

	std::string GetHelloWorld() const;
	void SetHelloWorld(const std::string & helloWorld);

private:
	std::string helloWorld;
};