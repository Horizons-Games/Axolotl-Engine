#pragma once

#include "Scripting\Script.h"

// This script is just to show how to instantiate/use the different types of variables

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
	void SetSentence(const std::string& sentence);

	GameObject* GetCharacter() const;
	void SetCharacter(GameObject* character);

	bool GetCheck() const;
	void SetCheck(bool check);

private:
	float value;
	std::string sentence;
	GameObject* character;
	bool check;
};