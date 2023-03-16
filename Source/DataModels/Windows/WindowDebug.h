#pragma once

#include "Window.h"

class WindowDebug : public Window
{
public:
	WindowDebug();
	~WindowDebug() override;

	void Draw(bool& enabled = defaultEnabled) override;

private:
	static bool defaultEnabled;
};

