#pragma once

#include "ComponentWindow.h"

class ComponentAudioListener;

class WindowComponentAudioListener : public ComponentWindow
{
public:
	WindowComponentAudioListener(ComponentAudioListener* component);
	~WindowComponentAudioListener() override;

protected:
	void DrawWindowContents() override;
};
