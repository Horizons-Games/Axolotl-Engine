#pragma once

#include "Windows/Window.h"

class SubWindow : public Window
{
public:
	~SubWindow();

	void Draw(bool& enabled = defaultEnabled) override;

protected:
	SubWindow(const std::string& name);
	virtual void DrawWindowContents() = 0;

private:
	static bool defaultEnabled;
};

