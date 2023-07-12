#include "StdAfx.h"

#include "WindowComponentTrail.h"
#include "Components/ComponentTrail.h"
#include "ModuleScene.h"
#include "Application.h"
#include "Scene/Scene.h"
#include "Cubemap/Cubemap.h"


WindowComponentTrail::WindowComponentTrail(ComponentTrail* component) : ComponentWindow("TRAIL", component)
{
}

WindowComponentTrail::~WindowComponentTrail()
{
}

void WindowComponentTrail::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
}
