#include "StdAfx.h"
#include "ActivationLogic.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Components/ComponentRigidBody.h"
#include "Components/ComponentAudioSource.h"
#include "Components/ComponentAnimation.h"
#include "Components/ComponentPlayer.h"

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
	componentAudio = owner->GetComponent<ComponentAudioSource>();
	componentAnimation = owner->GetComponent<ComponentAnimation>();
	GameObject::GameObjectView children = owner->GetChildren();
	auto childWithRigid = std::find_if(std::begin(children),
									   std::end(children),
									   [](const GameObject* child)
									   {
										   return child->HasComponent<ComponentRigidBody>();
									   });
	// not just assert, since it would crash on the next line
	if (childWithRigid == std::end(children))
	{
		LOG_ERROR("Expected one of {}'s children to have a ComponentRigidBody, but none was found", GetOwner());
		throw ComponentNotFoundException("ComponentRigidBody not found in children");
	}
	componentRigidBody = (*childWithRigid)->GetComponent<ComponentRigidBody>();
	componentRigidBody->Disable();
}

void ActivationLogic::Update(float deltaTime)
{

}

void ActivationLogic::OnCollisionEnter(ComponentRigidBody* other)
{
	LOG_DEBUG("{} enters in CollisionEnter", other->GetOwner());
	if (other->GetOwner()->CompareTag("Player"))
	{
		componentAnimation->SetParameter("IsActive", true);
		componentRigidBody->Disable();
		componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
	}
}

void ActivationLogic::OnCollisionExit(ComponentRigidBody* other)
{
	LOG_DEBUG("{} enters in CollisionExit", other->GetOwner());
	if (other->GetOwner()->CompareTag("Player"))
	{
		componentAnimation->SetParameter("IsActive", false);
		// Until the trigger works 100% of the time better cross a closed door than be closed forever
		// componentRigidBody->Enable();
		componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_CLOSE);
	}
}