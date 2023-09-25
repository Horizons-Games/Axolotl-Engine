#include "StdAfx.h"
#include "StateMachine.h"
#include "Resources/ResourceStateMachine.h"

StateMachine::StateMachine()
{
}

StateMachine::~StateMachine()
{
}

void StateMachine::Update(bool statePlayFinish)
{
	if (!stateMachine)
	{
		return;
	}

	if (actualState == nextState)
	{
		State* state = stateMachine->GetState(actualState);
		if (!state)
		{
			return;
		}

		Transition foundTransition;
		if (CheckTransitions(state, foundTransition, statePlayFinish))
		{
			nextState = foundTransition.destinationState;
			actualTransitionDuration = foundTransition.transitionDuration;
		}
	}
}

void StateMachine::FinishTransition()
{
	lastState = actualState;
	actualState = nextState;
}

bool StateMachine::CheckTransitions(const State* state, Transition& transition, bool statePlayFinish)
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
			if (actualTransition.waitUntilFinish && !statePlayFinish)
			{
				return false;
			}

			transition = actualTransition;
			lastTransition = idTransition;
			return true;
		}
	}

	return false;
}

bool StateMachine::IsTransitioning() const
{
	return actualState != nextState;
}

double StateMachine::GetActualTransitionDuration() const
{
	return actualTransitionDuration;
}

State* StateMachine::GetActualState() const
{
	return stateMachine->GetState(actualState);
}

State* StateMachine::GetNextState() const
{
	return stateMachine->GetState(nextState);
}

State* StateMachine::GetLastState() const
{
	if (lastState == NON_STATE)
	{
		return nullptr;
	}
	return stateMachine->GetState(lastState);
}

long long StateMachine::GetLastTranstionID() const
{
	return lastTransition;
}
