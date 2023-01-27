#pragma once

#include "EditorWindow.h"

#include "GL/glew.h"

class WindowScene : public EditorWindow
{
public:
	WindowScene();
	~WindowScene();

	ImVec2 GetStartingPos() const;
	ImVec2 GetEndingPos() const;

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;

private:
	void ManageResize();

	GLuint texture = 0;
	float currentWidth = 0;
	float currentHeight = 0;

	ImVec2 startingPos;
	ImVec2 endingPos;
};

inline ImVec2 WindowScene::GetStartingSize() const
{
	return ImVec2(900, 250);
}

inline ImVec2 WindowScene::GetStartingPos() const
{
	return startingPos;
}	

inline ImVec2 WindowScene::GetEndingPos() const
{
	return endingPos;
}