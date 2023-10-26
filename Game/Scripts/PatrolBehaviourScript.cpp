#include "PatrolBehaviourScript.h"

#include "Application.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentAudioSource.h"


#include "../Scripts/AIMovement.h"
#include "../Scripts/EnemyVenomiteScript.h"

#include "Auxiliar/Audio/AudioData.h"

#include "debugdraw.h"
#include "AxoLog.h"

REGISTERCLASS(PatrolBehaviourScript);

PatrolBehaviourScript::PatrolBehaviourScript() : Script(), ownerTransform(nullptr),
aiMovement(nullptr), currentWayPoint(0), isStoppedAtPatrol(true), patrolStopDuration(5.0f), totalPatrolTime(0.0f),
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

	currentWayPoint = 0;

	if (waypointsPatrol.empty())
	{
		waypointsPatrol.push_back(ownerTransform);
	}

	if (owner->HasComponent<EnemyVenomiteScript>())
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

				float3 target = waypointsPatrol[currentWayPoint]->GetGlobalPosition();

				aiMovement->SetTargetPosition(target);
				aiMovement->SetRotationTargetPosition(target);
				aiMovement->SetMovementStatuses(true, true);

				audioSource->PostEvent(AUDIO::SFX::NPC::FOOTSTEPS);

				componentAnimation->SetParameter(patrolAnimationParamater, true);
			}
		}
	}
}

void PatrolBehaviourScript::StartPatrol()
{
	float3 target = waypointsPatrol[currentWayPoint]->GetGlobalPosition();

	aiMovement->SetTargetPosition(target);
	aiMovement->SetRotationTargetPosition(target);
	aiMovement->SetMovementStatuses(true, true);
	componentAnimation->SetParameter(patrolAnimationParamater, true);
	audioSource->PostEvent(AUDIO::SFX::NPC::FOOTSTEPS);
	patrolStateActivated = true;
	isStoppedAtPatrol = false;
}

void PatrolBehaviourScript::StopPatrol()
{
	aiMovement->SetMovementStatuses(false, false);
	audioSource->PostEvent(AUDIO::SFX::NPC::FOOTSTEPS_STOP);
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

		audioSource->PostEvent(AUDIO::SFX::NPC::FOOTSTEPS_STOP);

		isStoppedAtPatrol = true;
		if (patrolAnimationParamater != "")
			componentAnimation->SetParameter(patrolAnimationParamater, false);
	}
}

void PatrolBehaviourScript::CheckNextWaypoint()
{
	currentWayPoint = (currentWayPoint + 1) % waypointsPatrol.size();
}

void PatrolBehaviourScript::RandomPatrolling(bool isFirstPatrolling)
{
	if (isFirstPatrolling)
	{
		GetNearestPatrollingPoint();
	}
	else if (ownerTransform->GetGlobalPosition().Equals(waypointsPatrol[currentWayPoint]->GetGlobalPosition(), 2.0f))
	{

		int randomWaypointSelected = currentWayPoint;
		
		while (currentWayPoint == randomWaypointSelected)
		{
			randomWaypointSelected = rand() % static_cast<int>(waypointsPatrol.size());
		}

		currentWayPoint = randomWaypointSelected;
	}
}

void PatrolBehaviourScript::GetNearestPatrollingPoint()
{
	for (int i = 0; i < waypointsPatrol.size(); ++i)
	{
		if (ownerTransform->GetGlobalPosition().Distance(waypointsPatrol[i]->GetGlobalPosition()) <=
			ownerTransform->GetGlobalPosition().Distance(waypointsPatrol[currentWayPoint]->GetGlobalPosition()))
		{
			currentWayPoint = i;
		}
	}
}