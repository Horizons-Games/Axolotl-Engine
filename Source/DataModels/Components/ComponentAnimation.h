#pragma once

#include "Components/Component.h"

#include "Resources/ResourceStateMachine.h"
#include <vector>
#include <memory>
#include <unordered_map>

class AnimationController;
class ResourceAnimation;
class ResourceStateMachine;
class State;
class Transition;

class ComponentAnimation : public Component
{
public:
	ComponentAnimation(const bool active, GameObject* owner);
	~ComponentAnimation() override;

	AnimationController* GetController();

	const std::shared_ptr<ResourceStateMachine>& GetStateMachine() const;
	void SetStateMachine(const std::shared_ptr<ResourceStateMachine>& stateMachine);

	void Update() override;
	void Draw() override;

	void DrawBones(GameObject* parent) const;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

private:
	Transition* CheckTransitions(State* state);

	AnimationController* controller;
	std::shared_ptr<ResourceStateMachine> stateMachine;
	std::unordered_map<std::string, TypeFieldPair> parameters;
	unsigned int actualState;
	unsigned int nextState;
};
