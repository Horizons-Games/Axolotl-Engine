#pragma once

#include "Auxiliar/Generics/Drawable.h"
#include "Auxiliar/Generics/Updatable.h"

#include "Components/Component.h"

#include "Resources/ResourceStateMachine.h"

#define NON_STATE 9999

class GameObject;
class AnimationController;
class ResourceAnimation;
struct State;
struct Transition;

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

	void SetParameter(const std::string& parameterName, ValidFieldTypeParameter value);
	void ActivateDrawBones(bool drawBones);

	bool IsDrawBonesActivated() const;

	bool IsPlaying() const;
	std::string& GetActualStateName() const;

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	bool CheckTransitions(const State* state, Transition& transition);
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

inline std::string& ComponentAnimation::GetActualStateName() const
{
	return stateMachine->GetState(actualState)->name;
}