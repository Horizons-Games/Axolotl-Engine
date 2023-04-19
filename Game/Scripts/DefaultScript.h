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

	float GetValue() const;
	void SetValue(float value);

	std::string GetSentence() const;
	void SetSentence(const std::string sentence);

private:
	float value;
	std::string sentence;
};