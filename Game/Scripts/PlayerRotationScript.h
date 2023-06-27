#pragma once

#include "Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class PlayerRotationScript : public Script
{
public:
    PlayerRotationScript();
	~PlayerRotationScript() override = default;

	void Start() override;
    void PreUpdate(float deltaTime) override;

	bool GetCanRotate() const;
	void SetCanRotate(bool canRotate);

private:
    void Rotation(float deltaTime);

	bool canRotate;
};

