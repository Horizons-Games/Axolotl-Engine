#pragma once

#include "EditorWindow.h"
#include "Windows/SubWindows/SubWindow.h"

#include <vector>
#include <memory>

class WindowConfiguration : public EditorWindow
{
public:
	WindowConfiguration();
	~WindowConfiguration();

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;
	bool firstDeployed = false;


private:
	std::vector<std::unique_ptr<SubWindow> > collapsingSubWindows;
};

inline ImVec2 WindowConfiguration::GetStartingSize() const
{
	return ImVec2(900, 250);
}
