#include "ComponentAnimation.h"
#include "Application.h"
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

ComponentAnimation::ComponentAnimation(const bool active, GameObject* owner) :
	Component(ComponentType::ANIMATION, active, owner, true),
	drawBones(false)
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
		if ((actualState == 0) && (lastState == NON_STATE)) // Entry State
		{
			SaveModelTransform(owner);
		}

		controller->Update();

		if (actualState == nextState)
		{
			State* state = stateMachine->GetState(actualState);
			if (state)
			{
				Transition foundTransition;
				if (CheckTransitions(state, foundTransition))
				{
					nextState = foundTransition.destinationState;
				}

				if (controller->GetPlay())
				{
					std::list<GameObject*> children = owner->GetAllDescendants();

					for (auto child : children)
					{
						float3 pos;
						Quat rot;

						if (controller->GetTransform(&child->GetName()[0], pos, rot))
						{
							ComponentTransform* transform = child->GetComponent<ComponentTransform>();
							transform->SetPosition(pos);
							transform->SetRotation(rot);
						}
					}
					owner->GetComponent<ComponentTransform>()->UpdateTransformMatrices();
				}
				else if (state->resource && state->loop)
				{
					controller->Play(std::dynamic_pointer_cast<ResourceAnimation>(state->resource), false);
				}
			}
		}
		else
		{
			actualState = nextState;
			const State* state = stateMachine->GetState(actualState);
			if (state->resource)
			{
				controller->Play(std::dynamic_pointer_cast<ResourceAnimation>(state->resource), false);
			}
			else
			{
				controller->Stop();
				LoadModelTransform(owner);
				owner->GetComponent<ComponentTransform>()->UpdateTransformMatrices();
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
	if (App->GetPlayState() == Application::PlayState::STOPPED && drawBones)
	{
		DrawBones(owner);
	}
}

void ComponentAnimation::DrawBones(GameObject* parent) const
{
	ComponentTransform* parentTransform = parent->GetComponent<ComponentTransform>();

	GameObject::GameObjectView children = parent->GetChildren();
	for (GameObject* child : children)
	{
		ComponentTransform* childTransform = child->GetComponent<ComponentTransform>();
		dd::line(childTransform->GetGlobalPosition(), parentTransform->GetGlobalPosition(), dd::colors::Blue);
		dd::axisTriad(childTransform->GetGlobalMatrix(), 0.1f, 2.0f);

		DrawBones(child);
	}
}

void ComponentAnimation::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool) active;
	meta["removed"] = (bool) canBeRemoved;

	UID uidState = 0;
	std::string assetPath = "";

	if (stateMachine)
	{
		uidState = stateMachine->GetUID();
		assetPath = stateMachine->GetAssetsPath();
	}

	meta["stateUID"] = (UID) uidState;
	meta["assetPathState"] = assetPath.c_str();
}

void ComponentAnimation::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];
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

	actualState = 0;
	nextState = 0;
}

bool ComponentAnimation::CheckTransitions(State* state, Transition& transition)
{
	if (!state)
	{
		return false;
	}

	for (UID idTransition : state->transitionsOriginedHere)
	{
		Transition& actualTransition = stateMachine->GetTransitions()[idTransition];
		bool conditionCheck = true;
		for (Condition& condition : actualTransition.conditions)
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

			if (!conditionCheck)
				break;
		}

		if (conditionCheck)
		{
			if (actualTransition.waitUntilFinish && controller->GetPlay())
			{
				return false;
			}

			transition = actualTransition;
			return true;
		}
	}

	return false;
}

void ComponentAnimation::SaveModelTransform(GameObject* gameObject)
{
	ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();

	defaultPosition[gameObject] = transform->GetLocalMatrix();

	for (GameObject* children : gameObject->GetChildren())
	{
		SaveModelTransform(children);
	}
}

void ComponentAnimation::LoadModelTransform(GameObject* gameObject)
{
	ComponentTransform* transform = gameObject->GetComponent<ComponentTransform>();

	float3 position;
	float3 scale;
	Quat rot;

	defaultPosition[gameObject].Decompose(position, rot, scale);
	transform->SetPosition(position);
	transform->SetRotation(rot);
	transform->SetScale(scale);

	for (GameObject* children : gameObject->GetChildren())
	{
		LoadModelTransform(children);
	}
}

bool ComponentAnimation::isPlaying() const
{
	return controller->GetPlay();
}
