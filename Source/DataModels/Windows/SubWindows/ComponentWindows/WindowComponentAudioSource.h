#pragma once

#include "ComponentWindow.h"

class ComponentAudioSource;

class WindowComponentAudioSource : public ComponentWindow
{
public:
	WindowComponentAudioSource(ComponentAudioSource* component);
	~WindowComponentAudioSource() override;

protected:
	void DrawWindowContents() override;
};
