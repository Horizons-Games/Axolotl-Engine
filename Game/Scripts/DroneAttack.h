#pragma once

#include "Scripting\Script.h"

#include "Components/ComponentTransform.h"

class ModuleInput;
class ComponentAudioSource;
class ComponentTransform;
class ComponentAnimation;
class Scene;

class DroneAttack : public Script
{

public:
	DroneAttack();
	~DroneAttack() override = default;

private:
	void Start() override;
	void Update(float deltaTime) override;

	void PerformAttack();
	bool isAttackAvailable();

	void ShootBullets(float deltaTime);

	float attackCooldown;
	float lastAttackTime;

	ComponentAudioSource* audioSource;
	ComponentTransform* transform;
	ComponentAnimation* animation;
	GameObject* animationGO;
	ComponentTransform* bulletOrigin;
	GameObject* bulletOriginGO;
	Scene* loadedScene;

	//Provisional
	float bulletVelocity;
	std::vector<ComponentTransform*> bullets;

	//For Debug
	ModuleInput* input;

};