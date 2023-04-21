#include "ResourceStateMachine.h"
#include <string> 

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

void ResourceStateMachine::SetParameter(const std::string& parameterName, ValidFieldType value)
{
	parameters[parameterName].second = value;
}

void ResourceStateMachine::AddParameter(std::string parameterName, FieldType type, ValidFieldType value)
{
	int nextNum = 0;
	for (const auto& it : parameters)
	{
		std::string noParenthesis = parameterName;
		size_t pos = it.first.find_first_of("(");
		if (pos != std::string::npos)
		{
			noParenthesis = it.first.substr(0, pos);
			if (noParenthesis == parameterName)
			{
				size_t posFinal = it.first.find_last_of(")");
				if (posFinal != std::string::npos)
				{
					std::string number = 
						it.first.substr(pos + 1, posFinal);
					number.pop_back();
					int num = std::stoi(number) + 1; //TODO I don't want to check if this is a number, help
					if (num > nextNum) nextNum = num;
				}
				else if (1 > nextNum) nextNum = 1;
				
			}
		}
		else if (noParenthesis == parameterName) if (1 > nextNum) nextNum = 1;
	}

	/*int count = std::count_if(std::begin(parameters), std::begin(parameters),
		[&parameterName](std::pair<const std::string,TypeFieldPair>& it)
		{
			std::string noParenthesis = parameterName;
			size_t pos = it.first.find_first_of("(");
			if (pos != std::string::npos) noParenthesis = it.first.substr(it.first.find_first_of("("));
			return noParenthesis == it.first;
		}
	);*/

	if (nextNum != 0)
	{
		parameterName = parameterName + "(" + std::to_string(nextNum) + ")";
	}

	parameters[parameterName] = std::make_pair(type, value);
}