#pragma once

#include "EditorWindow.h"

class SubWindow;

class WindowConfiguration : public EditorWindow
{
public:
	WindowConfiguration();
	~WindowConfiguration() override;

	bool DefaultActiveState() const override;

protected:
	void DrawWindowContents() override;

private:
	std::vector<std::unique_ptr<SubWindow>> collapsingSubWindows;
};

inline bool WindowConfiguration::DefaultActiveState() const
{
	return false;
}
