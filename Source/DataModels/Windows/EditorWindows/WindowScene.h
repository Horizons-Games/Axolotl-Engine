#pragma once

#include "EditorWindow.h"

#include "GL/glew.h"

class WindowScene : public EditorWindow
{
public:
	WindowScene();
	~WindowScene();

protected:
	void DrawWindowContents() override;

private:
	void ManageResize();

	GLuint texture = 0;
	float previousWidht = 0;
	float previousHeight = 0;
};

