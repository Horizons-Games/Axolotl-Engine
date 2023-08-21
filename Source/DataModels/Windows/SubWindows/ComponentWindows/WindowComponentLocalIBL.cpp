#include "StdAfx.h"

#include "WindowComponentLocalIBL.h"

#include "Application.h"

#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Components/ComponentLocalIBL.h"

WindowComponentLocalIBL::WindowComponentLocalIBL(ComponentLocalIBL* component) :
	ComponentWindow("LIGHT PROBE", component)
{
}

WindowComponentLocalIBL::~WindowComponentLocalIBL()
{
}

void WindowComponentLocalIBL::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
}
