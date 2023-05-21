#pragma once

#include "DataModels/Windows/Window.h"

class PopUpWindow : public Window
{
public:
	virtual ~PopUpWindow() override;

	void Draw(bool& enabled) override;

protected:
	PopUpWindow(const std::string& name);
	virtual void DrawWindowContents() = 0;
};

