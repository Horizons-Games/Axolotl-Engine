#include "StdAfx.h"

#include "WindowComponentLocalIBL.h"

#include "Application.h"

#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Components/ComponentLocalIBL.h"

WindowComponentLocalIBL::WindowComponentLocalIBL(ComponentLocalIBL* component) :
	WindowComponentLight("LOCAL IBL", component)
{
}

WindowComponentLocalIBL::~WindowComponentLocalIBL()
{
}

void WindowComponentLocalIBL::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
}
