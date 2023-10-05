#include "StdAfx.h"

#include "Application.h"
#include "ComponentAnimation.h"
#include "ComponentTransform.h"

#include "Animation/AnimationController.h"
#include "Animation/StateMachine.h"

#include "FileSystem/Json.h"
#include "FileSystem/ModuleResources.h"
#include "Resources/ResourceAnimation.h"
#include "Resources/ResourceStateMachine.h"

#include "ModuleEditor.h"

#include "GameObject/GameObject.h"

#include "debugdraw.h"

ComponentAnimation::ComponentAnimation(const bool active, GameObject* owner) :
	Component(ComponentType::ANIMATION, active, owner, true),
	drawBones(false),
	firstEntry(true),
	initTransition(false),
	controller(std::make_unique<AnimationController>()),
	stateMachineInstance(std::make_unique<StateMachine>())
{
}

ComponentAnimation::ComponentAnimation(const ComponentAnimation& toCopy) :
	Component(toCopy),
	controller(std::make_unique<AnimationController>()),
	stateMachineInstance(std::unique_ptr<StateMachine>(new StateMachine(*toCopy.GetStateMachineInstance()))),
	defaultPosition(toCopy.defaultPosition),
	firstEntry(toCopy.firstEntry),
	drawBones(toCopy.drawBones)
{
}

ComponentAnimation::~ComponentAnimation()
{
	App->GetModule<ModuleEditor>()->SetStateMachineWindowEditor(nullptr, "");
}

AnimationController* ComponentAnimation::GetController() const
{
	return controller.get();
}

StateMachine* ComponentAnimation::GetStateMachineInstance() const
{
	return stateMachineInstance.get();
}

std::shared_ptr<ResourceStateMachine> ComponentAnimation::GetStateMachine() const
{
	return stateMachineInstance->GetStateMachine();
}

void ComponentAnimation::SetStateMachine(std::shared_ptr<ResourceStateMachine> stateMachine)
{
	stateMachineInstance->SetStateMachine(std::move(stateMachine));
}

void ComponentAnimation::Update()
{
	if (!stateMachineInstance->GetStateMachine())
	{
		return;
	}

	if (firstEntry) // Entry State
	{
		SaveModelTransform(GetOwner());
		firstEntry = false;
	}

	controller->Update();
	stateMachineInstance->Update(!controller->GetPlay());
	bool transitioning = stateMachineInstance->IsTransitioning();
	if (controller->GetPlay() && (!transitioning || initTransition))
	{
		ApplyTransform();
	}
	else
	{
		if (transitioning)
		{
			if (!initTransition && stateMachineInstance->GetActualTransitionDuration() > 0.0)
			{
				InitAnimation(stateMachineInstance->GetNextState(), true);
				initTransition = true;
			}
			else
			{
				stateMachineInstance->FinishTransition();
				initTransition = false;
				InitAnimation(stateMachineInstance->GetActualState());
			}
		}
		else if (stateMachineInstance->GetActualState()->resource && stateMachineInstance->GetActualState()->loop)
		{
			InitAnimation(stateMachineInstance->GetActualState());
		}
	}
}

void ComponentAnimation::InitAnimation(State* newState, bool useTransitionDuration)
{
	if (newState->resource)
	{
		if (useTransitionDuration)
		{
			controller->Play(newState, false, stateMachineInstance->GetActualTransitionDuration());
		}
		else
		{
			controller->Play(newState, false);
		}
	}
	else
	{
		controller->Stop();
		LoadModelTransform(GetOwner());
		GetOwner()->GetComponentInternal<ComponentTransform>()->UpdateTransformMatrices();
	}
}

void ComponentAnimation::ApplyTransform()
{
	GameObject* owner = GetOwner();

	std::list<GameObject*> children = owner->GetAllDescendants();

	for (auto child : children)
	{
		float3 pos;
		Quat rot;

		if (controller->GetTransform(&child->GetName()[0], pos, rot, initTransition))
		{
			ComponentTransform* transform = child->GetComponentInternal<ComponentTransform>();
			transform->SetLocalPosition(pos);
			transform->SetLocalRotation(rot);
		}
	}

	owner->GetComponentInternal<ComponentTransform>()->UpdateTransformMatrices();
}

void ComponentAnimation::Draw() const
{
	if (App->GetPlayState() == Application::PlayState::STOPPED && drawBones)
	{
		DrawBones(GetOwner());
	}
}

void ComponentAnimation::DrawBones(GameObject* parent) const
{
	ComponentTransform* parentTransform = parent->GetComponentInternal<ComponentTransform>();

	GameObject::GameObjectView children = parent->GetChildren();
	for (GameObject* child : children)
	{
		ComponentTransform* childTransform = child->GetComponentInternal<ComponentTransform>();
		dd::line(childTransform->GetGlobalPosition(), parentTransform->GetGlobalPosition(), dd::colors::Blue);
		dd::axisTriad(childTransform->GetGlobalMatrix(), 0.1f, 2.0f);

		DrawBones(child);
	}
}

void ComponentAnimation::SetParameter(const std::string& parameterName, ValidFieldTypeParameter value)
{
	stateMachineInstance->SetParameter(parameterName, value);
}

void ComponentAnimation::InternalSave(Json& meta)
{
	UID uidState = 0;
	std::string assetPath = "";

	std::shared_ptr<ResourceStateMachine> stateMachine = stateMachineInstance->GetStateMachine();
	if (stateMachine)
	{
		uidState = stateMachine->GetUID();
		assetPath = stateMachine->GetAssetsPath();
	}

	meta["stateUID"] = (UID) uidState;
	meta["assetPathState"] = assetPath.c_str();
}

void ComponentAnimation::InternalLoad(const Json& meta)
{
	std::shared_ptr<ResourceStateMachine> resourceState;
#ifdef ENGINE
	std::string path = meta["assetPathState"];
	bool resourceExists = !path.empty() && App->GetModule<ModuleFileSystem>()->Exists(path.c_str());
	if (resourceExists)
	{
		resourceState = App->GetModule<ModuleResources>()->RequestResource<ResourceStateMachine>(path);
	}
#else
	UID uidState = meta["stateUID"];
	resourceState = App->GetModule<ModuleResources>()->SearchResource<ResourceStateMachine>(uidState);

#endif
	if (resourceState)
	{
		SetStateMachine(resourceState);
	}
}

void ComponentAnimation::SaveModelTransform(GameObject* gameObject)
{
	ComponentTransform* transform = gameObject->GetComponentInternal<ComponentTransform>();

	defaultPosition[gameObject] = transform->GetLocalMatrix();

	for (GameObject* children : gameObject->GetChildren())
	{
		SaveModelTransform(children);
	}
}

void ComponentAnimation::LoadModelTransform(GameObject* gameObject)
{
	ComponentTransform* transform = gameObject->GetComponentInternal<ComponentTransform>();

	float3 position;
	float3 scale;
	Quat rot;

	defaultPosition[gameObject].Decompose(position, rot, scale);
	transform->SetLocalPosition(position);
	transform->SetLocalRotation(rot);
	transform->SetLocalScale(scale);

	for (GameObject* children : gameObject->GetChildren())
	{
		LoadModelTransform(children);
	}
}

bool ComponentAnimation::isTransitioning()
{
	return stateMachineInstance->IsTransitioning();
}

bool ComponentAnimation::IsPlaying() const
{
	return controller->GetPlay();
}

void ComponentAnimation::ForceTransition()
{
	if (initTransition)
	{
		stateMachineInstance->FinishTransition();
		initTransition = false;
		InitAnimation(stateMachineInstance->GetActualState());
	}
}

std::string& ComponentAnimation::GetActualStateName() const
{
	return stateMachineInstance->GetActualStateName();
}
