#pragma once

#include "ComponentWindow.h"

class ComponentRigidBody;

class WindowComponentRigidBody :
	public ComponentWindow
{
public:
	WindowComponentRigidBody(ComponentRigidBody* component);
	~WindowComponentRigidBody() override;

protected:
	void DrawWindowContents() override;

};

