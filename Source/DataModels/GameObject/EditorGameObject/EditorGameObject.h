#pragma once

#include "GameObject/GameObject.h"

class EditorGameObject : public GameObject
{
public:
	explicit EditorGameObject(const char* name);
	EditorGameObject(const char* name, GameObject* parent);
	virtual ~EditorGameObject();

	void DrawBoundingBoxes();
	void DrawSelected();
	void DrawHighlight();
};

