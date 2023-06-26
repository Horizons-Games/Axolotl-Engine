#include "StdAfx.h"

#include "ModuleNavigation.h"

ModuleNavigation::ModuleNavigation()
{
}

ModuleNavigation::~ModuleNavigation()
{
}

bool ModuleNavigation::Init()
{
	return true;
}

bool ModuleNavigation::Start()
{
	return true;
}

bool ModuleNavigation::CleanUp()
{
	return true;
}

UpdateStatus ModuleNavigation::PreUpdate()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleNavigation::Update()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleNavigation::PostUpdate()
{
	return UpdateStatus::UPDATE_CONTINUE;
}
