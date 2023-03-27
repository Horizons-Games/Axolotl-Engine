#pragma once
#include "Component.h"

#include "Math/float3.h"

class ComponentMockState :
	public Component
{
public:
	ComponentMockState(bool active, GameObject* owner);
	~ComponentMockState() override;

	void Update() override;
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	bool GetIsWinState() const;
	bool GetIsFailState() const;

	void SetIsWinState(bool isWinState);
	void SetIsFailState(bool isFailState);

	std::string GetSceneName() const;
	void SetSceneName(const std::string& newTag);

private:

	bool isWinState;
	bool isFailState;
	std::string sceneName;
};

inline bool ComponentMockState::GetIsWinState() const
{
	return this->isWinState;
}

inline bool ComponentMockState::GetIsFailState() const
{
	return this->isFailState;
}

inline void ComponentMockState::SetIsWinState(bool isWinState)
{
	this->isWinState = isWinState;
}

inline void ComponentMockState::SetIsFailState(bool isFailState)
{
	this->isFailState = isFailState;
}

inline std::string ComponentMockState::GetSceneName() const
{
	return sceneName.c_str();
}

inline void ComponentMockState::SetSceneName(const std::string& newTag)
{
	sceneName = newTag;
}

