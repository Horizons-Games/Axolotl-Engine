#pragma once

#include "Scripting\Script.h"

class ModuleInput;
class ComponentAudioSource;
class ComponentTransform;
class ComponentAnimation;
class Scene;

class DroneFastAttack : public Script
{
public:
	DroneFastAttack();
	~DroneFastAttack() override = default;

	void Start() override;

	void PerformAttack();

private:
	bool IsAttackAvailable();

	float attackCooldown;
	float lastAttackTime;

	ComponentAudioSource* audioSource;
	ComponentTransform* transform;
	ComponentAnimation* animation;

	ComponentTransform* bulletOrigin;
	GameObject* bulletOriginGO;
	GameObject* bulletPrefab;
	float bulletVelocity;

	Scene* loadedScene;
};