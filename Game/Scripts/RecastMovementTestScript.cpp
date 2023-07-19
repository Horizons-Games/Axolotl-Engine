#include "RecastMovementTestScript.h"

#include "ModuleInput.h"

#include "Components/ComponentScript.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentAgent.h"

REGISTERCLASS(RecastMovementTestScript);

RecastMovementTestScript::RecastMovementTestScript() : Script(), target(nullptr)
{
	REGISTER_FIELD(target, GameObject*);
}

void RecastMovementTestScript::Start()
{
	agent = owner->GetComponent<ComponentAgent>();
	targetPosition = target->GetComponent<ComponentTransform>()->GetGlobalPosition();
	componentTransform = owner->GetComponent<ComponentTransform>();

	input = App->GetModule<ModuleInput>();
}

void RecastMovementTestScript::Update(float deltaTime)
{
	/*if (test)
	{
		agent->SetMoveTarget(targetPosition);
		test = false;
	}*/

	Move();
}

void RecastMovementTestScript::Move()
{
	float3 currentPosition = componentTransform->GetGlobalPosition();
	float3 dir = float3(0, 0, 0);

	if (input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		dir += float3(1, 0, 0);
	}
	if (input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		dir += float3(-1, 0, 0);
	}
	if (input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		dir += float3(0, 0, -1);
	}
	if (input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{
		dir += float3(0, 0, 1);
	}
	
	float3 newPosition = currentPosition + dir;

	//agent->SetMaxSpeed(10.0f);
	agent->SetMoveTarget(newPosition, false);
}
