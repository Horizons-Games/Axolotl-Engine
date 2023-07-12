#pragma once

#include <memory>
#include <unordered_map>
#include "Resources/ResourceStateMachine.h"

#define NON_STATE 9999

struct State;
struct Transition;

class StateMachine
{
public:
	StateMachine();
	~StateMachine();

	void Update(bool statePlayFinish);

	const std::shared_ptr<ResourceStateMachine>& GetStateMachine() const;
	void SetStateMachine(const std::shared_ptr<ResourceStateMachine>& stateMachine);

	void SetParameter(const std::string& parameterName, ValidFieldTypeParameter value);

	std::string& GetActualStateName() const;
	unsigned int GetActualStateID() const;

	State* GetActualState() const;
	State* GetNextState() const;
	State* GetLastState() const;

	long long GetLastTranstionID() const;

	bool CheckTransitions(const State* state, Transition& transition, bool statePlayFinish = true);
	bool IsTransitioning() const;

private:

	std::shared_ptr<ResourceStateMachine> stateMachine;
	std::unordered_map<std::string, TypeFieldPairParameter> parameters;
	
	long long lastTransition;
	unsigned int actualState;
	unsigned int nextState;
	unsigned int lastState;
};

inline const std::shared_ptr<ResourceStateMachine>& StateMachine::GetStateMachine() const
{
	return stateMachine;
}

inline void StateMachine::SetStateMachine(const std::shared_ptr<ResourceStateMachine>& stateMachine)
{
	this->stateMachine = stateMachine;
	if (stateMachine)
	{
		this->parameters = stateMachine->GetParameters();
	}
	actualState = 0;
	nextState = 0;
}

inline void StateMachine::SetParameter(const std::string& parameterName, ValidFieldTypeParameter value)
{
	parameters[parameterName].second = value;
}

inline std::string& StateMachine::GetActualStateName() const
{
	return stateMachine->GetState(actualState)->name;
}

inline unsigned int StateMachine::GetActualStateID() const
{
	return actualState;
}
