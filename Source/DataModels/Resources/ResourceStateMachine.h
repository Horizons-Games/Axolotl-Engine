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
	State* origin;
	State* destination;
	double transitionDuration;
	std::vector<Condition> conditions;
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
	std::vector<State*> GetStates() const;
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

protected:
	void InternalLoad() override {};
	void InternalUnload() override {};

private:
	std::vector<std::unique_ptr<State>> states;
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

inline std::vector<State*> ResourceStateMachine::GetStates() const
{
	std::vector<State*> rawStates;
	rawStates.resize(states.size());

	if (!states.empty())
		std::transform(std::begin(states), std::end(states), std::begin(rawStates),
			[](const std::unique_ptr<State>& go) { return go.get(); });

	return rawStates;
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
	states[id]->name = name;
}

inline void ResourceStateMachine::SetStateResource(unsigned int id, const std::shared_ptr<Resource>& resource)
{
	states[id]->resource = resource;
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

