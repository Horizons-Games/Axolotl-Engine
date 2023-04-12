#pragma once

#include "Components/Component.h"

#include <vector>
#include <memory>

class AnimationController;
class ResourceAnimation;

class ComponentAnimation : public Component
{
public:
	ComponentAnimation(const bool active, GameObject* owner);
	~ComponentAnimation() override;

	AnimationController* GetController();
	void SetAnimations(std::vector<std::shared_ptr<ResourceAnimation>> animations);

	void Update() override;
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

private:
	AnimationController* controller;
	std::vector<std::shared_ptr<ResourceAnimation>> animations;
	unsigned int animationIx;
};
