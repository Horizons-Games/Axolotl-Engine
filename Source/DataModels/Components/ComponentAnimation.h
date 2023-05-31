#pragma once

#include "Auxiliar/Generics/Drawable.h"
#include "Auxiliar/Generics/Updatable.h"

#include "Components/Component.h"

#include "Math/float4x4.h"
#include "Resources/ResourceStateMachine.h"
#include <memory>
#include <unordered_map>
#include <vector>

#define NON_STATE 9999

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
	void ActivateDrawBones(bool drawBones);

	bool IsDrawBonesActivated() const;

	bool isPlaying() const;

private:
	bool CheckTransitions(State* state, Transition& transition);
	void SaveModelTransform(GameObject* gameObject);
	void LoadModelTransform(GameObject* gameObject);

	AnimationController* controller;
	std::shared_ptr<ResourceStateMachine> stateMachine;
	std::unordered_map<std::string, TypeFieldPairParameter> parameters;
	std::unordered_map<GameObject*, float4x4> defaultPosition;

	unsigned int actualState;
	unsigned int nextState;
	int lastState;

	bool drawBones;
};

inline void ComponentAnimation::SetParameter(const std::string& parameterName, ValidFieldTypeParameter value)
{
	parameters[parameterName].second = value;
}

inline void ComponentAnimation::ActivateDrawBones(bool drawBones)
{
	this->drawBones = drawBones;
}

inline bool ComponentAnimation::IsDrawBonesActivated() const
{
	return drawBones;
}

inline bool ComponentAnimation::isPlaying() const
{
	return controller->GetPlay();
}
