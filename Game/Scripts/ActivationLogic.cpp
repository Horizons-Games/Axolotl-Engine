#include "ActivationLogic.h"

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
#include "Components/ComponentAnimation.h"

#include "GameObject/GameObject.h"

#include "DataStructures/Quadtree.h"
#include "Auxiliar/Audio/AudioData.h"

REGISTERCLASS(ActivationLogic);

ActivationLogic::ActivationLogic() : Script(),
componentAudio(nullptr), activeState(ActiveActions::INACTIVE)
{
}

ActivationLogic::~ActivationLogic()
{
}

void ActivationLogic::Start()
{
	//componentAudio = static_cast<ComponentAudioSource*>(owner->GetComponent(ComponentType::AUDIOSOURCE));
	componentAnimation = static_cast<ComponentAnimation*>(owner->GetComponent(ComponentType::ANIMATION));
}

void ActivationLogic::Update(float deltaTime)
{

}

void ActivationLogic::OnCollisionEnter(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetComponent(ComponentType::PLAYER))
	{
		componentAnimation->SetParameter("IsActive", true);
	}
}

void ActivationLogic::OnCollisionExit(ComponentRigidBody* other)
{
	if (other->GetOwner()->GetComponent(ComponentType::PLAYER))
	{
		componentAnimation->SetParameter("IsActive", false);
	}
}