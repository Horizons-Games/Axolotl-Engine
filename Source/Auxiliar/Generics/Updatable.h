#pragma once

class Updatable
{
public:
	virtual ~Updatable() = default;

	virtual void PreUpdate() {};
	virtual void Update() = 0;
	virtual void PostUpdate() {};
};
