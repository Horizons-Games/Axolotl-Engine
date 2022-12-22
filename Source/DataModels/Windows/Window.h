#pragma once

#include "Globals.h"
#include <string>

class Window
{
public:
	~Window() {}

	virtual void Draw(bool& enabled) = 0;

	inline const std::string& GetName() const { return name; }
	
protected:
	Window(const std::string& name)
	{
		this->name = name;
	}

	std::string name;
};

