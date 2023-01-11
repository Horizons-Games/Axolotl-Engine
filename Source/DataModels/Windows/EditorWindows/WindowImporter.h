#pragma once

#include "Windows/EditorWindows/EditorWindow.h"
class WindowImporter: public EditorWindow
{
public:
	WindowImporter();
	~WindowImporter() {};
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;
private:
	
};

inline ImVec2 WindowImporter::GetStartingSize() const
{
	return ImVec2(900, 250);
}