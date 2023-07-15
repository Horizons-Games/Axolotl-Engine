#include "StdAfx.h"
#include "EntityDetection.h"

#include "Application.h"

#include "ModuleInput.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "GameObject/GameObject.h"

#include "../Scripts/HealthSystem.h"

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


	vecForward = playerTransform->GetGlobalForward();
	originPosition = playerTransform->GetGlobalPosition() - vecForward.Normalized() * interactionOffset;

	if (interactionOffset > 0.0f)
	{
		interactionOffset = 0.0f;
	}

#ifdef ENGINE
	DrawDetectionLines();
#endif // ENGINE
	
	SelectEnemy();
}

void EntityDetection::DrawDetectionLines()
{
	float magnitude = rigidBody->GetRadius() * rigidBody->GetFactor();

	//Forward line
	float3 vecRotated = Quat::RotateAxisAngle(float3::unitY, math::DegToRad(interactionAngle)) * vecForward;
	dd::line(originPosition, originPosition + magnitude * vecRotated.Normalized(), dd::colors::BlueViolet);

	//Angle lines
	vecRotated = Quat::RotateAxisAngle(float3::unitY, math::DegToRad(-interactionAngle)) * vecForward;
	dd::line(originPosition, originPosition + magnitude * vecRotated.Normalized(), dd::colors::BlueViolet);
	dd::line(originPosition, originPosition + magnitude * vecForward.Normalized(), dd::colors::IndianRed);
}

void EntityDetection::SelectEnemy()
{
	enemySelected = nullptr;

	for (ComponentTransform* enemy : enemiesInTheArea)
	{
		if (!enemy->GetOwner()->GetComponent<HealthSystem>()->EntityIsAlive())
			continue;

		float3 vecForward = playerTransform->GetGlobalForward().Normalized();
		float3 vecTowardsEnemy = (enemy->GetGlobalPosition() - originPosition).Normalized();

		float angle = Quat::FromEulerXYZ(vecForward.x, vecForward.y, vecForward.z).
			AngleBetween(Quat::FromEulerXYZ(vecTowardsEnemy.x, vecTowardsEnemy.y, vecTowardsEnemy.z));

#ifdef ENGINE
		float3 color = dd::colors::Blue;
#endif // ENGINE

		float dotProduct = vecTowardsEnemy.Dot((playerTransform->GetGlobalPosition() - originPosition).Normalized());
		angle = math::Abs(math::RadToDeg(angle));

		if (angle < interactionAngle && dotProduct > 0) //Enemy is inside angle and in front of player
		{
#ifdef ENGINE
			color = dd::colors::Red;
#endif // ENGINE

			if (enemySelected == nullptr)
			{
				enemySelected = enemy;
			}
			//enemy is closer than the currently selected one
			else if (originPosition.Distance(enemy->GetGlobalPosition()) <
				originPosition.Distance(enemySelected->GetGlobalPosition()))
			{
				enemySelected = enemy;
			}
		}

#ifdef ENGINE
		//Draw enemies inside the sphere
		dd::sphere(enemy->GetGlobalPosition(), color, 0.5f);
	}

	if (enemySelected != nullptr)
	{
		//Draw arrow to the enemy selected
		dd::arrow(originPosition, enemySelected->GetGlobalPosition(), dd::colors::Red, 0.1f);
	}
#endif // ENGINE
}

void EntityDetection::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetTag() == "Enemy")
	{
		enemiesInTheArea.push_back(other->GetOwner()->GetComponent<ComponentTransform>());
	}
}

void EntityDetection::OnCollisionExit(ComponentRigidBody* other)
{
	if (enemySelected == other->GetOwner()->GetComponent<ComponentTransform>())
	{
		enemySelected = nullptr;
		SelectEnemy();
	}

	enemiesInTheArea.erase(
		std::remove_if(
			std::begin(enemiesInTheArea), std::end(enemiesInTheArea), [other](const ComponentTransform* transform)
			{
				return transform == other->GetOwner()->GetComponent<ComponentTransform>();
			}
		),
		std::end(enemiesInTheArea));
}



GameObject* EntityDetection::GetEnemySelected() const
{
	if (enemySelected != nullptr)
	{
		return enemySelected->GetOwner();
	}
	else
	{
		return nullptr;
	}
}