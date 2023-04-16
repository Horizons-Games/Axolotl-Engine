#pragma once

#include "Scripting\Script.h"

class DefaultScript : public Script
{
public:
	DefaultScript();
	~DefaultScript() override = default;

	void Init() override {};
	void Start() override {};
	void PreUpdate(float deltaTime) override {};
	void Update(float deltaTime) override;
	void PostUpdate(float deltaTime) override {};
	void CleanUp() override {};

	float GetValue() const
	{
		return value;
	}
	void SetValue(float value)
	{
		this->value = value;
	}
	float GetValue2() const
	{
		return value3;
	}
	void SetValue2(float value)
	{
		this->value3 = value;
	}

private:
	float value;
	float value3;
};