#include "PlayerCameraRotationVerticalScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentTransform.h"

REGISTERCLASS(PlayerCameraRotationVerticalScript);

PlayerCameraRotationVerticalScript::PlayerCameraRotationVerticalScript() : Script()
{
}

void PlayerCameraRotationVerticalScript::PreUpdate(float deltaTime)
{
	Rotation(deltaTime);
}

void PlayerCameraRotationVerticalScript::Rotation(float deltaTime)
{
	ComponentTransform* componentTransform = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));

	float verticalMotion = App->GetModule<ModuleInput>()->GetMouseMotion().y / 50.0f;
	float3 nRotation = componentTransform->GetRotationXYZ() + float3(verticalMotion, 0, 0);

	if (verticalMotion != 0 && nRotation.x > -15 && nRotation.x < 15)
	{
		componentTransform->SetRotation(nRotation);
	}
}
