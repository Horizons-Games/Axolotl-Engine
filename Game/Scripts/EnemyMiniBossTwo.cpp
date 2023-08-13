#include "EnemyMiniBossTwo.h"

#include "Components/ComponentScript.h"

#include "../Scripts/SeekBehaviourScript.h"

REGISTERCLASS(EnemyMiniBossTwo);

EnemyMiniBossTwo::EnemyMiniBossTwo() 
{
	REGISTER_FIELD(seekDistance, float);
}

void EnemyMiniBossTwo::Start()
{
	if (seekDistance < attackDistance)
	{
		seekDistance = attackDistance;
	}

	ownerTransform = owner->GetComponent<ComponentTransform>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	componentAudioSource = owner->GetComponent<ComponentAudioSource>();

	seekScript = owner->GetComponent<SeekBehaviourScript>();
	healthScript = owner->GetComponent<HealthSystem>();

	seekTarget = seekScript->GetTarget();
	seekTargetTransform = seekTarget->GetComponent<ComponentTransform>();

}

void EnemyMiniBossTwo::Update(float deltaTime)
{
	if (stunned)
	{
		if (timeStunned < 0)
		{
			timeStunned = 0;
			stunned = false;
		}
		else
		{
			timeStunned -= deltaTime;
			return;
		}
	}

	if (healthScript && !healthScript->EntityIsAlive())
	{
		return;
	}
}