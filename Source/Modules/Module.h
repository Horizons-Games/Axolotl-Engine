#pragma once

#include "Enums/UpdateStatus.h"

class Application;

class Module
{
public:
	Module()
	{
	}

	virtual ~Module()
	{
	}

	virtual bool Start();

	virtual bool Init();

	virtual UpdateStatus PreUpdate();

	virtual UpdateStatus Update();

	virtual UpdateStatus PostUpdate();

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

inline UpdateStatus Module::PreUpdate()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

inline UpdateStatus Module::Update()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

inline UpdateStatus Module::PostUpdate()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

inline bool Module::CleanUp()
{
	return true;
}
