#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class ComponentParticleSystem;

class WindowParticleSystemInput :
	public WindowFileBrowser
{
public:
	WindowParticleSystemInput(ComponentParticleSystem* state);
	~WindowParticleSystemInput() override;

	void DoThisIfOk() override;

private:
	ComponentParticleSystem* component;
};
