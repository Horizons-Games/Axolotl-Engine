#pragma once

#include "Resource.h"
#include <memory>

class ResourceStateMachine : virtual public Resource
{
public:
	struct State
	{
		std::string name;
		std::shared_ptr<Resource> resource;
	};

	struct Transition
	{
		State origin;
		State destination;
		double transitionDuration;
	};

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

protected:
	void InternalLoad() override {};
	void InternalUnload() override {};

private:
	std::vector<State> states;
	std::vector<Transition> transitions;
};

inline ResourceType ResourceStateMachine::GetType() const
{
	return ResourceType::StateMachine;
}