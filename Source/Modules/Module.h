#pragma once

#include "Globals.h"

class Application;

class Module
{
public:

	Module() {}

	virtual ~Module() {}

	virtual bool Start();

	virtual bool Init();

	virtual update_status PreUpdate();

	virtual update_status Update();

	virtual update_status PostUpdate();

	virtual bool CleanUp();
};

inline bool Module::Start()
{
	return true;
}

inline bool Module::Init()
{
	return true;
}

inline update_status Module::PreUpdate()
{
	return UPDATE_CONTINUE;
}

inline update_status Module::Update()
{
	return UPDATE_CONTINUE;
}

inline update_status Module::PostUpdate()
{
	return UPDATE_CONTINUE;
}

inline bool Module::CleanUp()
{
	return true;
}
