#pragma once

#include "Resource.h"
#include <memory>
#include <unordered_map>
#include <variant>

#include "Auxiliar/Reflection/TypeToEnum.h"

struct State
{
	UID id;
	std::string name;
	std::shared_ptr<Resource> resource;
	std::pair<int, int> auxiliarPos;
	std::vector<UID> transitionsOriginedHere;
	std::vector<UID> transitionsDestinedHere;
};

struct Transition
{
	State* origin;
	State* destination;
	double transitionDuration;
};

//for now only allow floats
using ValidFieldType = std::variant<float,bool>;
using TypeFieldPair = std::pair<FieldType, ValidFieldType>;

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
	const std::vector<State*>& GetStates() const;
	const std::unordered_map<UID, Transition>& GetTransitions() const;
	int GetIdState(const State& state) const;

	void AddNewState(int x, int y);
	void SetStateName(unsigned int id, std::string name);
	void SetStateResource(unsigned int id, const std::shared_ptr<Resource>& resource);

	void EraseState(unsigned int id);

	void AddNewTransition(int idOrigin, int idDestiny);

	void EditDestinyTransition(unsigned int idTransition, unsigned int idState);

	void EraseTransition(unsigned int id);

	void SetParameter(const std::string& parameterName, ValidFieldType value);
	void AddParameter(std::string parameterName, FieldType type, ValidFieldType value);
	const std::unordered_map<std::string, TypeFieldPair>& GetParameters() const;
	void EraseParameter(const std::string& parameterName);

protected:
	void InternalLoad() override {};
	void InternalUnload() override {};

private:
	std::vector<State*> states;
	std::unordered_map<UID, Transition> transitions;
	std::unordered_map<std::string, TypeFieldPair> parameters;
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

inline const std::vector<State*>& ResourceStateMachine::GetStates() const
{
	return states;
}

inline const std::unordered_map<UID, Transition>& ResourceStateMachine::GetTransitions() const
{
	return transitions;
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
	return parameters;
}

inline void ResourceStateMachine::EraseParameter(const std::string& parameterName)
{
	parameters.erase(parameterName);
}
