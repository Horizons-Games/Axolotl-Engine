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

update_status ModuleNavigation::PreUpdate()
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleNavigation::Update()
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleNavigation::PostUpdate()
{
	return update_status::UPDATE_CONTINUE;
}
