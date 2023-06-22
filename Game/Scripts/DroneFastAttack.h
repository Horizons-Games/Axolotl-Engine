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

	void StartAttack();
	void PerformAttack();
	void Reposition(float3 nextPosition);

	bool IsAttackAvailable() const;
	bool NeedReposition() const;
	bool MovingToNewReposition();

private:

	float attackCooldown;
	float lastAttackTime;

	bool needReposition;
	bool movingToNewReposition;

	ComponentAudioSource* audioSource;
	ComponentTransform* transform;
	ComponentAnimation* animation;

	ComponentTransform* bulletOrigin;
	GameObject* bulletOriginGO;
	GameObject* bulletPrefab;
	float bulletVelocity;

	Scene* loadedScene;

	float3 newReposition;
};