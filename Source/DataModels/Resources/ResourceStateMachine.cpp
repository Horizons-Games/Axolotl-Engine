#include "ResourceStateMachine.h"

ResourceStateMachine::ResourceStateMachine(UID resourceUID, const std::string& fileName, const std::string& assetsPath,
	const std::string& libraryPath) : Resource(resourceUID, fileName, assetsPath, libraryPath)
{
}

ResourceStateMachine::~ResourceStateMachine()
{
}

int ResourceStateMachine::GetIdState(const State& state) const
{
	for (int i = 0; i < states.size(); i++)
	{
		if (states[i]->name == state.name) return i;
	}
	return -1;
}

void ResourceStateMachine::AddNewState(int x, int y)
{
	State* state = new State;
	state->name = "NewState";
	state->id = UniqueID::GenerateUID();
	state->auxiliarPos = std::pair<int, int>(x, y);
	states.push_back(state);
}

void ResourceStateMachine::EraseState(unsigned int id)
{
	for (unsigned int transitionid : states[id]->transitionsOriginedHere)
	{
		transitions.erase(transitionid);
	}
	for (unsigned int transitionid : states[id]->transitionsDestinedHere)
	{
		transitions.erase(transitionid);
	}
	states.erase(states.begin() + id);
}

void ResourceStateMachine::AddNewTransition(int idOrigin, int idDestiny)
{
	for (UID transitionId : states[idOrigin]->transitionsOriginedHere)
	{
		const auto& it = transitions.find(transitionId);
		if (it->second.destination->id == states[idDestiny]->id) return;
	}

	Transition transition;
	transition.origin = states[idOrigin];
	transition.destination = states[idDestiny];
	UID uid = UniqueID::GenerateUID();
	states[idDestiny]->transitionsDestinedHere.push_back(uid);
	states[idOrigin]->transitionsOriginedHere.push_back(uid);
	transitions[uid] = transition;
}

void ResourceStateMachine::EditDestinyTransition(unsigned int idTransition, unsigned int idState)
{
	transitions[idTransition].destination = states[idState];
}

void ResourceStateMachine::EraseTransition(unsigned int id)
{
	State* stateOrigin = transitions[id].origin;
	remove(stateOrigin->transitionsOriginedHere.begin(), stateOrigin->transitionsOriginedHere.end(), id);
	remove(stateOrigin->transitionsDestinedHere.begin(), stateOrigin->transitionsDestinedHere.end(), id);
	transitions.erase(id);
}