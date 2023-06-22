#pragma once
#include "Script.h"
class PlayerCameraRotationVerticalScript :
    public Script
{
public:
    PlayerCameraRotationVerticalScript();

	void Start() override;

    void PreUpdate(float deltaTime) override;

	float GetRotationSensitivity() const;
	void SetRotationSensitivity(float rotationSensitivity);

private:
    void Rotation(float deltaTime);

private: 
	float rotationSensitivity;
};

inline float PlayerCameraRotationVerticalScript::GetRotationSensitivity() const
{
	return rotationSensitivity;
}

inline void PlayerCameraRotationVerticalScript::SetRotationSensitivity(float rotationSensitivity)
{
	this->rotationSensitivity = rotationSensitivity;
}
