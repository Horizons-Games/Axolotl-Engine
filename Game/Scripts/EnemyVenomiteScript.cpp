#include "EnemyVenomiteScript.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"

#include "../Scripts/PatrolBehaviourScript.h"
#include "../Scripts/HealthSystem.h"

REGISTERCLASS(EnemyVenomiteScript);

EnemyVenomiteScript::EnemyVenomiteScript() : Script(), venomiteState(VenomiteBehaviours::IDLE), patrolScript(nullptr),
	healthScript(nullptr), ownerTransform(nullptr), componentAnimation(nullptr), componentAudioSource(nullptr)
{
}

void EnemyVenomiteScript::Start()
{
	//ownerTransform = owner->GetComponent<ComponentTransform>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	//componentAudioSource = owner->GetComponent<ComponentAudioSource>();

	patrolScript = owner->GetComponent<PatrolBehaviourScript>();
	healthScript = owner->GetComponent<HealthSystem>();
}

void EnemyVenomiteScript::Update(float deltaTime)
{
	if (healthScript && !healthScript->EntityIsAlive())
	{
		return;
	}

	if (venomiteState != VenomiteBehaviours::PATROL)
	{
		venomiteState = VenomiteBehaviours::PATROL;
		patrolScript->StartPatrol();
	}

	if (patrolScript && venomiteState == VenomiteBehaviours::PATROL)
	{
		patrolScript->Patrolling();

		componentAnimation->SetParameter("IsWalking", true);
	}
}