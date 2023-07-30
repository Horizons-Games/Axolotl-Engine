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
	componentRigidBody = nullptr;
	for (GameObject* child : owner->GetChildren())
	{
		try {
			componentRigidBody = child->GetComponent<ComponentRigidBody>();
			LOG_VERBOSE("{} has Component Rigid Body", child->GetName());
		}
		catch (const ComponentNotFoundException&) {
			LOG_WARNING("{} has not Component Rigid Body", child->GetName());
		}
	}
	assert(componentRigidBody);
	componentRigidBody->Disable();
}

void ActivationLogic::Update(float deltaTime)
{

}

void ActivationLogic::OnCollisionEnter(ComponentRigidBody* other)
{
	LOG_VERBOSE("{} enter in CollisionEnter", other->GetOwner()->GetName());
	if (other->GetOwner()->GetTag() == "Player") {
		componentAnimation->SetParameter("IsActive", true);
		componentRigidBody->Disable();
		componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_OPEN);
	}	
}

void ActivationLogic::OnCollisionExit(ComponentRigidBody* other)
{
	LOG_VERBOSE("{} enter in CollisionExit",other->GetOwner()->GetName());
	if (other->GetOwner()->GetTag() == "Player") {
		componentAnimation->SetParameter("IsActive", false);
		//Until the trigger works 100% of the time better cross a closed door than be closed forever
		//componentRigidBody->Enable();
		componentAudio->PostEvent(AUDIO::SFX::AMBIENT::SEWERS::BIGDOOR_CLOSE);
	}
}