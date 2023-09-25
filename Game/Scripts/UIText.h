#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class GameObject;
class ComponentSlider;

class UIText : public Script
{
public:
	UIText();
	~UIText() override = default;

	void Init() override;
	void Update(float deltaTime) override;

	void AddInputVisuals(float numFrom);
	void CleanInputVisuals();

	void FpsMetric(float time);

private:

	std::deque<GameObject*> inputVisuals;
	std::vector<GameObject*> inputPositions;
	std::vector<GameObject*> numbersIMG;

	float actualNumber;
	float prevNumber;
	float currentTime;
	float previusTime = 0.0f;
	float fps;
	float ms;
	float timeDiff;
	int counter;
};