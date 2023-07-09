#pragma once

#include "Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class PlayerForceUseScript;
class ComponentRigidBody;
class btRigidBody;

class PlayerRotationScript : public Script
{
public:
    PlayerRotationScript();
	~PlayerRotationScript() override = default;

	void Start() override;
    void PreUpdate(float deltaTime) override;

	bool GetCanRotate() const;
	void SetCanRotate(bool canRotate);

	float GetHorizontalSensitivity() const;
	float GetVerticalSensitivity() const;

	void SetHorizontalSensitivity(float sensitivity);
	void SetVerticalSensitivity(float sensitivity);
private:
    void Rotation(float deltaTime);
	bool canRotate;

	float rotationSensitivityHorizontal;
	float rotationSensitivityVertical;

	PlayerForceUseScript* forceScript;
	ComponentRigidBody* rigidBody;
	btRigidBody* btRb;
};

inline float PlayerRotationScript::GetHorizontalSensitivity() const
{
	return rotationSensitivityHorizontal;
}

inline float PlayerRotationScript::GetVerticalSensitivity() const
{
	return rotationSensitivityVertical;
}

inline void PlayerRotationScript::SetHorizontalSensitivity(float sensitivity)
{
	rotationSensitivityHorizontal = sensitivity;
}

inline void PlayerRotationScript::SetVerticalSensitivity(float sensitivity)
{
	rotationSensitivityVertical = sensitivity;
}

