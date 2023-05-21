#include "ResourceStateMachine.h"
#include <string>
#include "AxoLog.h"

ResourceStateMachine::ResourceStateMachine(UID resourceUID, const std::string& fileName, const std::string& assetsPath,
	const std::string& libraryPath) : Resource(resourceUID, fileName, assetsPath, libraryPath)
{
}

ResourceStateMachine::~ResourceStateMachine()
{
	states.clear();
}

int ResourceStateMachine::GetIdState(const State& state) const
{
	for (int i = 0; i < states.size(); i++)
	{
		if (states[i] != nullptr && states[i]->id == state.id) return i;
	}
	return -1;
}

void ResourceStateMachine::AddNewState(int x, int y)
{
	std::unique_ptr<State> state = 
		std::make_unique<State>(UniqueID::GenerateUID(), "NewState",x,y);

	if (deadStates.empty()) 
	{
		states.push_back(std::move(state));
	}
	else 
	{
		unsigned int index = deadStates.back();
		states[index] = std::move(state);
		deadStates.pop_back();
	}
}

void ResourceStateMachine::EraseState(unsigned int id)
{
	for (UID transitionid : states[id]->transitionsOriginedHere)
	{
		EraseTransition(transitionid);
	}
	for (UID transitionid : states[id]->transitionsDestinedHere)
	{
		EraseTransition(transitionid);
	}
	deadStates.push_back(id);
	states[id]->~State();
	states[id] = nullptr;
}

void ResourceStateMachine::AddNewTransition(int idOrigin, int idDestiny)
{
	for (UID transitionId : states[idOrigin]->transitionsOriginedHere)
	{
		const auto& it = transitions.find(transitionId);
		if (it != std::end(transitions) && it->second.destinationState == idDestiny) return;
	}

	Transition transition (idOrigin, idDestiny, 0.0);
	UID uid = UniqueID::GenerateUID();
	states[idDestiny]->transitionsDestinedHere.push_back(uid);
	states[idOrigin]->transitionsOriginedHere.push_back(uid);
	transitions[uid] = transition;
}

void ResourceStateMachine::EraseTransition(UID id)
{
	State* stateOrigin = states[transitions[id].originState].get();
	stateOrigin->transitionsOriginedHere.erase(
		std::remove(
			std::begin(stateOrigin->transitionsOriginedHere),
			std::end(stateOrigin->transitionsOriginedHere),
			id),
		std::end(stateOrigin->transitionsOriginedHere));
	stateOrigin->transitionsDestinedHere.erase(
		std::remove(
			std::begin(stateOrigin->transitionsDestinedHere),
			std::end(stateOrigin->transitionsDestinedHere),
			id),
		std::end(stateOrigin->transitionsDestinedHere));
	transitions.erase(id);
}

void ResourceStateMachine::SetParameter(const std::string& parameterName, ValidFieldTypeParameter value)
{
	defaultParameters[parameterName].second = value;
}

void ResourceStateMachine::AddParameter(std::string parameterName, FieldTypeParameter type, ValidFieldTypeParameter value)
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
			ENGINE_LOG("Found non-numeric string inside parenthesis, skipping check. String found: {}", numberString);
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
	case FieldTypeParameter::FLOAT:
		condition.conditionType = ConditionType::GREATER;
		condition.value = 0.0f;
		break;
	case FieldTypeParameter::BOOL:
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

void ResourceStateMachine::SelectConditionValue(const ValidFieldTypeParameter value, Condition& condition)
{
	condition.value = value;
}

void ResourceStateMachine::EraseCondition(const UID transition, unsigned int index)
{
	transitions[transition].conditions.erase(std::begin(transitions[transition].conditions) + index);
}
