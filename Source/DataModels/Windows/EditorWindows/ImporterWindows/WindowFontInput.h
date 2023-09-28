#pragma once

#include "Windows/EditorWindows/WindowFileBrowser.h"


class WindowFontInput : public WindowFileBrowser
{
public:
	WindowFontInput();
	~WindowFontInput() override;

	void DoThisIfOk() override;	

};
