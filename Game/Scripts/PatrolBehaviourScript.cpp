#include "PatrolBehaviourScript.h"

#include "Application.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentAudioSource.h"


#include "../Scripts/AIMovement.h"
#include "../Scripts/WaypointStateScript.h"
#include "../Scripts/EnemyVenomiteScript.h"
#include "../Scripts/EnemyClass.h"

#include "Auxiliar/Audio/AudioData.h"

#include "debugdraw.h"
#include "AxoLog.h"
#include "EnemyMiniBossTwo.h"
#include "FinalBossScript.h"

REGISTERCLASS(PatrolBehaviourScript);

PatrolBehaviourScript::PatrolBehaviourScript() : Script(), ownerTransform(nullptr),
	aiMovement(nullptr), currentWaypoint(0), isStoppedAtPatrol(true), patrolStopDuration(5.0f), totalPatrolTime(0.0f),
	patrolStateActivated(false), componentAnimation(nullptr), patrolAnimationParamater(""), hasWalkAnim(false),
	audioSource(nullptr)
{
	REGISTER_FIELD(waypointsPatrol, std::vector<ComponentTransform*>);
	REGISTER_FIELD(patrolStopDuration, float);
	REGISTER_FIELD(patrolAnimationParamater, std::string);
}

void PatrolBehaviourScript::Start()
{
	ownerTransform = owner->GetComponent<ComponentTransform>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	aiMovement = owner->GetComponent<AIMovement>();
	audioSource = owner->GetComponent<ComponentAudioSource>();

	currentWaypoint = 0;

	if (waypointsPatrol.empty())
	{
		waypointsPatrol.push_back(ownerTransform);
	}

	EnemyTypes enemyType = owner->GetComponent<EnemyClass>()->GetEnemyType();
	if (enemyType == EnemyTypes::FINAL_BOSS || enemyType == EnemyTypes::MINI_BOSS || enemyType == EnemyTypes::VENOMITE)
	{
		hasWalkAnim = true;
	}
}

void PatrolBehaviourScript::Update(float deltaTime)
{
	if (patrolStateActivated)
	{
		if (!isStoppedAtPatrol)
		{
			Patrolling();
		}
		else
		{
			totalPatrolTime += deltaTime;
			if (waypointsPatrol.size() > 1 && totalPatrolTime >= patrolStopDuration)
			{
				isStoppedAtPatrol = false;
				totalPatrolTime = 0;

				CheckNextWaypoint();

				float3 target = waypointsPatrol[currentWaypoint]->GetGlobalPosition();

				aiMovement->SetTargetPosition(target);
				aiMovement->SetRotationTargetPosition(target);
				aiMovement->SetMovementStatuses(true, true);

				if (hasWalkAnim)
				{
					audioSource->PostEvent(AUDIO::SFX::NPC::FOOTSTEPS);
				}

				componentAnimation->SetParameter(patrolAnimationParamater, true);
			}
		}
	}
}

void PatrolBehaviourScript::StartPatrol()
{
	float3 target = waypointsPatrol[currentWaypoint]->GetGlobalPosition();

	aiMovement->SetTargetPosition(target);
	aiMovement->SetRotationTargetPosition(target);
	aiMovement->SetMovementStatuses(true, true);
	componentAnimation->SetParameter(patrolAnimationParamater, true);
	if (hasWalkAnim)
	{
		audioSource->PostEvent(AUDIO::SFX::NPC::FOOTSTEPS);
	}
	patrolStateActivated = true;
	isStoppedAtPatrol = false;
}

void PatrolBehaviourScript::StopPatrol()
{
	aiMovement->SetMovementStatuses(false, false);
	if (hasWalkAnim)
	{
		audioSource->PostEvent(AUDIO::SFX::NPC::FOOTSTEPS_STOP);
	}
	patrolStateActivated = false;
	CheckNextWaypoint();
}

void PatrolBehaviourScript::Patrolling()
{
	if (aiMovement->GetIsAtDestiny())
	{
		float3 target = ownerTransform->GetGlobalPosition();

		aiMovement->SetTargetPosition(target);
		aiMovement->SetRotationTargetPosition(target);
		aiMovement->SetMovementStatuses(false, false);

		if (hasWalkAnim)
		{
			audioSource->PostEvent(AUDIO::SFX::NPC::FOOTSTEPS_STOP);
		}

		isStoppedAtPatrol = true;
		if (patrolAnimationParamater != "")
		{
			componentAnimation->SetParameter(patrolAnimationParamater, false);
		}
	}
}

void PatrolBehaviourScript::CheckNextWaypoint()
{
	currentWaypoint = (currentWaypoint + 1) % waypointsPatrol.size();
}

void PatrolBehaviourScript::RandomPatrolling(bool isFirstPatrolling)
{
	if (isFirstPatrolling)
	{
		GetNearestPatrollingPoint();
	}
	else if (ownerTransform->GetGlobalPosition().Equals(waypointsPatrol[currentWaypoint]->GetGlobalPosition(), 2.0f))
	{
		int randomWaypointSelected = currentWaypoint;
		WaypointStates selectedWaypointState = WaypointStates::UNAVAILABLE;
		
		while (currentWaypoint == randomWaypointSelected || selectedWaypointState == WaypointStates::UNAVAILABLE)
		{
			randomWaypointSelected = rand() % static_cast<int>(waypointsPatrol.size());
			selectedWaypointState = waypointsPatrol[randomWaypointSelected]->
				GetOwner()->GetComponent<WaypointStateScript>()->GetWaypointState();
		}

		currentWaypoint = randomWaypointSelected;
	}
}

void PatrolBehaviourScript::GetNearestPatrollingPoint()
{
	for (int i = 0; i < waypointsPatrol.size(); ++i)
	{
		WaypointStateScript* currentWaypointState = 
			waypointsPatrol[currentWaypoint]->GetOwner()->GetComponent<WaypointStateScript>();
		if (currentWaypointState->GetWaypointState() == WaypointStates::UNAVAILABLE)
		{
			continue;
		}

		float distanceToThisWaypoint = ownerTransform->GetGlobalPosition().Distance(waypointsPatrol[i]->GetGlobalPosition());
		float distanceToNearestWaypointUntilNow =
			ownerTransform->GetGlobalPosition().Distance(waypointsPatrol[currentWaypoint]->GetGlobalPosition());

		if (distanceToThisWaypoint <= distanceToNearestWaypointUntilNow)
		{
			currentWaypoint = i;
		}
	}
}