#pragma once
#include "Windows/EditorWindows/EditorWindow.h"

class WindowImporter : public EditorWindow
{
public:
	WindowImporter();
	virtual ~WindowImporter() ;
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;

	
	
protected:
	bool isLoad = false;
	const char* dialogName;
	const char* title;
	const char* filters;
	const char* startPath;
};

inline ImVec2 WindowImporter::GetStartingSize() const
{
	return ImVec2(900, 250);
}