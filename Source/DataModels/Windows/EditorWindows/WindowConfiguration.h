#pragma once

#include "EditorWindow.h"

class SubWindow;

class WindowConfiguration : public EditorWindow
{
public:
	WindowConfiguration();
	~WindowConfiguration() override;

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;


private:
	std::vector<std::unique_ptr<SubWindow> > collapsingSubWindows;
};

inline ImVec2 WindowConfiguration::GetStartingSize() const
{
	return ImVec2(900, 250);
}
