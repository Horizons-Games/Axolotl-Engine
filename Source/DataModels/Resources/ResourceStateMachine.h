#pragma once

#include "Resource.h"
#include <memory>
#include <unordered_map>
#include <variant>
#include "Auxiliar/Reflection/TypeToEnum.h"


using ValidFieldType = std::variant<float, bool>;
using TypeFieldPair = std::pair<FieldType, ValidFieldType>;

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
};

struct Condition
{
	std::string parameter;
	ConditionType conditionType;
	ValidFieldType value;
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
	const std::vector<State*>& GetStates() const;
	std::unordered_map<UID, Transition>& GetTransitions();
	int GetIdState(const State& state) const;
	void SetStates(const std::vector<State*>& states);
	void SetMapParameters(const std::unordered_map<std::string, TypeFieldPair>& parameters);
	void SetTransitions(const std::unordered_map<UID, Transition>& transitions);

	void AddNewState(int x, int y);
	void SetStateName(unsigned int id, std::string name);
	void SetStateResource(unsigned int id, const std::shared_ptr<Resource>& resource);
	void EraseState(unsigned int id);

	void AddNewTransition(int idOrigin, int idDestiny);
	void SetDurationTransition(UID id, double transition);
	void EraseTransition(UID id);

	void SetParameter(const std::string& parameterName, ValidFieldType value);
	void AddParameter(std::string parameterName, FieldType type, ValidFieldType value);
	const std::unordered_map<std::string, TypeFieldPair>& GetParameters() const;
	void EraseParameter(const std::string& parameterName);

	void AddCondition(const UID transition);

	void SelectConditionParameter(const std::string& parameter, Condition& condition);
	void SelectCondition(const ConditionType& type, Condition& condition);
	void SelectConditionValue(const ValidFieldType value, Condition& condition);

protected:
	void InternalLoad() override {};
	void InternalUnload() override {};

private:
	std::vector<State*> states;
	std::unordered_map<UID, Transition> transitions;
	std::unordered_map<std::string, TypeFieldPair> defaultParameters;
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

inline const std::vector<State*>& ResourceStateMachine::GetStates() const
{
	return states;
}

inline std::unordered_map<UID, Transition>& ResourceStateMachine::GetTransitions()
{
	return transitions;
}

inline void ResourceStateMachine::SetStates(const std::vector<State*>& states)
{
	this->states = states;
}

inline void ResourceStateMachine::SetMapParameters(const std::unordered_map<std::string, TypeFieldPair>& parameters)
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

inline const std::unordered_map<std::string, TypeFieldPair>& ResourceStateMachine::GetParameters() const
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

