#pragma once

#include "RuntimeInclude.h"
#include "Scripting\Script.h"

RUNTIME_MODIFIABLE_INCLUDE;

// This script handles the activation of the doors

class ComponentAudioSource;
class ComponentAnimation;
class ComponentRigidBody;



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
	void TutorialEnd();
	int GetTutorialCurrentState() const;

	int GetTutorialSlideSize() const;
	void SetTutorialSlideSize(int tutorialTotalStates);

private:
	int tutorialCurrentState;
	int tutorialTotalStates;
	bool userControllable;
	std::vector<GameObject*> tutorialUI;
	GameObject* currentTutorialUI;
	GameObject* NextTutorialUI;


	ComponentAudioSource* componentAudio;
	ComponentAnimation* componentAnimation;
	ComponentRigidBody* componentRigidBody;
};
