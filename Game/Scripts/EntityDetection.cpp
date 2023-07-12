#include "StdAfx.h"
#include "EntityDetection.h"

#include "Application.h"

#include "ModuleInput.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "GameObject/GameObject.h"

#include "debugdraw.h"

#include <set>

#include "AxoLog.h"

REGISTERCLASS(EntityDetection);

EntityDetection::EntityDetection() : Script(), input(nullptr), rigidBody(nullptr), player(nullptr), 
interactionAngle(50.0f), playerTransform(nullptr), enemySelected(nullptr), interactionOffset(1.0f)
{
	REGISTER_FIELD(player, GameObject*);
	REGISTER_FIELD(interactionAngle, float);
	REGISTER_FIELD(interactionOffset, float);
}

void EntityDetection::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	playerTransform = player->GetComponent<ComponentTransform>();

	input = App->GetModule<ModuleInput>();

	rigidBody->SetKpForce(50);
}

void EntityDetection::Update(float deltaTime)
{
	rigidBody->SetPositionTarget(playerTransform->GetGlobalPosition());


	float3 vecForward = playerTransform->GetGlobalForward();
	float3 originPosition = playerTransform->GetGlobalPosition() - vecForward.Normalized() * interactionOffset;

	float newMagnitude = rigidBody->GetRadius() * rigidBody->GetFactor();

	float3 vecRotated = Quat::RotateAxisAngle(float3::unitY, math::DegToRad(interactionAngle)) * vecForward;
	dd::line(originPosition,
		originPosition + newMagnitude * vecRotated.Normalized(),
		dd::colors::BlueViolet);

	vecRotated = Quat::RotateAxisAngle(float3::unitY, math::DegToRad(-interactionAngle)) * vecForward;
	dd::line(originPosition,
		originPosition + newMagnitude * vecRotated.Normalized(),
		dd::colors::BlueViolet);


	dd::line(originPosition, originPosition +
		playerTransform->GetGlobalForward().Normalized() * rigidBody->GetRadius() * rigidBody->GetFactor(),
		dd::colors::IndianRed);

	enemySelected = nullptr;
	
	for (GameObject* enemy : enemiesInTheArea) 
	{
		float3 vecTowardsEnemy = enemy->GetComponent<ComponentTransform>()->GetGlobalPosition() - originPosition;
	
		vecForward = vecForward.Normalized();
		vecTowardsEnemy = vecTowardsEnemy.Normalized();

		float angle = Quat::FromEulerXYZ(vecForward.x, vecForward.y, vecForward.z).
			AngleBetween(Quat::FromEulerXYZ(vecTowardsEnemy.x, vecTowardsEnemy.y, vecTowardsEnemy.z));

		
		float3 color = dd::colors::Blue;
		float finalAngle = math::Abs(math::RadToDeg(angle));
		if (finalAngle < interactionAngle)
		{
			color = dd::colors::Red;

			if (enemySelected == nullptr)
			{
				enemySelected = enemy;
			}
			else
			{
				float3 currentEnemyPosition = enemy->GetComponent<ComponentTransform>()->GetGlobalPosition();
				float3 lastEnemyPosition = enemySelected->GetComponent<ComponentTransform>()->GetGlobalPosition();
				if (originPosition.Distance(currentEnemyPosition) < originPosition.Distance(lastEnemyPosition))
				{
					enemySelected = enemy;
				}
			}

		}

		dd::sphere(enemy->GetComponent<ComponentTransform>()->GetGlobalPosition(),
			color, 0.5f);
	}

	if (enemySelected != nullptr)
	{
		dd::arrow(originPosition, enemySelected->GetComponent<ComponentTransform>()->GetGlobalPosition(),
			dd::colors::Red, 0.5f);
	}

}

void EntityDetection::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetTag() == "Enemy")
	{
		enemiesInTheArea.push_back(other->GetOwner());
	}
}

void EntityDetection::OnCollisionExit(ComponentRigidBody* other)
{
	enemiesInTheArea.erase(
		std::remove_if(
			std::begin(enemiesInTheArea), std::end(enemiesInTheArea), [other](const GameObject* gameObject)
			{
				return gameObject == other->GetOwner();
			}
		),
		std::end(enemiesInTheArea));
}