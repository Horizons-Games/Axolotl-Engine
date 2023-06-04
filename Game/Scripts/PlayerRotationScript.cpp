#include "PlayerRotationScript.h"

#include "Application.h"
#include "ModuleInput.h"

#include "Components/ComponentRigidBody.h"

REGISTERCLASS(PlayerRotationScript);

PlayerRotationScript::PlayerRotationScript() : Script(), rotationSensitivity(50.0f)
{
	REGISTER_FIELD(rotationSensitivity, float);
}

void PlayerRotationScript::Start()
{
	rotationSensitivity /= 50.0f;
}

void PlayerRotationScript::PreUpdate(float deltaTime)
{
	Rotation(deltaTime);
}

void PlayerRotationScript::Rotation(float deltaTime)
{
	ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));
	btRigidBody* btRb = rigidBody->GetRigidBody();

	float horizontalMotion = App->GetModule<ModuleInput>()->GetMouseMotion().x * rotationSensitivity;
	btVector3 angularVelocity(0, 0, 0);

	if (horizontalMotion != 0)
	{
		btRb->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));
		angularVelocity = btVector3(0.0f, -horizontalMotion * deltaTime, 0.0f);
	}

	btRb->setAngularVelocity(angularVelocity);
}
