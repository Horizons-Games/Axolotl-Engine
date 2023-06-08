#include "PlayerCameraRotationVerticalScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentTransform.h"

REGISTERCLASS(PlayerCameraRotationVerticalScript);

PlayerCameraRotationVerticalScript::PlayerCameraRotationVerticalScript() : Script(), rotationSensitivity(50.0f)
{
	REGISTER_FIELD(rotationSensitivity, float);
}

void PlayerCameraRotationVerticalScript::Start()
{
	rotationSensitivity /= 50.0f;
}

void PlayerCameraRotationVerticalScript::PreUpdate(float deltaTime)
{
	Rotation(deltaTime);
}

void PlayerCameraRotationVerticalScript::Rotation(float deltaTime)
{
	ComponentTransform* componentTransform = owner->GetComponent<ComponentTransform>();

	float verticalMotion = App->GetModule<ModuleInput>()->GetMouseMotion().y / 50.0f * rotationSensitivity;
	float3 nRotation = componentTransform->GetRotationXYZ() + float3(verticalMotion, 0, 0);

	if (verticalMotion != 0 && nRotation.x > -15 && nRotation.x < 15)
	{
		componentTransform->SetRotation(nRotation);
	}
}
