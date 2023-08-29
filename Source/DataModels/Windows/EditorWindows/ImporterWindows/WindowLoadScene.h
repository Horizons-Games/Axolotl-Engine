#pragma once
#include "Windows/EditorWindows/WindowFileBrowser.h"

class WindowLoadScene : public WindowFileBrowser
{
public:
	WindowLoadScene();
	~WindowLoadScene() override;

	void DoThisIfOk() override;

	bool IsLoadingScene() const;

private:
	bool loadingScene;
};

inline bool WindowLoadScene::IsLoadingScene() const
{
	return loadingScene;
}
