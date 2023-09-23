#include "StdAfx.h"
#include "EntityDetection.h"

#include "Application.h"

#include "ModuleInput.h"
#include "ModulePhysics.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentScript.h"
#include "GameObject/GameObject.h"
#include "../Scripts/EnemyClass.h"

#include "../Scripts/HealthSystem.h"

#include "debugdraw.h"

#include <set>

#include "AxoLog.h"

REGISTERCLASS(EntityDetection);

EntityDetection::EntityDetection() : Script(), input(nullptr), rigidBody(nullptr), player(nullptr),
interactionAngle(50.0f), playerTransform(nullptr), enemySelected(nullptr), interactionOffset(1.0f),
angleThresholdEnemyIntersection(1.0f)
{
	REGISTER_FIELD(player, GameObject*);
	REGISTER_FIELD(interactionAngle, float);
	REGISTER_FIELD(interactionOffset, float);
	REGISTER_FIELD(angleThresholdEnemyIntersection, float);
}

void EntityDetection::Start()
{
	rigidBody = owner->GetComponent<ComponentRigidBody>();
	playerTransform = player->GetComponent<ComponentTransform>();

	input = App->GetModule<ModuleInput>();
}

void EntityDetection::UpdateEnemyDetection(float distanceFilter)
{
	rigidBody->UpdateRigidBody();

	vecForward = playerTransform->GetGlobalForward();
	originPosition = playerTransform->GetGlobalPosition() - vecForward.Normalized() * interactionOffset;

	if (interactionOffset > 0.0f)
	{
		interactionOffset = 0.0f;
	}

#ifdef ENGINE
	DrawDetectionLines(distanceFilter);
#endif // ENGINE
	enemiesInTheArea.clear();
	App->GetModule<ModulePhysics>()->GetCollisions(rigidBody, enemiesInTheArea, "Enemy");
	App->GetModule<ModulePhysics>()->GetCollisions(rigidBody, enemiesInTheArea, "PriorityTarget");
	SelectEnemy(distanceFilter);
}

void EntityDetection::DrawDetectionLines(float distanceFilter)
{
	float magnitude = rigidBody->GetRadius() * rigidBody->GetFactor();

	dd::circle(originPosition, float3(0, 1, 0), dd::colors::DarkRed, distanceFilter,20);
	dd::circle(originPosition, float3(0, 1, 0), dd::colors::DarkRed, magnitude, 20);

	//Forward line
	float3 vecRotated = Quat::RotateAxisAngle(float3::unitY, math::DegToRad(interactionAngle)) * vecForward;
	dd::line(originPosition, originPosition + magnitude * vecRotated.Normalized(), dd::colors::BlueViolet);

	//Angle lines
	vecRotated = Quat::RotateAxisAngle(float3::unitY, math::DegToRad(-interactionAngle)) * vecForward;
	dd::line(originPosition, originPosition + magnitude * vecRotated.Normalized(), dd::colors::BlueViolet);
	dd::line(originPosition, originPosition + magnitude * vecForward.Normalized(), dd::colors::IndianRed);
}

void EntityDetection::SelectEnemy(float distanceFilter)
{
	ComponentTransform* lastenemySelected = enemySelected;
	enemySelected = nullptr;
	float angleActualSelected = 0;
	bool actualIsSpecialTarget = false;

	for (ComponentRigidBody* enemyRigid : enemiesInTheArea)
	{
		ComponentTransform* enemy = enemyRigid->GetOwnerTransform();
		bool insideDistanceFilter = true;
		if (distanceFilter != 0)
		{
			float3 enemyPosition = enemy->GetGlobalPosition();
			enemyPosition.y = originPosition.y;
			insideDistanceFilter = originPosition.Distance(enemyPosition) <= distanceFilter;
		}

		bool equalPriorityLevel = !actualIsSpecialTarget || enemy->GetOwner()->CompareTag("PriorityTarget");

		if (!enemy->GetOwner()->GetComponent<HealthSystem>()->EntityIsAlive() || 
			!equalPriorityLevel || !insideDistanceFilter)
		{
			continue;
		}

		float3 vecForward = playerTransform->GetGlobalForward().Normalized();
		float3 vecTowardsEnemy = (enemy->GetGlobalPosition() - originPosition).Normalized();

		float angle = Quat::FromEulerXYZ(vecForward.x, vecForward.y, vecForward.z).
			AngleBetween(Quat::FromEulerXYZ(vecTowardsEnemy.x, vecTowardsEnemy.y, vecTowardsEnemy.z));

#ifdef ENGINE
		float3 color = dd::colors::Blue;
#endif // ENGINE

		angle = math::Abs(math::RadToDeg(angle));

		if (angle < interactionAngle) //Enemy is inside angle and in front of player
		{
#ifdef ENGINE
			color = dd::colors::Red;
#endif // ENGINE

			float minActualThresholdAngle = (angleActualSelected - angleThresholdEnemyIntersection);
			float maxActualThresholdAngle = (angleActualSelected + angleThresholdEnemyIntersection);


			bool inFrontOfActualSelected = false;
			if (enemySelected != nullptr) 
			{
				inFrontOfActualSelected =
					angle <= maxActualThresholdAngle && originPosition.Distance(enemy->GetGlobalPosition()) <
					originPosition.Distance(enemySelected->GetGlobalPosition());
			}

			if (enemySelected == nullptr || angle < minActualThresholdAngle || inFrontOfActualSelected)
			{
				enemySelected = enemy;
				angleActualSelected = angle;
				actualIsSpecialTarget = enemySelected->GetOwner()->CompareTag("PriorityTarget");
			}
		}

#ifdef ENGINE
		//Draw enemies inside the sphere
		dd::sphere(enemy->GetGlobalPosition(), color, 0.5f);
#endif // ENGINE
	}

#ifdef ENGINE
	if (enemySelected != nullptr)
	{
		//Draw arrow to the enemy selected
		dd::arrow(originPosition, enemySelected->GetGlobalPosition(), dd::colors::Red, 0.1f);
	}
#endif // ENGINE

	if (lastenemySelected != enemySelected) 
	{
		if (lastenemySelected != nullptr)
		{
			VisualParticle(false, lastenemySelected->GetOwner());
		}
		if (enemySelected != nullptr)
		{
			VisualParticle(true, enemySelected->GetOwner());
		}
	}
}

void EntityDetection::VisualParticle(bool activate, GameObject* enemy) 
{
	enemy->GetComponent<EnemyClass>()->VisualTarget(activate);
}

void EntityDetection::OnCollisionEnter(ComponentRigidBody* other)
{
	/*if (other->GetOwner()->CompareTag("Enemy") ||
		other->GetOwner()->CompareTag("PriorityTarget") && other->GetOwner()->IsEnabled())
	{
		enemiesInTheArea.push_back(other);
	}*/
}

void EntityDetection::OnCollisionExit(ComponentRigidBody* other)
{
	/*if (other->GetOwner()->CompareTag("Enemy") ||
		other->GetOwner()->CompareTag("PriorityTarget") && other->GetOwner()->IsEnabled())
	{
		if (enemySelected == other->GetOwner()->GetComponent<ComponentTransform>())
		{
			VisualParticle(false, enemySelected->GetOwner());
			enemySelected = nullptr;
		}

		enemiesInTheArea.erase(
			std::remove_if(
				std::begin(enemiesInTheArea), std::end(enemiesInTheArea), [other](const ComponentRigidBody* rigid)
				{
					return rigid == other;
				}
			),
			std::end(enemiesInTheArea));
	}*/
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

bool EntityDetection::AreAnyEnemiesInTheArea() const
{
	return !enemiesInTheArea.empty();
}

std::vector<ComponentRigidBody*>& EntityDetection::GetEnemiesInTheArea()
{
	return enemiesInTheArea;
}

void EntityDetection::FilterEnemiesByDistance(float distanceFilter, 
											std::vector<ComponentRigidBody*>& enemiesInTheAreaFiltered)
{
	if (distanceFilter > rigidBody->GetRadius())
	{
		distanceFilter = rigidBody->GetRadius();
	}

	std::for_each(enemiesInTheArea.begin(), enemiesInTheArea.end(),
		[&distanceFilter, &enemiesInTheAreaFiltered, this](ComponentRigidBody* enemy)
		{
			float3 enemyPosition = enemy->GetOwner()->GetComponent<ComponentTransform>()->GetGlobalPosition();
			enemyPosition.y = originPosition.y;
			if (originPosition.Distance(enemyPosition) <= distanceFilter)
			{
				enemiesInTheAreaFiltered.push_back(enemy);
			}
		});
}
