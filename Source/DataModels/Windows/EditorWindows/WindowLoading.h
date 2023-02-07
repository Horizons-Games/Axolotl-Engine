#pragma once

#include "EditorWindow.h"

class WindowLoading : public EditorWindow
{
public:
	WindowLoading();
	~WindowLoading();

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;
};

inline ImVec2 WindowLoading::GetStartingSize() const
{
	return ImVec2(900, 250);
}
