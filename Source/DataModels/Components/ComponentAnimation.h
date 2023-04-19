#pragma once

#include "Components/Component.h"

#include <vector>
#include <memory>

class AnimationController;
class ResourceAnimation;
class ResourceStateMachine;

class ComponentAnimation : public Component
{
public:
	ComponentAnimation(const bool active, GameObject* owner);
	~ComponentAnimation() override;

	AnimationController* GetController();
	void SetAnimations(std::vector<std::shared_ptr<ResourceAnimation>> animations);

	const std::shared_ptr<ResourceStateMachine>& GetStateMachine() const;
	void SetStateMachine(const std::shared_ptr<ResourceStateMachine>& stateMachine);

	void Update() override;
	void Draw() override;

	void DrawBones(GameObject* parent, GameObject* lastNonExtraNode) const;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

private:
	AnimationController* controller;
	std::shared_ptr<ResourceStateMachine> stateMachine;
	std::vector<std::shared_ptr<ResourceAnimation>> animations;
	unsigned int animationIx;
};
