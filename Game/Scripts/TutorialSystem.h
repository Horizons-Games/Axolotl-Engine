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
	int GetTutorialCurrentState() const;
	float GetNumControllableState() const;

	int GetTutorialSlideSize() const;
	void SetTutorialSlideSize(int tutorialTotalStates);

private:
	int tutorialCurrentState;
	int tutorialTotalStates;
	bool userControllable;
	bool isWaiting;
	float numNotControllableStates;
	float stateWaitTime;
	float totalStateWaitTime;
	std::vector<GameObject*> tutorialUI;
	GameObject* currentTutorialUI;
	GameObject* NextTutorialUI;
	GameObject* dummy;
	HealthSystem* dummyHealthSystem;


	ComponentAudioSource* componentAudio;
	ComponentAnimation* componentAnimation;
	ComponentRigidBody* componentRigidBody;
	UIImageDisplacementControl* displacementControl;
};
