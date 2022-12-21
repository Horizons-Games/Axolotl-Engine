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

	ImVec2 GetStartingSize() const override
	{
		return ImVec2(900, 250);
	}

private:
	void ManageResize();

	GLuint texture = 0;
	float previousWidht = 0;
	float previousHeight = 0;
};

