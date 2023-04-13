#pragma once

#ifndef ISCRIPT_INCLUDED
#define ISCRIPT_INCLUDED

//#include "RuntimeCompiler/"

class GameObject;

struct IScript : public IObject
{
	virtual void Init() = 0;
	virtual void Start() = 0;
	virtual void PreUpdate() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void PostUpdate() = 0;
};

#endif // ISCRIPT_INCLUDED