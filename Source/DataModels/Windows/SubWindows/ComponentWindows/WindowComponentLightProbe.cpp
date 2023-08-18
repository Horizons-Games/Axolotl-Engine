#include "StdAfx.h"

#include "WindowComponentLightProbe.h"

#include "Application.h"

#include "ModuleScene.h"

#include "Scene/Scene.h"

#include "Components/ComponentLightProbe.h"


WindowComponentLightProbe::WindowComponentLightProbe(ComponentLightProbe* component) :
	ComponentWindow("LIGHT PROBE", component)
{
}

WindowComponentLightProbe::~WindowComponentLightProbe()
{
}

void WindowComponentLightProbe::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
}
