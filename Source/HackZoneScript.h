#pragma once

#include "Scripting\Script.h"
class HackZoneScript : public Script
{
public:
	HackZoneScript();
	~HackZoneScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;
};
