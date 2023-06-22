#pragma once
#include "Script.h"
class PlayerRotationScript :
    public Script
{
public:

    PlayerRotationScript();

	void Start() override;

    void PreUpdate(float deltaTime) override;

	float GetRotationSensitivity() const;
	void SetRotationSensitivity(float rotationSensitivity);

private:
    void Rotation(float deltaTime);

private:
	float rotationSensitivity;
};

inline float PlayerRotationScript::GetRotationSensitivity() const
{
	return rotationSensitivity;
}

inline void PlayerRotationScript::SetRotationSensitivity(float rotationSensitivity)
{
	this->rotationSensitivity = rotationSensitivity;
}
