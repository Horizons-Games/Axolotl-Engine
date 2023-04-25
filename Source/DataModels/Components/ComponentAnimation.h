#pragma once

#include "Auxiliar/Generics/Drawable.h"
#include "Auxiliar/Generics/Updatable.h"

#include "Components/Component.h"

#include "Resources/ResourceStateMachine.h"
#include "Math/float4x4.h"
#include <vector>
#include <memory>
#include <unordered_map>

class AnimationController;
class ResourceAnimation;
class ResourceStateMachine;
class State;
class Transition;
class GameObject;

class ComponentAnimation : public Component, public Drawable, public Updatable
{
public:
	ComponentAnimation(const bool active, GameObject* owner);
	~ComponentAnimation() override;

	AnimationController* GetController();

	const std::shared_ptr<ResourceStateMachine>& GetStateMachine() const;
	void SetStateMachine(const std::shared_ptr<ResourceStateMachine>& stateMachine);

	void Update() override;
	void Draw() const override;

	void DrawBones(GameObject* parent) const;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void SetParameter(const std::string& parameterName, ValidFieldTypeParameter value);

private:
	Transition* CheckTransitions(State* state);
	void SaveModelTransform(GameObject* gameObject);
	void LoadModelTransform(GameObject* gameObject);


	AnimationController* controller;
	std::shared_ptr<ResourceStateMachine> stateMachine;
	std::unordered_map<std::string, TypeFieldPairParameter> parameters;
	unsigned int actualState;
	unsigned int nextState;

	std::unordered_map<GameObject*, float4x4> defaultPosition;
};

inline void ComponentAnimation::SetParameter(const std::string& parameterName, ValidFieldTypeParameter value)
{
	parameters[parameterName].second = value;
}