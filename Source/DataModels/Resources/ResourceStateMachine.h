#pragma once

#include "Resource.h"
#include <memory>
#include <unordered_map>

struct State
{
	std::string name;
	std::shared_ptr<Resource> resource;
	std::pair<int, int> auxiliarPos;
	std::vector<unsigned int> transitionsOriginedHere;
	std::vector<unsigned int> transitionsDestinedHere;
};

struct Transition
{
	State* origin;
	State* destination;
	double transitionDuration;
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
	const std::vector<State*>& GetStates() const;
	const std::unordered_map<unsigned int, Transition>& GetTransitions() const;
	int GetIdState(const State& state) const;

	void AddNewState(int x, int y);
	void EditState(unsigned int id, State* state);

	void EraseState(unsigned int id);

	void AddNewTransition(int idOrigin, int idDestiny);

	void EditDestinyTransition(unsigned int idTransition, unsigned int idState);

	void EraseTransition(unsigned int id);

protected:
	void InternalLoad() override {};
	void InternalUnload() override {};

private:
	std::vector<State*> states;
	std::unordered_map<unsigned int, Transition> transitions;
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

inline const std::unordered_map<unsigned int, Transition>& ResourceStateMachine::GetTransitions() const
{
	return transitions;
}

inline int ResourceStateMachine::GetIdState(const State& state) const
{
	for(int i = 0; i < states.size(); i++)
	{
		if (states[i]->name == state.name) return i;
	}
	return -1;
}

inline void ResourceStateMachine::AddNewState(int x, int y)
{
	State* state = new State;
	state->name = "NewState";
	state->auxiliarPos = std::pair<int, int>(x,y);
	states.push_back(state);
}

inline void ResourceStateMachine::EditState(unsigned int id, State* state)
{
	states[id] = state;
}

inline void ResourceStateMachine::EraseState(unsigned int id)
{
	for(unsigned int transitionid : states[id]->transitionsOriginedHere)
	{
		transitions.erase(transitionid);
	}
	for (unsigned int transitionid : states[id]->transitionsDestinedHere)
	{
		transitions.erase(transitionid);
	}
	states.erase(states.begin() + id);
}

inline void ResourceStateMachine::AddNewTransition(int idOrigin, int idDestiny)
{
	Transition transition;
	transition.origin = states[idOrigin];
	transition.destination = states[idDestiny];
	states[idDestiny]->transitionsDestinedHere.push_back(transitions.size());
	states[idOrigin]->transitionsOriginedHere.push_back(transitions.size());
	transitions[transitions.size()] = transition;
}

inline void ResourceStateMachine::EditDestinyTransition(unsigned int idTransition, unsigned int idState )
{
	transitions[idTransition].destination = states[idState];
}

inline void ResourceStateMachine::EraseTransition(unsigned int id)
{
	State* stateOrigin = transitions[id].origin;
	remove(stateOrigin->transitionsOriginedHere.begin(), stateOrigin->transitionsOriginedHere.end(), id);
	remove(stateOrigin->transitionsDestinedHere.begin(), stateOrigin->transitionsDestinedHere.end(), id);
	transitions.erase(id);
}