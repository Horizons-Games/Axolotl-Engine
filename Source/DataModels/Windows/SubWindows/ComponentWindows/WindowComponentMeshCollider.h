#pragma once

#include "ComponentWindow.h"

class ComponentMeshCollider;

class WindowComponentMeshCollider :
	public ComponentWindow
{
public:
	WindowComponentMeshCollider(ComponentMeshCollider* component);
	~WindowComponentMeshCollider() override;

protected:
	void DrawWindowContents() override;

};
