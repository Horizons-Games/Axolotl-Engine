#pragma once

class Updatable
{
public:
	virtual ~Updatable() = default;

	virtual void Update() = 0;
};
