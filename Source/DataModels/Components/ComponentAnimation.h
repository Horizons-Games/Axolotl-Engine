#pragma once

#include "Auxiliar/Generics/Drawable.h"
#include "Auxiliar/Generics/Updatable.h"

#include "Components/Component.h"
#include "Math/float4x4.h"
#include "Resources/ResourceStateMachine.h"

#define NON_STATE 9999

class GameObject;
class AnimationController;
class ResourceAnimation;
struct State;
struct Transition;
class StateMachine;

class ComponentAnimation : public Component, public Drawable, public Updatable
{
public:
	ComponentAnimation(const bool active, GameObject* owner);
	ComponentAnimation(const ComponentAnimation& toCopy);
	~ComponentAnimation() override;

	AnimationController* GetController() const;

	StateMachine* GetStateMachineInstance() const;
	std::shared_ptr<ResourceStateMachine> GetStateMachine() const;
	void SetStateMachine(std::shared_ptr<ResourceStateMachine> stateMachine);

	void Update() override;
	void Draw() const override;

	void DrawBones(GameObject* parent) const;

	void SetParameter(const std::string& parameterName, ValidFieldTypeParameter value);
	void ActivateDrawBones(bool drawBones);

	bool IsDrawBonesActivated() const;

	bool isTransitioning();
	bool IsPlaying() const;
	std::string& GetActualStateName() const;

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	void SaveModelTransform(GameObject* gameObject);
	void LoadModelTransform(GameObject* gameObject);

	void ApplyTransform();
	
	std::unique_ptr<AnimationController> controller;
	std::unique_ptr<StateMachine> stateMachineInstance;
	std::unordered_map<GameObject*, float4x4> defaultPosition;

	bool firstEntry;
	bool drawBones;
	bool initTransition;
};

inline void ComponentAnimation::ActivateDrawBones(bool drawBones)
{
	this->drawBones = drawBones;
}

inline bool ComponentAnimation::IsDrawBonesActivated() const
{
	return drawBones;
}