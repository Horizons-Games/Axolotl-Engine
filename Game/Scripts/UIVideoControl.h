#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform2D;
class ComponentVideo;
class SceneLoadingScript;

class UIVideoControl : public Script
{
public:
	UIVideoControl();
	~UIVideoControl() override = default;

	void Start() override;
	void Update(float deltaTime) override;
	
	void ChangeVideoTransform();

private:

	float3 startPosition;
	float3 endPosition;
	float3 currentPositon;

	bool playingVideo;
	bool changeVideoTransform;
	bool playVideo;
	bool isFirstCutscene;

	GameObject* videoObject;
	GameObject* object;
	ComponentTransform2D* imageTransform;
	ComponentVideo* videoImage;
	SceneLoadingScript* loadingScreenScript;
};
