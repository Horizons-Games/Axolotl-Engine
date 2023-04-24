#include "ResourceStateMachine.h"
#include <string>
#include "EngineLog.h"

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
		if (states[i]->id == state.id) return i;
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
	defaultParameters[parameterName].second = value;
}

void ResourceStateMachine::AddParameter(std::string parameterName, FieldType type, ValidFieldType value)
{
	int nextNum = 0;
	for (const auto& it : defaultParameters)
	{
		if (it.first == parameterName)
		{
			++nextNum;
			continue;
		}
		std::string noParenthesis = parameterName;
		size_t openParenthesis = it.first.find_first_of("(");
		if (openParenthesis == std::string::npos)
		{
			continue;
		}
		noParenthesis = it.first.substr(0, openParenthesis);
		if (noParenthesis != parameterName)
		{
			continue;
		}
		size_t closeParenthesis = it.first.find_last_of(")");
		if (closeParenthesis == std::string::npos)
		{
			continue;
		}
		size_t firstDigitPosition = openParenthesis + 1;
		std::string numberString = it.first.substr(firstDigitPosition,  closeParenthesis - firstDigitPosition);
		bool stringIsNotNumeric =
			std::any_of(std::begin(numberString), std::end(numberString), [](char c) { return !isdigit(c); });
		if (stringIsNotNumeric)
		{
			ENGINE_LOG("Found non-numeric string inside parenthesis, skipping check."
				"String found: %s", numberString.c_str());
			continue;
		}
		int num = std::stoi(numberString);
		nextNum = std::max(num + 1, nextNum);
	}

	if (nextNum != 0)
	{
		parameterName = parameterName + "(" + std::to_string(nextNum) + ")";
	}

	defaultParameters[parameterName] = std::make_pair(type, value);
}

void ResourceStateMachine::AddCondition(const UID transition)
{
	Condition condition;
	if (defaultParameters.size() > 0)
	{
		SelectConditionParameter(defaultParameters.begin()->first, condition);
	}
	transitions[transition].conditions.push_back(std::move(condition));
}

void ResourceStateMachine::SelectConditionParameter(const std::string& parameter, Condition& condition)
{
	condition.parameter = parameter;
	const auto& it = defaultParameters.find(parameter);
	switch (it->second.first)
	{
	case FieldType::FLOAT:
		condition.conditionType = ConditionType::GREATER;
		condition.value = 0.0f;
		break;
	case FieldType::BOOL:
		condition.conditionType = ConditionType::TRUECONDITION;
		condition.value = true;
		break;
	default:
		break;
	}
}

void ResourceStateMachine::SelectCondition(const ConditionType& type, Condition& condition)
{
	condition.conditionType = type;
}

void ResourceStateMachine::SelectConditionValue(const ValidFieldType value, Condition& condition)
{
	condition.value = value;
}
