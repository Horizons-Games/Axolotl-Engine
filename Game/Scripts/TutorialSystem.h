#pragma once

#include "RuntimeInclude.h"
#include "Scripting\Script.h"

RUNTIME_MODIFIABLE_INCLUDE;

// This script handles the activation of the doors

class ComponentAudioSource;
class ComponentAnimation;
class ComponentRigidBody;
class ComponentImage;
class ComponentVideo;
class ComponentTransform2D;
class UIImageDisplacementControl;
class HealthSystem;
class PlayerMoveScript;

class TutorialSystem : public Script
{
public:
	TutorialSystem();
	~TutorialSystem() override = default;

	void Start() override;
	void Update(float deltaTime) override;
	void TutorialStart();
	void NextState();
	void DeployUI();
	void UnDeployUI();
	void TutorialEnd();
	void TutorialSkip();
	int GetTutorialCurrentState() const;
	float GetNumControllableState() const;
	void SetNumControllableState(int controllableState);

	int GetTutorialSlideSize() const;
	void SetTutorialSlideSize(int tutorialTotalStates);

	UIImageDisplacementControl* GetDisplacementControl();

private:
	float tutorialCurrentState;
	float tutorialTotalStates;
	bool userControllable;
	bool isWaiting;
	float numNotControllableStates;
	float stateWaitTime;
	float totalStateWaitTime;
	std::vector<GameObject*> tutorialUI;
	GameObject* currentTutorialUI;
	GameObject* NextTutorialUI;
	GameObject* player;
	GameObject* dummy;
	HealthSystem* dummyHealthSystem;
	PlayerMoveScript* componentMoveScript;

	float3 initialPos;
	float3 stayPos;

	ComponentAudioSource* componentAudio;
	ComponentAnimation* componentAnimation;
	ComponentRigidBody* componentRigidBody;
	ComponentTransform2D* transform2D;
	UIImageDisplacementControl* displacementControl;

};
