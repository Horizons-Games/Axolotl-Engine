#pragma once
#include "PopUpWindow.h"

class WindowBuild : public PopUpWindow
{
public:
	WindowBuild();
	~WindowBuild() override;

	bool StartedBuild() const;

protected:
	void DrawWindowContents() override;

private:
	void DrawSceneComboBox();
	void DrawBuildTypeComboBox();
	void DrawGenerateZipCheckmark();
	void DrawBuildButton();

	std::string selectedScene = "-";
	std::string selectedBuild = "-";
	bool generateZip = true;

	bool startedBuild = false;
};

inline bool WindowBuild::StartedBuild() const
{
	return startedBuild;
}
