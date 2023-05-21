#pragma once

#include "Resource.h"
#include <memory>
#include <unordered_map>
#include <variant>

enum class FieldTypeParameter
{
	FLOAT,
	BOOL
};

using ValidFieldTypeParameter = std::variant<float, bool>;
using TypeFieldPairParameter = std::pair<FieldTypeParameter, ValidFieldTypeParameter>;

enum class ConditionType
{
	GREATER,
	LESS,
	EQUAL,
	NOTEQUAL,
	TRUECONDITION,
	FALSECONDITION
};

struct State
{
	UID id;
	std::string name;
	std::shared_ptr<Resource> resource;
	std::pair<int, int> auxiliarPos;
	std::vector<UID> transitionsOriginedHere;
	std::vector<UID> transitionsDestinedHere;
	bool loop;

	State() :
		id(0),
		loop(false)
	{
	}

	State(const UID& id, const std::string& name, int x, int y) : 
		id(id), 
		name(name), 
		auxiliarPos(std::pair<int, int>(x, y)),
		loop(false)
	{
	}

	~State() = default;
};

struct Condition
{
	std::string parameter;
	ConditionType conditionType;
	ValidFieldTypeParameter value;
};


struct Transition
{
	unsigned int originState;
	unsigned int destinationState;
	bool waitUntilFinish;
	double transitionDuration;
	std::vector<Condition> conditions;

	Transition() : 
		originState(0), 
		destinationState(0),
		waitUntilFinish(false),
		transitionDuration(0.0)
	{
	}
	
	Transition(unsigned int originState, unsigned int destinationState, double transitionDuration) :
		originState(originState),
		destinationState(destinationState),
		transitionDuration(transitionDuration),
		waitUntilFinish(false)
	{
	}

	~Transition() = default;
};

class ResourceStateMachine : virtual public Resource
{
public:
	ResourceStateMachine(UID resourceUID,
		const std::string& fileName,
		const std::string& assetsPath,
		const std::string& libraryPath);
	virtual ~ResourceStateMachine() override;

	ResourceType GetType() const override;

	void SaveImporterOptions(Json& meta) override {};
	void LoadImporterOptions(Json& meta) override {};

	void SaveLoadOptions(Json& meta) override {};
	void LoadLoadOptions(Json& meta) override {};

	unsigned int GetNumStates() const;
	unsigned int GetNumTransitions() const;
	unsigned int GetNumParameters() const;
	State* GetState(size_t stateIndex) const;
	std::unordered_map<UID, Transition>& GetTransitions();
	int GetIdState(const State& state) const;
	
	void AddState(std::unique_ptr<State> state);
	void ClearAllStates();
	void SetMapParameters(const std::unordered_map<std::string, TypeFieldPairParameter>& parameters);
	void SetTransitions(const std::unordered_map<UID, Transition>& transitions);

	void AddNewState(int x, int y);
	void SetStateName(unsigned int id, std::string name);
	void SetStateResource(unsigned int id, const std::shared_ptr<Resource>& resource);
	void EraseState(unsigned int id);
	void SetDeadStates(const std::vector<unsigned int>& deadStates);

	void AddNewTransition(int idOrigin, int idDestiny);
	void SetDurationTransition(UID id, double transition);
	void EraseTransition(UID id);

	void SetParameter(const std::string& parameterName, ValidFieldTypeParameter value);
	void AddParameter(std::string parameterName, FieldTypeParameter type, ValidFieldTypeParameter value);
	const std::unordered_map<std::string, TypeFieldPairParameter>& GetParameters() const;
	void EraseParameter(const std::string& parameterName);

	void AddCondition(const UID transition);
	void SelectConditionParameter(const std::string& parameter, Condition& condition);
	void SelectCondition(const ConditionType& type, Condition& condition);
	void SelectConditionValue(const ValidFieldTypeParameter value, Condition& condition);
	void EraseCondition(const UID transition, unsigned int index);

protected:
	void InternalLoad() override {};
	void InternalUnload() override {};

private:
	std::vector<std::unique_ptr<State>> states;
	std::vector<unsigned int> deadStates;

	std::unordered_map<UID, Transition> transitions;
	std::unordered_map<std::string, TypeFieldPairParameter> defaultParameters;
};

inline ResourceType ResourceStateMachine::GetType() const
{
	return ResourceType::StateMachine;
}

inline unsigned int ResourceStateMachine::GetNumStates() const
{
	return states.size();
}

inline unsigned int ResourceStateMachine::GetNumTransitions() const
{
	return transitions.size();
}

inline unsigned int ResourceStateMachine::GetNumParameters() const
{
	return defaultParameters.size();
}

inline State* ResourceStateMachine::GetState(size_t stateIndex) const
{
	if (states.size() <= stateIndex)
	{
		return nullptr;
	}

	return states[stateIndex].get();
}

inline std::unordered_map<UID, Transition>& ResourceStateMachine::GetTransitions()
{
	return transitions;
}

inline void ResourceStateMachine::AddState(std::unique_ptr<State> state)
{
	states.push_back(std::move(state));
}

inline void ResourceStateMachine::ClearAllStates()
{
	states.clear();
}

inline void ResourceStateMachine::SetMapParameters(const std::unordered_map<std::string, TypeFieldPairParameter>& parameters)
{
	this->defaultParameters = parameters;
}

inline void ResourceStateMachine::SetTransitions(const std::unordered_map<UID, Transition>& transitions)
{
	this->transitions = transitions;
}

inline void ResourceStateMachine::SetStateName(unsigned int id, std::string name)
{
	if(states[id] != nullptr)
	{
		states[id]->name = name;
	}
}


inline void ResourceStateMachine::SetDeadStates(const std::vector<unsigned int>& deadStates)
{
	this->deadStates = deadStates;
}

inline void ResourceStateMachine::SetStateResource(unsigned int id, const std::shared_ptr<Resource>& resource)
{
	if (states[id] != nullptr)
	{
		states[id]->resource = resource;
	}
}

inline const std::unordered_map<std::string, TypeFieldPairParameter>& ResourceStateMachine::GetParameters() const
{
	return defaultParameters;
}

inline void ResourceStateMachine::SetDurationTransition(UID id, double transition)
{
	transitions[id].transitionDuration = transition;
}

inline void ResourceStateMachine::EraseParameter(const std::string& parameterName)
{
	defaultParameters.erase(parameterName);
}

