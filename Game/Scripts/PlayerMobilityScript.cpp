#include "PlayerMobilityScript.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

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

PlayerMobilityScript::PlayerMobilityScript() : Script(), componentPlayer(nullptr), speed(6.0f),
												jumpParameter(80.0f), dashForce(50.0f), canDash(true),
												canDoubleJump(true) , jumps(0), isCrouch(false),
												componentAudio(nullptr), playerState(PlayerActions::IDLE)
{
	REGISTER_FIELD_WITH_ACCESSORS(Speed, float);
	REGISTER_FIELD_WITH_ACCESSORS(JumpParameter, float);
	REGISTER_FIELD_WITH_ACCESSORS(DashForce, float);
	REGISTER_FIELD_WITH_ACCESSORS(CanDash, bool);
}

PlayerMobilityScript::~PlayerMobilityScript()
{
	delete componentPlayer;
}

void PlayerMobilityScript::Start()
{
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
		Move();
		Rotate();
	}
	
}

void PlayerMobilityScript::Move()
{
	float deltaTime = (App->GetDeltaTime() < 1.f) ? App->GetDeltaTime() : 1.f;

	ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	ComponentRigidBody* rigidBody = static_cast<ComponentRigidBody*>(owner->GetComponent(ComponentType::RIGIDBODY));


	// Dash pressing E during 0.2 sec
	ModuleInput* input = App->GetModule<ModuleInput>();

	btRigidBody* btRb = rigidBody->GetRigidBody();
	btRb->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

	btVector3 force(0, 0, 0);

	// Forward
	if (input->GetKey(SDL_SCANCODE_W) != KeyState::IDLE)
	{
		force += btVector3(0, 0, 1);
	}

	if (input->GetKey(SDL_SCANCODE_S) != KeyState::IDLE)
	{
		force += btVector3(0, 0, -1);
	}

	if (input->GetKey(SDL_SCANCODE_D) != KeyState::IDLE)
	{

		force += btVector3(-1, 0, 0);
	}

	if (input->GetKey(SDL_SCANCODE_A) != KeyState::IDLE)
	{
		force += btVector3(1, 0, 0);
	}

	if (!force.isZero())
	{
		/*btRb->setFriction(1.0);
		btRb->setDamping(0.9, 0.9);*/
		btRb->applyCentralImpulse(force*speed);
		
	}
}

void PlayerMobilityScript::Rotate()
{
	//float deltaTime = App->GetDeltaTime();
	//ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	//float3 newRot = trans->GetRotationXYZ();
	//ModuleInput* input = App->GetModule<ModuleInput>();
	//newRot.y += -input->GetMouseMotion().x * deltaTime;
	//trans->SetRotation(newRot);
	//trans->UpdateTransformMatrices();


	////Corroborate that you don't fuse with a owner
	//ComponentMeshCollider* collider = static_cast<ComponentMeshCollider*>(owner->GetComponent(ComponentType::MESHCOLLIDER));
	//math::vec points[8];
	//trans->GetObjectOBB().GetCornerPoints(points);
	//std::vector<float3> frontPoints = { points[1], points[3], points[5], points[7] };
	//float3 direction = (points[1] - points[0]).Normalized();
	//if (collider->IsColliding(frontPoints, -direction, trans->GetLocalAABB().Size().z * 0.5f))
	//{
	//	float deltaTime = App->GetDeltaTime();
	//	ComponentTransform* trans = static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM));
	//	float3 newRot = trans->GetRotationXYZ();
	//	newRot.y += input->GetMouseMotion().x * deltaTime;
	//	trans->SetRotation(newRot);
	//	trans->UpdateTransformMatrices();
	//}
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