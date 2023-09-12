#include "PlayerRotationScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentRigidBody.h"


#include "Components/ComponentScript.h"
#include "PlayerForceUseScript.h"

REGISTERCLASS(PlayerRotationScript);

PlayerRotationScript::PlayerRotationScript() : Script(), rotationSensitivityHorizontal(1.0f),
	rotationSensitivityVertical(1.0f), canRotate(true)
{
	REGISTER_FIELD(rotationSensitivityVertical, float);
	REGISTER_FIELD(rotationSensitivityHorizontal, float);
	REGISTER_FIELD(canRotate, bool);
}

void PlayerRotationScript::Start()
{
	if (owner->HasComponent<PlayerForceUseScript>())
	{
		forceScript = owner->GetComponent<PlayerForceUseScript>();
	}
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	btRigidbody = rigidBody->GetRigidBody();
}

void PlayerRotationScript::PreUpdate(float deltaTime)
{
	if (forceScript)
	{
		if (forceScript->IsForceActive())
		{
			Rotation(deltaTime);
		}
		else
		{
			btRigidbody->setAngularVelocity({ 0.0f,0.0f,0.0f });
		}
	}
	else
	{
		btRigidbody->setAngularVelocity({ 0.0f,0.0f,0.0f });
	}
}
	

void PlayerRotationScript::Rotation(float deltaTime)
{
	if (!canRotate)
	{
		return;
	}

	float horizontalMotion = App->GetModule<ModuleInput>()->GetMouseMotion().x * rotationSensitivityHorizontal;
	btVector3 angularVelocity(0, 0, 0);

	if (horizontalMotion != 0)
	{
		btRigidbody->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));
		angularVelocity = btVector3(0.0f, -horizontalMotion * deltaTime, 0.0f);
	}
	btRigidbody->setAngularVelocity(angularVelocity);
}

bool PlayerRotationScript::GetCanRotate() const
{
	return canRotate;
}

void PlayerRotationScript::SetCanRotate(bool canRotate)
{
	this->canRotate = canRotate;
}

float PlayerRotationScript::GetHorizontalSensitivity() const
{
	return rotationSensitivityHorizontal;
}

float PlayerRotationScript::GetVerticalSensitivity() const
{
	return rotationSensitivityVertical;
}

void PlayerRotationScript::SetHorizontalSensitivity(float sensitivity)
{
	rotationSensitivityHorizontal = sensitivity;
}

void PlayerRotationScript::SetVerticalSensitivity(float sensitivity)
{
	rotationSensitivityVertical = sensitivity;
}
