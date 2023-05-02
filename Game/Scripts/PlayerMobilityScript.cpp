#include "PlayerMobilityScript.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Components/Component.h"
#include "Components/ComponentScript.h"
#include "Components/ComponentMeshCollider.h"
#include "Components/ComponentRigidBody.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentPlayer.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentAudioSource.h"

#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"
#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(PlayerMobilityScript);

PlayerMobilityScript::PlayerMobilityScript() : Script(), componentPlayer(nullptr), 
												moveScript(nullptr), rotateScript(nullptr),
												speed(6.0f),
												jumpParameter(80.0f), dashForce(50.0f), canDash(true),
												canDoubleJump(true) , jumps(0), isCrouch(false),
												componentAudio(nullptr), playerState(PlayerActions::IDLE)
{
	REGISTER_FIELD(Speed, float);
	REGISTER_FIELD(JumpParameter, float);
	REGISTER_FIELD(DashForce, float);
	REGISTER_FIELD(CanDash, bool);
}

void PlayerMobilityScript::Start()
{

	std::vector<ComponentScript*> gameObjectScripts =
		owner->GetComponentsByType<ComponentScript>(ComponentType::SCRIPT);

	for (int i = 0; i < gameObjectScripts.size(); ++i)
	{
		if (gameObjectScripts[i]->GetConstructName() == "PlayerMobilityScript")
		{
			moveScript = gameObjectScripts[i];
			moveScript->GetScript()->Start();
		}

		else if (gameObjectScripts[i]->GetConstructName() == "PlayerRotationScript")
		{
			rotateScript = gameObjectScripts[i];
			rotateScript->GetScript()->Start();
		}
	}

	if (canDoubleJump) 
	{
		jumps = 2;
	}
	else
	{
		jumps = 1;
	}

	componentPlayer = static_cast<ComponentPlayer*>(owner->GetComponent(ComponentType::PLAYER));
	componentAudio = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));
}

void PlayerMobilityScript::PreUpdate(float deltaTime)
{
	
	if (!componentPlayer->IsStatic() && App->GetModule<ModuleCamera>()->GetSelectedPosition() == 0
		&& !SDL_ShowCursor(SDL_QUERY))
	{
		moveScript->PreUpdate();
		//Move();
		Rotate();
	}
	
}

void PlayerMobilityScript::Move()
{
	

	//top
	/*if (!collider->IsColliding(topPoints, verticalDirection, speed * deltaTime * 1.1f, 0.0f))
	{
	}*/
}

void PlayerMobilityScript::Rotate()
{
	float deltaTime = App->GetDeltaTime();
	ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	float3 newRot = trans->GetRotationXYZ();
	ModuleInput* input = App->GetModule<ModuleInput>();
	newRot.y += -input->GetMouseMotion().x * deltaTime;
	trans->SetRotation(newRot);
	trans->UpdateTransformMatrices();


	//Corroborate that you don't fuse with a owner
	ComponentMeshCollider* collider = static_cast<ComponentMeshCollider*>(owner->GetComponent(ComponentType::MESHCOLLIDER));
	math::vec points[8];
	trans->GetObjectOBB().GetCornerPoints(points);
	std::vector<float3> frontPoints = { points[1], points[3], points[5], points[7] };
	float3 direction = (points[1] - points[0]).Normalized();
	if (collider->IsColliding(frontPoints, -direction, trans->GetLocalAABB().Size().z * 0.5f))
	{
		float deltaTime = App->GetDeltaTime();
		ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
		float3 newRot = trans->GetRotationXYZ();
		newRot.y += input->GetMouseMotion().x * deltaTime;
		trans->SetRotation(newRot);
		trans->UpdateTransformMatrices();
	}
}

float PlayerMobilityScript::GetSpeed() const
{
	return speed;
}

void PlayerMobilityScript::SetSpeed(float speed)
{
	this->speed = speed;
}

float PlayerMobilityScript::GetJumpParameter() const
{
	return jumpParameter;
}

void PlayerMobilityScript::SetJumpParameter(float jumpParameter)
{
	this->jumpParameter = jumpParameter;
}

float PlayerMobilityScript::GetDashForce() const
{
	return dashForce;
}

void PlayerMobilityScript::SetDashForce(float dashForce)
{
	this->dashForce = dashForce;
}

bool PlayerMobilityScript::GetCanDash() const
{
	return canDash;
}

void PlayerMobilityScript::SetCanDash(bool canDash)
{
	this->canDash = canDash;
}

bool PlayerMobilityScript::GetCanDoubleJump() const
{
	return canDoubleJump;
}

void PlayerMobilityScript::SetCanDoubleJump(bool canDoubleJump)
{
	this->canDoubleJump = canDoubleJump;
}