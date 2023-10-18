#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class SceneLoadingScript : public Script
{
public:
	SceneLoadingScript();
	~SceneLoadingScript() override = default;

	void Init() override;

	void StartLoad() const;

private:
	void OnLoadingScreenLoaded() const;

private:
	std::string sceneToLoad;
};
