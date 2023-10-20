#pragma once

#include "Scripting\Script.h"

class ForceZoneScript : public Script
{
public:
	ForceZoneScript();
	~ForceZoneScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

	float GetInfluenceRadius() const;

private:
	float influenceRadius;
	float3 position;

};
