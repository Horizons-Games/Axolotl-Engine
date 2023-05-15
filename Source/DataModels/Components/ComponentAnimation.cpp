#include "Application.h"
#include "ComponentAnimation.h"
#include "ComponentTransform.h"
#include "Globals.h"

#include "Animation/AnimationController.h"

#include "FileSystem/Json.h"
#include "FileSystem/ModuleResources.h"
#include "Resources/ResourceAnimation.h"
#include "Resources/ResourceStateMachine.h"

#include "ModuleInput.h"

#include "GameObject/GameObject.h"

#include "Math/float3.h"

#include "debugdraw.h"

ComponentAnimation::ComponentAnimation(const bool active, GameObject* owner)
	: Component(ComponentType::ANIMATION, active, owner, false) 
{
	controller = new AnimationController();
	lastState = NON_STATE;
}

ComponentAnimation::~ComponentAnimation()
{
	delete controller;
}

AnimationController* ComponentAnimation::GetController()
{
	return controller;
}

const std::shared_ptr<ResourceStateMachine>& ComponentAnimation::GetStateMachine() const
{
	return stateMachine;
}

void ComponentAnimation::SetStateMachine(const std::shared_ptr<ResourceStateMachine>& stateMachine)
{
	this->stateMachine = stateMachine;
	if (stateMachine)
	{
		this->parameters = stateMachine->GetParameters();
	}
	actualState = 0;
}

void ComponentAnimation::Update()
{
	if (stateMachine)
	{
		if ((actualState == 0) && (lastState == NON_STATE)) //Entry State 
		{
			SaveModelTransform(owner);
		}

		controller->Update();

		if(actualState == nextState)
		{
			State* state = stateMachine->GetState(actualState);
			if (state && controller->GetPlay())
			{
				std::list<GameObject*> children = owner->GetGameObjectsInside();

				for (auto child : children)
				{
					float3 pos;
					Quat rot;

					if (controller->GetTransform(&child->GetName()[0], pos, rot))
					{
						ComponentTransform* transform =
							static_cast<ComponentTransform*>(child->GetComponent(ComponentType::TRANSFORM));
						transform->SetPosition(pos);
						transform->SetRotation(float4x4(rot));
					}
				}
				static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM))->UpdateTransformMatrices();
			}

			Transition foundTransition;
			if (CheckTransitions(state, foundTransition))
			{
				nextState = foundTransition.destinationState;
			}
		}
		else 
		{
			actualState = nextState;
			State* state = stateMachine->GetState(actualState);
			if(state->resource) 
			{
				controller->Play(std::dynamic_pointer_cast<ResourceAnimation>(state->resource), true);
			}
			else
			{
				controller->Stop();
				LoadModelTransform(owner);
				static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM))->UpdateTransformMatrices();
			}
		}
		lastState = actualState;
	}
	else
	{
		lastState = NON_STATE;
	}
}

void ComponentAnimation::Draw() const
{
	if (!App->IsOnPlayMode())
	{
		DrawBones(owner);
	}
}

void ComponentAnimation::DrawBones(GameObject* parent) const
{
	ComponentTransform* parentTransform = 
		static_cast<ComponentTransform*>(parent->GetComponent(ComponentType::TRANSFORM));

	GameObject::GameObjectView children = parent->GetChildren();
	for (GameObject* child : children)
	{
		ComponentTransform* childTransform =
			static_cast<ComponentTransform*>(child->GetComponent(ComponentType::TRANSFORM));
		dd::line(childTransform->GetGlobalPosition(), parentTransform->GetGlobalPosition(), dd::colors::Blue);
		dd::axisTriad(childTransform->GetGlobalMatrix(), 0.1f, 2.0f);

		DrawBones(child);
	}
}

void ComponentAnimation::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;

	UID uidState = 0;
	std::string assetPath = "";

	if (stateMachine)
	{
		uidState = stateMachine->GetUID();
		assetPath = stateMachine->GetAssetsPath();
	}

	meta["stateUID"] = (UID)uidState;
	meta["assetPathState"] = assetPath.c_str();
}

void ComponentAnimation::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
	std::shared_ptr<ResourceStateMachine> resourceState;
#ifdef ENGINE
	std::string path = meta["assetPathState"];
	bool resourceExists = path != "" && App->GetModule<ModuleFileSystem>()->Exists(path.c_str());
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

	actualState = 0;
	nextState = 0;
}

bool ComponentAnimation::CheckTransitions(State* state, Transition& transition)
{
	if(!state)
	{
		return false;
	}

	for (UID idTransition : state->transitionsOriginedHere)
	{
		Transition& actualTransition = stateMachine->GetTransitions()[idTransition];
		bool conditionCheck = true;
		for(Condition& condition : actualTransition.conditions)
		{
			const auto& itParameter = parameters.find(condition.parameter);
			if (itParameter != parameters.end()) 
			{
				ValidFieldTypeParameter& value = itParameter->second.second;
				switch (condition.conditionType)
				{
				case ConditionType::GREATER:
					conditionCheck = value > condition.value;
					break;
				case ConditionType::LESS:
					conditionCheck = value < condition.value;
					break;
				case ConditionType::EQUAL:
					conditionCheck = value == condition.value;
					break;
				case ConditionType::NOTEQUAL:
					conditionCheck = value != condition.value;
					break;
				case ConditionType::TRUECONDITION:
					conditionCheck = (std::get<bool>(value) == true);
					break;
				case ConditionType::FALSECONDITION:
					conditionCheck = (std::get<bool>(value) == false);
					break;
				default:
					break;
				}
			}

			if (!conditionCheck) break;
		}
		
		if (conditionCheck) 
		{
			transition = actualTransition;
			return true;
		}
	}

	return false;
}

void ComponentAnimation::SaveModelTransform(GameObject* gameObject)
{
	ComponentTransform* transform =
		static_cast<ComponentTransform*>(gameObject->GetComponent(ComponentType::TRANSFORM));

	defaultPosition[gameObject] = transform->GetLocalMatrix();

	for(GameObject* children : gameObject->GetChildren())
	{
		SaveModelTransform(children);
	}
}

void ComponentAnimation::LoadModelTransform(GameObject* gameObject)
{
	ComponentTransform* transform =
		static_cast<ComponentTransform*>(gameObject->GetComponent(ComponentType::TRANSFORM));
	
	float3 position;
	float3 scale;
	float4x4 rot;

	defaultPosition[gameObject].Decompose(position, rot, scale);
	transform->SetPosition(position);
	transform->SetRotation(rot);
	transform->SetScale(scale);

	for (GameObject* children : gameObject->GetChildren())
	{
		LoadModelTransform(children);
	}
}
